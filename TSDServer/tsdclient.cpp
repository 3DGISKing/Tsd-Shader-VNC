#include "tsdclient.h"
#include "../Common/tsd/tsdprotocol.h"
#include "../Common/tsd/filetransferiteminfo.h"
#include "tsdserver.h"
#include "eventhandler.h"
#include "highlighter.h"
#include "tsdserverapp.h"
#include "chatdlg.h"
#include "reboot.h"
#include "guiuty.h"

#include <QMessageBox>
#include <QFileInfoList>
#include <QDir>
#include <QDateTime>
#include <QTimer>


TSDClient::TSDClient(QObject *parent,int socketdescriter)
	: QObject(parent)
{
	m_SocketDescriper=socketdescriter;

	m_pSocket=new TSDSocket();

	m_pSocket->setSocketDescriptor(m_SocketDescriper);

	connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)),	this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(m_pSocket,SIGNAL(readyRead()),this,SLOT(onReceived()));
	connect(m_pSocket,SIGNAL(disconnected()),this,SLOT(onDisconnected()));

	m_pServer=NULL;

	m_pFileToRead=NULL;
	m_pFileToRead=NULL;
	m_pFileToWrite=NULL;
	m_bUploadStarted=false;
	m_bDownloadStarted=false;

	m_BlockSize=4096;
}

void TSDClient::onDisconnected()
{
	emit disconnected(this);
}


void TSDClient::readMouseEvent()
{
	MouseEventPacket packet;
	m_pSocket->readExact((char*)&packet,sizeof(MouseEventPacket));

#ifdef Q_OS_WIN
	EventHandler::winSetMouseMessage(EventHandler::convertQEventTypeToWindowsMessage((QEvent::Type)packet.eventtype,(Qt::MouseButton)packet.button,(Qt::MouseButtons)packet.buttons),
		QPoint((double)packet.x*m_pServer->m_dxWidthScalar,(double)packet.y*m_pServer->m_dyHeightScalar),
		EventHandler::convertQKeyboardModifierToWindowsVirtualKeyFlags((Qt::KeyboardModifier)packet.keyboardmodifiers),
		0);
#endif

#ifdef Q_OS_LINUX
	EventHandler::linuxSetMouseMessage(EventHandler::convertQEventTypeToLinuxMessage((QEvent::Type)packet.eventtype),
		EventHandler::convertQtMouseButtonToLinuxButton((Qt::MouseButton)packet.button),
		QPoint(packet.x,packet.y),
		EventHandler::convertQKeyboardModifierToLinuxVirtualKeyFlags((Qt::KeyboardModifier)packet.keyboardmodifiers),
		0);
#endif

#ifdef Q_OS_MAC
	if((QEvent::Type)packet.eventtype==QEvent::MouseButtonDblClick && (Qt::MouseButton)packet.button==Qt::LeftButton)
		EventHandler::macSetMouseLButtonDblClick(packet.x,packet.y);
	else if((QEvent::Type)packet.eventtype==QEvent::MouseButtonDblClick && (Qt::MouseButton)packet.button==Qt::RightButton)
		EventHandler::macSetMouseRButtonDblClick(packet.x,packet.y);

	else if((QEvent::Type)packet.eventtype==QEvent::MouseButtonDblClick && (Qt::MouseButton)packet.button==Qt::MidButton)
		EventHandler::macSetMouseMButtonDblClick(packet.x,packet.y);
	else
	{
		CGEventRef mouse = CGEventCreateMouseEvent(NULL, EventHandler::convertQEventTypeToMacEventType((QEvent::Type)packet.eventtype,(Qt::MouseButton)packet.button,(Qt::MouseButtons)packet.buttons), CGPointMake(packet.x,packet.y), kCGMouseButtonLeft);
		CGEventPost(kCGHIDEventTap, mouse );
		CFRelease(mouse);
	}
#endif
}

