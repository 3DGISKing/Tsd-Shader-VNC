#include <QSettings>
#include <QNetworkInterface>

#include "tsdclient.h"
#include "mainwindow.h"
#include "config.h"
#include "systeminfo.h"

#define CONF_SET_MAINWNDPOS			"main_wnd_pos"
#define CONF_SET_MAINWNDSIZE		"main_wnd_size"
#define CONF_SET_SHOWTOOLBAR		"show_toolbar"
#define CONF_SET_SHOWFULLSCREEN		"show_fullscreen"

#define CONF_SET_POSITION_TASK		"task_panel_splitter"
#define CONF_SET_POSITION_USER		"User_panel_splitter"
#define CONF_SET_POSITION_CHAT		"Chat_panel_splitter"

#define CONF_SET_SRVADDR			"srv_addr"
#define CONF_SET_WEBSRVADDR			"webserveraddr"

#define CONF_SET_CAPTUREDIR			"dir_capture"
#define CONF_SET_SHOTDIR			"dir_shot"

Config::Config()
{
	m_fullScreen = false;

	m_bStartmode =false;
	m_bLogin = false;

	m_srvAddr[0] = 0;

	m_dirCature[0] = 0;
	m_dirShot[0] = 0;

	m_posTask = 0;
	m_posUesr = 0;
	m_posChat = 0;

	QString str(/* "208.9.30.101"*/"");
	QByteArray ba = str.toAscii();
	const char *tmp = ba.constData();
	strncpy(m_webAddr, tmp, CONF_SRVADDR_LEN);
	
}

Config::~Config()
{
}

bool Config::Init()
{
	return true;
}

void Config::Release()
{
}

QString Config::getIPAddress() //asd add 2013.5.9
{
	QString ipAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

	for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
				ipAddress = ipAddressesList.at(i).toString();
				break;
		}
	}

	if (ipAddress.isEmpty())
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	return ipAddress;
}

QString Config::generatePinCode() //asd add 2013.5.9
{
	QString pineCode;
	QList<int> xx;

	//-----------------------------------------------------
	QString uUid = QUuid::createUuid().toString();
	uUid.remove(QRegExp("[{}\\-]"));
	bool ok;
	for(int jj = 0; jj < uUid.size(); jj = jj + 2)
		xx << uUid.mid(jj, 2).toInt(&ok, 16);
	//-----------------------------------------------------

	QList<int> yy;

	int result_sum = 0;
	int pow_part = 0;
	int num_part = 0;

	for(int jj = 0; jj < CONF_PINCODE_SIZE; jj++) {
		for(int ii = 0; ii < xx.count(); ii++) {
			pow_part = (long)pow((double)xx[ii], (double)((jj % 3) + 1)) % 10;
			num_part = (ii + jj) % 10;
			result_sum += (pow_part * num_part) % 10;
		}
		yy << (int)result_sum % 10;
	}

	for(int kk = 0; kk < CONF_PINCODE_SIZE; kk++)
		pineCode += QString::number(yy[kk]);

	return pineCode;
}
QString Config::getClientInfo() //asd add 2013.5.9
{
	QString info;
	info=SystemInfo::getSystemInfo();

	QString clientInfo;

	//device type

	clientInfo.append(CONF_DEV_DESKTOP);
	clientInfo.append("|");

	//os type

#if defined Q_OS_WIN || defined Q_OS_WINCE
	clientInfo.append(CONF_OS_WINDOWS);
#endif

#ifdef Q_OS_LINUX
	clientInfo.append(CONF_OS_LINUX);
#endif

#ifdef Q_OS_MAC
	clientInfo.append(CONF_OS_MACINTOSH);
#endif

	clientInfo.append("|");
	clientInfo.append(info);

	return clientInfo;
}

