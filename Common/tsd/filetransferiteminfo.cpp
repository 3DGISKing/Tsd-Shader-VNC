/********************************************************************
	created:	2013/05/24
	created:	24:5:2013   9:34
	filename: 	e:\FileManage\FileManage\filetransferiteminfo.cpp
	file path:	e:\FileManage\FileManage
	file base:	filetransferiteminfo
	file ext:	cpp
	author:		ugi
	
	purpose:	
*********************************************************************/

#include "filetransferiteminfo.h"

bool compareSize(const FTITEMINFO* a,const FTITEMINFO* b)
{
	if(a->Size>b->Size)
		return true;

	return false;
}


FileTransferItemInfo::FileTransferItemInfo()
{
	m_NumEntries = 0;
}



bool FileTransferItemInfo::isFile(int Number)
{
	if ((Number < 0) && (Number > m_NumEntries)) 
		return false;

	if (m_pEntries[Number]->Size==-1 || m_pEntries[Number]->Size==-2) 
		return false;
	return true;
}

FileTransferItemInfo::~FileTransferItemInfo()
{
	free();
}

void FileTransferItemInfo::sort()
{
	qSort(m_pEntries.begin(),m_pEntries.end(),compareSize);
}
void FileTransferItemInfo::add(QString Name, unsigned int Size, unsigned int Data)
{
	FTITEMINFO* info=new FTITEMINFO;

	info->Name=Name;

	info->Size=Size;
	info->Data=Data;

	m_pEntries.push_back(info);

	m_NumEntries++;
}

void FileTransferItemInfo::free()
{

	FTITEMINFO* info;

	foreach(info,m_pEntries)
		delete info;

	m_pEntries.clear();
	m_NumEntries = 0;
}

QString FileTransferItemInfo::getNameAt(int Number)
{
	if ((Number >= 0) && (Number <= m_NumEntries))
		return m_pEntries[Number]->Name;
	return NULL;
}

unsigned int FileTransferItemInfo::getSizeAt(int Number)
{
	if ((Number >= 0) && (Number <= m_NumEntries)) 
		return m_pEntries[Number]->Size; 
	return 0;
}

unsigned int FileTransferItemInfo::getDataAt(int Number)
{
	if ((Number >= 0) && (Number <= m_NumEntries)) 
		return m_pEntries[Number]->Data; 
	return 0;
}

int FileTransferItemInfo::getNumEntries()
{
	return m_NumEntries;
}

int FileTransferItemInfo::getSummaryNamesLength()
{
	int sumLen = 0;

	for (int i = 0; i < m_NumEntries; i++)
	{
		QByteArray data;

		data=m_pEntries[i]->Name.toUtf8();

		sumLen += data.size();
	}
	return sumLen;
}