void TSDClient::readMouseWheelEvent()
{
	MouseWheelEventPacket packet;
	
	m_pSocket->readExact((char*)& packet,sizeof(MouseWheelEventPacket));


#ifdef Q_OS_WIN
	EventHandler::winSetMouseMessage(WM_MOUSEWHEEL,
		QPoint((double)packet.x*m_pServer->m_dxWidthScalar,(double)packet.y*m_pServer->m_dxWidthScalar),
		EventHandler::convertQKeyboardModifierToWindowsVirtualKeyFlags((Qt::KeyboardModifier)packet.keyboardmodifiers),
		packet.delta);
#endif

#ifdef Q_OS_LINUX
	EventHandler::linuxSetMouseMessage(WheelMove, 0,
		QPoint(packet.x,packet.y),
		EventHandler::convertQKeyboardModifierToLinuxVirtualKeyFlags((Qt::KeyboardModifier)packet.keyboardmodifiers),
		packet.delta);

#endif

#ifdef Q_OS_MAC
	CGEventRef mouseWheel=CGEventCreateScrollWheelEvent(NULL,0,1,packet.delta);
	CGEventPost(kCGHIDEventTap,mouseWheel);
	CFRelease(mouseWheel);
#endif
}

void TSDClient::readKeyEvent()
{
	KeyEventPacket packet;
	m_pSocket->readExact((char*)&packet,sizeof(KeyEventPacket));

#ifdef Q_OS_WIN
	WORD wm;
	if(packet.type==Packet_KeyPressEvent)
		wm=WM_KEYDOWN;
	else
		wm=WM_KEYUP;

	EventHandler::winSetKBMessage(wm,EventHandler::convertQtKeyToWindowsVirtualKey(packet.key),packet.count,0);
#endif

#ifdef Q_OS_LINUX
	int wm;
	if(packet.type==Packet_KeyPressEvent)
		wm=KeyPress;
	else
		wm=KeyRelease;

	EventHandler::linuxSetKBMessage(wm,EventHandler::convertQtKeyToLinuxKey(packet.key),packet.count,
		EventHandler::convertQKeyboardModifierToLinuxVirtualKeyFlags((Qt::KeyboardModifier)packet.keyboardmodifiers));
#endif

#ifdef Q_OS_MAC
	bool bl;

	if(packet.type==Packet_KeyPressEvent)
		bl=true;
	else
		bl=false;

	CGEventRef move1,move2;
	move1 = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)EventHandler::convertQtKeyToMacVirtualKey(packet.key), bl);
	//move2 = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)packet.key, false);
	CGEventSetFlags(move1,EventHandler::convertQKeyboardModifierToMacEventFlagMask((Qt::KeyboardModifier)packet.keyboardmodifiers));
	CGEventPost(kCGHIDEventTap, move1);
	//sleep(1);
	CFRelease(move1);
	//CFRelease(move2);
#endif
}

	


void TSDClient::onReceived()
{
start:

	int packet_type;
	if(!m_pSocket->peekInt(packet_type))
		return;

	switch(packet_type)
	{
	case Packet_RequestScreenShare:
		{
			readRequestScreenShare();
			qDebug("received request screen share packet");
		}
		break;
	case Packet_MouseEvent:
		{
			readMouseEvent();
			qDebug("received mouse event packet");
		}
		break;
	case Packet_MouseWheelEvent:
		{
			readMouseWheelEvent();
			qDebug("received mousewheel event packet");
		}
		break;
	case Packet_KeyPressEvent:
	case Packet_KeyReleaseEvent:
		{
			readKeyEvent();
			qDebug("received keyevent packet");
		}
		break;
	case Packet_StopScreenCapture:
		{
			readStopScreenCapture();
			qDebug("received stop screen capture packet");
		}
		break;
	case Packet_StartScreenCapture:
		{
			readStartScreenCapture();
			qDebug("received start screen capture packet");
		}
		break;
	case Packet_FileListRequest:
		{
			readFileListRequestPacket();
			qDebug("received file list request packet");
		}
		break;
	case Packet_FileDownloadRequest:
		{
			readFileDownloadRequest();
			qDebug("received file download request");
		}
		break;
	case Packet_FileDownloadCancel:
		{
			readFileDownloadCancel();
			 qDebug("received file download cancel ");
		}
		break;
	case Packet_FileUploadRequest:
		{
			readFileUploadRequest();
			qDebug("received FileUploadRequest ");
		}
		break;
	case Packet_FileUploadFailed:
		{
			readFileUploadFailed();
			qDebug("received FileUploadFailed");
		}
		break;
	case Packet_FileUploadData:
		{
			readFileUploadData();
			qDebug("received FileUploadData");
		}
		break;
	case Packet_HighLighterStart:
		{
			readHighLighterStart();
			qDebug("received highlight start packet");
		}
		break;
	case Packet_HighLighterEnd:
		{
			readHighLighterEnd();
			qDebug("received highlight end packet ");
		}
		break;
	case Packet_HighLighterData:
		{
			readHighLighterData();
			qDebug("received highlight data packet");
		}
		break;
	case Packet_Chat:
		{
			readChat();
			qDebug("received chat packet");
		}
		break;
	case Packet_Reboot:
		{
			readReboot();
			return;
		}
		break;

	case Packet_StartSessionRequest:
		{
			readStartSessionRequest();
			qDebug("received start session request packet");
		}
		break;
	case Packet_StopSessionRequest:
		{
			readStopSessionRequest();
			return;
		}
		break;
	default:
		qDebug("Error unknown packet");
	}

	if(m_pSocket->bytesAvailable()>0)
		goto start;
}

