/********************************************************************
	created:	2013/05/23
	created:	23:5:2013   19:34
	filename: 	filemanager.cpp
	file path:	\
	file base:	filemanager
	file ext:	cpp
	author:		ugi
	
	purpose:	
*********************************************************************/

#include "filemanager.h"
#include "../Common/tsd/tsdprotocol.h"
#include "tsdclientconnection.h"

#include <QtCore/QFile>
#include <QtGui/QApplication>
#include <QtCore/QDateTime>
#include <QStyle>
#include <QtGui/QApplication>


FileManager* g_FileManager=NULL;

#define FILEMANAGER_TEST

FileManager::FileManager()
{
	m_pFileManagerWnd=NULL;
	m_PeerPathTmp="";

	g_FileManager=this;
	m_bBrowserPeerItem=false;

	m_bFirstFileDownloadMsg=true;
	m_numOfFilesToDownload=-1;
	m_currentDownloadIndex = -1;
	m_bTransferEnable=true;
	m_ClientPath=QDir::homePath()+"/";
	m_DownloadRead=0;
	m_bReportUploadCancel=false;

	m_pFiletoRead=NULL;
	m_bUploadStarted = false;
}

void FileManager::setFileManagerWnd(FileManagerWindow* w)
{
	m_pFileManagerWnd=w;
	m_pFocusItem=m_pFileManagerWnd->remoteExplorer()->client()->dirView()->invisibleRootItem();

}

void FileManager::readFileListData()
{
	FileListDataPacket fld;

	m_pConnection->readExact((char*)&fld,sizeof(fld));

	FTSIZEDATA *pftSD=new FTSIZEDATA[fld.numFiles];

	m_pConnection->readExact((char*)pftSD,sizeof(FTSIZEDATA)*fld.numFiles);

	char *pFilenames=new char[fld.compressedSize];

	m_pConnection->readExact(pFilenames,fld.compressedSize);

	if(m_bBrowserPeerItem==false)
	{
		m_FTPeerItemInfo.free();
		createPeerItemInfoList(&m_FTPeerItemInfo, pftSD, fld.numFiles, pFilenames, fld.dataSize);
		//m_FTPeerItemInfo.sort();

		m_pFileManagerWnd->remoteExplorer()->client()->contentView()->deleteAllItems();

		m_pFileManagerWnd->remoteExplorer()->client()->contentView()->showFileTransferItems(&m_FTPeerItemInfo);
	}


	//for dirview  

	int childcount=m_pFocusItem->childCount();

	for(int i=0;i<childcount;i++)
	{
		QTreeWidgetItem* item=m_pFocusItem->child(i);

		m_pFocusItem->removeChild(item);
		delete item;
	}

	 QStyle *style = QApplication::style();

	int pos=0;
	for(int i=0;i<fld.numFiles;i++)
	{
		if(pftSD[i].size==-1 || pftSD[i].size==-2)   //drive or folder
		{
			QString name=QString::fromUtf8(pFilenames + pos);
			QTreeWidgetItem * item=new QTreeWidgetItem(m_pFocusItem);

			QTreeWidgetItem* nullitem=new QTreeWidgetItem(item);

			item->setText(0,name);


			if(pftSD[i].size==-1)
				item->setIcon(0,style->standardIcon(QStyle::SP_DirIcon));
			else
				item->setIcon(0,style->standardIcon(QStyle::SP_DriveHDIcon));
		}

		pos += strlen(pFilenames + pos) + 1;
	}

	delete []pFilenames;
	delete []pftSD;
}

void FileManager::onRemoteDirViewExpanded(QTreeWidgetItem * item)
{
	QTreeWidgetItem* parent=item->parent();	

	m_bBrowserPeerItem=true;

	m_pFocusItem=item;

	int childcount=m_pFocusItem->childCount();

	Q_ASSERT(childcount>0);

	if(childcount>1) return ; // this item has already loaded children items.

	Q_ASSERT(childcount==1);
	
	QTreeWidgetItem* child=m_pFocusItem->child(0);

	QString childtext=child->text(0);

	if(childtext!="") return; //this item has already one loaded children item.

	m_pFocusItem->removeChild(child);
	delete child;

	QString path=item->text(0);
	
	while(parent!=NULL)
	{
		path=parent->text(0)+"/"+path;	
		parent=parent->parent();
	}

	sendFileListRequest(path,0x10);
}

