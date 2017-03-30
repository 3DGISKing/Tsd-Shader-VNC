#include "tsdserverapp.h"

#include <QtGui/QMenu>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QAction>
#include "tsdserver.h"
#include "chatdlg.h"

TSDServerApp* g_app;

TSDServerApp::TSDServerApp(int argc, char **argv) :
QtSingleApplication(argc, argv)
{
	workDir= applicationDirPath();

	tsdserver=new TSDServer(this);

	m_chatDlg = new ChatDlg;
	updateChatDlg();

	disableNewClientAction=NULL;
	killAllClientAction=NULL;
	quitAction=NULL;
	propertyAction=NULL;
	chatAction=NULL;	

	trayIcon=NULL;
	trayIconMenu=NULL;
	g_app=this;
}


bool TSDServerApp::startServer()
{
	return tsdserver->start();
}

TSDServerApp::~TSDServerApp()
{
	if (m_chatDlg)
		delete m_chatDlg;
	if(tsdserver)
		delete tsdserver;
	if(disableNewClientAction)
		delete disableNewClientAction;
	if(killAllClientAction)
		delete killAllClientAction;
	if(quitAction)
		delete quitAction;
	if(propertyAction)
		delete propertyAction;
	if(chatAction)
		delete chatAction;

	if(trayIcon)
		delete trayIcon;

	if(trayIconMenu)
		delete trayIconMenu;
}

QString TSDServerApp::getResource(QString &fileName)
{
	QString path = workDir + QString("/res") + fileName;
	return path;
}


void TSDServerApp::createTrayIcon()
{
	propertyAction = new QAction(tr("Property"), this); 

	connect(propertyAction, SIGNAL(triggered()), this, SLOT(onProperty())); 


	disableNewClientAction = new QAction(tr("Disable New Client"), this);
	disableNewClientAction->setCheckable( true );
	connect(disableNewClientAction, SIGNAL(triggered()), this, SLOT(onDisableNewClient()));

	killAllClientAction = new QAction(tr("Kill All Client"), this);
	connect(killAllClientAction, SIGNAL(triggered()), this, SLOT(onKillAllClient()));

	chatAction = new QAction(tr("Chat"), this);
	connect(chatAction, SIGNAL(triggered()), this, SLOT(onChat()));

	quitAction = new QAction(tr("Quit"), this);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(onQuit()));

	trayIcon = new QSystemTrayIcon(this);
	trayIconMenu = new QMenu;

	trayIconMenu->clear();

	trayIconMenu->addAction(propertyAction);
	
	trayIconMenu->addSeparator(); 

	trayIconMenu->addAction(disableNewClientAction);
	trayIconMenu->addAction(killAllClientAction);

	trayIconMenu->addSeparator();
		
	trayIconMenu->addAction(chatAction);

	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);
	

	trayIcon->setToolTip(tr("TSDServer 1.0"));

	trayIcon->setContextMenu(trayIconMenu );

	QIcon eicon;
	
	QString efname = QString("/images/tsdIcon.png");	
	eicon.addPixmap(this->getResource(efname));
	
	trayIcon->setContextMenu(trayIconMenu );
	trayIcon->setIcon(eicon);
	trayIcon->show();

}

void TSDServerApp::onChat()
{
	if(tsdserver->clientCount()==0)
	{
	   onMessage("exists no session.");
	   return;
	}

	m_chatDlg->OnChatMaximize();
}

void TSDServerApp::onMessage(QString msg)
{
	trayIcon->showMessage("TSDServer",msg);
}
void TSDServerApp::onKillAllClient()
{

}

void TSDServerApp::onDisableNewClient()
{

}


void TSDServerApp::onProperty()
{

}

void TSDServerApp::onQuit()
{
	tsdserver->finishScreenDelivers();
	tsdserver->removeAllClient();

	qApp->quit();
}

void TSDServerApp::init()
{
	tsdserver->finishScreenDelivers();
	tsdserver->removeAllClient();
}


int TSDServerApp::run()
{	
	return exec();
}

void	TSDServerApp::updateChatDlg()
{
	m_chatDlg->SetWorkDir(workDir);
	m_chatDlg->OnChatClear();
	m_chatDlg->UpdateDlg();
}