void TSDClient::readStopSessionRequest()
{
	StopSessionRequestPacket packet;

	m_pSocket->readExact((char*)&packet,sizeof(StopSessionRequestPacket));

	m_pSocket->disconnectFromHost();
	g_app->onMessage("one session stopped");
}

void TSDClient::readStartSessionRequest()
{
	StartSessionRequestPacket packet;

	m_pSocket->readExact((char*)&packet,sizeof(StartSessionRequestPacket));
	
	int ret=gu_MessageBox(NULL,"TSDServer","One client requests starting session. Do you want?",MSGBOX_ICONQUESTION|MSGBOX_IDYESNO);
	
	if(ret==MSGBOX_IDYES)
	{
		sendStartSessionAgree();
		sendDisplayInfo();
		sendTessellationInfo();
		m_pServer->startScreenPieceDelivers();
	}
	else
	{
		sendStartSessionRefuse();
	}
}

void TSDClient::sendStartSessionRefuse()
{
	StartSessionRefusePacket packet;

	packet.type=Packet_StartSessionRefuse;

	m_pSocket->writeExact((char*)&packet,sizeof(StartSessionRefusePacket));
}

void TSDClient::sendStartSessionAgree()
{
	StartSessionAgreePacket packet;

	packet.type=Packet_StartSessionAgree;

	m_pSocket->writeExact((char*)&packet,sizeof(StartSessionAgreePacket));
}


void TSDClient::readHighLighterStart()
{
	if(m_pServer->getHighLighter()->started())
	{

	}
	else
	{
		HighLighterStartPacket hls;

		m_pSocket->readExact((char*)&hls,sizeof(HighLighterStartPacket));
		m_pServer->getHighLighter()->start();
	}
	emit message("highlighter started. please look at highlighted part of your computer carefully.");

}

void TSDClient::readReboot()
{
	RebootPacket packet;

	m_pSocket->readExact((char*)&packet,sizeof(RebootPacket));

	g_app->onMessage("system  will soon be rebooted.");

	g_app->init();
	
	systemReboot();
}

void TSDClient::readHighLighterEnd()
{
	HighLighterEndPacket hle;

	m_pSocket->readExact((char*)&hle,sizeof(HighLighterEndPacket));
	m_pServer->getHighLighter()->end();
	emit message("highlight end");
}

void TSDClient::readHighLighterData()
{
	HighLighterDataPacket hld;
	
	m_pSocket->readExact((char*)&hld,sizeof(HighLighterDataPacket));
	
	int* data=new int[hld.pointnum*2];

	m_pSocket->readExact((char*)data,sizeof(int)*2*hld.pointnum);

	QVector<QPoint> pointvec;

	for(int i=0;i<hld.pointnum;i++)
	{
		QPoint p;

		p.setX( data[2*i]);
		p.setY(data[2*i+1]);
		pointvec.push_back(p);
	}
	m_pServer->getHighLighter()->addPointVector(pointvec);
	m_pServer->getHighLighter()->update();

	delete []data;
}

