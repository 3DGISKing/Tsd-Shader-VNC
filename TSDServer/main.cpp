#include "tsdserver.h"
#include "tsdserverapp.h"
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QMessageBox>

int main(int argc, char *argv[])
{
	TSDServerApp a(argc, argv);

	if(a.isRunning())
		return 0;

	if (!QSystemTrayIcon::isSystemTrayAvailable()) 
	{
		QMessageBox::critical(0, QObject::tr("TSDServer"),
			QObject::tr("I couldn't detect any system tray on this system."));
		return 0;
	}

	a.setQuitOnLastWindowClosed(false);

	a.createTrayIcon();


	if(!a.startServer())
	{
		QMessageBox::critical(0, QObject::tr("TSDServer"),
			QObject::tr("Failed to start server!"));
	}
		
	
	return a.run();
}
