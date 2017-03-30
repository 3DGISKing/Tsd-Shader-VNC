/********************************************************************
	created:	2013/05/23
	created:	23:5:2013   16:31
	filename: 	c:\Documents and Settings\Administrator\My Documents\Visual Studio 2005\Projects\FileManage\FileManage\filemanagerwindow.h
	file path:	c:\Documents and Settings\Administrator\My Documents\Visual Studio 2005\Projects\FileManage\FileManage
	file base:	filemanagerwindow
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/
#ifndef FILEMANAGERWINDOW_H
#define FILEMANAGERWINDOW_H


#include "exploler.h"
#include "remoteexplorer.h"

#include <QtGui/QProgressBar>
#include <QtGui/QLabel>


class QSplitter;


class FileManagerWindow :public QSplitter
{
	Q_OBJECT
public:
	FileManagerWindow(QWidget* parent=NULL);
	~FileManagerWindow();

	inline RemoteExplorler* remoteExplorer(){return m_pRemoteExplorer;}
	inline Exploler*        localExplorer(){return m_pLocalExplorer;}
	inline void showProgressBar() {m_pProgressBar->show();}
	inline void hideProgressBar() {m_pProgressBar->hide();}

	inline void showStatusLabel(){m_pStatusLabel->show();}
	inline void hideStatusLabel(){m_pStatusLabel->hide();}
	inline void setStatusText(QString info){m_pStatusLabel->setText(info);}

	void        blockingWindow();
	void        activateWindow();
	void        initProgressBar(int pos,int min,int max,int step);
	inline QProgressBar* progressBar(){return m_pProgressBar;}


private:
	Exploler*        m_pLocalExplorer;
	RemoteExplorler* m_pRemoteExplorer;
	
	QSplitter*       m_pMainWindow;
	QProgressBar*    m_pProgressBar;
	QLabel*          m_pStatusLabel;           
	
};

#endif