void TSDClient::readChat()
{
	ChatPacket packet;

	m_pSocket->readExact((char*)&packet,sizeof(ChatPacket));

	char* username_data=new char[packet.usernamesize];
	char* msg_data=new char[packet.msgsize];
	
	m_pSocket->readExact(username_data,packet.usernamesize);

	m_pSocket->readExact(msg_data,packet.msgsize);

	QString username=QString::fromUtf8(username_data);		
	QString msg=QString::fromUtf8(msg_data);
	
	QDateTime dt;

	dt.setTime_t(packet.time);

	QString strdt=dt.toString("h:m:s A");
	
	delete [] username_data;
	delete [] msg_data;

	g_app->chatDlg()->AppendChatMessage(username,strdt,msg);
	g_app->chatDlg()->NewChatMessage(username,strdt,msg);
	g_app->chatDlg()->show();
	QTimer::singleShot(10000,g_app->chatDlg(),SLOT(hide()));
	g_app->chatDlg()->OnChatMaximize();
}

void TSDClient::sendChat(QString username,QString msg,uint t)
{
	ChatPacket packet;

	packet.type=Packet_Chat;
	packet.time=t;

	QByteArray chat_data=msg.toUtf8();

	packet.msgsize=chat_data.size()+1;

	QByteArray username_data=username.toUtf8();

	packet.usernamesize=username_data.size()+1;

	m_pSocket->writeExact((char*)&packet,sizeof(ChatPacket));
	m_pSocket->writeExact(username_data.data(),packet.usernamesize);
	m_pSocket->writeExact(chat_data.data(),packet.msgsize);
}

void TSDClient::readFileUploadRequest()
{
	FileUploadRequestPacket fupr;

	m_pSocket->readExact((char*)&fupr,sizeof(fupr));

	char* path_data=new char[fupr.fNameSize];
	
	m_pSocket->readExact(path_data,fupr.fNameSize);

	m_UploadFilename=QString::fromUtf8(path_data);

	m_pFileToWrite=new QFile(m_UploadFilename);

	if(m_pFileToWrite->open(QIODevice::WriteOnly))
	{
		m_bUploadStarted=true;
	}
	else
	{
		sendFileUploadCancel(m_pFileToWrite->errorString());
		delete m_pFileToWrite;
		m_pFileToWrite=NULL;
	}

	delete [] path_data;
}


void TSDClient::readFileUploadData()
{
	FileUploadDataPacket fud;

	m_pSocket->readExact((char*)&fud,sizeof(FileUploadDataPacket));

	if ((fud.realSize == 0) && (fud.compressedSize==0)) 
	{
		uint mtime=m_pSocket->readExact((char*)&mtime,sizeof(uint));

		//set time

		m_pFileToWrite->close();
		delete m_pFileToWrite;
		m_pFileToWrite=NULL;

		m_bUploadStarted=false;
		emit message("upload complete. "+m_UploadFilename);
		return;
	}

	char* buf=new char[fud.realSize];

	m_pSocket->readExact(buf,fud.realSize);

	if(m_pFileToWrite->write(buf,fud.realSize)==-1)
	{
		emit message("Error writing file data\n"+m_pFileToWrite->errorString());
		sendFileUploadCancel(m_pFileToWrite->errorString());
		closeUndoneFileTransfer();
		
	}

	delete []buf;
}

void TSDClient::readFileUploadFailed()
{
	FileUploadFailedPacket fuf;

	m_pSocket->readExact((char*)&fuf,sizeof(FileUploadFailedPacket));

	char* reason=new char[fuf.reasonLen];

	m_pSocket->readExact(reason,fuf.reasonLen);

	QString strreason=QString::fromUtf8(reason);

	Q_ASSERT(m_pFileToWrite!=NULL);
	delete m_pFileToWrite;
	emit message("upload failed. \n "+strreason);

	delete [] reason;
}

