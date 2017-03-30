#include "remoteexplorer.h"

#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QApplication>

#include "../Common/tsd/filetransferiteminfo.h"
#include "filemanager.h"

#define DEFAULT_HEIGHT  30
#define BUTTON_WIDTH    100
#define TEXTEDIT_WIDTH  500

RemoteAddressBar::RemoteAddressBar(QWidget* parent/* =NULL */)
:QWidget(parent)
{
	m_pAddressButton=new QPushButton(this);
	m_pAddressButton->setGeometry(QRect(0, 0, BUTTON_WIDTH, DEFAULT_HEIGHT));

	m_pAddressTextEdit=new QLineEdit(this);
	m_pAddressTextEdit->setGeometry(QRect(BUTTON_WIDTH, 0, TEXTEDIT_WIDTH, DEFAULT_HEIGHT));

	this->setFixedHeight(DEFAULT_HEIGHT);
	this->setWindowFlags(Qt::Tool);
	this->setAddressTitle("Remote Site:");

}

void RemoteAddressBar::resizeEvent(QResizeEvent * event)
{
	int newwidth=event->size().width();	
	m_pAddressTextEdit->setGeometry(BUTTON_WIDTH,0, newwidth-BUTTON_WIDTH,DEFAULT_HEIGHT);
}

void RemoteAddressBar::setCurrentPath(QString path)
{
	m_pAddressTextEdit->setText(path);
}

inline void RemoteAddressBar::setAddressTitle(QString title)
{
	m_pAddressButton->setText(title);
}




RemoteAddressBar::~RemoteAddressBar()
{

}


RemoteDirView::RemoteDirView(QWidget* parent)
:QTreeWidget(parent)
{
	this->setHeaderHidden(true);
	connect(this,SIGNAL(itemExpanded(QTreeWidgetItem *)),g_FileManager,SLOT(onRemoteDirViewExpanded(QTreeWidgetItem*)));
	connect(this,SIGNAL(itemClicked(QTreeWidgetItem *,int)),g_FileManager,SLOT(onRemoteDirViewItemClicked(QTreeWidgetItem*,int)));

}


RemoteContentView::RemoteContentView(QWidget* parent)
:QTreeWidget(parent)
{
	QStringList headers;
	headers << tr("Name") << tr("Size") << tr("Type")
		<< tr("Date Modified") ;
	 setHeaderLabels(headers);
	 setSelectionBehavior(QAbstractItemView::SelectRows);
	 setRootIsDecorated(false);

	 connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem * , int  )),g_FileManager,SLOT(onRemoteContentViewDblClick(QTreeWidgetItem * , int )));
	 connect(this,SIGNAL(currentItemChanged(QTreeWidgetItem * , QTreeWidgetItem * )),this,SLOT(onCurrentItemChanged(QTreeWidgetItem * , QTreeWidgetItem *)));

	

	 setSelectionMode(QAbstractItemView::ExtendedSelection);

}

RemoteContentView::~RemoteContentView()
{

}

void RemoteContentView::onCurrentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	if(current==NULL )return;
	QString name=current->text(0);
	QString type=current->text(2);

	if(type!="" && type!="File Folder" && type!="Drive")
		g_FileManager->setPeerFileName(name);
}


void RemoteContentView::mousePressEvent(QMouseEvent *event)
{
	QTreeWidget::mousePressEvent(event);
}
void RemoteContentView::deleteAllItems()
{
	QTreeWidgetItem* root=invisibleRootItem();

start:
	for(int i=0;i<root->childCount();i++)
	{
		QTreeWidgetItem* child=root->child(i);
		root->removeChild(child);
		delete child;
		goto start;
	}
}

RemoteExplorerClient::RemoteExplorerClient(QWidget* parent)
:QSplitter(parent)
{
	setOrientation(Qt::Vertical);

	m_ContentView.setBuddy(&m_DirView);
	m_DirView.setBuddy(&m_ContentView);

	this->addWidget(&m_DirView);
	this->addWidget(&m_ContentView);
}

RemoteExplorler::RemoteExplorler(QWidget* parent/* =0 */)
:QSplitter(parent)
{
	m_ClientArea.setParent(this);
	setOrientation(Qt::Vertical);

	this->addWidget(&m_AddressBar);
	this->addWidget(&m_ClientArea);
}

RemoteExplorler::~RemoteExplorler()
{

}


void RemoteContentView::showFileTransferItems(FileTransferItemInfo * info)
{
	QTreeWidgetItem* root=invisibleRootItem();

	QStyle* style=QApplication::style();

	
	for (int i=0; i<info->getNumEntries(); i++) 
	{ 
		QTreeWidgetItem* item=new QTreeWidgetItem(root);

#ifdef Q_OS_WIN
		if(info->getSizeAt(i)==-2)
		{
			item->setText(2,"Drive");
			item->setIcon(0,style->standardIcon(QStyle::SP_DriveHDIcon));

		}
#endif
		else if(info->getSizeAt(i)==-1)
		{
			item->setText(2,"File Folder");
			item->setIcon(0,style->standardIcon(QStyle::SP_DirIcon));
		}
		else if(info->getSizeAt(i)>=0)
		{
			item->setText(2,"File");
			item->setIcon(0,style->standardIcon(QStyle::SP_FileIcon));
		}

		item->setText(0,info->getNameAt(i));
	}
}
