/********************************************************************
	created:	2013/05/23
	created:	23:5:2013   15:30
	filename: 	remoteexplorer.h
	file path:	\
	file base:	remoteexplorer
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/

#ifndef REMOTEEXPLORER_H
#define REMOTEEXPLORER_H

#include <QtGui/QWidget>
#include <QtGui/QResizeEvent>
#include <QtGui/QListWidget>
#include <QtGui/QSplitter>
#include <QtGui/QTreeWidget>


class QLineEdit;
class QPushButton;
class ExplolerClient;
class FileTransferItemInfo;
class RemoteContentView;


class RemoteAddressBar :public QWidget
{
	Q_OBJECT

public:
	RemoteAddressBar(QWidget* parent=NULL);
	~RemoteAddressBar();

	inline void setAddressTitle(QString title);
	void setCurrentPath(QString path);


protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
	
	QPushButton *       m_pAddressButton;
	QLineEdit *         m_pAddressTextEdit;
};

class RemoteDirView :public QTreeWidget
{
	Q_OBJECT
public:
	RemoteDirView(QWidget* parent=0);
	void setBuddy(RemoteContentView* w){m_pContentViewBuddy=w;}
private:
	RemoteContentView* m_pContentViewBuddy;
};

class RemoteContentView:public QTreeWidget
{
	Q_OBJECT
public:
	RemoteContentView(QWidget* parent=0);
	~RemoteContentView();
	void showFileTransferItems(FileTransferItemInfo * info);
	void deleteAllItems();
	void setBuddy(RemoteDirView* w){m_pDirViewBuddy=w;}



private slots:
	void onCurrentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);

protected:
	virtual void    mousePressEvent(QMouseEvent *event);
private:
	RemoteDirView* m_pDirViewBuddy;


};


class RemoteExplorerClient :public QSplitter
{
	Q_OBJECT
public:

	RemoteExplorerClient(QWidget* parent=0);
	inline RemoteContentView * contentView(){return &m_ContentView;};
	inline RemoteDirView *     dirView(){return &m_DirView;}

private:
	RemoteDirView			 m_DirView;
	RemoteContentView        m_ContentView;

};	

class RemoteExplorler :public QSplitter
{
	Q_OBJECT

public:
	RemoteExplorler(QWidget* parent=0);
	~RemoteExplorler();
	inline RemoteExplorerClient* client() { return &m_ClientArea;}
	inline RemoteAddressBar*     addressBar() {return &m_AddressBar;} 

private:
	RemoteAddressBar        m_AddressBar;
	RemoteExplorerClient    m_ClientArea;
	

	
};

#endif