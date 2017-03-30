#include "tsdclientconnection.h"

#include "../Common/tsd/tsdprotocol.h"
#include "guiuty.h"
#include "tsdclient.h"
#include "mainwindow.h"
#include "renderframe.h"
#include "filemanager.h"
#include "leftframe.h"

#include <QEvent>
#include <QImage>

QImage g_ReceivedScreenPiece;

TSDClientConnection::TSDClientConnection(QObject *parent,quint16 port)
	: QObject(parent)
{
	m_pSocket=new TSDSocket();
	m_port=port;

	connect(m_pSocket,SIGNAL(readyRead()),this,SLOT(onReceived()));
	connect(m_pSocket,SIGNAL(connected()),this,SLOT(onConnected()));
	connect(m_pSocket,SIGNAL(disconnected()),this,SLOT(onDisconnected()));
	connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onError(QAbstractSocket::SocketError)));
}

TSDClientConnection::~TSDClientConnection()
{
	//½å¶®·ÃÂÙ ÊïË±
	//delete m_pSocket;
}



void TSDClientConnection::sendStopScreenCapture()
{
	StopScreenCapturePacket packet;

	packet.type=Packet_StopScreenCapture;
	m_pSocket->writeExact((char*)&packet,sizeof(StopScreenCapturePacket));
	qDebug("send stop screen capture packet");
}

void TSDClientConnection::sendStartScreenCapture()
{
	StartScreenCapturePacket packet;

	packet.type=Packet_StartScreenCapture;
	m_pSocket->writeExact((char*)&packet,sizeof(StartScreenCapturePacket));
	qDebug("send start screen capture packet");
}

void TSDClientConnection::sendReboot()
{
	RebootPacket packet;

	packet.type=Packet_Reboot;
	m_pSocket->writeExact((char*)&packet,sizeof(RebootPacket));
	qDebug("send reboot packet");
}

void TSDClientConnection::sendStartSessionRequest()
{
	StartSessionRequestPacket packet;

	packet.type=Packet_StartSessionRequest;

	m_pSocket->writeExact((char*)&packet,sizeof(StartSessionRequestPacket));
	qDebug("send start session request");
}

void TSDClientConnection::sendStopSession()
{
	StopSessionRequestPacket packet;

	packet.type=Packet_StopSessionRequest;
	
	m_pSocket->writeExact((char*)&packet,sizeof(StopSessionRequestPacket));
	qDebug("send stop session request");
}



void TSDClientConnection::sendChat(QString username,QString msg,uint t)
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

void TSDClientConnection::readChat()
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

	g_clientapp->GetMainWindow()->GetLeftFrame()->OnNewChatMessage(username,msg,strdt);
}


void TSDClientConnection::readStartSessionAgree()
{
	StartSessionAgreePacket packet;

	m_pSocket->readExact((char*)&packet,sizeof(StartSessionAgreePacket));
	emit startSessionAgree();
}

void TSDClientConnection::readStartSessionRefuse()
{
	StartSessionRefusePacket packet;

	m_pSocket->readExact((char*)&packet,sizeof(StartSessionRefusePacket));
	m_pSocket->disconnectFromHost();
}

void TSDClientConnection::sendHighLighterStart()
{
	HighLighterStartPacket hls;

	hls.type=Packet_HighLighterStart;

	m_pSocket->writeExact((char*)&hls,sizeof(HighLighterStartPacket));
}

void TSDClientConnection::sendHighLighterEnd()
{
	HighLighterEndPacket hle;

	hle.type=Packet_HighLighterEnd;

	m_pSocket->writeExact((char*)&hle,sizeof(HighLighterEndPacket));
}

void TSDClientConnection::sendHighLighterData(QVector<QPoint> pointvec)
{
	HighLighterDataPacket hld;

	hld.type=Packet_HighLighterData;
	hld.pointnum=pointvec.size();

	int* data=new int[hld.pointnum*2];
	

	int x,y;

	for(int i=0;i<hld.pointnum;i++)
	{
		x=pointvec[i].x();
		y=pointvec[i].y();

		if(!g_clientapp->GetMainWindow()->GetRenderFrame()->TransformCoord(x,y)) 
			continue;

		data[2*i]=x;
		data[2*i+1]=y;
	}

	m_pSocket->writeExact((char*)&hld,sizeof(HighLighterDataPacket));
	m_pSocket->writeExact((char*)data,sizeof(int)*2*hld.pointnum);

	delete [] data;
}

bool TSDClientConnection::readExact(char*buf,int size)
{
	return m_pSocket->readExact(buf,size);
}

bool TSDClientConnection::writeExact( char*buf,int size)
{
	return m_pSocket->writeExact(buf,size);
}


