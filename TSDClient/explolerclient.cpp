/********************************************************************
	created:	2013/05/08
	created:	8:5:2013   22:15
	filename: 	explolerclient.cpp
	file path:	\
	file base:	explolerclient
	file ext:	cpp
	author:		ugi
	
	purpose:	
*********************************************************************/
#include "explolerclient.h"
#include <QtGui/QSplitter>
#include "exploler.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>

#include "filemanager.h"

DirView::DirView(QWidget * parent/* =0 */)
:QTreeView(parent)
{
	this->setHeaderHidden(true);
	m_DirModel.setRootPath(QDir::homePath());
	m_DirModel.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	this->setModel(&m_DirModel);
	this->setRootIndex(m_DirModel.index(QDir::homePath()));

	int count=m_DirModel.columnCount();

	for(int i=1;i<count;i++)
		this->setColumnHidden(i,true);
}

void DirView::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
	QString filepath=m_DirModel.filePath(current);

	m_ContentViewBuddy->setRootPath(filepath);
	getParent()->getParent()->setCurrentPath(filepath);
}

void DirView::setCurrentPath(QString path)
{
	this->setCurrentIndex(m_DirModel.index(path));
	this->expand(m_DirModel.index(path));
}

void DirView::setRootPath(QString path)
{
	m_DirModel.setRootPath(path);
	this->setRootIndex(m_DirModel.index(path));
}

ContentView::ContentView(QWidget* parent/* =0 */)
:QTreeView(parent)
{
	this->setItemsExpandable(false);
	setRootIsDecorated(false);
	m_SubDirFileModel.setRootPath(QDir::homePath());

	this->setModel(&m_SubDirFileModel);
	this->setRootIndex(m_SubDirFileModel.index(QDir::homePath()));

	m_pDownLoadAction=new QAction(tr("DownLoad"),this);
	m_pUpLoadAction=new QAction(tr("Upload"),this);
	m_pCancelAction=new QAction(tr("Cancel"),this);

	m_pCancelAction->setEnabled(false);

	connect(m_pDownLoadAction,SIGNAL(triggered()),this,SLOT(onDownLoad()));
	connect(m_pUpLoadAction,SIGNAL(triggered()),this,SLOT(onUpLoad()));
	connect(m_pCancelAction,SIGNAL(triggered()),this,SLOT(onCancel()));

	setSelectionMode(QAbstractItemView::ExtendedSelection);
}

ContentView::~ContentView()
{
	delete m_pDownLoadAction;
	delete m_pUpLoadAction;
	delete m_pCancelAction;
}

void ContentView::onDownLoad()
{
	g_FileManager->download();
}

void ContentView::onUpLoad()
{
	g_FileManager->upload();
}

void ContentView::onCancel()
{
	g_FileManager->cancel();
}

void ContentView::mouseDoubleClickEvent ( QMouseEvent * event )
{
	QString path=m_SubDirFileModel.filePath(this->currentIndex());

	QFileInfo fileinfo(path);

	if(fileinfo.isDir())
	{
		this->setRootIndex(this->currentIndex());
		getParent()->getParent()->setCurrentPath(path);

		m_DirViewBuddy->setCurrentPath(path);
		getParent()->getParent()->setCurrentPath(path);
	}
	
}


void ContentView::setRootPath(QString path)
{
	this->setRootIndex(m_SubDirFileModel.index(path));
}

void ContentView::mousePressEvent(QMouseEvent *event)
{
	if(event->button()==Qt::RightButton)
	{
		QMenu popupmenu(this);
		
		popupmenu.addAction(m_pDownLoadAction);
		popupmenu.addAction(m_pUpLoadAction);
		popupmenu.addAction(m_pCancelAction);

	

		QPoint point=event->globalPos();

		popupmenu.exec(point);
	}
	
	QTreeView::mousePressEvent(event);
}

ExplolerClient::ExplolerClient(QWidget* parent)
 :QSplitter(parent)
{
	 setOrientation(Qt::Vertical);

	m_DirView.setBuddy(&m_ContentView);
	m_ContentView.setBuddy(&m_DirView);
	
	m_DirView.setParent(this);
	m_ContentView.setParent(this);



	this->addWidget(&m_DirView);
	this->addWidget(&m_ContentView);

}

ExplolerClient::~ExplolerClient()
{

}

void ExplolerClient::setCurrentPath(QString path)
{
	m_DirView.setRootPath(path);
	m_ContentView.setRootPath(path);
}