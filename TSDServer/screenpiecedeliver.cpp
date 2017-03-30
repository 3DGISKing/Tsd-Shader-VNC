/********************************************************************
created:	2013/04/26
created:	26:4:2013   21:39
filename:   screenpiecedeliver.cpp
file path:	\
file base:	screenpiecedeliver
file ext:	cpp
author:		ugi

purpose:	
*********************************************************************/
#include "screenpiecedeliver.h"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPixmap>
#include <QtCore/QBuffer>
#include "tsdserver.h"



#ifdef Q_OS_WIN
#define _AFXDLL
#include <afxole.h>
#include <atlimage.h>
#endif

ScreenPieceDeliver::ScreenPieceDeliver(QObject* parent,TSDServer* server,QRect rect)
:QThread(parent)
{
	m_pServer=server;
	m_ScreenPieceRect=rect;
	m_IsFinish=false;
	connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}

//#define SCREENPIECEDELIVER_TEST

#ifdef SCREENPIECEDELIVER_TEST
#include <QtCore/QTime>
#include <QImage>

#endif
#ifdef Q_OS_WIN

void ScreenPieceDeliver::run_win()
{
	int left=m_ScreenPieceRect.left();
	int top=m_ScreenPieceRect.top();
	int width=m_ScreenPieceRect.width();
	int height=m_ScreenPieceRect.height();

	COleStreamFile ImageStream;
	HGLOBAL   hImageBuffer = NULL;
	IStream * pImageStream = NULL;

	CImage   Image;

	HDC display_dc = GetDC(0);
	HDC bitmap_dc = CreateCompatibleDC(display_dc);
	HBITMAP bitmap = CreateCompatibleBitmap(display_dc, m_ScreenPieceRect.width(), m_ScreenPieceRect.height());

	HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);

	HDC window_dc = GetDC(GetDesktopWindow());

	int outbytes;
	char* poutbuffer;

	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = width;
	bmi.bmiHeader.biHeight      = height;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biBitCount    = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage   = width * height * 4;

	uchar *previous = (uchar *) qMalloc(bmi.bmiHeader.biSizeImage);
	uchar *current = (uchar *) qMalloc(bmi.bmiHeader.biSizeImage);

	while(true)
	{
		if(m_IsFinish)
			break;

		g_Mutex.lock();
		if(m_pServer->clientCount()==0)
		{
			g_Mutex.unlock();
			msleep(100);
			continue;
		}

		if(g_bStopScreenCapture)
			g_StopScreenCaptureCond.wait(&g_Mutex);
		g_Mutex.unlock();


#ifdef SCREENPIECEDELIVER_TEST
		QTime t;
		t.start();
#endif
		BitBlt(bitmap_dc, 0, 0, width, height, window_dc,left, top, SRCCOPY);

		if (GetDIBits(display_dc, bitmap, 0, height, current, &bmi, DIB_RGB_COLORS)) 
		{
			if(memcmp(current,previous,bmi.bmiHeader.biSizeImage)==0)
			{
				msleep(50);
				continue;
			}
			else
				memcpy(previous,current,bmi.bmiHeader.biSizeImage);
		}

		Image.Attach(bitmap);

		if(ImageStream.CreateMemoryStream())
		{
			pImageStream = ImageStream.GetStream();

			if (SUCCEEDED(Image.Save(pImageStream,Gdiplus::ImageFormatJPEG)))
			{
				outbytes = (DWORD)ImageStream.GetLength();

				if (SUCCEEDED(GetHGlobalFromStream(pImageStream,&hImageBuffer)))
					poutbuffer = (char *)GlobalLock(hImageBuffer);
			}
		}

		m_pServer->updateDisplayInfo(poutbuffer,outbytes,m_ScreenPieceRect);
	
		Image.Detach();

		GlobalUnlock(hImageBuffer);

		pImageStream->Release();

#ifdef SCREENPIECEDELIVER_TEST
		qDebug("Capture  time %d ms thread id %d",t.elapsed(), QThread::currentThread());
#endif
	   msleep(100);
	}

	ReleaseDC(GetDesktopWindow(), window_dc);
	SelectObject(bitmap_dc, null_bitmap);
	DeleteDC(bitmap_dc);
	
	delete [] current;
	delete [] previous;
}

#else

void ScreenPieceDeliver::run_unix_mac()
{
	QImage prevpiece,curpiece;
	QBuffer buffer;

	int left=m_ScreenPieceRect.left();
	int top=m_ScreenPieceRect.top();
	int width=m_ScreenPieceRect.width();
	int height=m_ScreenPieceRect.height();

	while(true)
	{
		if(m_IsFinish) return;
		if(m_pParent->getConnectionCount()==0)
		{
			msleep(50);
			continue;
		}

		g_FullScreenImageMutex.lock();

		curpiece=g_FullScreenImage.copy(left,top,width,	height);

		g_FullScreenImageMutex.unlock();

		if(curpiece==prevpiece)
		{
			msleep(50);
			continue;
		}

		if(!buffer.open(QIODevice::WriteOnly)) 
			continue;

		curpiece.save(&buffer,IMAGE_FORMAT);

		buffer.close();
		
		m_pParent->sendImageData(buffer.data().data(),buffer.size(),m_ScreenPieceRect);
		prevpiece=curpiece;
		msleep(50);
	}
}

#endif
void ScreenPieceDeliver::run()
{
#ifdef Q_OS_WIN
	run_win();
#else
	run_unix_mac();
#endif
}
