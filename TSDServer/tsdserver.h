#ifndef TSDSERVER_H
#define TSDSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QList>
#include <QRect>  
#include <QMutex>
#include <QWaitCondition>

class TSDClient;
class ScreenPieceDeliver;
class HighLighter;


class TSDListner: public QTcpServer
{
	 Q_OBJECT
public:
	enum State
	{
		Stopped,
		Paused,
		Started
	};

	TSDListner(QObject* parent, quint16 port);

	bool start();
	bool pause();
protected:
	void incomingConnection(int socketDescriptor);

private:
	quint16 m_Port;
	State   m_State;

};


class TSDServer : public QObject
{
	Q_OBJECT

public:
	TSDServer(QObject *parent);
	~TSDServer();

	void addClient(int socketdescripter);
	bool start();

	inline int 	 screenWidth() {return m_ScreenWidth;}
	inline int 	 screenHeight(){return m_ScreenHeight;}
	inline int 	 screenDepth(){return m_ScreenDepth;}
	inline int 	 xGridNumber(){return m_XGridNumber;}
	inline int 	 yGridNumber(){return m_YGridNumber;}

	inline int   updatedDisplayBufferSize(){return m_UpdatedDisplayBufferSize;}
	inline char* updatedDisplayBuffer(){return m_pUpdatedDisplayBuffer;}
	inline QRect updatedDisplayRect() {return m_UpdatedDisplayRect;}

	void  		 createScreenDelivers();
	void  		 updateDisplayInfo(char* buf,int size,QRect rect);

	inline int   clientCount(){return m_ClientList.size();};
	void         removeClient(TSDClient *client);

	inline int   screenPieceDeliverCount(){return m_ScreenPieceDeliverList.size();}
	void         finishScreenDelivers();

	inline       HighLighter* getHighLighter() {return m_pHighLighter;}
	void         sendChatMessage(QString msg);

	void         removeAllClient();
	inline QString userName(){return m_UserName;}

	void         stopScreenPieceDelivers();
	void         startScreenPieceDelivers();


#ifdef Q_OS_WIN
	double       m_dxWidthScalar,m_dyHeightScalar;         //for Windows SendInput API function using
#endif

private:
	 TSDListner*        m_pListner;

	 QList<TSDClient*>	m_ClientList;

	 int	         	m_MaxConnectionCount;

	 int                m_ScreenWidth,m_ScreenHeight;    // this data members define remote controled rectangle 
	 int                m_ScreenDepth;
	 int                m_XGridNumber,m_YGridNumber;              //full screen is captured in m_XGridNumber*m_YGridNumber grids
	 QList<ScreenPieceDeliver*>   m_ScreenPieceDeliverList;                 //list of thread for screen piece capture
     
	 QMutex             m_Mutex;

	 QRect              m_UpdatedDisplayRect;
	 char*              m_pUpdatedDisplayBuffer;
	 int                m_UpdatedDisplayBufferSize;

	 HighLighter*       m_pHighLighter;    
	 QString            m_UserName;

private slots:
	 void onStartSessionRefuse(TSDClient *c);     
	 void onDisconnected(TSDClient *c); 
signals:
	 void               updateDisplay();
};

extern QMutex           g_Mutex;
extern bool             g_bStopScreenCapture;
extern QWaitCondition   g_StopScreenCaptureCond;

#endif // TSDSERVER_H
