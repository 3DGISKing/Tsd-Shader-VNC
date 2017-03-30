#ifndef GUIUTY_H
#define GUIUTY_H

#include <QWidget>
#include <QString>

//for messge box
#define MSGBOX_IDOK			0x0001
#define MSGBOX_IDCANCEL		0x0002
#define MSGBOX_IDOKCANCEL	(MSGBOX_IDOK | MSGBOX_IDCANCEL)
#define MSGBOX_IDYES		0x0004
#define MSGBOX_IDNO			0x0008
#define MSGBOX_IDYESNO		(MSGBOX_IDYES | MSGBOX_IDNO)

#define MSGBOX_ICONQUESTION	0x0100
#define MSGBOX_ICONINFO		0x0200
#define MSGBOX_ICONWARNING	0x0400
#define MSGBOX_ICONCRITICAL	0x0800

#define MSGBOX_ID(f)		(f & 0x00ff)
#define MSGBOX_ICON(f)		(f & 0xff00)

int gu_MessageBox(QWidget *parent, const QString &title, const QString &text, unsigned int flag = 0);


#define MAINWINDOW_ICON_NAME	"/images/tsdIcon.png"

#define DESKTOPIMAGE			"desktop.png"
#define NOTEBOOKIMAGE			"notebook.png"
#define TELEPHONEIMAGE			"telephone.png"

#define TOURBAR_BOTTOM_NARGIN      20


#define RENDERAREA_STYLE_FNAME		"/res/qss/renderarea.qss"
#define TOURBAR_STYLE_FNAME			"/res/qss/tourbar.qss"
#define PIN_STYLE_FNAME				"/res/qss/pin.qss"
#define MAINWINDOW_STYLE_FNAME		"/res/qss/tsdclient.qss"


#endif // GUIUTY_H