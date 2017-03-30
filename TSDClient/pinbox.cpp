#include "pinbox.h"
#include "guiuty.h"
#include "tsdclient.h"
#include "mainwindow.h"
#include "leftframe.h"

PinBox::PinBox(QWidget *parent, int type)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_parent = (MainWindow*)parent;
	m_type = type;
	
	QString ssPath = g_clientapp->GetWorkDir() + PIN_STYLE_FNAME;
	QFile file(ssPath);
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	QPixmap pmap;
	QString fname(QString("/images/stop.png"));	QString imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.btnPinExit->setIcon(pmap);

	ui.btnPinExit->setStyleSheet(styleSheet);
	ui.groupPincode->setStyleSheet(styleSheet);
	ui.groupRequest->setStyleSheet(styleSheet);
	connect(ui.btnPinExit,SIGNAL(clicked()),m_parent->GetLeftFrame(), SLOT(OnClosePin()));
}

PinBox::~PinBox()
{

}

void PinBox::Init(QString pinMessage)
{
	ui.lblPin->setText(pinMessage);
	QSize sz;
	sz = g_clientapp->GetMainWindow()->geometry().size();
	int x= (sz.width()-width())/2;
	int y = (sz.height()-height())/2;
	QPoint pos(x,y);
	move(pos);
	CreateFrame(m_type);
	show();
}

bool PinBox::CreateFrame(int type)
{
	ui.groupRequest->hide();
	ui.groupPincode->show();
	return true;
}