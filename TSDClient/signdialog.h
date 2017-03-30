#ifndef SIGNDIALOG_H
#define SIGNDIALOG_H

#include <QWidget>
#include "ui_signdialog.h"

class SignDialog : public QWidget
{
	Q_OBJECT

public:
	SignDialog(QWidget *parent = 0);
	~SignDialog();
	bool Init();
	void initialData();

	Ui::SignDialog ui;

private:
	bool CreateFrame(void);
	void SetIcon(void);
private slots:
		void Click_ButLogIn(); //asd add 2013.5.4
};

#endif // SIGNDIALOG_H