void TSDClientConnection::sendEvent(QEvent *event)
{
	QEvent::Type type=event->type();

	if(type==QEvent::MouseButtonPress || 
		type==QEvent::MouseButtonDblClick ||	
		type==QEvent::MouseButtonRelease ||
		type==QEvent::MouseMove)
	{
		QMouseEvent* mouseevent=(QMouseEvent*)event;

		MouseEventPacket packet;

		packet.type=Packet_MouseEvent;
		packet.eventtype=mouseevent->type();
		packet.keyboardmodifiers=mouseevent->modifiers();
		packet.button=mouseevent->button();
		packet.buttons=mouseevent->buttons();
		packet.globalx=mouseevent->globalX();
		packet.globaly=mouseevent->globalY();
		packet.x=mouseevent->x();
		packet.y=mouseevent->y();

		if(!g_clientapp->GetMainWindow()->GetRenderFrame()->TransformCoord(packet.x,packet.y)) return;
		m_pSocket->writeExact((char*)&packet,sizeof(packet));
	}
	else if(type==QEvent::Wheel)
	{
		QWheelEvent * wheelevent=(QWheelEvent*)event;

		MouseWheelEventPacket packet;
		
		packet.type=Packet_MouseWheelEvent;
		packet.keyboardmodifiers=wheelevent->modifiers();
		packet.buttons=wheelevent->buttons();
		packet.globalx=wheelevent->globalX();
		packet.globaly=wheelevent->globalY();
		packet.x=wheelevent->x();
		packet.y=wheelevent->y();
		packet.delta=wheelevent->delta();

		if(!g_clientapp->GetMainWindow()->GetRenderFrame()->TransformCoord(packet.x,packet.y)) return;

		m_pSocket->writeExact((char*)&packet,sizeof(packet));
	}
	else if(type==QEvent::KeyPress || type==QEvent::KeyRelease)
	{
		QKeyEvent* keyevent=(QKeyEvent*)event;

		KeyEventPacket packet;

		if(type==QEvent::KeyPress)
			packet.type=Packet_KeyPressEvent;
		else
			packet.type=Packet_KeyReleaseEvent;

		packet.keyboardmodifiers=keyevent->modifiers();
		packet.nativemodifiers=keyevent->nativeModifiers();
		packet.nativescancode=keyevent->nativeScanCode();
		packet.nativevirtualkey=keyevent->nativeVirtualKey();
		packet.key=keyevent->key();
		packet.count=keyevent->count();

		m_pSocket->writeExact((char*)&packet,sizeof(packet));
		qDebug("send KeyEvent");
	}
}

void TSDClientConnection::readTessellationInfo()
{
	TessellationInfoPacket packet;

	m_pSocket->readExact((char*)&packet,sizeof(packet));

	m_XGridNumber=packet.xgridnumber;
	m_YGridNumber=packet.ygridnumber;

	qDebug("received tessellation info x=%d, y=%d",packet.xgridnumber,packet.ygridnumber);

	g_clientapp->GetMainWindow()->GetRenderFrame()->CreateImageTiles(packet.xgridnumber,packet.ygridnumber);
}

void TSDClientConnection::readDisplayInfo()
{
	DisplayInfoPacket packet;

	m_pSocket->readExact((char*)&packet,sizeof(packet));

	m_ScreenWidth=packet.width;
	m_ScreenHeight=packet.height;
	m_ScreenDepth=packet.depth;

	//qDebug("received displayinfo w=%d,h=%d",packet.width,packet.height);
	g_clientapp->GetMainWindow()->GetRenderFrame()->SetControledDisplaySize(packet.width,packet.height);
}

void TSDClientConnection::sendRequestScreenShare()
{
	RequestScreenSharePacket packet;

	packet.type=Packet_RequestScreenShare;

	m_pSocket->writeExact((char*)&packet,sizeof(packet));

	qDebug("send request screen share ");
}

void TSDClientConnection::readImagePacket()
{
   ImagePacket packet;

   m_pSocket->readExact((char*)&packet,sizeof(packet));

   char* imagedata=new char[packet.size];

   Q_ASSERT(m_pSocket->readExact(imagedata,packet.size)==true);

   qDebug("receive image packet left=%d, top=%d width=%d, height=%d size=%d",packet.left,packet.top,packet.width,packet.height,packet.size);

   if(g_ReceivedScreenPiece.loadFromData((const uchar*)imagedata,packet.size,"jpg"))
   {

	   g_clientapp->GetMainWindow()->GetRenderFrame()->UpdateFrame(&g_ReceivedScreenPiece,QRect(packet.left,
																								   packet.top,
																								   packet.width,
																								   packet.height));

   }
   else
	   qDebug("failed to parsing image packet size=%d .Make sure that Qt image plugin drivers exists  in correct path.",packet.size);

  delete [] imagedata;
}




void TSDClientConnection::onReceived()
{
start:

	int packet_type;
	if(!m_pSocket->peekInt(packet_type))
	{
		qDebug("onReceived return");
		return;
	}

	switch(packet_type)
	{
	case Packet_DisplayInfo:
		{
			readDisplayInfo();
		}
		break;
	case Packet_TessellationInfo:
		{
			readTessellationInfo();
			sendRequestScreenShare();
		}
		break;
	case Packet_Image:
		{
			readImagePacket();
		}
		break;
	case Packet_FileListData:
		{
			g_clientapp->GetFileManager()->readFileListData();
		}
		break;
	case Packet_FileDownloadFailed:
		{
			g_clientapp->GetFileManager()->readFileDownloadFailed();		
		}
		break;
	case Packet_FileUploadCancel:
		{
			g_clientapp->GetFileManager()->readFileUploadCancel();
		}
		break;
	case Packet_FileDownloadData:
		{
			g_clientapp->GetFileManager()->fileTransferDownload();
			break;
		}
	case Packet_Chat:
		{
			g_clientapp->GetCurrentConnection()->readChat();
		}
		break;
	case Packet_StartSessionAgree:
		{
			g_clientapp->GetCurrentConnection()->readStartSessionAgree();
		}
		break;
	case Packet_StartSessionRefuse:
		{
			g_clientapp->GetCurrentConnection()->readStartSessionRefuse();
			return;
		}
		break;
	default:
		qDebug("Error unknown packet type=%d",packet_type);

	}

	if(m_pSocket->bytesAvailable()>0)
		goto start;
}


void TSDClientConnection::onConnected()
{
	emit connected();
}

void TSDClientConnection::onDisconnected()
{
	emit disconnected();
}

void TSDClientConnection::onError(QAbstractSocket::SocketError err)
{
	emit error(err);
}

void TSDClientConnection::connectToPeer()
{
	m_pSocket->connectToHost(m_PeerIp,m_port);
	emit startConnect();
}

