#include "chatdlg.h"
#include "tsdserverapp.h"
#include "tsdserver.h"

#include <QTimer>
#include <QDesktopWidget>
#include <QDateTime>
#include <QTextTable>
#include <QScrollBar>
#include <QtGui/QSound>


ChatDlg::ChatDlg(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);

	connect(ui.btnChatMinium,SIGNAL(clicked()),this, SLOT(OnChatMinimize()));
	connect(ui.btnChatMaxium,SIGNAL(clicked()),this, SLOT(OnChatMaximize()));
	connect(ui.btnSend,SIGNAL(clicked()),this, SLOT(OnChatSend()));
}

QString CurrentTime()
{
	QTime t=QTime::currentTime();

	return t.toString("h:m:s A");
}

ChatDlg::~ChatDlg()
{

}

void ChatDlg::SetWorkDir(QString workDir)
{
		m_workdir = workDir;
}
void ChatDlg::UpdateDlg()
{
	QPixmap pmap;
	QString fname(QString("/images/max.png"));	 QString imgname = m_workdir + QString("/res") + fname;	pmap.load(imgname);
	ui.btnChatMaxium->setIcon(pmap);
	fname = QString("/images/min.png");	 imgname = m_workdir + QString("/res") + fname;	pmap.load(imgname);
	ui.btnChatMinium->setIcon(pmap);
	fname = QString("/images/chatmark.png");	 imgname = m_workdir + QString("/res") + fname;pmap.load(imgname);
	ui.lblChatMark->setPixmap(pmap);
	fname = QString("/images/arrow.png");	 imgname = m_workdir + QString("/res") + fname;	pmap.load(imgname);
	ui.btnSend->setIcon(pmap);
	fname = QString("/images/tsdIcon.png");	 imgname = m_workdir + QString("/res") + fname;	pmap.load(imgname);
	ui.lblChatMark->setPixmap(pmap.scaled(16,16));
	ui.edtCustomerMessage->hide();
	ui.edtMyMessage->hide();

	ui.edtCustomerMessage->setFocusPolicy(Qt::NoFocus);
	ui.edtMyMessage->setFocusPolicy(Qt::NoFocus);
	ui.messageView->setReadOnly(true);
	ui.messageView->setFocusPolicy(Qt::NoFocus);
	tableFormat.setWidth( ui.messageView->width()-10 );
	tableFormat.setBorder(0);
	OnChatMinimize();

}

void ChatDlg::OnChatClear()
{
	ui.messageView->clear();
}

void ChatDlg::OnChatMinimize()
{
	QDesktopWidget wdt;
	QRect rc = wdt.availableGeometry();
	QSize sz(rc.size());
	QRect rt;
	rt.setBottom(sz.height()-25);
	rt.setTop(sz.height()-ui.groupChatTop->height()-30);
	rt.setLeft(sz.width()-ui.groupChatTop->width()-4);
	rt.setRight(sz.width()-4);

	move(rt.top(),rt.left());
	setGeometry(rt);
	ui.groupChatTop->move(0,2);
	ui.groupChatTop->show();
	ui.groupChatContent->hide();

	ui.btnChatMaxium->setDisabled(false);
	ui.btnChatMinium->setDisabled(true);
	hide();
}

void ChatDlg::OnChatMaximize()
{
	QDesktopWidget wdt;
	QRect rc = wdt.availableGeometry();
	QSize sz(rc.size());
	sz = wdt.geometry().size();
	QRect rt;
	rt.setBottom(sz.height()-30);
	rt.setTop(sz.height()-ui.groupChatTop->height()-ui.groupChatContent->height()-30);
	rt.setLeft(sz.width()-ui.groupChatTop->width()-4);
	rt.setRight(sz.width()-4);

	setGeometry(rt);
	ui.groupChatTop->move(0,2);
	ui.groupChatContent->show();
	ui.btnChatMaxium->setDisabled(true);
	ui.btnChatMinium->setDisabled(false);

	ui.edtMessage->setFocus(Qt::ActiveWindowFocusReason);
	ui.groupMessageEdit->show();
	show();
}

void ChatDlg::OnChatSend()
{
	QString sendData(ui.edtMessage->toPlainText());
	
	if(sendData.isEmpty()) return;

	
	/*ui.edtMyMessage->setPlainText(sendData);	
	ui.edtMyMessage->show();

	QTimer::singleShot(3000,ui.edtMyMessage,SLOT(hide()));*/

	AppendChatMessage(g_app->server()->userName(), CurrentTime(), sendData);

	ui.edtMessage->setPlainText("");

	ui.edtMessage->setFocus(Qt::ActiveWindowFocusReason);

	g_app->server()->sendChatMessage(sendData);
}

//precondition user,message is not empty

void ChatDlg::AppendChatMessage(QString  user,  QString  strtime, QString  message)
{
	QTextCursor cursor(ui.messageView->textCursor());
	cursor.movePosition(QTextCursor::End);

	QTextTable *table = cursor.insertTable(1, 1, tableFormat);

	QTextTableCell txtCell = table->cellAt(0, 0);

	QTextCharFormat txtFormat;

	if(user==g_app->server()->userName()) // my chat message
		txtFormat.setForeground(QBrush(QColor(Qt::red)));
	else	  //from other
		txtFormat.setForeground(QBrush(QColor(Qt::blue)));

	txtCell.setFormat(txtFormat);
	table->cellAt(0, 0).firstCursorPosition().insertText( "["+strtime+"] " + user +" : ");

	table = cursor.insertTable(1, 1, tableFormat);
	table->cellAt(0, 0).firstCursorPosition().insertText(message);

	QScrollBar *bar = ui.messageView->verticalScrollBar();
	bar->setValue(bar->maximum());
}

void ChatDlg::NewChatMessage(QString user,QString time,QString msg)
{
	/*ui.edtCustomerMessage->setPlainText("["+time+"] "+user+" : \n"+msg);
	ui.edtCustomerMessage->show();

	QTimer::singleShot(3000,ui.edtCustomerMessage,SLOT(hide()));*/
	QSound::play(m_workdir+"/res/sounds/message.wav");
}