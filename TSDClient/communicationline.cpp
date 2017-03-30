#include "tsdclient.h"
#include "msgdataclient.h"
#include "guiuty.h"
#include "mainwindow.h"
#include "communicationline.h"

#include <QtNetwork/QHttp>
#include <QtCore/QUrl>
#include <QtGui/QMessageBox>

CommunicationLine::CommunicationLine(QObject *parent)
	: QObject(parent)
{
	m_httpGetId = 0;
	m_thisUser = "";
}

CommunicationLine::~CommunicationLine()
{

}

void CommunicationLine::SetHost()
{
	m_http->setHost(g_clientapp->GetWebServerIp());
	sendRequest(SRV_CONNECT);
}

bool CommunicationLine::Init()
{
	m_http = new QHttp(this);

	connect(m_http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpRequestFinished(int, bool)));

	m_loginTimer = new QTimer();
	m_sessionTimer = new QTimer();

	return true;
}

void CommunicationLine::Release()
{
}

void CommunicationLine::sendRequest(int key, QString val, QString page)
{
	QByteArray path = CONF_WEBSERVER_PATH;
	path = path.append(page);
	path = path.append("?key=" + QString("%1").arg(key));

	if(!val.isEmpty())
		path = path.append("&val=" + val);

	path = QUrl::toPercentEncoding(path, "?&!$'()*+,;=:@/");
	m_httpGetId = m_http->get(path);
}


void CommunicationLine::httpRequestFinished(int requestId, bool error)
{
	if (requestId != m_httpGetId)
		return;

	if (error) 
	{
		m_http->abort();
		g_clientapp->GetMainWindow()->ResponseProcess(QStringList(QString("%1").arg(SRV_ERROR)));
		return;
	}

	QByteArray data = m_http->readAll();
	QStringList rData = QString(data).split("|");
	int key = rData[0].toInt();

	switch(key) 
	{
	case DB_ERROR:
		if(m_loginTimer->isActive()) 
			m_loginTimer->stop();

		if(m_sessionTimer->isActive()) 
			m_sessionTimer->stop();
		break;
	case SRV_CONNECT:
		m_loginTimer->setInterval(2000);
		connect(m_loginTimer, SIGNAL(timeout()), this, SLOT(monitorLogin()));

		m_sessionTimer->setInterval(2000);
		connect(m_sessionTimer, SIGNAL(timeout()), this, SLOT(monitorSession()));
		break;
	case REQ_LOGIN:
		if(m_loginTimer->isActive()) 
			m_loginTimer->stop();

		m_thisUser = rData[1];
		break;
	case REQ_PINCODE:
		if(!m_sessionTimer->isActive()) 
			m_sessionTimer->start();

		break;
	case REQ_SESSION:
		if(m_sessionTimer->isActive()) 
			m_sessionTimer->stop();

		break;
	case REQ_LOGOUT:
		if(m_loginTimer->isActive()) 
			m_loginTimer->stop();

		if(m_sessionTimer->isActive())
			m_sessionTimer->stop();

		delete m_loginTimer;
		m_loginTimer = NULL;

		delete m_sessionTimer;
		m_sessionTimer = NULL;
		break;
	}

	if(key > -1) 
		g_clientapp->GetMainWindow()->ResponseProcess(rData);
}

void CommunicationLine::monitorLogin()
{
	sendRequest(REQ_LOGIN);
}

void CommunicationLine::monitorSession()
{
	sendRequest(REQ_SESSION, m_thisUser);
}