void TSDClient::readFileDownloadRequest()
{
	FileDownloadRequestPacket fdr;

	m_pSocket->readExact((char*)&fdr,sizeof(FileDownloadRequestPacket));

	char * strpath=new char[fdr.fNameSize];

	m_pSocket->readExact(strpath,fdr.fNameSize);

	QString path=QString::fromUtf8(strpath);

	m_DownloadFilename=path;

	//we assume m_DownloadFilename is not directory ,is a file and is existing.

	m_pFileToRead=new QFile(path);

	QFileInfo finfo(path);

	QDateTime t=finfo.lastModified();
	m_ModTime=t.toTime_t();
	if(m_pFileToRead->open(QIODevice::ReadOnly))
	{
		m_bDownloadStarted = true;
		sendFileDownloadPortion();
		emit message(path+" is being downloading...");
	}
	else
	{
		QString err=m_pFileToRead->errorString();

		sendFileDownloadFailed(err);

		emit message("download failed: "+path+"\n "+err);
		delete m_pFileToRead;
		m_pFileToRead=NULL;
	}
}


void TSDClient::readFileDownloadCancel()
{
	FileDownloadCancelPacket fdc;

	m_pSocket->readExact((char*)&fdc,sizeof(FileDownloadCancelPacket));

	char* reason=new char[fdc.reasonLen];

	m_pSocket->readExact(reason,fdc.reasonLen);

	QString strreason=QString::fromUtf8(reason);

	if(m_pFileToRead!=NULL)
	{
		m_pFileToRead->close();
		delete m_pFileToRead;
		m_pFileToRead=NULL;
	}

	m_bDownloadStarted=false;
	killTimer(m_DownLoadTimerId);

	emit message("download canceled. \n "+strreason);

	delete [] reason;
}

void TSDClient::sendFileDownloadPortion()
{
	m_DownLoadTimerId=startTimer(10);
}

void TSDClient::timerEvent(QTimerEvent* event)
{
	if(event->timerId()==m_DownLoadTimerId)
	{
		if (!m_bDownloadStarted) 
			return;

		char *pBuff = new char[m_BlockSize];

		qint64 NumberOfBytesRead=m_pFileToRead->read(pBuff,m_BlockSize);

		if(NumberOfBytesRead==0)
		{
			/* This is the end of the file. */

			sendFileDownloadData(m_ModTime);
			m_pFileToRead->close();
			delete m_pFileToRead;
			m_pFileToRead=NULL;
			m_bDownloadStarted=false;
			killTimer(m_DownLoadTimerId);
			return;
		}

		sendFileDownloadData(NumberOfBytesRead,pBuff);

		delete [] pBuff;
	}
}

void TSDClient::sendFileDownloadData(uint mTime)
{
	int msgLen = sizeof(FileDownloadDataPacket)+sizeof(uint);

	char *pAllFDDMessage = new char[msgLen];

	FileDownloadDataPacket *pFDD = (FileDownloadDataPacket *) pAllFDDMessage;

	unsigned int *pFollow = (uint *) &pAllFDDMessage[sizeof(FileDownloadDataPacket)];
	pFDD->type = Packet_FileDownloadData;
	pFDD->compressLevel = 0;
	pFDD->compressedSize = 0;
	pFDD->realSize =0;

	memcpy(pFollow, &mTime, sizeof(uint));
	Q_ASSERT(m_pSocket->writeExact(pAllFDDMessage, msgLen)==true);

	delete [] pAllFDDMessage;
}

void TSDClient::sendFileDownloadFailed(QString reason)
{
	QByteArray data=reason.toUtf8();

	int reasonLen=data.size()+1;

	int msgLen = sizeof(FileDownloadFailedPacket) + reasonLen;

	char *pAllFDFMessage = new char[msgLen];

	FileDownloadFailedPacket *pFDF = (FileDownloadFailedPacket *) pAllFDFMessage;

	char *pFollow = &pAllFDFMessage[sizeof(FileDownloadFailedPacket)];
	pFDF->type =Packet_FileDownloadFailed;
	pFDF->reasonLen = reasonLen;

	memcpy(pFollow, data.data(), reasonLen);
	Q_ASSERT(m_pSocket->writeExact(pAllFDFMessage, msgLen)==true);

	delete [] pAllFDFMessage;
}

