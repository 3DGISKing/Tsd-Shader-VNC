/********************************************************************
	created:	2013/05/08
	created:	8:5:2013   22:46
	filename: 	exploler.cpp
	file path:	\
	file base:	exploler
	file ext:	cpp
	author:		ugi
	
	purpose:	
*********************************************************************/

#include "exploler.h"
#include "filemanager.h"

Exploler::Exploler(QWidget* parent/* =0 */)
:QSplitter(parent)
{
	m_ClientArea.setParent(this);
	setOrientation(Qt::Vertical);
	m_AddressBar.setClient(&m_ClientArea);
	this->addWidget(&m_AddressBar);
	this->addWidget(&m_ClientArea);
}

Exploler::~Exploler()
{

}

void Exploler::setCurrentPath(QString path)
{
	if(path.right(1)!="/")	//check last character
		path=path+"/";
	m_AddressBar.setCurrentPath(path);
	g_FileManager->setClientPath(path);
}