void Config::ReadSettings()
{
	QSettings set(CONF_SET_ORG, CONF_SET_APP);

	bool res;

	//main window
	m_wndPos = set.value(CONF_SET_MAINWNDPOS, QPoint(0, 0)).toPoint();
	m_wndSize = set.value(CONF_SET_MAINWNDSIZE, QSize(0, 0)).toSize();
	//View Size
	m_fullScreen = set.value(CONF_SET_SHOWFULLSCREEN, m_fullScreen).toBool();
	//start mode
	m_bStartmode = set.value(CONF_SET_STARTMODE, m_bStartmode).toBool();

	//server
	QString srvAddr = set.value(CONF_SET_SRVADDR, QString("")).toString();
	QString webAddr = set.value(CONF_SET_WEBSRVADDR, QString("")).toString();
	
	QByteArray ba = srvAddr.toAscii();
	const char *tmp = ba.constData();
	strncpy(m_srvAddr, tmp, CONF_SRVADDR_LEN);
	ba = webAddr.toAscii();
	tmp = ba.constData();
	strncpy(m_webAddr, tmp, CONF_SRVADDR_LEN);

	//screen save
	QString dirCapture = set.value(CONF_SET_CAPTUREDIR, QString("")).toString();
	QString dirShot = set.value(CONF_SET_SHOTDIR, QString("")).toString();
	ba = dirCapture.toAscii();	tmp = ba.constData();
	strncpy(m_dirCature, tmp, CONF_SRVADDR_LEN);
	ba = dirShot.toAscii();	tmp = ba.constData();
	strncpy(m_dirShot, tmp, CONF_SRVADDR_LEN);

	m_posTask = set.value(CONF_SET_POSITION_TASK, m_posTask).toUInt();
	m_posUesr = set.value(CONF_SET_POSITION_USER, m_posUesr).toUInt();
	m_posChat = set.value(CONF_SET_POSITION_CHAT, m_posChat).toUInt();
}

void Config::WriteSettings()
{
	QSettings set(CONF_SET_ORG, CONF_SET_APP);

	//main windows
	MainWindow *mainWnd = g_clientapp->GetMainWindow();
	if (mainWnd && mainWnd->isEnabled())
	{
		QPoint pos = mainWnd->pos();
		QSize size = mainWnd->size();
		if (m_fullScreen || mainWnd->isMaximized() || mainWnd->isMinimized())
		{
			pos = QPoint(0,0);
			size = QSize(0,0);
		}
		set.setValue(CONF_SET_MAINWNDPOS, pos);
		set.setValue(CONF_SET_MAINWNDSIZE, size);
	}
	//View Size
	set.setValue(CONF_SET_SHOWFULLSCREEN, m_fullScreen);
	//start mode
	set.setValue(CONF_SET_STARTMODE, m_bStartmode);

	//server
	set.setValue(CONF_SET_SRVADDR, QString(m_srvAddr));
	set.setValue(CONF_SET_WEBSRVADDR, QString(m_webAddr));

	//screen save
	set.setValue(CONF_SET_CAPTUREDIR, QString(m_dirCature));
	set.setValue(CONF_SET_SHOTDIR, QString(m_dirShot));


	set.setValue(CONF_SET_POSITION_TASK, m_posTask);
	set.setValue(CONF_SET_POSITION_USER, m_posUesr);
	set.setValue(CONF_SET_POSITION_CHAT, m_posChat);
}

void Config::SetServer(char *addr, quint16 port,char *webaddr)
{
	strncpy(m_srvAddr, addr, CONF_SRVADDR_LEN);
	strncpy(m_webAddr, webaddr, CONF_SRVADDR_LEN);
}
void Config::SetScreenDir(char *capdir,char *shotdir)
{
	strncpy(m_dirCature, capdir, CONF_SRVADDR_LEN);
	strncpy(m_dirShot, shotdir, CONF_SRVADDR_LEN);	
}

QString Config::GetServerIP()
{
	QString ServerIP(m_srvAddr);
	return ServerIP;
}

QString Config::GetWebServerIP()
{
	QString ServerIP(m_webAddr);
	return ServerIP;
}



