#ifndef COMMUNICATIONLINE_H
#define COMMUNICATIONLINE_H

#include "config.h"

#include <QObject>
#include <QTimer>

class QHttp;
class QHttpResponseHeader;

class CommunicationLine : public QObject
{
	Q_OBJECT

public:
	CommunicationLine(QObject *parent);
	~CommunicationLine();

	bool Init();
	void Release();
	void sendRequest(int key, QString val = "", QString page = CONF_WEBPAGE_CONTROL);

	void startMonitorLogin()
	{
		if(!m_loginTimer->isActive()) 
			m_loginTimer->start(); 
	}

	void SetHost();

private:
	int      m_httpGetId;
	QString  m_thisUser;
	QHttp   *m_http;
	QTimer  *m_loginTimer;
	QTimer  *m_sessionTimer;
	
private slots:
	void monitorLogin();
	void monitorSession();
	void httpRequestFinished(int requestId, bool error);
};

#endif // COMMUNICATIONLINE_H
