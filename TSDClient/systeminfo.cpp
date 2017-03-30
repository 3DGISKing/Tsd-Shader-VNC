#include "systeminfo.h"
#include <QtCore/QSysInfo>
#include <QtCore/QStringList>
#include <QtCore/QProcess>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <tchar.h>

#endif


#define UNKNOWN "Unknown"

QString SystemInfo::getSystemInfo()
{
	QString system = getSystemVersion() + "\n\n";

	int MHz;
	QString vendor;

	if(!getCPUFrequencyAndVendorIdentifier(MHz, vendor))
	{
		MHz=0;
		vendor=UNKNOWN;
	}

	int processorcount;
	QString cputype,processorarchitecture,cpulevel,cpurevision;

	getCPUTypeCountArchitectureLevelRevision(cputype, processorcount, processorarchitecture, cpulevel, cpurevision);

	QString   processor("Processor : ");
	processor  +=  cputype + " " + cpurevision + "  CPU " + QString("  x %1       ").arg(processorcount);
	processor	+=   QString("%1GHz").arg(MHz)+"\n";

	QString ramsize;
	ramsize.setNum( getRAMCapacityMB(), 'g', 4 ); ramsize = "RAM :  " + ramsize; 

	QString device;

	device = processor + "\n" + ramsize;

	QString user;

	user = "Computer : " + getComputerName() + "\n\n" + "User Name : " +getUserName();

	return system + "," +device + "," +user;
}