void FileManager::onRemoteContentViewDblClick(QTreeWidgetItem * item, int column)
{

	QString name=item->text(0);

	RemoteDirView *dirview=m_pFileManagerWnd->remoteExplorer()->client()->dirView();

	int childcount=m_pFocusItem->childCount();

	for(int i=0;i<childcount;i++)
	{
		QTreeWidgetItem* item=m_pFocusItem->child(i);

		if(item->text(0)==name)
		{
			dirview->setCurrentItem(item);
			m_pFocusItem=item;
			break;
		}
	}

	QString type=item->text(2);

	if(type==tr("File Folder") || type==tr("Drive"))
	{
		if(m_PeerPathTmp=="")
			m_PeerPathTmp=item->text(0)+"/";
		else
			m_PeerPathTmp=m_PeerPathTmp+item->text(0)+"/";
		m_bBrowserPeerItem=false;
		sendFileListRequest(m_PeerPathTmp,0);

		RemoteAddressBar* bar=m_pFileManagerWnd->remoteExplorer()->addressBar();
		bar->setCurrentPath(m_PeerPathTmp);
	}
}


void FileManager::onRemoteDirViewItemClicked(QTreeWidgetItem * item, int column)
{
	QString path=item->text(0);

	QTreeWidgetItem * parent=item->parent();

	while(parent!=NULL)
	{
		path=parent->text(0)+"/"+path;	
		parent=parent->parent();
	}

	m_pFocusItem=item;
	if(path==m_PeerPathTmp) return;

	sendFileListRequest(path,0x0);
	

	if(m_bBrowserPeerItem==true) m_bBrowserPeerItem=false;
	m_PeerPathTmp=path+"/";

	m_pFileManagerWnd->remoteExplorer()->addressBar()->setCurrentPath(m_PeerPathTmp);
}

void FileManager::sendFileDownloadCancel(QString reason)
{
	FileDownloadCancelPacket fdc;

	fdc.type=Packet_FileDownloadCancel;

	QByteArray data=reason.toUtf8();

	fdc.reasonLen=data.size()+1;

	m_pConnection->writeExact((char*)&fdc,sizeof(FileDownloadCancelPacket));
	m_pConnection->writeExact(data.data(),fdc.reasonLen);
}

void FileManager::closeUndoneFileTransfers()
{
	/*if (m_bUploadStarted)
	{
		m_bUploadStarted = FALSE;
		CloseHandle(m_hFiletoRead);
	}*/
	if (m_bDownloadStarted)
	{
		m_bDownloadStarted = false;
		m_bFirstFileDownloadMsg = true;
		m_currentDownloadIndex = -1;
		m_numOfFilesToDownload = -1;
	
		m_pFiletoWrite->close();
		delete m_pFiletoWrite;
		m_pFiletoWrite=NULL;

		QFile::remove(m_DownloadFilename);
	}
}

void FileManager::cancelDownLoad(QString reason)
{
	sendFileDownloadCancel(reason);
	m_pFileManagerWnd->setStatusText(reason);

	closeUndoneFileTransfers();
	m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(false);
	m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(true);
	m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(true);
	m_pFileManagerWnd->activateWindow();

	m_pFileManagerWnd->remoteExplorer()->setEnabled(true);

	m_bDownloadStarted=false;
	m_pFileManagerWnd->progressBar()->setValue(0);
	m_DownloadRead=0;
}


