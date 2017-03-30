/********************************************************************
	created:	2013/05/08
	created:	8:5:2013   22:16
	filename: 	explolerclient.h
	file path:	\
	file base:	explolerclient
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/

#ifndef EXPLOLERCLIENT_H
#define EXPLOLERCLIENT_H

#include <QtGui/QSplitter>
#include <QtGui/QFileSystemModel>
#include <QtGui/QTreeView>
#include <QtGui/QListView>
#include <QtGui/QAction>

class ContentView;
class Exploler;
class ExplolerClient;
class QAction;

class DirView: public QTreeView
{
	Q_OBJECT
public:
	DirView(QWidget * parent=0);

	void setBuddy(ContentView* view){m_ContentViewBuddy=view;}
	void setParent(ExplolerClient* ex){m_pParent=ex;}
	ExplolerClient* getParent(){return m_pParent;}

	void setCurrentPath(QString path);
	void setRootPath(QString path);

protected:
	virtual void	currentChanged ( const QModelIndex & current, const QModelIndex & previous );


private:
	QFileSystemModel  m_DirModel;
	ContentView*      m_ContentViewBuddy;
	ExplolerClient*   m_pParent;
};

class ContentView :public QTreeView
{
	Q_OBJECT 
public:
	ContentView(QWidget* parent=0);
	~ContentView();
	void setBuddy(DirView* view){m_DirViewBuddy=view;};
	void setParent(ExplolerClient* ex){m_pParent=ex;}
	void setRootPath(QString path);
	ExplolerClient* getParent(){return m_pParent;}

	inline QAction* downloadAction(){return m_pDownLoadAction;}
	inline QAction* uploadAction(){return m_pUpLoadAction;}
	inline QAction* cancelAction(){return m_pCancelAction;}

	inline QModelIndexList getSelectedIndexes(){return selectedIndexes();}

protected:
	virtual void	mouseDoubleClickEvent ( QMouseEvent * event );
	virtual void    mousePressEvent(QMouseEvent *event);

private slots:
	void            onDownLoad();
	void            onUpLoad();
	void            onCancel();
private:
	QFileSystemModel  m_SubDirFileModel;
	DirView*          m_DirViewBuddy;
	ExplolerClient*   m_pParent;

	QAction*          m_pDownLoadAction;       
	QAction*		  m_pUpLoadAction; 
	QAction*          m_pCancelAction; 
};

class ExplolerClient :public QSplitter
{
	Q_OBJECT

public:
	ExplolerClient(QWidget * parent=0);
	~ExplolerClient();

	void setParent(Exploler* ex){m_pParent=ex;}
	Exploler* getParent(){return m_pParent;}

	void setCurrentPath(QString path);

	inline DirView*      dirView(){return &m_DirView;}
	inline ContentView*  contentView(){return &m_ContentView;}


private:
	
	DirView          m_DirView;
	ContentView      m_ContentView;
	Exploler*        m_pParent;
};

#endif