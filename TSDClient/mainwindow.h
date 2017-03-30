#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBoxLayout>
#include <QSplitter>
#include "ui_mainwindow.h"

#include <QtGui/QSystemTrayIcon>

class RenderFrame;
class LeftFrame;
class SignDialog;
class FileManagerWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	bool						 Init();
	void						 Release();
	
	inline RenderFrame*          GetRenderFrame()	 {	return m_renderFrame;}
	inline LeftFrame*            GetLeftFrame()		 {	return m_leftFrame;}
	inline FileManagerWindow*    GetFileManagerWnd() {  return m_fileManagerWnd;}
	inline SignDialog*           GetSignDlg()	     {	return m_signDlg;}
	inline QSplitter*			 GetSplitter()		 {	return m_splitter;}

	inline QString				 GetThisDevice()		 {  return m_thisDevice;}             
	inline QString				 GetThisUser()  		 {  return m_thisUser;}              
	inline QString				 GetThisIP()    		 {  return m_thisIp;}  

	void			             ResponseProcess(QStringList data); 

protected:
	void						 resizeEvent(QResizeEvent *event);

private slots:
	void						 OnShowLogIn();
	void						 OnShowConfigWnd();
	void						 End();	
	void                         SetWebServerIp();

private:
	bool						 CreateFrame();
	void						 DestroyFrame();
	void						 SetIcon();
	void						 ModifyTray();
	QSystemTrayIcon*             CreateTray();

private:
	Ui::MainWindow	ui;

	QSplitter*					 m_splitter;
	SignDialog *		    	 m_signDlg;
	LeftFrame*					 m_leftFrame;
	RenderFrame *				 m_renderFrame;
	FileManagerWindow*           m_fileManagerWnd;

	QString						 m_thisUser;
	QString						 m_thisIp; 
	QString						 m_thisDevice;
	
	int							 m_optionStartmode;
	bool						 m_bServerConnect;

	QAction *					 mainwndAction;
	QAction *					 configAction; 
	QAction *					 loginAction;
	QAction *					 webServerIpAction;
	QAction *					 quitAction;

	QSystemTrayIcon*        	 trayIcon;
	QMenu *				    	 trayIconMenu;
};

#endif // MAINWINDOW_H