void FileManager::fileTransferDownload()
{
	FileDownloadDataPacket fdd;

	m_pConnection->readExact((char*)&fdd,sizeof(FileDownloadDataPacket));

	if(m_bFirstFileDownloadMsg)
	{
		//download started
		m_DownloadBlockSize = fdd.compressedSize;
		
		m_DownloadFilename=m_ClientPath+m_PeerFileName;

		m_pFiletoWrite=new QFile(m_DownloadFilename);

		m_bOpendFileToWrite=m_pFiletoWrite->open(QIODevice::WriteOnly);

		int amount = m_SizeDownloadFile / ((m_DownloadBlockSize + 1) * 10);

		m_pFileManagerWnd->initProgressBar(0,0,amount,1);

		m_bFirstFileDownloadMsg = false;
		m_bDownloadStarted = true;

		QString status="Downloading: "+m_DownloadFilename+" <- "+m_PeerPathTmp+m_PeerFileName;
		m_pFileManagerWnd->setStatusText(status);
	}

	if ((fdd.realSize == 0) && (fdd.compressedSize == 0)) 
	{
		//download finished

		uint mTime;
		m_pConnection->readExact((char *) &mTime, sizeof(uint));

		if (m_bOpendFileToWrite == false) 
		{
			cancelDownLoad("Could not open file");
			/*MessageBox(m_hwndFileTransfer, "Download failed: could not create local file",
				"Download Failed", MB_ICONEXCLAMATION | MB_OK);*/
			m_pFileManagerWnd->progressBar()->setValue(0);
			return;
		}

		/*FILETIME Filetime;
		Time70ToFiletime(mTime, &Filetime);
		SetFileTime(m_hFiletoWrite, &Filetime, &Filetime, &Filetime);*/

		m_pFileManagerWnd->progressBar()->setValue(0);
		m_pFiletoWrite->close();

		delete m_pFiletoWrite;
		m_pFiletoWrite=NULL;

		m_bFirstFileDownloadMsg = true;
		m_bDownloadStarted = false;
		m_DownloadRead=0;
	
		m_pFileManagerWnd->setStatusText("complete");
		download();  //next file download start
		m_pFileManagerWnd->setStatusText("");
		return;
	}

	char * pBuff =new char[fdd.realSize];

	m_pConnection->readExact(pBuff,fdd.realSize);

	int  NumberOfBytesWritten;

	if (!m_bTransferEnable) 
	{
		cancelDownLoad("Download cancelled by user");
		delete [] pBuff;
		return;
	}

	if (m_bOpendFileToWrite == false)
	{
		cancelDownLoad("Could not create file");
		/*MessageBox(m_hwndFileTransfer, "Download failed: could not create local file",
			"Download Failed", MB_ICONEXCLAMATION | MB_OK);*/
		delete [] pBuff;
		return;
	}

	NumberOfBytesWritten=m_pFiletoWrite->write(pBuff,fdd.compressedSize);

#ifdef FILEMANAGER_TEST
	Q_ASSERT(NumberOfBytesWritten==fdd.compressedSize);
	qDebug("write file data %d",NumberOfBytesWritten);
#endif
	
	m_DownloadRead += NumberOfBytesWritten;
	
	m_pFileManagerWnd->progressBar()->setValue(m_DownloadRead/((m_DownloadBlockSize + 1) * 10));
#ifdef FILEMANAGER_TEST
	qDebug("progressbar value %d",m_DownloadRead/((m_DownloadBlockSize + 1) * 10));
#endif
	delete [] pBuff;
}


void FileManager::sendFileUploadData(uint mtime)
{
	FileUploadDataPacket msg;
	msg.type = Packet_FileUploadData;
	msg.compressedLevel = 0;
	msg.realSize = 0;
	msg.compressedSize = 0;

	char data[sizeof(FileUploadDataPacket) + sizeof(uint)];
	
	memcpy(data, &msg, sizeof(FileUploadDataPacket));
	memcpy(&data[sizeof(FileUploadDataPacket)], &mtime, sizeof(uint));

	m_pConnection->writeExact(data, sizeof(FileUploadDataPacket) + sizeof(uint));
}

