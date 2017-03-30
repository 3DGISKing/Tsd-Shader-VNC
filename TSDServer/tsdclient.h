#ifndef TSDCLIENT_H
#define TSDCLIENT_H

#include <QRect>
#include "../Common/tsd/tsdsocket.h"

class TSDServer;
class QFile;

class TSDClient : public QObject
{
	Q_OBJECT

public:
	TSDClient(QObject *parent,int socketdescripter);
	~TSDClient();

	void sendDisplayInfo();
	void sendTessellationInfo();
	void sendFileDownloadFailed(QString reason);
	void sendFileDownloadPortion();
	void sendFileDownloadData(uint mTime);
	void sendFileDownloadData(ushort filesize,char* data);
	void sendFileUploadCancel(QString reason);

	void readRequestScreenShare();
	void readMouseEvent();
	void readMouseWheelEvent();
	void readKeyEvent();
	void readStopScreenCapture();
	void readStartScreenCapture();
	void readFileListRequestPacket();
	void readFileDownloadRequest();
	void readFileDownloadCancel();
	void readFileUploadRequest();
	void readFileUploadData();
	void readFileUploadFailed();

	void readHighLighterStart();
	void readHighLighterEnd();
	void readHighLighterData();

	void readChat();
	void sendChat(QString username,QString msg,uint t);

	void readReboot();

	void readStartSessionRequest();
	void sendStartSessionAgree();
	void sendStartSessionRefuse();

	void readStopSessionRequest();

	void closeUndoneFileTransfer();

	void  setServer(TSDServer* server);
	inline TSDServer* tsdServer(){return m_pServer;}
	
	inline TSDSocket* socket(){return m_pSocket;}
private slots:
	void sendImagePacket();
	void onError(QAbstractSocket::SocketError error);
	void onReceived();
	void onDisconnected();
protected:
	virtual void timerEvent(QTimerEvent* event);

private:
	TSDSocket*	 m_pSocket;
	int			 m_SocketDescriper;
	TSDServer*	 m_pServer;

	QString       m_DownloadFilename;
	QString       m_UploadFilename;

	QFile*		 m_pFileToRead;
	QFile*		 m_pFileToWrite;

	bool		 m_bDownloadStarted;
	bool		 m_bUploadStarted;

	uint		 m_ModTime;

	int          m_DownLoadTimerId;
	int          m_BlockSize;
signals:
	void         startSessionRefuse(TSDClient* c);
	void         disconnected(TSDClient* c);
	void         message(QString);

};

#endif // TSDCLIENT_H