QString SystemInfo::getSystemVersion()
{
#ifdef Q_OS_WIN
	QSysInfo::WinVersion version=QSysInfo::windowsVersion();	

	switch(version)
	{
	case QSysInfo::WV_NT:
		return "Windows NT (operating system version 4.0)";
	case QSysInfo::WV_2000:
		return "Windows 2000 (operating system version 5.0)";
	case QSysInfo::WV_XP:
		return "Windows XP (operating system version 5.1)";
	case QSysInfo::WV_2003:
		return "Windows Server 2003, Windows Server 2003 R2, Windows Home Server, Windows XP Professional x64 Edition (operating system version 5.2)";
	case QSysInfo::WV_VISTA:
		return "Windows Vista, Windows Server 2008 (operating system version 6.0)";
	case QSysInfo::WV_WINDOWS7:
		return "Windows 7, Windows Server 2008 R2 (operating system version 6.1)";
	case QSysInfo::WV_CE:
		return "Windows CE";
	case QSysInfo::WV_CENET:
		return "Windows CE .NET";
	case QSysInfo::WV_CE_5:
		return "Windows CE 5.x";
	case QSysInfo::WV_CE_6:
		return "Windows CE 6.x";
	default:
		return "Unrecognized Windows";
	}
#endif
#ifdef Q_OS_MAC
	QSysInfo::MacVersion version=QSysinfo::MacVersion();

	switch(version)
	{
	case QSysInfo::MV_9:
		return "Mac OS 9 (unsupported)";
	case QSysInfo::MV_10_0:
		return "Mac OS X 10.0 (unsupported)";
	case QSysInfo::MV_10_1:
		return "Mac OS X 10.1 (unsupported)";
	case QSysInfo::MV_10_2:
		return "Mac OS X 10.2 (unsupported)";
	case QSysInfo::MV_10_3:
		return "Mac OS X 10.3";
	case QSysInfo::MV_10_4:
		return "Mac OS X 10.4";
	case QSysInfo::MV_10_5:
		return "Mac OS X 10.5";
	case QSysInfo::MV_10_6
		return 	"Mac OS X 10.6";
	case QSysInfo::MV_Unknown:
		return "An unknown and currently unsupported platform";
	default:
		return "Unrecognized MAC System";
	}
#endif
}
#
bool SystemInfo::getCPUFrequencyAndVendorIdentifier(int& MHz,QString& vendor)
{
#ifdef Q_OS_WIN

	LONG result;
	HKEY hKey;
	DWORD data, dataSize;
	WCHAR vendorData [64];

	// Get the processor speed info.
	result = ::RegOpenKeyEx (HKEY_LOCAL_MACHINE,
		L"Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey);

	if(result==ERROR_SUCCESS)
	{
		dataSize=MAX_PATH;
		result = ::RegQueryValueEx (hKey, L"~MHz", NULL, NULL,
			(LPBYTE)&data, &dataSize);

		if(result!=ERROR_SUCCESS) return false;
		
		MHz=data;

		dataSize = sizeof (vendorData);

		result = ::RegQueryValueEx (hKey, L"VendorIdentifier", NULL, NULL,
			(LPBYTE)vendorData, &dataSize);

		if(result!=ERROR_SUCCESS) return false;

		vendor=QString::fromWCharArray(vendorData);
		RegCloseKey (hKey);

	}
	else
		return false;
	
#endif
	return true;


	


}

void SystemInfo::getCPUTypeCountArchitectureLevelRevision(QString &cputype,int & processorcount,QString& processorarchitecture,QString& cpulevel,QString & cpurevision)
{
#ifdef Q_OS_WIN
	SYSTEM_INFO sysInfo;

	::GetSystemInfo (&sysInfo);

	if (sysInfo.dwProcessorType  == PROCESSOR_INTEL_386)
		cputype = ("Intel 386");
	else if (sysInfo.dwProcessorType  == PROCESSOR_INTEL_486)
		cputype =  ("Intel 486");
	else if (sysInfo.dwProcessorType  == PROCESSOR_INTEL_PENTIUM)
		cputype =  ("Intel Pentium");
	else if (sysInfo.dwProcessorType  == PROCESSOR_MIPS_R4000)
		cputype =  ("MIPS");
	else if (sysInfo.dwProcessorType  == PROCESSOR_ALPHA_21064)
		cputype =  ("Alpha");
	else
		cputype =  ("Unknown");

	processorcount=sysInfo.dwNumberOfProcessors;

	QString tmpStr;
	char str [MAX_PATH];

	if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
	{
		processorarchitecture =  ("Pentium");
		switch (sysInfo.wProcessorLevel)
		{
		case 3:
			cpulevel = ("Intel 80386");
			cpurevision = HIBYTE (sysInfo.wProcessorRevision);
			cpurevision +=  ("A");
			tmpStr = LOBYTE (sysInfo.wProcessorRevision);
			cpurevision += tmpStr;
			
			break;
		case 4:
			cpulevel =  ("Intel 80486");
			{
				cpurevision = HIBYTE (sysInfo.wProcessorRevision);
				cpurevision +=  ("A");
				tmpStr = LOBYTE (sysInfo.wProcessorRevision);
				cpurevision += tmpStr;
			}
			break;
		case 5:
			cpulevel =  ("Pentium");
			// Check if the MMX instruction set is availbale or not.

			if (IsProcessorFeaturePresent (PF_MMX_INSTRUCTIONS_AVAILABLE))
			{
				cpulevel +=  (" MMX");
			}

			
			{
				cpurevision =  ("Model ");
				sprintf_s (str, "%d", HIBYTE (sysInfo.wProcessorRevision));
				tmpStr = str;
				cpurevision.append (tmpStr);
				cpurevision.append ( (", Stepping "));
				sprintf_s (str, "%d", LOBYTE (sysInfo.wProcessorRevision));
				tmpStr = str;
				cpurevision += tmpStr;
			}
			break;
		case 6:
			cpulevel =  ("Pentium (II/Pro)");
			break;
		}
	}
	else if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_MIPS)
	{
		processorarchitecture = ("MIPS");
		if (sysInfo.wProcessorLevel == 0004) {
			cpulevel =  ("MIPS R4000");
		}
		else {
			cpulevel =  ("Unknown");
		}
	}
	else if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA)
	{
		processorarchitecture =  ("Alpha");

		_itoa_s(sysInfo.wProcessorLevel , str, 10);

		tmpStr = processorarchitecture;
		tmpStr.append (str);
		cpulevel.append (tmpStr);
	}
	else if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_PPC)
	{
		processorarchitecture =  ("PPC");

		switch (sysInfo.wProcessorLevel)
		{
		case 1:
			cpulevel =  ("PPC 601");
			break;
		case 3:
			cpulevel =  ("PPC 603");
			break;
		case 4:
			cpulevel =  ("PPC 604");
			break;
		case 6:
			cpulevel =  ("PPC 603+");
			break;
		case 9:
			cpulevel =  ("PPC 604+");
			break;
		case 20:
			cpulevel =  ("PPC 620");
			break;
		}
	}
	else if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_UNKNOWN)
	{
		processorarchitecture =  ("Unknown");
	}
#endif
}

float SystemInfo::getRAMCapacityBytes()
{
#ifdef Q_OS_WIN
	MEMORYSTATUS memoryStatus;

	memset (&memoryStatus, sizeof (MEMORYSTATUS), 0);
	memoryStatus.dwLength = sizeof (MEMORYSTATUS);

	::GlobalMemoryStatus (&memoryStatus);

	return (float) memoryStatus.dwTotalPhys;
#endif	
}