void FileManager::fileTransferUpload()
{
	int numOfFilesToUpload = 0, currentUploadIndex = -1;
	qint64 sz_rfbFileSize;
	ulong sz_rfbBlockSize= 8192;
	ulong dwNumberOfBytesRead = 0;
	unsigned int mTime = 0;

	QString path;
	bool bResult;

	QModelIndexList list=m_pFileManagerWnd->localExplorer()->client()->contentView()->getSelectedIndexes();

	numOfFilesToUpload=list.size()/4;// one row is constructed from 4 column

	if (numOfFilesToUpload==0)
	{
		m_pFileManagerWnd->setStatusText(tr("No file selected, nothing to upload."));
		m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(false);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(true);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(true);
		return;
	}

	for(int i=0;i<list.size();i++)
	{
		QModelIndex index=list[i];

		int column=index.column();

		if(column!=0) continue;    //we select name columns,ignore type,size, last modified  

		m_pFileManagerWnd->remoteExplorer()->setEnabled(false);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(true);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(false);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(false);

		m_ClientFileName=index.data().toString();

		path=m_ClientPath+m_ClientFileName;

		m_UploadFilename=path;

		QFileInfo info(path);

		if(!info.exists())
		{
			m_pFileManagerWnd->setStatusText(tr("Could not find selected file, can't upload"));
			continue;
		}
		else if(info.isDir())
		{
			m_pFileManagerWnd->setStatusText(tr("Cannot upload a directory"));
			continue;
		}
		else
		{
			sz_rfbFileSize = info.size();

			QDateTime t=info.lastModified();

			mTime = t.toTime_t();
			m_PeerFileName=info.fileName();
		}
			
		if ((sz_rfbFileSize != 0) && (sz_rfbFileSize <= sz_rfbBlockSize)) 
			sz_rfbBlockSize = sz_rfbFileSize;

		m_pFiletoRead=new QFile(path);

		if(!m_pFiletoRead->open(QIODevice::ReadOnly))
		{
			m_pFileManagerWnd->setStatusText(tr("Upload failed: could not open selected file"));
			m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(false);
			m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(true);
			m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(true);
			delete m_pFiletoRead;
			m_pFiletoRead=NULL;
			continue;
		}

		QString status="Uploading: "+m_ClientPath+m_ClientFileName+" -> "+m_PeerPathTmp+m_PeerFileName;
		m_pFileManagerWnd->setStatusText(status);

		path=m_PeerPathTmp+m_ClientFileName;

		QByteArray path_data=path.toUtf8();

		int pathLen=path_data.size()+1;

		char *pAllFURMessage = new char[sizeof(FileUploadRequestPacket) + pathLen];
	
		FileUploadRequestPacket *pFUR = (FileUploadRequestPacket *) pAllFURMessage;
		char *pFollowMsg = &pAllFURMessage[sizeof(FileUploadRequestPacket)];
		pFUR->type = Packet_FileUploadRequest;
		pFUR->compressedLevel = 0;
		pFUR->fNameSize = pathLen;
		pFUR->position = 0;

		memcpy(pFollowMsg, path_data.data(), pathLen);

		Q_ASSERT(m_pConnection->writeExact(pAllFURMessage, sizeof(FileUploadRequestPacket) + pathLen)==true);
	
		delete [] pAllFURMessage;
		
		if(sz_rfbFileSize==0)
		{
			sendFileUploadData(mTime);
		}
		else
		{
			int amount = sz_rfbFileSize / (sz_rfbBlockSize * 10);

			m_pFileManagerWnd->initProgressBar(0, 0, amount, 1);

			ulong dwPortionRead = 0;

			char *pBuff = new char [sz_rfbBlockSize];
			m_bUploadStarted = true;
			
			while(m_bUploadStarted)
			{
				if (m_bTransferEnable == false)	   
				{
					m_pFileManagerWnd->setStatusText(tr("File transfer canceled"));
					m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(false);
					m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(true);
					m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(true);
					
					QString reason= "File transfer canceled by user";
					QByteArray reason_data=reason.toUtf8();

					int reasonLen = reason_data.size()+1;
					char *pFUFMessage = new char[sizeof(FileUploadFailedPacket) + reasonLen];
				
					FileUploadFailedPacket *pFUF = (FileUploadFailedPacket *) pFUFMessage;
					char *pReason = &pFUFMessage[sizeof(FileUploadFailedPacket)];
					pFUF->type = Packet_FileUploadFailed;
					pFUF->reasonLen = reasonLen;
					memcpy(pReason, reason_data.data(), reasonLen);

					Q_ASSERT(m_pConnection->writeExact(pFUFMessage, sizeof(FileUploadFailedPacket) + reasonLen)==true);
		
					delete [] pFUFMessage;
					m_pFileManagerWnd->progressBar()->setValue(0);

					break;
				}

				dwNumberOfBytesRead = m_pFiletoRead->read(pBuff, sz_rfbBlockSize);
				if (dwNumberOfBytesRead == 0) 
				{
					/* This is the end of the file. */
					sendFileUploadData(mTime);
					break;
				}

				sendFileUploadData((unsigned short)dwNumberOfBytesRead, pBuff);
				dwPortionRead += dwNumberOfBytesRead;

				m_pFileManagerWnd->progressBar()->setValue(dwPortionRead/((sz_rfbBlockSize + 1) * 10));
				qApp->processEvents();
			}   //while
			
			if (m_bTransferEnable == false)
				break;
			m_bUploadStarted = false;
			delete [] pBuff;
		}//else

		m_pFileManagerWnd->progressBar()->setValue(0);

		m_pFileManagerWnd->setStatusText("");

		m_pFiletoRead->close();
		
		delete m_pFiletoRead;
		m_pFiletoRead=NULL;

	}//for
	
	m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(false);
	m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(true);
	m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(true);
	m_pFileManagerWnd->remoteExplorer()->setEnabled(true);
	sendFileListRequest(m_PeerPathTmp, 0);
}


