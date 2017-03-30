#ifndef TSDSERVERAPP_H
#define TSDSERVERAPP_H

#include "../Common/qtsingleapplication/qtsingleapplication.h"

class QMenu;
class QSystemTrayIcon;
class QAction;
class TSDServer;
class ChatDlg;


class TSDServerApp : public QtSingleApplication
{
	Q_OBJECT

public:
	TSDServerApp(int argc, char **argv);
	~TSDServerApp();

	int    			 run();
	QString			 getResource(QString &fileName);
	void   			 createTrayIcon();
	void			 updateChatDlg();
	bool   			 startServer();
	void             init();

	inline TSDServer* server(){return tsdserver;}
	inline ChatDlg* chatDlg(){return m_chatDlg;}

	
private slots:
	void 			onDisableNewClient();
	void 			onKillAllClient();
	void 			onProperty();
	void 			onQuit();
	void            onChat();
public slots:
   	void            onMessage(QString msg);
private:
	QAction 		*disableNewClientAction;
	QAction 		*killAllClientAction;
	QAction 		*propertyAction; 
	QAction 		*chatAction;
	QAction 		*quitAction;

	QSystemTrayIcon *trayIcon;
	QMenu           *trayIconMenu;

	QString         workDir;

	TSDServer*      tsdserver;
	ChatDlg*		m_chatDlg;
	
};

extern TSDServerApp* g_app;
#endif // TSDSERVERAPP_H
