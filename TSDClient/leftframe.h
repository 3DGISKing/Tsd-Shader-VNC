#ifndef LEFTFRAME_H
#define LEFTFRAME_H

#include <QBasicTimer>
#include <QMouseEvent>
#include "ui_leftframe.h"

class QStandardItemModel;
class MainWindow;
class PinBox;

class LeftFrame : public QWidget
{
	Q_OBJECT

public:
	LeftFrame(QWidget *parent = 0);
	~LeftFrame();
	bool				Init(void);
	bool				Release(void);
	void				ResizeFrame(QResizeEvent *event);

	void				InsertUser(QString dev, QString name, QString ip,QString pincode); //asd add 2013.5.8
	void				DeleteUser(QString pin);                                           //asd add 2013.5.8
	void                DeleteUserAll();
	
	void				ShowPinBox(QString pinCode);                                      //asd add 2013.5.30

	void				UpdateInterface(bool sessionstarted);
	void				OnNewChatMessage(QString user,QString msg,QString datetime);

private:	
	bool				CreateFrame(void);

	QString				GetStringInUserTable(int row,int column);                         //rikr add 2013.5.12
	void				SetUserStatusInTable(int row, QString w);                         //rikr add 2013.5.12
	
	void				AppendChatMessage( QString  user,  QString  strtime, QString  message);
	void				InitialChat();

private slots:
	void				OnFullScreen();
	void				OnFileManage();
	void				OnReboot();
	void				OnAreaHighLighter();
	void				OnHideLeftFrame();

	void				OnChatSend();
	void				OnChatMinimize();
	void				OnChatMaximize();

	void				OnCreatePincode();
	void				OnStartSession();
	void				OnStopSession();
	void				OnDeleteSession();
	void				OnClosePin();

	void				OnChangedModel();

	void    			OnSelectedRow(QModelIndex idx);
	void		        timeEvent(); 

private:
	Ui::LeftFrame       ui;

	MainWindow*			m_pMainWnd;
	PinBox *			m_pinBox;	
	int					m_curRow;     //asd add 2013.5.8
	QString				m_curPinCode; //asd add 2013.5.11
	QTimer *			m_connTimer;

	bool				m_bFullScreen;

	QTextTableFormat	tableFormat;
	QStandardItemModel *tableModel;
};

#endif // LEFTFRAME_H
