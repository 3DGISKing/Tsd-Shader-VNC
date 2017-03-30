#ifndef CHATDLG_H
#define CHATDLG_H

#include <QWidget>
#include "ui_chatdlg.h"

class ChatDlg : public QWidget
{
	Q_OBJECT

public:
	ChatDlg(QWidget *parent = 0);
	~ChatDlg();


	void    SetWorkDir(QString workDir);
	void    UpdateDlg();

	void    OnChatClear();
	
	void    AppendChatMessage(QString  user,  QString  strtime, QString  message);
	void    NewChatMessage(QString user,QString time,QString msg);

private:
	Ui::ChatDlgClass ui;

	QString m_workdir;
	QTextTableFormat			tableFormat;

public slots:

	void OnChatMinimize();
	void OnChatMaximize();
	void OnChatSend();
};

#endif // CHATDLG_H
