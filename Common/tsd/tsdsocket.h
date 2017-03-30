#ifndef TSDSOCKET_H
#define TSDSOCKET_H

#include <QTcpSocket>

class TSDSocket : public QTcpSocket
{
	Q_OBJECT

public:
	TSDSocket(QObject *parent=NULL);
	~TSDSocket();


	bool readExact(char *inbuf, int wanted);
	bool writeExact(char *outbuf, int wanted);

	bool peekInt(int & data);

private:
	
};

#endif // TSDSOCKET_H
