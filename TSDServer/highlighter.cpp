#include "highlighter.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QVector>
#include <QDesktopWidget>
#include <QApplication>

HighLighter::HighLighter(QWidget *parent)
	: QLabel(parent)
{
	QDesktopWidget* pDesktop = QApplication::desktop();
	m_nWidth = pDesktop->width();
	m_nHeight = pDesktop->height();

	setParent(NULL, 0);
	setAttribute(Qt::WA_NativeWindow);
	setMouseTracking(true);

	m_StrokeVector.clear();
	m_bStarted=false;
}

HighLighter::~HighLighter()
{

}

void HighLighter::addPointVector(QVector<QPoint> pointvec)
{
	m_StrokeVector.push_back(pointvec);
}

void HighLighter::end()
{
	m_StrokeVector.clear();
	hide();
	m_bStarted=false;
}

void HighLighter::start()
{
	m_bStarted=true;
	m_pixBackground = QPixmap::grabWindow(QApplication::desktop()->winId(), 0, 0, m_nWidth, m_nHeight);
	m_StrokeVector.clear();
	showFullScreen();
}

void HighLighter::paintEvent(QPaintEvent *  event )
{
	QPainter painter(this);
	QPen pen;
	pen.setColor(Qt::red);
	pen.setWidth(3);
	pen.setStyle(Qt::SolidLine);
	
	painter.setPen(pen);
	
	painter.drawImage(0, 0, m_pixBackground.toImage(), 0, 0, m_nWidth, m_nHeight);

	QPoint ptBegin, ptEnd;
	
	if(m_StrokeVector.size() != 0)
	{
		QVector<QVector<QPoint>>::Iterator strokeIter = m_StrokeVector.begin();
		for(;strokeIter != m_StrokeVector.end(); strokeIter++)
		{
			QVector<QPoint>::Iterator pointIter = strokeIter->begin();
			ptBegin = *pointIter;
			pointIter++;
			for(; pointIter != strokeIter->end(); pointIter++)
			{
				ptEnd = *pointIter;
				painter.drawLine(ptBegin, ptEnd);
				ptBegin = *pointIter;
			}
		}
	}
}

