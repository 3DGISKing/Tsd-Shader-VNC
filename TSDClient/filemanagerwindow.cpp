/********************************************************************
	created:	2013/05/23
	created:	23:5:2013   16:31
	filename: 	c:\Documents and Settings\Administrator\My Documents\Visual Studio 2005\Projects\FileManage\FileManage\filemanagerwindow.cpp
	file path:	c:\Documents and Settings\Administrator\My Documents\Visual Studio 2005\Projects\FileManage\FileManage
	file base:	filemanagerwindow
	file ext:	cpp
	author:		ugi
	
	purpose:	
*********************************************************************/

#include "filemanagerwindow.h"

#define  DEFAULT_HEIGHT 30

FileManagerWindow::FileManagerWindow(QWidget* parent/* =NULL */)
:QSplitter(parent)
{
	m_pMainWindow=new QSplitter(this);

	m_pLocalExplorer=new Exploler(this);
	m_pRemoteExplorer=new RemoteExplorler(this);
	
	m_pMainWindow->addWidget(m_pLocalExplorer);
	m_pMainWindow->addWidget(m_pRemoteExplorer);

	this->setOrientation(Qt::Vertical);
	this->addWidget(m_pMainWindow);

	m_pStatusLabel=new QLabel(this);
	this->addWidget(m_pStatusLabel);
	m_pStatusLabel->setFixedHeight(DEFAULT_HEIGHT);

	m_pProgressBar=new QProgressBar(this);

	m_pProgressBar->setFixedHeight(DEFAULT_HEIGHT-10);
	this->addWidget(m_pProgressBar);

	//m_pProgressBar->hide();
	//m_pStatusLabel->hide();
}


void FileManagerWindow::initProgressBar(int pos,int min,int max,int step)
{
	if(max<=0) max=1;
	m_pProgressBar->setRange(min,max);
	m_pProgressBar->setValue(pos);

}

void FileManagerWindow::activateWindow()
{
	m_pMainWindow->setEnabled(true);
}
void FileManagerWindow::blockingWindow()
{
	m_pMainWindow->setEnabled(false);
}

FileManagerWindow::~FileManagerWindow()
{
	delete m_pProgressBar;
	delete m_pStatusLabel;

	delete m_pLocalExplorer;
	delete m_pRemoteExplorer;

	delete m_pMainWindow;
}