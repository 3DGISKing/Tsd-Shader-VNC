#include "signdialog.h"
#include "tsdclient.h"
#include "config.h"
#include "guiuty.h"
#include "msgdataclient.h"
#include "mainwindow.h"

SignDialog::SignDialog(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QPixmap pmap;
	QString fname(QString("/images/mark.png"));	QString imgname = g_clientapp->GetResource(fname);	pmap.load(imgname);
	ui.lblMark->setPixmap(pmap);

}

SignDialog::~SignDialog()
{

}
bool SignDialog::Init()
{
	CreateFrame();
	return true;
}

bool SignDialog::CreateFrame(void)
{
	SetIcon();
	connect(ui.btnLogin, SIGNAL(clicked()), this, SLOT(Click_ButLogIn()));
	return true;
}
void SignDialog::SetIcon(void)
{
	QString iconName = g_clientapp->GetResource(QString(MAINWINDOW_ICON_NAME));
	QIcon icon(iconName);
	if(icon.isNull() == false)
		setWindowIcon(icon);
	else
		qDebug("Icon Error!!!");	
}

void SignDialog::Click_ButLogIn() //asd add 2013.5.4
{
	QString user = ui.edtUsername->text().trimmed();
	QString pass = ui.edtPassword->text().trimmed();

	if(user.isEmpty()) {
		gu_MessageBox(this, MSG_WARNING_TITLE, MSG_WARNING_LOGIN_USER, MSGBOX_ICONWARNING);
		ui.edtUsername->setFocus();
		return;
	}

	if(pass.isEmpty()){
		gu_MessageBox(this, MSG_WARNING_TITLE, MSG_WARNING_LOGIN_PASS, MSGBOX_ICONWARNING);
		ui.edtPassword->setFocus();
		return;
	}

	QString data = user + "|" + pass;
}
void SignDialog::initialData()
{
	ui.edtPassword->setText("");
	ui.edtUsername->setText("");
	
}