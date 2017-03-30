#include "tsdclient.h"
#include "config.h"
#include "msgdataclient.h"
#include "guiuty.h"

#include "mainwindow.h"
#include "leftframe.h"
#include "renderframe.h"

#include "tsdclientconnection.h"
#include "filemanager.h"

#ifdef Q_WS_MAC
#define APP_TRANS_FNAME		"/res/qss/tsd_mac.qm"
#else
#define APP_TRANS_FNAME		"/res/qss/tsd.qm"
#endif

#define RELEASE_DATA(r) \
{\
	if (r)\
{\
	r->Release();\
	delete r;\
	r = NULL;\
}\
}

#define DEFAULT_PORT         12342

TSDClient::TSDClient(int argc, char **argv) :
						QtSingleApplication(argc, argv)
{
	m_mainWnd = NULL;

	//set organization & application name

	setOrganizationName(QString(CONF_SET_ORG));
	setApplicationName(QString(CONF_SET_APP));

	// get working directory
	m_workDir = applicationDirPath();
	m_CurrentConnection=NULL;
	m_WebServerIp="127.0.0.1";
	
	m_pFileManager=new FileManager();
	connect(m_pFileManager,SIGNAL(fileDownloadFailed(QString)),this,SLOT(OnFileDownloadFailed(QString)));
	connect(m_pFileManager,SIGNAL(message(QString)),this,SLOT(OnFileManagerMessage(QString)));

}

TSDClient::~TSDClient()
{

}

bool TSDClient::Init()
{
	QString  pluginDir = m_workDir+ "/plugin" ;
	addLibraryPath(pluginDir);

	if(InitConfig() && InitComm()) 
		return true;
	else
		return false;
}

bool TSDClient::InitConfig()
{
	m_config = new Config();
	if (m_config)
	{
		m_config->Init();
		m_config->ReadSettings();

		QString msgPath = m_workDir + APP_TRANS_FNAME;	
		if (m_translator.load(msgPath))
			this->installTranslator(&m_translator);
		return true;
	}
	return false;
}

bool TSDClient::InitComm() 
{
	m_serverComm = new CommunicationLine(this);
	if (m_serverComm)
		return m_serverComm->Init();
	else
		return false;
}

void TSDClient::Release()
{
	if (m_config)
		m_config->WriteSettings();
	RELEASE_DATA(m_config);
	RELEASE_DATA(m_serverComm);
}

int TSDClient::Run()
{
	return exec();
}

QString TSDClient::GetResource(QString &fileName)
{
	QString path = m_workDir + QString("/res") + fileName;
	return path;
}

QString TSDClient::Trans(const char *s, const char *c)
{
	QString res = m_translator.translate(NULL, s, c);
	if (res.isEmpty() && s!=NULL)
		res = s;
	return res;

}

QString TSDClient::Trans(const QString &s)
{
	QByteArray byteArr = s.toAscii();
	QString res = m_translator.translate(NULL, byteArr.data(), NULL);
	if (res.isEmpty() && s.length() > 0)
		res = s;
	return res;
}

bool TSDClient::CreateConnection()
{
	m_CurrentConnection=new TSDClientConnection(NULL,DEFAULT_PORT);
	if(m_CurrentConnection)
	{
		connect(m_CurrentConnection,SIGNAL(connected()),this,SLOT(OnConnected()));
		connect(m_CurrentConnection,SIGNAL(disconnected()),this,SLOT(OnDisconnected()));
		connect(m_CurrentConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onError(QAbstractSocket::SocketError)));

		connect(m_CurrentConnection,SIGNAL(startConnect()),this,SLOT(OnStartConnect()));
		connect(m_CurrentConnection,SIGNAL(startSessionAgree()),this,SLOT(OnStartSessionAgree()));
		return true;
	}

	return false;
}

void TSDClient::DeleteCurrentConnection()
{
	if(m_CurrentConnection)
		delete m_CurrentConnection;

	m_CurrentConnection=NULL;
}

void TSDClient::OnStartConnect()
{
	m_mainWnd->setEnabled(false);
	m_mainWnd->setCursor(Qt::WaitCursor);
	m_mainWnd->GetRenderFrame()->RenderString("Connecting...");
}

void TSDClient::OnConnected()
{
   TSDClientConnection* c=qobject_cast<TSDClientConnection*>(sender());
   c->sendStartSessionRequest()	;
   m_mainWnd->GetRenderFrame()->RenderString("");

}

void TSDClient::OnDisconnected()
{
	m_mainWnd->setEnabled(true);
	m_mainWnd->setCursor(Qt::ArrowCursor);
	m_mainWnd->GetLeftFrame()->UpdateInterface(false);

	TSDClientConnection *c = qobject_cast<TSDClientConnection *>(sender());

	Q_ASSERT(m_CurrentConnection==c);
	DeleteCurrentConnection();

	gu_MessageBox(NULL,"TSDClient","Server close the session!",MSGBOX_IDOK|MSGBOX_ICONINFO);
}

void TSDClient::OnStartSessionAgree()
{
	m_mainWnd->setEnabled(true);
	m_mainWnd->setCursor(Qt::ArrowCursor);
	m_mainWnd->GetLeftFrame()->UpdateInterface(true);
	m_pFileManager->setConnetion(m_CurrentConnection);
	m_pFileManager->sendFileListRequest("",0);
}

void TSDClient::onError(QAbstractSocket::SocketError error)
{
	m_mainWnd->setEnabled(true);
	m_mainWnd->setCursor(Qt::ArrowCursor);
	m_mainWnd->GetRenderFrame()->DeleteImageTiles();	
	m_mainWnd->GetLeftFrame()->UpdateInterface(false);
	m_mainWnd->GetRenderFrame()->show();
	m_mainWnd->GetFileManagerWnd()->hide();
	 m_mainWnd->GetRenderFrame()->RenderString("");

	QString info;

	TSDClientConnection *connection = qobject_cast<TSDClientConnection *>(sender());

	if(error==QAbstractSocket::RemoteHostClosedError)
	{
		info="Session stopped or deleted!";
		gu_MessageBox(NULL, tr("TSDClient"), info, MSGBOX_ICONINFO);
	}
	else
	{
		info=connection->socket()->errorString();

		gu_MessageBox(NULL, tr("TSDClient Error"), info, MSGBOX_ICONCRITICAL);
	}
	
	DeleteCurrentConnection();
}

void TSDClient::OnFileDownloadFailed(QString reason)
{
	 gu_MessageBox(NULL, tr("TSDClient"), "download failed. \n"+reason, MSGBOX_ICONINFO);
}

void TSDClient::OnFileManagerMessage(QString msg)
{
	gu_MessageBox(NULL, tr("TSDClient"),msg, MSGBOX_ICONINFO);
}