void TSDClient::sendFileDownloadData(ushort filesize,char* data)
{
	int msgLen = sizeof(FileDownloadDataPacket) + filesize;

	char *pAllFDDMessage = new char[msgLen];

	FileDownloadDataPacket *pFDD = (FileDownloadDataPacket *) pAllFDDMessage;

	char *pFollow = &pAllFDDMessage[sizeof(FileDownloadDataPacket)];

	pFDD->type = Packet_FileDownloadData;
	pFDD->compressLevel = 0;
	pFDD->compressedSize = filesize;
	pFDD->realSize = filesize;

	memcpy(pFollow, data, filesize);
	Q_ASSERT(m_pSocket->writeExact(pAllFDDMessage, msgLen)==true);

	delete [] pAllFDDMessage;
}

void TSDClient::sendFileUploadCancel(QString reason)
{
	QByteArray data=reason.toUtf8();

	int reasonLen=data.size()+1;

	int msgLen = sizeof(FileUploadCancelPacket) + reasonLen;

	char *pAllFUCMessage = new char[msgLen];

	FileUploadCancelPacket *pFUC = (FileUploadCancelPacket *) pAllFUCMessage;

	char *pFollow = &pAllFUCMessage[sizeof(FileUploadCancelPacket)];
	pFUC->type =Packet_FileUploadCancel;
	pFUC->reasonLen = reasonLen;

	memcpy(pFollow, data.data(), reasonLen);
	Q_ASSERT(m_pSocket->writeExact(pAllFUCMessage, msgLen)==true);
	
	delete [] pAllFUCMessage;
}

void TSDClient::closeUndoneFileTransfer()
{
	if (m_bUploadStarted)
	{
		m_bUploadStarted =false;
		m_pFileToWrite->close();
		delete m_pFileToWrite;
		m_pFileToWrite=NULL;

		QFile::remove(m_UploadFilename);
	}

	if (m_bDownloadStarted)
	{
		m_bDownloadStarted = false;
		m_pFileToRead->close();
		delete m_pFileToRead;
		m_pFileToRead=NULL;
	}
}

void TSDClient::readFileListRequestPacket()
{
	FileListRequestPacket flr;

	m_pSocket->readExact((char*)&flr,sizeof(FileListRequestPacket));

	char* strpath=new char[flr.dirNameSize];
	m_pSocket->readExact(strpath,flr.dirNameSize);

	QString path=QString::fromUtf8(strpath);
	delete [] strpath;

	FileTransferItemInfo ftii;

	if(path=="")
	{
		QFileInfoList list=QDir::drives();

		for(int i=0;i<list.size();i++)
		{
			QFileInfo fileinfo=list.at(i);
#ifdef Q_OS_WIN		
			QString drive=fileinfo.filePath();

			drive.remove("/");
			ftii.add(drive,-2,0); //drive
#else
			ftii.add(fileinfo.filePath(),-1,0);  //folder for unix
#endif
		}
	}
	else
	{
		QDir dir(path);

		QFileInfoList list=dir.entryInfoList();

		for(int i=0;i<list.size();i++)
		{
			QFileInfo info=list[i];

			if(info.fileName()=="." || info.fileName()=="..") 
				continue;

			if(info.isDir())
				ftii.add(info.fileName(),-1,0);
			else //for file
				ftii.add(info.fileName(),info.size(),0);
		}

	}

	int dsSize = ftii.getNumEntries() * sizeof(FTSIZEDATA);

	int msgLen = sizeof(FileListDataPacket) + dsSize + ftii.getSummaryNamesLength() + ftii.getNumEntries();

	char *pAllMessage = new char [msgLen];

	FileListDataPacket *pFLD = (FileListDataPacket *) pAllMessage;

	FTSIZEDATA *pftsd = (FTSIZEDATA *) &pAllMessage[sizeof(FileListDataPacket)];
	char *pFilenames = &pAllMessage[sizeof(FileListDataPacket) + dsSize];

	pFLD->type =Packet_FileListData;
	pFLD->flags = flr.flags & 0xF0;
	pFLD->numFiles = ftii.getNumEntries();
	pFLD->dataSize = ftii.getSummaryNamesLength() + ftii.getNumEntries();
	pFLD->compressedSize = pFLD->dataSize;

	for (int i = 0; i < ftii.getNumEntries(); i++)
	{
		pftsd[i].size = ftii.getSizeAt(i);
		pftsd[i].data = ftii.getDataAt(i);

		QByteArray data=ftii.getNameAt(i).toUtf8();

		char* name=data.data();

		strcpy(pFilenames, name);
		pFilenames = pFilenames + strlen(pFilenames) + 1;
	}

	Q_ASSERT(m_pSocket->writeExact(pAllMessage,msgLen)==true);
	qDebug("send file list data");

	delete [] pAllMessage;
}


