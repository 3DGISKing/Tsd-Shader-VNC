/********************************************************************
	created:	2013/05/23
	created:	23:5:2013   19:34
	filename: 	filemanager.h
	file path:	\
	file base:	filemanager
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <qglobal.h>
#include <QtCore/QString>
#include <QtNetwork/QTcpSocket>

#include "filemanagerwindow.h"
#include "../Common/tsd/filetransferiteminfo.h"
#include "../Common/tsd/tsdprotocol.h"

class QFile;
class TSDClientConnection;

class FileManager :public QObject
{
	Q_OBJECT

public:
	FileManager();
	~FileManager();

	inline void		setClientPath(QString path){m_ClientPath=path;}
	
	inline void		setClientFileName(QString name){m_ClientFileName=name;}
	inline QString  getClientFileName(){return m_ClientFileName;}

	inline void		setPeerFileName(QString name){m_PeerFileName=name;};
	inline QString  getPeerFileName(){return m_PeerFileName;}

	void            download();
	void            upload();
	void            cancel();
	void            fileTransferDownload();

	inline void setConnetion(TSDClientConnection* c){m_pConnection=c;}
	void setFileManagerWnd(FileManagerWindow* w);

	void sendFileListRequest(QString filename ,unsigned char flag);

	void readFileListData();
	void readFileDownloadFailed();
	void readFileUploadCancel();
private slots:
	void onRemoteContentViewDblClick(QTreeWidgetItem * item, int column);
	void onRemoteDirViewExpanded(QTreeWidgetItem * item);
	void onRemoteDirViewItemClicked(QTreeWidgetItem * item, int column);

private:
	void sendFileDownloadRequest();
	void sendFileDownloadCancel(QString reason);
	void sendFileUploadData(uint mtime);
	void sendFileUploadData(unsigned short size, char *pFile);
	void createPeerItemInfoList(FileTransferItemInfo *pftii, FTSIZEDATA *ftsd, int ftsdNum, char *pfnames, int fnamesSize);
	
	void fileTransferUpload();
	void cancelDownLoad(QString reason);
	void closeUndoneFileTransfers();
	
	FileManagerWindow      *m_pFileManagerWnd;

	quint16					m_Port;
	QString					m_PeerAddress;

	FileTransferItemInfo    m_FTPeerItemInfo;

	QTreeWidgetItem*        m_pFocusItem;
	bool                    m_bBrowserPeerItem;

	QString                 m_ClientFileName;  //downloaded file name into client 
	QString                 m_ClientPath;

	QString                 m_PeerPathTmp;
	QString                 m_PeerPath;
	QString                 m_PeerFileName;    //filename of file as source for downloading. 

	int                     m_SizeDownloadFile;

	bool                    m_bFirstFileDownloadMsg;
	bool                    m_bDownloadStarted;
	int                     m_DownloadBlockSize;
	QString                 m_DownloadFilename;

	QFile*                  m_pFiletoWrite;
	QFile*                  m_pFiletoRead;
	int                     m_DownloadRead;
	int                     m_numOfFilesToDownload;
	int                     m_currentDownloadIndex;
	bool                  	m_bTransferEnable;
	QList<QTreeWidgetItem *> m_SelectedPeerItems; 
	bool                    m_bOpendFileToWrite;

	bool                    m_bReportUploadCancel;
	QString                 m_UploadFilename;
	bool                    m_bUploadStarted;
	TSDClientConnection*    m_pConnection;
signals:
	void       fileDownloadFailed(QString);
	void       message(QString);
};



extern FileManager*         g_FileManager;  
#endif