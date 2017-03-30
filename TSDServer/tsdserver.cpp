#include "tsdserver.h"
#include "tsdclient.h"
#include "tsdserverapp.h"


#include <QApplication>
#include <QDesktopWidget>
#include <QDateTime>
#include <QProcess>

#include "screenpiecedeliver.h"
#include "highlighter.h"

#define DEFAULT_PORT 12342

#define DEFAULT_MAX_CONNECTION_COUNT 100

#define DEFAULT_X_GRID_NUMBER         2
#define DEFAULT_Y_GRID_NUMBER         2

#define MAX_IMAGE_BUFFER_SIZE          1024*1024*10 //10MB

QMutex           g_Mutex;
bool             g_bStopScreenCapture=false;
QWaitCondition   g_StopScreenCaptureCond;

TSDListner::TSDListner(QObject* parent, quint16 port)
:QTcpServer(parent)
{
	m_Port=port;
	m_State=Stopped;
}

bool TSDListner::start()
{
	if(m_State!=Stopped)
		return false;

	if(!listen(QHostAddress::Any,m_Port))
	{
		return false;
	}

	m_State=Started;
	return true;
}


bool TSDListner::pause()
{
	if (m_State != Started)
	{
		return false;
	}

	m_State = Paused;

	close();

	return true;
}

void TSDListner::incomingConnection(int socketDescriptor)
{
	TSDServer* server=(TSDServer*)this->parent();

	server->addClient(socketDescriptor);
}

TSDServer::TSDServer(QObject *parent)
	: QObject(parent)
{
   m_pListner=new TSDListner(this,DEFAULT_PORT);
   m_MaxConnectionCount=DEFAULT_MAX_CONNECTION_COUNT;


   m_ScreenWidth=QApplication::desktop()->screenGeometry().width();
   m_ScreenHeight=QApplication::desktop()->screenGeometry().height();

   m_ScreenDepth=32;

   m_XGridNumber=DEFAULT_X_GRID_NUMBER;
   m_YGridNumber=DEFAULT_Y_GRID_NUMBER;

   
   m_pUpdatedDisplayBuffer=new char[MAX_IMAGE_BUFFER_SIZE];
   m_pHighLighter=new HighLighter(NULL);
   
#ifdef Q_OS_WIN
   // Get the scalars for converting mouse coordinates
   m_dxWidthScalar = 65535.0 / (double)m_ScreenWidth;
   m_dyHeightScalar = 65535.0 / (double)m_ScreenHeight;
#endif


   QStringList envVariables;
   envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
	   << "HOSTNAME.*" << "DOMAINNAME.*";

   QStringList environment = QProcess::systemEnvironment();
   foreach (QString string, envVariables) {
	   int index = environment.indexOf(QRegExp(string));
	   if (index != -1) {
		   QStringList stringList = environment.at(index).split('=');
		   if (stringList.size() == 2) {
			   m_UserName = stringList.at(1).toUtf8();
			   break;
		   }
	   }
   }

   if (m_UserName.isEmpty())
		m_UserName="Unknown";
}


TSDServer::~TSDServer()
{
   if(m_pListner)
	   delete m_pListner;

   delete [] m_pUpdatedDisplayBuffer;
   delete m_pHighLighter;
}

void TSDServer::addClient(int socketdescripter)
{
	if(m_ClientList.size()>=m_MaxConnectionCount)
		m_pListner->pause();
	else
	{
	   	TSDClient *client=new TSDClient(NULL,socketdescripter);

		connect(client,SIGNAL(startSessionRefuse(TSDClient*)),this,SLOT(onStartSessionRefuse(TSDClient*)));
		connect(client,SIGNAL(disconnected(TSDClient*)),this,SLOT(onDisconnected(TSDClient*)));
		connect(client,SIGNAL(message(QString)),g_app,SLOT(onMessage(QString)));

		client->setServer(this);
		m_ClientList.push_back(client);

	}
}

void TSDServer::onStartSessionRefuse(TSDClient * c)
{
	for(int i=0;i<m_ClientList.size();i++)
	{
		if(m_ClientList[i]==c)
		{
			m_ClientList.removeOne(c);
			delete c;
			break;
		}
	}
}

