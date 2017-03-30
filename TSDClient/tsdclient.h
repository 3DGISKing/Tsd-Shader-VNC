#ifndef TSDCLIENT_H
#define TSDCLIENT_H

#include <QApplication>
#include <QtGui>
#include <QTranslator>
#include <QTcpSocket> //asd add 2013.5.2
#include "../Common/qtsingleapplication/qtsingleapplication.h"
#include "communicationline.h" 


class MainWindow;
class Config;
class TSDClientConnection;
class FileManager;


class TSDClient : public QtSingleApplication
{
	Q_OBJECT
public:
	TSDClient(int argc, char **argv);
	~TSDClient();

	inline  QString& GetWorkDir()             				 {return m_workDir; }
	inline  QTranslator* GetTranslator()      				 {return &m_translator; }
	inline  void SetMainWindow(MainWindow *w) 				 {m_mainWnd = w; }
	inline  MainWindow* GetMainWindow()	     				 {return m_mainWnd; }
	inline  Config* GetConfig()	             				 {return m_config; }

	bool    CreateConnection();
	void    DeleteCurrentConnection();
	inline  TSDClientConnection* GetCurrentConnection()       {return m_CurrentConnection;};
	inline  void SetCurrentConnection(TSDClientConnection* c) {m_CurrentConnection=c;}

	inline  FileManager* GetFileManager()                     {return m_pFileManager;}
	inline  CommunicationLine* GetComm() 	                  {return m_serverComm; }

	inline  QString GetWebServerIp()    {return m_WebServerIp;}
	inline  void SetWebServerIp(QString ip){m_WebServerIp=ip; m_serverComm->SetHost();}
	bool 	Init();
	bool 	InitConfig();
	bool 	InitComm(); 
	void 	Release();
	int  	Run();

	QString GetResource(QString &fileName);
	QString Trans(const char *s, const char *c=0);
	QString Trans(const QString &s);

private slots:
	void OnStartConnect();
	void OnStartSessionAgree();

	void OnConnected();
	void OnDisconnected();
	void onError(QAbstractSocket::SocketError error);

	void OnFileDownloadFailed(QString reason);
	void OnFileManagerMessage(QString msg);

private:
	MainWindow		     *m_mainWnd;
	Config			     *m_config;
	TSDClientConnection  *m_CurrentConnection;
	FileManager          *m_pFileManager;
	CommunicationLine    *m_serverComm;

	QString              m_workDir;
	QString              m_WebServerIp;
	QTranslator          m_translator;
};

extern TSDClient *g_clientapp;


#endif // TSDCLIENT_H
