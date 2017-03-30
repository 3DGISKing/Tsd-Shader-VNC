/********************************************************************
	created:	2013/05/07
	created:	7:5:2013   14:01
	filename: 	systeminfo.h
	file path:	\
	file base:	systeminfo
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/
#ifndef  SYSTEMINFO_H

#define SYSTEMINFO_H

#include <QtCore/QString>

class SystemInfo
{
public:
	static QString    getSystemInfo();
	static QString    getSystemVersion();
	static bool       getCPUFrequencyAndVendorIdentifier(int& MHz,QString& vendor);
	static void       getCPUTypeCountArchitectureLevelRevision(QString& type,int& count,QString& archi,QString& level,QString& revision);
	static float      getRAMCapacityKB();
	static float      getRAMCapacityMB();
	static float      getRAMCapacityGB();
	static float      getRAMCapacityBytes();
	static QString    getUserName();
	static QString    getComputerName();
	static QString    getProcessorArchitecture_s();
	static QString    getProcessorIdentifier_s();
	static QString    getProcessorLevel_s();
	static QString    getProcessorRevision_s();
	static int        getProcessorCount_s();
	static QString    getSystem_s();
};

#endif


