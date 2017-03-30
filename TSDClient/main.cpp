#include <QtGui>
#include <QtGui/QApplication>
#include "tsdclient.h"
#include "msgdataclient.h"
#include "guiuty.h"
#include "mainwindow.h"

TSDClient *g_clientapp;

int main(int argc, char *argv[])
{
	TSDClient a(argc, argv);

	g_clientapp = &a;

	if (a.isRunning())
		return 0;
	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("TSDClient"),
			QObject::tr("I couldn't detect any system tray "
			"on this system."));
		return 1;
	}
	a.setQuitOnLastWindowClosed(false);
	if (!a.Init())
	{
		gu_MessageBox(NULL, MSG_ERR_TITLE, MSG_ERR_INITERROR, MSGBOX_ICONCRITICAL);
		return 0;
	}
	MainWindow mainWnd;

	if (!mainWnd.Init())
	{
		gu_MessageBox(NULL, MSG_ERR_TITLE, MSG_ERR_INITERROR, MSGBOX_ICONCRITICAL);
		mainWnd.Release();
		return 0;
	}

	return a.Run();
}