float SystemInfo::getRAMCapacityKB()
{

	return getRAMCapacityBytes()/(1024);

}

float SystemInfo::getRAMCapacityMB()
{

	return getRAMCapacityBytes()/(1024*1024);

}


float SystemInfo::getRAMCapacityGB()
{

	return getRAMCapacityBytes()/(1024*1024*1024);

}

QString SystemInfo::getUserName()
{
	QString username;

	QStringList envVariables;
	envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
		<< "HOSTNAME.*" << "DOMAINNAME.*";

	QStringList environment = QProcess::systemEnvironment();
	foreach (QString string, envVariables) {
		int index = environment.indexOf(QRegExp(string));
		if (index != -1) {
			QStringList stringList = environment.at(index).split('=');
			if (stringList.size() == 2) {
				username = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	if (username.isEmpty())
#ifndef Q_OS_SYMBIAN
		username = "unknown";
#else
		username = "QtS60";
#endif
	return username;
}

QString SystemInfo::getComputerName()
{
	QString name;

	QStringList envVariables;
	envVariables << "COMPUTERNAME.*";
		

	QStringList environment = QProcess::systemEnvironment();
	foreach (QString string, envVariables) {
		int index = environment.indexOf(QRegExp(string));
		if (index != -1) {
			QStringList stringList = environment.at(index).split('=');
			if (stringList.size() == 2) {
				name = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	if (name.isEmpty())
		name = "unknown";

	return name;
}


QString SystemInfo::getProcessorArchitecture_s()
{
	QString name;

	QStringList envVariables;
	envVariables << "PROCESSOR_ARCHITECTURE.*";


	QStringList environment = QProcess::systemEnvironment();
	foreach (QString string, envVariables) {
		int index = environment.indexOf(QRegExp(string));
		if (index != -1) {
			QStringList stringList = environment.at(index).split('=');
			if (stringList.size() == 2) {
				name = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	if (name.isEmpty())
		name = "unknown";

	return name;
}

QString SystemInfo::getProcessorIdentifier_s()
{
	QString name;

	QStringList envVariables;
	envVariables << "PROCESSOR_IDENTIFIER.*";


	QStringList environment = QProcess::systemEnvironment();
	foreach (QString string, envVariables) {
		int index = environment.indexOf(QRegExp(string));
		if (index != -1) {
			QStringList stringList = environment.at(index).split('=');
			if (stringList.size() == 2) {
				name = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	if (name.isEmpty())
		name = "unknown";

	return name;
}

QString SystemInfo::getProcessorLevel_s()
{
	QString name;

	QStringList envVariables;
	envVariables << "PROCESSOR_LEVEL.*";


	QStringList environment = QProcess::systemEnvironment();
	foreach (QString string, envVariables) {
		int index = environment.indexOf(QRegExp(string));
		if (index != -1) {
			QStringList stringList = environment.at(index).split('=');
			if (stringList.size() == 2) {
				name = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	if (name.isEmpty())
		name = "unknown";

	return name;
}

QString SystemInfo::getProcessorRevision_s()
{
	QString name;

	QStringList envVariables;
	envVariables << "PROCESSOR_REVISION.*";


	QStringList environment = QProcess::systemEnvironment();
	foreach (QString string, envVariables) {
		int index = environment.indexOf(QRegExp(string));
		if (index != -1) {
			QStringList stringList = environment.at(index).split('=');
			if (stringList.size() == 2) {
				name = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	if (name.isEmpty())
		name = "unknown";

	return name;
}

QString SystemInfo::getSystem_s()
{
	QString name;

	QStringList envVariables;
	envVariables << "OS.*";


	QStringList environment = QProcess::systemEnvironment();
	foreach (QString string, envVariables) {
		int index = environment.indexOf(QRegExp(string));
		if (index != -1) {
			QStringList stringList = environment.at(index).split('=');
			if (stringList.size() == 2) {
				name = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	if (name.isEmpty())
		name = "unknown";

	return name;
}


int SystemInfo::getProcessorCount_s()
{
	QString name;

	QStringList envVariables;
	envVariables << "NUMBER_OF_PROCESSOR.*";


	QStringList environment = QProcess::systemEnvironment();
	foreach (QString string, envVariables) {
		int index = environment.indexOf(QRegExp(string));
		if (index != -1) {
			QStringList stringList = environment.at(index).split('=');
			if (stringList.size() == 2) {
				name = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	if (name.isEmpty())
		name = "unknown";

	return name.toInt();
}




