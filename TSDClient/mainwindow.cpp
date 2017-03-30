#include "tsdclient.h"
#include "config.h"
#include "msgdataclient.h"
#include "guiuty.h"
#include "pinbox.h" 

#include "leftframe.h"
#include "renderframe.h"
#include "mainwindow.h"
#include "signdialog.h"
#include "tsdclientconnection.h"
#include "communicationline.h" 
#include "filemanagerwindow.h"
#include "filemanager.h"

#include <QTime>
#include <QSplitter>

#define MAINWINDOW_TITLE_USER		"Tech Shedar"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setStatusBar(NULL );
	QMenuBar * mainmenu = menuBar();
	delete mainmenu;
	mainmenu = NULL;

	m_splitter = NULL;
	m_renderFrame = NULL;
	m_fileManagerWnd=NULL;
	m_signDlg=NULL;

	m_optionStartmode = false;
	m_bServerConnect = false;

	QString ssPath = g_clientapp->GetWorkDir() + MAINWINDOW_STYLE_FNAME;
	QFile file(ssPath);
	file.open(QFile::ReadOnly);

	QString styleSheet = QLatin1String(file.readAll());

	setStyleSheet(styleSheet);
	m_thisDevice="Unknown";
	m_thisUser="Unknown";
	m_thisIp="";
}

MainWindow::~MainWindow()
{

}

bool MainWindow::Init()
{
	g_clientapp->SetMainWindow(this);

	//set working path and title, icon
	QString workDir = g_clientapp->GetWorkDir();

	setWindowFilePath(workDir);
	setWindowTitle(MSG_APP_TITLE);

	if (!g_clientapp->InitConfig())
		return false; 

	Config *conf = g_clientapp->GetConfig();   
	m_optionStartmode = conf->m_bStartmode;   

	if (!CreateFrame())
		return false;

	resize(1000,600);

#ifdef LOCAL_TEST
	//show();

	m_bServerConnect = true;

	QStringList paras;

	paras<<QString("%1").arg(REQ_LOGIN)
		 <<QString("%1").arg("")
		 <<QString("%1").arg("")
		 <<QString("%1").arg("");

	ResponseProcess(paras);
#endif

	return true;
}

bool MainWindow::CreateFrame()
{
	trayIcon = CreateTray();

	if (!trayIcon)
	{
		gu_MessageBox(this, MSG_ERR_TITLE, MSG_ERR_INITERROR_TRAYICON, MSGBOX_ICONCRITICAL);
		return false;
	}

	trayIcon->show();

	setWindowTitle(MAINWINDOW_TITLE_USER);
	SetIcon();

	m_leftFrame = new LeftFrame(this);

	if (m_leftFrame == NULL)
		return false;
	if (!m_leftFrame->Init())
		return false;

	m_renderFrame = new RenderFrame(this);
	if(m_renderFrame==NULL)
		return false;

	m_renderFrame->show();

	m_splitter = new QSplitter(Qt::Horizontal);
	if (m_splitter == NULL)
		return false;

	m_fileManagerWnd=new FileManagerWindow(this);

	if(m_fileManagerWnd==NULL)
		return false;

	m_fileManagerWnd->hide();

	g_clientapp->GetFileManager()->setFileManagerWnd(m_fileManagerWnd);
	m_splitter->addWidget(m_leftFrame);
	m_splitter->addWidget(m_renderFrame);
	m_splitter->addWidget(m_fileManagerWnd);

	setCentralWidget(m_splitter);
	m_splitter->setChildrenCollapsible(false);	   

	m_renderFrame->ResizeFrame();

	return true;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	m_leftFrame->ResizeFrame(event);
	m_renderFrame->ResizeFrame();
}

void MainWindow::SetIcon()
{
	QString fname("/images/tsdicon.png");
	QIcon icon(g_clientapp->GetResource(fname));
	if(icon.isNull() == false)
		setWindowIcon(icon);
	else
		qDebug("Icon Error!!!");
}

void MainWindow::DestroyFrame()
{
	if (!m_renderFrame)
	{
		delete m_renderFrame;
		m_renderFrame = NULL;
	}

	if (!m_leftFrame)
	{
		delete m_leftFrame;
		m_leftFrame = NULL;
	}
}

void MainWindow::Release()
{
	if (!m_leftFrame->Release())
		return ;

	DestroyFrame();

	if (m_signDlg)
	{
		delete m_signDlg;
		m_signDlg =NULL;
	}

	if (g_clientapp)
		g_clientapp->Release();
}

