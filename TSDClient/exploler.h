/********************************************************************
	created:	2013/05/08
	created:	8:5:2013   22:44
	filename: 	exploler.h
	file path:	\
	file base:	exploler
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/

#ifndef EXPLOLER_H

#define EXPLOLER_H

#include <QtGui/QSplitter>

#include "addressbar.h"
#include "explolerclient.h"


class Exploler :public QSplitter
{
	Q_OBJECT

public:
	Exploler(QWidget* parent=0);
	~Exploler();

	void setCurrentPath(QString path);
	inline ExplolerClient* client(){return &m_ClientArea;}

private:
	AddressBar       m_AddressBar;
	ExplolerClient   m_ClientArea;

};
#endif