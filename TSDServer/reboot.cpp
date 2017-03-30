/********************************************************************
	created:	2013/05/19
	created:	19:5:2013   16:09
	filename: 	d:\TSDClient\TSDClient\reboot.cpp
	file path:	d:\TSDClient\TSDClient
	file base:	reboot
	file ext:	cpp
	author:		ugi
	
	purpose:	
*********************************************************************/

#include "reboot.h"
#include <qglobal.h>

#ifdef Q_OS_WIN

#include <Windows.h>

BOOL windowsReboot()
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 

	// Get the LUID for the shutdown privilege. 

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	if (GetLastError() != ERROR_SUCCESS) 
		return FALSE; 

	// Shut down the system and force all applications to close. 

	if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 
		SHTDN_REASON_MAJOR_OPERATINGSYSTEM )) 
		return FALSE; 

	return TRUE;
}
#endif

#ifdef Q_OS_LINUX
	#include <QProcess>
	#include <QStringList>
	
	void linuxReboot()
	{

		/*
			QStringList arg<<"-r"<<"now";
			QProcess::execute("shutdown",arg);
		*/
		QProcess::execute("reboot");
	}
#endif

bool systemReboot()
{
#ifdef Q_OS_WIN
	BOOL ret=windowsReboot();

	if(ret) 
		return true;
	else
		return false;
#endif

#ifdef Q_OS_LINUX
	linuxReboot();
#endif
}

