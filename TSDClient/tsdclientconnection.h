#ifndef TSDCLIENTCONNECTION_H
#define TSDCLIENTCONNECTION_H

#include "../Common/tsd/tsdsocket.h"
	
class TSDClientConnection : public QObject
{
	Q_OBJECT

public:
	TSDClientConnection(QObject *parent,quint16 port);
	~TSDClientConnection();

	bool        writeExact( char*buf,int size);
	bool        readExact(char*buf,int size);
	
	void        connectToPeer();

	inline void setPeerIp(QString ip){m_PeerIp=ip;}

	void        sendEvent(QEvent* event);

	void        sendStopScreenCapture();
	void        sendStartScreenCapture();

	void        sendHighLighterStart();
	void        sendHighLighterEnd();
	void        sendHighLighterData(QVector<QPoint>);

	void        sendChat(QString username,QString msg,uint t);
	void        readChat();

	void        sendReboot();

	void        sendStartSessionRequest();
	void        readStartSessionAgree();
	void        readStartSessionRefuse();

	void        sendStopSession();

	inline TSDSocket* socket(){ return m_pSocket;}

private slots:
	void 		onReceived();
	void 		onConnected();
	void 		onDisconnected();
	void        onError(QAbstractSocket::SocketError err);

private:
	void 		sendRequestScreenShare();

	void 		readDisplayInfo();
	void 		readTessellationInfo();
	void 		readImagePacket();

	TSDSocket* m_pSocket; 

	quint16    m_port;

	int        m_ScreenWidth, m_ScreenHeight, m_ScreenDepth;
	int        m_XGridNumber, m_YGridNumber;
	QString    m_PeerIp;

signals:
	void       connected();
	void       disconnected();
	void       error(QAbstractSocket::SocketError);

	void       startConnect();
	void	   startSessionAgree();


};

#endif // TSDCLIENTCONNECTION_H
