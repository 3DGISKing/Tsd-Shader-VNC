#include "tsdsocket.h"

TSDSocket::TSDSocket(QObject *parent)
	: QTcpSocket(parent)
{

}

TSDSocket::~TSDSocket()
{

}

bool TSDSocket::peekInt(int & data)
{
	int available_bytes=this->bytesAvailable();

loop:
	if(available_bytes<sizeof(data))
	{
		if(!this->waitForReadyRead());
			return false;
		goto loop;
	}
	
	int read_bytes=this->peek((char*)&data,sizeof(int));

	if(read_bytes==-1) return false;
	return true;
}

bool TSDSocket::readExact(char *inbuf, int wanted)
{
	int read_bytes;
	int	total_read_bytes=0;
	int available_bytes;

	while(total_read_bytes<wanted)
	{
		available_bytes=this->bytesAvailable();

		if(available_bytes>=wanted-total_read_bytes)
			read_bytes=this->read(inbuf+total_read_bytes,wanted-total_read_bytes);
		else
		{
			read_bytes=this->read(inbuf+total_read_bytes,wanted-total_read_bytes);
			
			if(!waitForReadyRead())
				return false;
		}

		if (read_bytes==-1) 
			return false ;
		total_read_bytes=total_read_bytes+read_bytes;
	}

	return true;
}


bool TSDSocket::writeExact(char *outbuf, int wanted)
{
	int write_bytes;
	int	total_write_bytes=0;
	
	while(total_write_bytes<wanted)
	{
		write_bytes=this->write(outbuf,wanted-total_write_bytes);

		if (write_bytes==-1) 
			return false ;

		total_write_bytes=total_write_bytes+write_bytes;
	}
	this->flush();
	return true;
}