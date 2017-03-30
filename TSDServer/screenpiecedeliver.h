/********************************************************************
	created:	2013/04/26
	created:	26:4:2013   21:38
	filename: 	screenpiecedeliver.h
	file path:	\
	file base:	screenpiecedeliver
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/

#ifndef SCREENPIECEDELIVER_H
#define SCREENPIECEDELIVER_H

#include <QtCore/QThread>
#include <QRect>
#include <QtCore/QMutex>

class TSDServer;

class ScreenPieceDeliver :public QThread			
{
	Q_OBJECT
public:
	ScreenPieceDeliver(QObject* parent,TSDServer* server,QRect rect);

	void finish() {m_IsFinish=true;}
protected:
	virtual void	run();

private:
#ifdef Q_OS_WIN
	void            run_win();
#else
	void            run_unix_mac();
#endif

private:
	QRect          m_ScreenPieceRect;    
	bool           m_IsFinish;
	QMutex         m_Mutex;
	TSDServer*     m_pServer;
};

#endif`
