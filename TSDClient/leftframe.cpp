#include "tsdclient.h"
#include "config.h"
#include "guiuty.h"
#include "mainwindow.h"
#include "leftframe.h"
#include "msgdataclient.h"
#include "pinbox.h"
#include <QtGui/QSound>

#include <QTime>
#include "renderframe.h"
#include "communicationline.h" //asd add 2013.5.29
#include "tsdclientconnection.h"
#include "filemanagerwindow.h"
#include "filemanager.h"

#define LEFTWIDTH 341
#define LEFTHEIGHT 620

QString CurrentTime()
{
	QTime t=QTime::currentTime();

	return t.toString("h:m:s A");
}


//#define LOCAL_TEST

LeftFrame::LeftFrame(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_pMainWnd = (MainWindow*)parent;

	setMinimumSize(LEFTWIDTH, LEFTHEIGHT );
	setMaximumWidth(LEFTWIDTH);
	QSizePolicy sizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);	

	QPixmap pmap;
	QString fname(QString("/images/max.png"));	 QString imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnChatMaxium->setIcon(pmap);
	fname = QString("/images/min.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnChatMinium->setIcon(pmap);
	fname = QString("/images/chatmark.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.lblChatMark->setPixmap(pmap);
	ui.edtCustomerMessage->hide();
	ui.edtMyMessage->hide();

	fname = QString("/images/arrow.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnSend->setIcon(pmap);

	fname = QString("/images/markborderu.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.lblmarkU->setPixmap(pmap);
	ui.lblmarkU->setSizePolicy( sizePolicy );	ui.lblmarkU->setMinimumSize(QSize(0, ui.lblmarkU->height()));ui.lblmarkU->setScaledContents(true);
	fname = QString("/images/markborderb.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.lblmarkB->setPixmap(pmap);
	ui.lblmarkB->setSizePolicy( sizePolicy );	ui.lblmarkB->setMinimumSize(QSize(0, ui.lblmarkB->height()));ui.lblmarkB->setScaledContents(true);
	fname = QString("/images/mark.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.lblMark->setPixmap(pmap);
	fname = QString("/images/start.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.lblUser->setPixmap(pmap);
	fname = QString("/images/end.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.lblSelDel->setPixmap(pmap);
	fname = QString("/images/plus.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnCreatePin->setIcon(pmap);
	fname = QString("/images/highlighter.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnAreaHigh->setIcon(pmap);
	fname = QString("/images/fullscreen.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnShare->setIcon(pmap);
	fname = QString("/images/filetrans.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnFiletrans->setIcon(pmap);
	fname = QString("/images/reboot.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnReboot->setIcon(pmap);
	fname = QString("/images/leftclose.png");	 imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnLeftHide->setIcon(pmap);

	ui.edtCustomerMessage->setFocusPolicy(Qt::NoFocus);
	ui.edtMyMessage->setFocusPolicy(Qt::NoFocus);
	ui.messageView->setReadOnly(true);
	ui.messageView->setFocusPolicy(Qt::NoFocus);
	tableFormat.setWidth( ui.messageView->width()-10 );
	tableFormat.setBorder(0);
	
	m_bFullScreen = false;

	m_pinBox = NULL;

	tableModel = new QStandardItemModel;
	setMouseTracking( true );
	m_curRow=-1;

	UpdateInterface(false);
#ifdef LOCAL_TEST
	ui.btnStartSession->setEnabled(true);
#else
	ui.btnStartSession->setEnabled(false);
#endif	

}

LeftFrame::~LeftFrame()
{

}

bool LeftFrame::Init(void)
{
	if (!CreateFrame())
	{
		return false;
	}

	m_connTimer = new QTimer(); 
	m_connTimer->setInterval(1000);
	connect(m_connTimer, SIGNAL(timeout()), this, SLOT(timeEvent()));

	return true;
}

bool LeftFrame::CreateFrame(void)
{
	connect(ui.btnChatMinium,SIGNAL(clicked()),this, SLOT(OnChatMinimize()));
	connect(ui.btnChatMaxium,SIGNAL(clicked()),this, SLOT(OnChatMaximize()));
	connect(ui.btnSend,SIGNAL(clicked()),this, SLOT(OnChatSend()));

	connect(ui.tblUser,SIGNAL(clicked(QModelIndex) ),this,SLOT(OnSelectedRow(QModelIndex)));


	connect(ui.btnShare,SIGNAL(clicked()),this, SLOT(OnFullScreen()));
	connect(ui.btnFiletrans,SIGNAL(clicked()),this, SLOT(OnFileManage()));
	connect(ui.btnReboot,SIGNAL(clicked()),this, SLOT(OnReboot()));
	connect(ui.btnAreaHigh,SIGNAL(clicked()),this, SLOT(OnAreaHighLighter()));

	connect(ui.btnLeftHide,SIGNAL(clicked()),this, SLOT(OnHideLeftFrame()));

	connect(ui.btnCreatePin,SIGNAL(clicked()),this, SLOT(OnCreatePincode()));
	connect(ui.btnStartSession,SIGNAL(clicked()),this, SLOT(OnStartSession()));
	connect(ui.btnStopSession,SIGNAL(clicked()),this, SLOT(OnStopSession()));
	connect(ui.btnSelDel,SIGNAL(clicked()),this, SLOT(OnDeleteSession()));

	connect(tableModel,SIGNAL(rowsInserted( QModelIndex, int, int)), this,SLOT(OnChangedModel())); 
	connect(tableModel,SIGNAL(rowsRemoved( QModelIndex, int, int)),this, SLOT(OnChangedModel()));
	connect(tableModel,SIGNAL(itemChanged( QStandardItem* )),this, SLOT(OnChangedModel()));

	InitialChat();
	return true;
}

void LeftFrame::OnFullScreen()
{
	m_pMainWnd->GetFileManagerWnd()->hide();
	m_pMainWnd->GetRenderFrame()->show();

	if (!m_bFullScreen)
	{
		m_pMainWnd->showFullScreen();
		m_bFullScreen = true;
	}
	else
	{
		m_pMainWnd->showMaximized();
		m_bFullScreen = false;
	}
	show();
	g_clientapp->GetCurrentConnection()->sendStartScreenCapture();
}
void LeftFrame::OnFileManage()
{
	m_pMainWnd->GetRenderFrame()->hide();
	m_pMainWnd->GetFileManagerWnd()->show();

	g_clientapp->GetCurrentConnection()->sendStopScreenCapture();
	//g_clientapp->GetFileManager()->sendFileListRequest("",0);

}


void LeftFrame::OnSelectedRow(QModelIndex idx) 
{
	int row = idx.row();
	if (m_curRow == row) return;

	m_curRow = row;
}


void LeftFrame::OnAreaHighLighter()
{
	m_pMainWnd->GetFileManagerWnd()->hide();
	m_pMainWnd->GetRenderFrame()->show();

	bool bHighLight = m_pMainWnd->GetRenderFrame()->GetRenderArea()->GetHighlighter();

	if (bHighLight)
	{
		m_pMainWnd->GetRenderFrame()->GetRenderArea()->setCursor(Qt::ArrowCursor);
		m_pMainWnd->GetRenderFrame()->GetRenderArea()->EndHighlighter();
		g_clientapp->GetCurrentConnection()->sendStartScreenCapture();

		g_clientapp->GetCurrentConnection()->sendHighLighterEnd();
	}
	else	
	{
		m_pMainWnd->GetRenderFrame()->GetRenderArea()->StartHighlighter();
		QCursor cursor(g_clientapp->GetResource(QString("/images/pen.png")),0,0);
		m_pMainWnd->GetRenderFrame()->GetRenderArea()->setCursor(cursor);
		g_clientapp->GetCurrentConnection()->sendStopScreenCapture();
		g_clientapp->GetCurrentConnection()->sendHighLighterStart();
	}
}

void LeftFrame::OnReboot()
{
	g_clientapp->GetCurrentConnection()->sendReboot();
}

void LeftFrame::timeEvent()
{
	if(m_curRow < 0) return;
	if(GetStringInUserTable(m_curRow, 4) != "start") return;
	QString strTime = GetStringInUserTable(m_curRow, 5);
	QStringList part = strTime.split(":");
	QTime time(part[0].toInt(), part[1].toInt(), part[2].toInt());
	time = time.addSecs(1);
	strTime = time.toString("HH:mm:ss");
	QStandardItem *item = new QStandardItem(strTime);
	item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
	tableModel->setItem(m_curRow, 5, item);
}

void LeftFrame::OnHideLeftFrame()
{
	hide();
	m_pMainWnd->GetRenderFrame()->ResizeFrame();
}

//precondition user,message is not empty

void LeftFrame::AppendChatMessage(QString  user,  QString  strtime, QString  message)
{
	QTextCursor cursor(ui.messageView->textCursor());
	cursor.movePosition(QTextCursor::End);

	tableFormat.setProperty ( QVariant::Color, QVariant( Qt::red ));

	QTextTable *table = cursor.insertTable(1, 1, tableFormat);
				
	QTextTableCell txtCell = table->cellAt(0, 0);

	QTextCharFormat txtFormat;
	
	if(user==m_pMainWnd->GetThisUser()) // my chat message
	   txtFormat.setForeground(QBrush(QColor(Qt::red)));
	else   //from other
		 txtFormat.setForeground(QBrush(QColor(Qt::blue)));

	txtCell.setFormat(txtFormat);

	table->cellAt(0, 0).firstCursorPosition().insertText( "["+strtime+"] " + user +" : ");

	table = cursor.insertTable(1, 1, tableFormat);
	table->cellAt(0, 0).firstCursorPosition().insertText(message);

	QScrollBar *bar = ui.messageView->verticalScrollBar();
	bar->setValue(bar->maximum());
}

void LeftFrame::OnNewChatMessage(QString user,QString msg,QString datetime)
{
	AppendChatMessage(user, datetime, msg);

	/*ui.edtCustomerMessage->setPlainText("["+datetime+"] "+user+" : \n"+msg);
	ui.edtCustomerMessage->show();

	QTimer::singleShot(3000,ui.edtCustomerMessage,SLOT(hide()));*/

	QSound::play(g_clientapp->GetWorkDir()+"/res/sounds/message.wav");
	OnChatMaximize();
}

void LeftFrame::InitialChat()
{
	int i=0;
	ui.edtMessageCount->setText(QString("%1").arg(i));
	OnChatMinimize();
}

void LeftFrame::OnChatMinimize()
{
	QSize sz;
	sz = m_pMainWnd->geometry().size();
	QRect rt;
	rt.setTop(ui.groupTask->pos().y()+ui.userTopBar->pos().y()-ui.groupChatTop->height()-4);
	rt.setLeft(ui.btnChat->width());
	rt.setBottom(ui.groupTask->pos().y()+ui.userTopBar->pos().y());
	rt.setRight(rt.left()+ui.frmChat->width());
	
	ui.frmChat->setGeometry(rt);
	ui.groupChatTop->move(0,2);
	ui.groupChatTop->show();
	ui.groupChatContent->hide();

	ui.btnChatMaxium->setDisabled(false);
	ui.btnChatMinium->setDisabled(true);
}
void LeftFrame::OnChatMaximize()
{
	QSize sz;
	sz = m_pMainWnd->geometry().size();
	QRect rt;
	rt.setTop(ui.groupTask->pos().y()+ui.userTopBar->pos().y()-ui.groupChatTop->height()-ui.groupChatContent->height()-4);
	rt.setLeft(ui.btnChat->width());
	rt.setBottom(ui.groupTask->pos().y()+ui.userTopBar->pos().y());
	rt.setRight(rt.left()+ui.frmChat->width());


	ui.frmChat->setGeometry(rt);
	ui.groupChatTop->move(0,2);
	ui.groupChatContent->show();
	ui.btnChatMaxium->setDisabled(true);
	ui.btnChatMinium->setDisabled(false);

	ui.edtMessage->setFocus(Qt::ActiveWindowFocusReason);
}
void LeftFrame::OnChatSend()
{
	QString sendData(ui.edtMessage->toPlainText());

	if(sendData.isEmpty()) return;
	
	/*ui.edtMyMessage->setPlainText(sendData);	
	ui.edtMyMessage->show();

	QTimer::singleShot(3000,ui.edtMyMessage,SLOT(hide()));*/

	AppendChatMessage(m_pMainWnd->GetThisUser(), CurrentTime(), sendData);

	ui.edtMessage->setPlainText("");

	bool ok;
	int msgcount=ui.edtMessageCount->text().toInt(&ok, 10);
	ui.edtMessageCount->setText(QString::number(msgcount+1));

	ui.edtMessage->setFocus(Qt::ActiveWindowFocusReason);

	QDateTime dt=QDateTime::currentDateTime();

	uint time=dt.toTime_t();
	g_clientapp->GetCurrentConnection()->sendChat(m_pMainWnd->GetThisUser(),sendData,time);
}

void LeftFrame::OnChangedModel()
{
	ui.tblUser->setModel(tableModel);

	ui.tblUser->setColumnWidth(0,0);
	ui.tblUser->hideColumn(0);
	ui.tblUser->setColumnWidth(1,115);
	ui.tblUser->setColumnWidth(2,0);
	ui.tblUser->hideColumn(2);
	ui.tblUser->setColumnWidth(3,100);
	ui.tblUser->setColumnWidth(4,0);
	ui.tblUser->hideColumn(4);
	ui.tblUser->setColumnWidth(5,100);
}

void LeftFrame::InsertUser(QString dev, QString name, QString ip, QString pincode) //asd add 2013.5.8
{
	QList<QStandardItem *> itemList;
	
	QStandardItem *item1 = new QStandardItem;

	QVariant var;
	
	QString path = g_clientapp->GetResource(QString("/images/"));
	QString iconname;
	if(dev == "Desktop") 
		iconname = DESKTOPIMAGE;
	else 
		iconname = TELEPHONEIMAGE;

	iconname = path + iconname;
	QIcon icon(iconname);
	 var.setValue(icon);
	 item1->setData(var,Qt::DecorationRole );
	 item1->setData(name, Qt::DisplayRole);

	QStandardItem *item0 = new QStandardItem(dev);
	item0->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
	QStandardItem *item2 = new QStandardItem(ip);
	item1->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
	QStandardItem *item3 = new QStandardItem(pincode);
	item3->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
	QStandardItem *item4 = new QStandardItem("stop");
	item4->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
	QStandardItem *item5 = new QStandardItem("00:00:00");
	item5->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);

	itemList	<< item0 << item1 << item2 << item3 << item4 << item5;

	tableModel->insertRow(0, itemList);
	ui.btnStartSession->setEnabled(true);
}

void LeftFrame::DeleteUser(QString pin) 
{
	int rows = tableModel->rowCount();
	for(int row = 0; row < rows; row++)
	{
		if(tableModel->data(tableModel->index(row, 3)).toString() == pin)
		{
			tableModel->removeRow(row);
		}
	}

	if(tableModel->rowCount()==0)
		ui.btnStartSession->setEnabled(false);

}

//precondition row>=0
QString LeftFrame::GetStringInUserTable(int row,int column) 
{
	QStandardItem * item  = tableModel->item(row, column);
	return item->text();
}

//precondition row>=0
void  LeftFrame::SetUserStatusInTable(int row, QString w)
{
	QStandardItem *item = new QStandardItem(w);
	tableModel->setItem(row, 4, item);
}

bool LeftFrame::Release(void)
{
	if (tableModel!= NULL)
	{
		delete tableModel;
		tableModel = NULL;
	}
	return true;
}

void LeftFrame::ResizeFrame(QResizeEvent *event)
{
	QSize oldSz(event->oldSize());
	if (oldSz.isEmpty())		return;
	QSize newSz(event->size());
	int dY = newSz.height()-oldSz.height();
	
	QRect rtTask(ui.groupTask->geometry());
	ui.groupTask->move(0,ui.groupTask->pos().y() + dY);
	ui.groupTemp->resize(ui.groupTemp->width(),ui.groupTemp->height() + dY);
	ui.frmChat->move(ui.btnChat->width(),ui.frmChat->pos().y() + dY);
}

void LeftFrame::OnCreatePincode() //asd mod 2013.5.30
{	
	OnClosePin();
	ui.lblPincode->setText("");

	m_curPinCode = g_clientapp->GetConfig()->generatePinCode();

	//send pincode to server
	QString data = m_curPinCode;
	data += "|" + m_pMainWnd->GetThisDevice(); 
	data += "|" + m_pMainWnd->GetThisUser(); 
	g_clientapp->GetComm()->sendRequest(REQ_PINCODE, data);
}

void LeftFrame::ShowPinBox(QString pinCode) //asd mod 2013.5.30
{	
	m_curPinCode = pinCode;
	m_pinBox = new PinBox(m_pMainWnd,PINCODE);
	m_pinBox->Init(m_curPinCode);
	m_pinBox->show();
	ui.lblPincode->setText(m_curPinCode);
}


#ifdef LOCAL_TEST
#include <QtGui/QInputDialog>

#endif

void LeftFrame::OnStartSession() 
{
#ifdef LOCAL_TEST
	bool ok;

	QString ip = QInputDialog::getText(NULL, tr("Enter Ip"),tr("Ip:"), QLineEdit::Normal,"localhost", &ok);

	if (ok && !ip.isEmpty())
	{
		
		if(g_clientapp->CreateConnection())
		{
			TSDClientConnection* connection=g_clientapp->GetCurrentConnection();
			connection->setPeerIp(ip);

			connection->connectToPeer();
		}
	}
#else
	if(m_curRow<0)
	{
		gu_MessageBox(NULL,MSG_WARNING_TITLE,"no exist user", MSGBOX_ICONWARNING);
		return ;
	}

	QString ip=GetStringInUserTable(m_curRow,2);

	if(g_clientapp->CreateConnection())
	{
		TSDClientConnection* connection=g_clientapp->GetCurrentConnection();
		connection->setPeerIp(ip);

		connection->connectToPeer();
	}
	else
	{
		g_clientapp->DeleteCurrentConnection();

		gu_MessageBox(NULL,MSG_ERR_TITLE,"failed to create connection!",MSGBOX_ICONCRITICAL);
	}

#endif
	g_clientapp->GetComm()->sendRequest(REQ_START, m_curPinCode);

	if(!m_connTimer->isActive()) 
		m_connTimer->start();
}

void LeftFrame::OnStopSession()
{
	g_clientapp->GetCurrentConnection()->sendStopSession();
	g_clientapp->GetComm()->sendRequest(REQ_STOP, m_curPinCode);
	SetUserStatusInTable(m_curRow, "stop");

	for(int ii = 0; ii < tableModel->rowCount(); ii++)
		if(GetStringInUserTable(ii, 4) == "start")
			return;
	if(m_connTimer->isActive())
		m_connTimer->stop();
}

void LeftFrame::OnDeleteSession()
{
	if (MSGBOX_IDYES ==gu_MessageBox(this,MSG_WARNING_TITLE,MSG_WARNING_SELECT_USER_DELETE,MSGBOX_IDYESNO)) 
	{
#ifndef LOCAL_TEST
		QString pin = GetStringInUserTable(m_curRow, 3);
		g_clientapp->GetComm()->sendRequest(REQ_DELETE, pin);
#endif
		g_clientapp->GetCurrentConnection()->sendStopSession();
	}
	if(tableModel->rowCount() == 0)
		if(m_connTimer->isActive()) 
			m_connTimer->stop();
}

void LeftFrame::DeleteUserAll()
{
	QString pin;
	int rows = tableModel->rowCount();
	for (int row = 0;  row < rows; ++row) {
		pin = tableModel->data(tableModel->index(row, 3)).toString(); 
		g_clientapp->GetComm()->sendRequest(REQ_DELETE, pin);
	}
}

void LeftFrame::OnClosePin()
{
	if (m_pinBox!=NULL)
	{
		delete m_pinBox;
		m_pinBox = NULL;
	}
}

void LeftFrame::UpdateInterface(bool sessionstarted) 
{
	ui.btnStartSession->setEnabled(!sessionstarted);
	ui.btnStopSession->setEnabled(sessionstarted);
	ui.sessionDelete->setEnabled(sessionstarted);

	ui.btnShare->setEnabled(sessionstarted);
	ui.btnAreaHigh->setEnabled(sessionstarted);
	ui.btnFiletrans->setEnabled(sessionstarted);
	ui.btnReboot->setEnabled(sessionstarted);
	ui.frmChat->setEnabled(sessionstarted);
}

