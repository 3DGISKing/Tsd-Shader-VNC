/********************************************************************
	created:	2013/05/24
	created:	24:5:2013   9:34
	filename: 	e:\FileManage\FileManage\filetransferiteminfo.h
	file path:	e:\FileManage\FileManage
	file base:	filetransferiteminfo
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/


#ifndef FILETRANSFERITEMINFO_H
#define FILETRANSFERITEMINFO_H
#include <QtCore/QString>
#include <QtCore/QList>



typedef struct tagFTITEMINFO
{
    QString       Name;
	unsigned int Size;
	unsigned int Data;
} FTITEMINFO;

typedef struct tagFTSIZEDATA
{
	unsigned int size;
	unsigned int data;
} FTSIZEDATA;

class FileTransferItemInfo  
{
public:
	FileTransferItemInfo();
	virtual ~FileTransferItemInfo();


	int          getNumEntries();
	QString      getNameAt(int Number);
	unsigned int getSizeAt(int Number);
	unsigned int getDataAt(int Number);
	void         free();
	void         add(QString Name, unsigned int Size, unsigned int Data);
	int          getSummaryNamesLength();
	void         sort();
	bool         isFile(int Number);


private:
	QList<FTITEMINFO*>   m_pEntries;
	int                 m_NumEntries;
};



#endif