QSystemTrayIcon* MainWindow::CreateTray()
{	
	loginAction = new QAction(tr("LogIn"), this);
	connect(loginAction, SIGNAL(triggered()), this, SLOT(OnShowLogIn()));

	mainwndAction = new QAction(tr("MainScreen"), this);
	connect(mainwndAction, SIGNAL(triggered()), this, SLOT(show()));

	configAction = new QAction(tr("Autorun when start"), this); 
	configAction->setCheckable( true );
	configAction->setChecked(g_clientapp->GetConfig()->m_bStartmode);
	connect(configAction, SIGNAL(triggered()), this, SLOT(OnShowConfigWnd())); 

	webServerIpAction = new QAction(tr("Setting Web Server Url..."), this);
	connect(webServerIpAction, SIGNAL(triggered()), this, SLOT(SetWebServerIp()));

	quitAction = new QAction(tr("Quit"), this);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(End()));

	trayIcon = new QSystemTrayIcon(this);
	trayIconMenu = new QMenu;

	ModifyTray(); //---------insert network connection part ----2013.5.12
	trayIcon->setToolTip(MSG_APP_TITLE);
	
	return trayIcon;
}

void MainWindow::ModifyTray()
{
	Config *conf = g_clientapp->GetConfig();   
	
	trayIconMenu->clear();
	trayIconMenu->addAction(mainwndAction);

	mainwndAction->setEnabled(conf->m_bLogin && m_bServerConnect);

	trayIconMenu->addSeparator(); 
	trayIconMenu->addAction(configAction);
	configAction->setEnabled(m_bServerConnect); 
	trayIconMenu->addAction(loginAction);
	trayIconMenu->addAction(webServerIpAction);
	loginAction->setEnabled( /*m_bServerConnect && */!conf->m_bLogin);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	QString efname;
	QIcon eicon;

	if (m_bServerConnect)
	{
		efname = QString("/images/tsdIcon.png");	
		eicon.addPixmap(g_clientapp->GetResource(efname));
	}
	else
	{
		efname = QString("/images/tsdIcondisconnection.png");
		eicon.addPixmap(g_clientapp->GetResource(efname));
	}

	trayIcon->setContextMenu(trayIconMenu );
	trayIcon->setIcon(eicon);

}

void MainWindow::OnShowLogIn()
{
	QByteArray connUrl = QByteArray("http://") + g_clientapp->GetWebServerIp().toAscii() + CONF_WEBSERVER_PATH + CONF_WEBPAGE_LOGIN;
	QDesktopServices::openUrl(QUrl(QString(connUrl), QUrl::TolerantMode));

	g_clientapp->GetComm()->startMonitorLogin();
}

void MainWindow::OnShowConfigWnd()
{
	if (m_optionStartmode)
		m_optionStartmode = false;
	else
		m_optionStartmode = true;

	configAction->setChecked(m_optionStartmode);

	g_clientapp->GetConfig()->m_bStartmode = m_optionStartmode;

	if(m_optionStartmode) 
	{ 
#ifdef Q_WS_MAC
		
#else
		QString path = g_clientapp->GetWorkDir() + "/TSDClient.exe";
		QSettings settings(CONF_REG_PATH, QSettings::NativeFormat);
		settings.setValue("TSDClient", path);
#endif
	} 
	else
	{
#ifdef Q_WS_MAC
		
#else
		QSettings settings(CONF_REG_PATH, QSettings::NativeFormat);
		settings.remove("TSDClient");
#endif
	}
}

void MainWindow::SetWebServerIp()
{
	bool ok;

	QString ip = QInputDialog::getText(NULL, tr("Enter Ip"),tr("Ip:"), QLineEdit::Normal,g_clientapp->GetWebServerIp(), &ok);

	if (ok && !ip.isEmpty())
	{
		 g_clientapp->SetWebServerIp(ip);
		
	}
}

void MainWindow::End()
{
	GetLeftFrame()->DeleteUserAll();

#ifdef LOCAL_TEST
	qApp->quit();
#else
	g_clientapp->GetComm()->sendRequest(REQ_LOGOUT, m_thisUser);
#endif
}

void MainWindow::ResponseProcess(QStringList data) //asd mod 2013.5.30
{
	int key = data[0].toInt();

	switch(key) 
	{
	case DB_ERROR:
		break;	
	case SRV_CONNECT:
		m_bServerConnect = true;
		ModifyTray();
		break;	
	case REQ_LOGIN:
		m_thisUser = data[1];
		m_thisIp = data[2];
		m_thisDevice = data[3];
		g_clientapp->GetConfig()->m_bLogin = true;
		ModifyTray();
		showMaximized();
		break;	
	case REQ_PINCODE:
		GetLeftFrame()->ShowPinBox(data[1]);
		break;
	case REQ_SESSION:     //UserDeviceType, UserName, UserIP, PinCode
		for (int row = 0; row < data.count() / 4; row++)
			GetLeftFrame()->InsertUser(data[row * 4 + 1], data[row * 4 + 2], data[row * 4 + 3], data[row * 4 + 4]);
		break;
	case REQ_DELETE:
		GetLeftFrame()->DeleteUser(data[1]);
		break;
	case REQ_LOGOUT:
		Release();
		qApp->quit();
		break;
	case SRV_ERROR:
		//m_bServerConnect = true;
		break;	
	}
}