void FileManager::sendFileUploadData(unsigned short size, char *pFile)
{
	int msgLen =  sizeof(FileUploadDataPacket) + size;
	char *pAllFUDMessage = new char[msgLen];

	FileUploadDataPacket *pFUD = (FileUploadDataPacket *) pAllFUDMessage;

	char *pFollow = &pAllFUDMessage[ sizeof(FileUploadDataPacket)];
	pFUD->type = Packet_FileUploadData;
	pFUD->compressedLevel = 0;
	pFUD->realSize = size;
	pFUD->compressedSize = size;
	memcpy(pFollow, pFile, size);

	Q_ASSERT(m_pConnection->writeExact(pAllFUDMessage, msgLen)==true);

	delete [] pAllFUDMessage;
}
void FileManager::upload()
{
	if(m_ClientPath=="" || m_PeerPathTmp=="")
	{
		m_pFileManagerWnd->setStatusText("Cannot transfer files: illegal directory.");
		return;
	}

	m_bTransferEnable = true;
	m_bReportUploadCancel = true;

	m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(true);
	m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(false);
	m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(false);

	fileTransferUpload();	
}

void FileManager::cancel()
{
	m_bTransferEnable=false;
}

//multiple file download
void FileManager::download()
{
	//download complete
	if(m_numOfFilesToDownload == 0)
	{
		m_numOfFilesToDownload = -1 ;
		m_currentDownloadIndex = -1;
		m_SelectedPeerItems.clear();

		m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(false);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(true);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(true);
		
		m_pFileManagerWnd->remoteExplorer()->setEnabled(true);

		return ;
	}

	// This is the first call for currently select file list.
	// Count of slected files is not calculated yet 

	if(m_numOfFilesToDownload == -1)
	{
		m_SelectedPeerItems=m_pFileManagerWnd->remoteExplorer()->client()->contentView()->selectedItems();
		m_numOfFilesToDownload =m_SelectedPeerItems.size();

		if (m_numOfFilesToDownload == 0)
		{
			m_pFileManagerWnd->setStatusText(tr("No file is selected, nothing to download."));

			m_numOfFilesToDownload  = -1;
			m_currentDownloadIndex = -1;

			m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(false);
			m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(true);
			m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(true);
			m_pFileManagerWnd->remoteExplorer()->setEnabled(true);

			return ;
		}
		else
		{ 
			// file transfer will start for all the selected files now. set m_bTransferEnable to true
			// Enable cancel button and disable rest of the UI components.
			m_bTransferEnable = true;
			m_pFileManagerWnd->remoteExplorer()->setEnabled(false);

			m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(false);
			m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(false);
			m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(true);
		}
	}

	// Calculate the next selected index for which file download request has to be sent.
	int index = -1;

	QTreeWidgetItem* item=m_SelectedPeerItems[m_numOfFilesToDownload-1];

	QString tt=item->text(0);

	
	index=m_pFileManagerWnd->remoteExplorer()->client()->contentView()->indexOfTopLevelItem(item);

	if (index < 0)
	{
		m_pFileManagerWnd->setStatusText(tr("No file is selected, nothing to download."));

		m_numOfFilesToDownload  = -1;
		m_currentDownloadIndex = -1;

		m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(false);

		m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(true);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(true);
		m_pFileManagerWnd->remoteExplorer()->setEnabled(true);

		return ;
	}

	// If Cancel button is clicked, dont send the file download request.
	if(m_bTransferEnable == false)
	{
		m_numOfFilesToDownload  = -1;
		m_currentDownloadIndex = -1;

		m_pFileManagerWnd->localExplorer()->client()->contentView()->cancelAction()->setEnabled(false);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->downloadAction()->setEnabled(true);
		m_pFileManagerWnd->localExplorer()->client()->contentView()->uploadAction()->setEnabled(true);

		m_pFileManagerWnd->remoteExplorer()->setEnabled(true);

		return ;
	}

	// Update member variables for next call.
	m_currentDownloadIndex = index;
	m_numOfFilesToDownload --;

	sendFileDownloadRequest();
}

