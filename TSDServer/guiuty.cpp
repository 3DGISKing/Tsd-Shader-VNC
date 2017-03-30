
#include <QMessageBox>
#include <QPushButton>
#include "guiuty.h"

int gu_MessageBox(QWidget *parent, const QString &title, const QString &text, unsigned int flag)
{
	QMessageBox msgBox(parent);
	msgBox.setWindowModality(Qt::NonModal);
	QAbstractButton *okButton = NULL, *yesButton = NULL;
	QAbstractButton*noButton = NULL, *cancelButton = NULL;
	int res = 0;

	unsigned int id = MSGBOX_ID(flag);
	unsigned int ic = MSGBOX_ICON(flag);

	if (title.length() > 0)
		msgBox.setWindowTitle(title);
	if (text.length() > 0)
		msgBox.setText(text);

	if (id == 0)
		id = MSGBOX_IDOK;

	if (id & MSGBOX_IDOK)
		okButton = msgBox.addButton(MSG_OK, QMessageBox::AcceptRole);
	if (id & MSGBOX_IDYES)
		yesButton = msgBox.addButton(MSG_YES, QMessageBox::YesRole);
	if (id & MSGBOX_IDNO)
		noButton = msgBox.addButton(MSG_NO, QMessageBox::NoRole);
	if (id & MSGBOX_IDCANCEL)
		cancelButton = msgBox.addButton(MSG_CANCEL, QMessageBox::RejectRole);

	if (ic == MSGBOX_ICONQUESTION)
		msgBox.setIcon(QMessageBox::Question);
	else if (ic == MSGBOX_ICONINFO)
		msgBox.setIcon(QMessageBox::Information);
	else if (ic == MSGBOX_ICONWARNING)
		msgBox.setIcon(QMessageBox::Warning);
	else if (ic == MSGBOX_ICONCRITICAL)
		msgBox.setIcon(QMessageBox::Critical);

	msgBox.exec();

	if (msgBox.clickedButton() == okButton)
		res = MSGBOX_IDOK;
	else if (msgBox.clickedButton() == cancelButton)
		res = MSGBOX_IDCANCEL;
	else if (msgBox.clickedButton() == yesButton)
		res = MSGBOX_IDYES;
	else if (msgBox.clickedButton() == noButton)
		res = MSGBOX_IDNO;
	return res;
}