void TSDClient::readStopScreenCapture()
{
	StopScreenCapturePacket packet;

	m_pSocket->readExact(( char*) &packet,sizeof(StopScreenCapturePacket));
	
	m_pServer->stopScreenPieceDelivers();

}

void TSDClient::readStartScreenCapture()
{
	StartScreenCapturePacket packet;

	m_pSocket->readExact(( char*) &packet,sizeof(StartScreenCapturePacket));
	
	m_pServer->startScreenPieceDelivers();
	
}

void TSDClient::setServer(TSDServer* server)
{
	m_pServer=server;

	connect(server,SIGNAL(updateDisplay()),this,SLOT(sendImagePacket()));
}

void TSDClient::sendImagePacket()
{
	g_Mutex.lock();

	ImagePacket packet;

	packet.type=Packet_Image;

	QRect rect=tsdServer()->updatedDisplayRect();

	packet.left=rect.left();
	packet.top=rect.top();
	packet.height=rect.height();
	packet.width=rect.width();
	packet.size=tsdServer()->updatedDisplayBufferSize();

	Q_ASSERT(m_pSocket->writeExact((char*)&packet,sizeof(ImagePacket))==true);
	Q_ASSERT(m_pSocket->writeExact(tsdServer()->updatedDisplayBuffer(),packet.size)==true);

	g_Mutex.unlock();

	qDebug("send image packet left=%d, top=%d, height=%d, width=%d size=%d",packet.left,packet.top,packet.height,packet.width,packet.size);
}

void TSDClient::sendTessellationInfo()
{
	TSDServer* server=(TSDServer*)this->tsdServer();

	TessellationInfoPacket packet;

	packet.type=Packet_TessellationInfo;
	packet.xgridnumber=server->xGridNumber();
	packet.ygridnumber=server->yGridNumber();
	m_pSocket->writeExact((char*)&packet,sizeof(TessellationInfoPacket));

	qDebug("send tesselation info x=%d,y=%d",packet.xgridnumber,packet.ygridnumber);
}

void TSDClient::sendDisplayInfo()
{
	TSDServer* server=(TSDServer*)this->tsdServer();

	DisplayInfoPacket packet;

	packet.type=Packet_DisplayInfo;
	packet.width=server->screenWidth();
	packet.height=server->screenHeight();
	packet.depth=server->screenDepth();

	m_pSocket->writeExact((char*)&packet,sizeof(DisplayInfoPacket));

	qDebug("send displayinfo w=%d, h=%d",packet.width,packet.height);
}

void TSDClient::readRequestScreenShare()
{
	RequestScreenSharePacket packet;

	m_pSocket->readExact((char*)&packet,sizeof(RequestScreenSharePacket));

	TSDServer *server=(TSDServer*)this->tsdServer();

	if(server->screenPieceDeliverCount()!=0) 
		return ;

	server->createScreenDelivers();
}

void TSDClient::onError(QAbstractSocket::SocketError error)
{
	if (TSDSocket *connection = qobject_cast<TSDSocket *>(sender()))
	{
		QString info=connection->errorString();

		QString address=connection->peerAddress().toString();

		info=info+"\n"+"client: "+address+" will be removed";

		QAbstractSocket::SocketState state=connection->state();

		disconnect(tsdServer(),0,this,0);

		tsdServer()->removeClient(this);

		//QMessageBox::critical(0, QObject::tr("TSDServer Error"),info);
	}
}

TSDClient::~TSDClient()
{
//	if(m_pSocket) //ΩÂ∂Æ ∑√¬Ÿ  ÔÀ±
	//	delete m_pSocket;
}