/**
	
	@preconditon m_PeerFileName must be setted by user. This file is source file for downloading.

**/

void FileManager::sendFileDownloadRequest()
{
	if (!m_FTPeerItemInfo.isFile(m_currentDownloadIndex))
	{
		m_pFileManagerWnd->setStatusText(tr("Cannot download. Please select regular files but directory."));
		emit message("Cannot download. Please select regular files but directory.");
		
		// Send message to start download for next selected file.
		//PostMessage(m_hwndFileTransfer, WM_COMMAND, IDC_FTCOPY, 0);

		download();
		return;
	}

	m_PeerFileName=m_FTPeerItemInfo.getNameAt(m_currentDownloadIndex);

	m_ClientFileName=m_PeerFileName;

	m_SizeDownloadFile = m_FTPeerItemInfo.getSizeAt(m_currentDownloadIndex);

	FileDownloadRequestPacket fdr;
	fdr.type = Packet_FileDownloadRequest;
	fdr.compressedLevel = 0;
	fdr.position = 0;

	QString path=m_PeerPathTmp+m_PeerFileName;

	QByteArray data=path.toUtf8();

	fdr.fNameSize=data.size()+1;

	m_pConnection->writeExact((char *)&fdr, sizeof(FileDownloadRequestPacket));
	m_pConnection->writeExact(data.data(), data.size()+1);

	qDebug("send file download request for %s",path);
}



void FileManager::createPeerItemInfoList(FileTransferItemInfo *pftii, FTSIZEDATA *ftsd, int ftsdNum, char *pfnames, int fnamesSize)
{
	int pos = 0;

	for (int i = 0; i < ftsdNum; i++) 
	{
		QString name=QString::fromUtf8(pfnames+pos);

		pftii->add(name, ftsd[i].size, ftsd[i].data);

		pos += strlen(pfnames + pos) + 1;
	}
}

FileManager::~FileManager()
{
	if(m_pFileManagerWnd)
		delete m_pFileManagerWnd;
	g_FileManager=NULL;

}

void FileManager::sendFileListRequest(QString filename ,unsigned char flag)
{
	QByteArray filename_data=filename.toUtf8();
	
	FileListRequestPacket packet;

	packet.type=Packet_FileListRequest;
	packet.flags=flag;
	packet.dirNameSize=filename_data.size()+1;

	m_pConnection->writeExact(( char*)(&packet),sizeof(packet));
	m_pConnection->writeExact(filename_data.data(),filename_data.size()+1);

	qDebug("send filelist request dir=%s",filename);
}

void FileManager::readFileDownloadFailed()
{
	FileDownloadFailedPacket fdf;

	m_pConnection->readExact((char*)&fdf,sizeof(FileDownloadFailedPacket));

	char * reason=new char[fdf.reasonLen];

	m_pConnection->readExact(reason,fdf.reasonLen);

	QString strreason=QString::fromUtf8(reason);

	delete []reason;

	m_pFileManagerWnd->remoteExplorer()->setEnabled(true);

	QString path=m_PeerPathTmp+m_PeerFileName;
	emit fileDownloadFailed(strreason+" : "+path+" in remote site");
}

void FileManager::readFileUploadCancel()
{
	FileUploadCancelPacket fuc;

	m_pConnection->readExact((char*)&fuc,sizeof(FileUploadCancelPacket));

	char * reason=new char[fuc.reasonLen];

	m_pConnection->readExact(reason,fuc.reasonLen);

	QString strreason=QString::fromUtf8(reason);

	delete []reason;

	m_pFileManagerWnd->remoteExplorer()->setEnabled(true);

	QString path=m_PeerPathTmp+m_PeerFileName;
	emit message("upload failed.\n"+strreason+" : "+path);
}