void TSDServer::onDisconnected(TSDClient * c)
{
	stopScreenPieceDelivers();
	for(int i=0;i<m_ClientList.size();i++)
	{
		if(m_ClientList[i]==c)
		{
			m_ClientList.removeOne(c);
			delete c;
			break;
		}
	}
}


void TSDServer::startScreenPieceDelivers()
{
	g_Mutex.lock();
	g_bStopScreenCapture=false;
	g_StopScreenCaptureCond.wakeAll();
	g_Mutex.unlock();
}

void TSDServer::stopScreenPieceDelivers()
{
	g_Mutex.lock();
	g_bStopScreenCapture=true;
	g_Mutex.unlock();
}

void TSDServer::removeClient(TSDClient *removeclient)
{
	g_Mutex.lock();
	TSDClient* client;
	foreach(client,m_ClientList)
	{
		if(client==removeclient)
		{
			m_ClientList.removeOne(removeclient);
			delete removeclient;
			qDebug("One client removed.");
		}
	}
	g_Mutex.unlock();
}

void TSDServer::createScreenDelivers()
{
	// Create the rectangular comparison region
	QPoint TopLeft(0,0);
	QPoint BottomRight(m_ScreenWidth / m_XGridNumber -1,m_ScreenHeight / m_YGridNumber -1);

	QRect ScreenRect = QRect(TopLeft,BottomRight);

	// Build the update areas per thread
	bool bThreads = true;

	for (int x = 0;x < m_YGridNumber;x++)
	{
		for (int y = 0;y < m_YGridNumber;y++)
		{
			// There could be an odd number of regions in the last thread
			if (bThreads)
			{
				// Debugging
				TopLeft = ScreenRect.topLeft();
				BottomRight = ScreenRect.bottomRight();

				qDebug("Thread=%d Rect=(%d,%d)-(%d,%d)\n",x,TopLeft.x(),TopLeft.y(),BottomRight.x(),BottomRight.y());

				ScreenPieceDeliver* pThread=new ScreenPieceDeliver(NULL,this,ScreenRect);

				m_ScreenPieceDeliverList.push_back(pThread);

				pThread->start(/*QThread::TimeCriticalPriority*/);

				// Update the rectangle
				ScreenRect.translate(ScreenRect.width(),0);

				// Boundary test
				if (ScreenRect.left() == (m_ScreenWidth))
				{
					// Reset the boundary
					ScreenRect.setLeft(0)  ;
					ScreenRect.setRight(m_ScreenWidth / m_XGridNumber-1 )  ;

					// Update the rectangle
					ScreenRect.translate(0,ScreenRect.height());

					// Boundary test
					if (ScreenRect.top() == (m_ScreenHeight))
						bThreads = false;
				}
			}
		}
	}
}

void TSDServer::finishScreenDelivers()
{
	g_Mutex.lock();

	if(g_bStopScreenCapture)
	{
		g_bStopScreenCapture=false;
		g_StopScreenCaptureCond.wakeAll();
	}
	g_Mutex.unlock();

	ScreenPieceDeliver* deliver;

	foreach(deliver,m_ScreenPieceDeliverList)
	{
		deliver->finish();
	}

	m_ScreenPieceDeliverList.clear();
}

void TSDServer::updateDisplayInfo(char *buf, int size, QRect rect)
{
	g_Mutex.lock();

	memcpy(m_pUpdatedDisplayBuffer,buf,size);
	m_UpdatedDisplayRect=rect;
	m_UpdatedDisplayBufferSize=size;

	qDebug("updateDisplay signal is emitted in thread %d",QThread::currentThread());
	emit updateDisplay();

	g_Mutex.unlock();
}

bool TSDServer::start()
{
	return m_pListner->start();
}

void TSDServer::sendChatMessage(QString msg)
{
	QDateTime dt=QDateTime::currentDateTime();

	uint t =dt.toTime_t();

	for(int i=0;i<m_ClientList.size();i++)
	{
		m_ClientList[i]->sendChat(m_UserName,msg,t);
	}
}

void TSDServer::removeAllClient()
{
	for(int i=0;i<m_ClientList.size();i++)
	{
		//m_ClientList[i]->socket()->disconnectFromHost();	  //ΩÂ∂Æ∑√¬Ÿ  ÔÀ±

		delete m_ClientList[i];
	}

	m_ClientList.clear();
}