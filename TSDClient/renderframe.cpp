
#include <QtGui/QLabel>

#include "renderframe.h"
#include "tsdclient.h"
#include "mainwindow.h"
#include "guiuty.h"
#include "leftframe.h"
#include "config.h"
#include "tsdclientconnection.h"

RenderArea::RenderArea(QWidget *parent)
: QLabel(parent)
{
	this->setMouseTracking(true);
	m_bHighLighter = false;
	m_bLeftButtonPress=false;
}

void RenderArea::paintEvent(QPaintEvent *  event )
{
	QPainter painter(this);
	
	float scale=m_pParent->GetScaleFactor();

	float gapwidth=m_pParent->m_GapWidth;

	float gapheight= m_pParent->m_GapHeight	;
	
	QRect rect,updaterect;
	float width,height;
	QImage *im;

	for(int i=0;i<m_pParent->m_Cols;i++)
		for(int j=0;j<m_pParent->m_Rows;j++)
		{
			im   = m_pParent->m_pImageTiles[j*m_pParent->m_Cols+i].GetImage();
			rect = m_pParent->m_pImageTiles[j*m_pParent->m_Cols+i].GetOriginalRect();

			updaterect.setLeft(gapwidth+rect.left()*scale);
			updaterect.setTop(gapheight+rect.top()*scale);

			updaterect.setWidth(rect.width()*scale+1);
			updaterect.setHeight(rect.height()*scale+1);
			
			painter.drawImage(updaterect,*im,
							  QRect(0,0,im->width(),im->height()));
		}

		if (m_bHighLighter)
		{
			painter.save();
			QPen pen;
			pen.setColor(Qt::red);
			pen.setWidth(3);
			pen.setStyle(Qt::SolidLine);
			painter.setPen(pen);

			QPoint ptBegin, ptEnd;
			if(m_PointVector.size() != 0)
			{
				QVector<QPoint>::Iterator pointIter = m_PointVector.begin();
				ptBegin = *pointIter;
				pointIter++;
				for(; pointIter != m_PointVector.end(); pointIter++)
				{
					ptEnd = *pointIter;
					painter.drawLine(ptBegin, ptEnd);
					ptBegin = *pointIter;
				}
			}
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

			painter.restore();
		}

	if(!m_pParent->m_RenderedString.isEmpty())
	{
		int x=m_pParent->width()/2;
		int y=m_pParent->height()/2;
		painter.drawText(x,y,m_pParent->m_RenderedString);
	}
}


void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
	if(event->x() < 2)
		g_clientapp->GetMainWindow()->GetLeftFrame()->show();
	
	if (m_bHighLighter && m_bLeftButtonPress)
	{	
		m_PointVector.push_back(event->pos());
		repaint();
	}
	
	TSDClientConnection* connection=g_clientapp->GetCurrentConnection();

	if(connection)
		connection->sendEvent(event);
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
	TSDClientConnection* connection=g_clientapp->GetCurrentConnection();

	if(connection)
		connection->sendEvent(event);

	if (m_bHighLighter && event->button()==Qt::LeftButton)
	{
		m_PointVector.clear();
		m_PointVector.push_back(event->pos());
		m_bLeftButtonPress=true;
	}
}

void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_bHighLighter && event->button()==Qt::LeftButton)
	{
		if(m_PointVector.size() > 1)
			m_StrokeVector.push_back(m_PointVector);
		repaint();
		m_bLeftButtonPress=false;

		g_clientapp->GetCurrentConnection()->sendHighLighterData(m_PointVector);
	}

	TSDClientConnection* connection=g_clientapp->GetCurrentConnection();

	if(connection)
		connection->sendEvent(event);
}

void RenderArea::mouseDoubleClickEvent(QMouseEvent *event)
{
	TSDClientConnection* connection=g_clientapp->GetCurrentConnection();

	if(connection)
		connection->sendEvent(event);
}
void RenderArea::wheelEvent(QWheelEvent *event)
{
	TSDClientConnection* connection=g_clientapp->GetCurrentConnection();

	if(connection)
		connection->sendEvent(event);
}

void RenderArea::EndHighlighter()
{
	QVector<QVector<QPoint>>::Iterator strokeIter = m_StrokeVector.begin();
	for(;strokeIter != m_StrokeVector.end(); strokeIter++)
	{
		strokeIter->clear();
	}
	m_StrokeVector.clear();
	m_PointVector.clear();
	repaint();
	m_bHighLighter = false;
}

void RenderArea::StartHighlighter()
{
	m_PointVector.clear();
	m_StrokeVector.clear();
	m_bHighLighter = true;
}

RenderFrame::RenderFrame(QWidget *parent)
: QScrollArea(parent)
{
	QString ssPath = g_clientapp->GetWorkDir() + RENDERAREA_STYLE_FNAME;
	QFile file(ssPath);
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	

	m_pRenderArea = new RenderArea();
	m_pRenderArea->setStyleSheet(styleSheet);
	m_pRenderArea->m_pParent=this;
	
	m_pRenderArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	m_pRenderArea->setScaledContents(true);

	this->setFrameShape(QFrame::NoFrame);
	this->setWidget(m_pRenderArea);

	m_Cols=0;
	m_Rows=0;
	m_pImageTiles=NULL;

	m_DisplaySize.setWidth(QApplication::desktop()->screenGeometry().width());
	m_DisplaySize.setHeight(QApplication::desktop()->screenGeometry().height());
}

bool RenderFrame::CreateImageTiles(int x,int y)
{
	m_Cols=x;
	m_Rows=y;
	m_pImageTiles=new Tile [x*y];

	for(int i=0;i<m_Cols;i++)
		for(int j=0;j<m_Rows;j++)
			m_pImageTiles[j*m_Cols+j].SetInfo(i,j);

	if(!m_pImageTiles) return false;

	return true;
}

void RenderFrame::RenderString(QString info)
{
	m_RenderedString=info;
	m_pRenderArea->update();
}

void RenderFrame::DeleteImageTiles()
{
	if(m_pImageTiles)
		delete [] m_pImageTiles;
	m_pImageTiles=NULL;
	m_pRenderArea->update();
	m_Cols=0;
	m_Rows=0;
}

void RenderFrame::ResizeRenderArea()
{
	float width=m_RCControlledDisplaySize.width();
	float height=m_RCControlledDisplaySize.height();

	width=width*this->m_ScaleFactor;
	height=height*this->m_ScaleFactor;

	if(width<this->width())
		width=this->width();

	if(height<this->height())
		height=this->height();

	m_pRenderArea->resize(width,height);
}

void RenderFrame::keyPressEvent(QKeyEvent *event)
{
	TSDClientConnection* connection=g_clientapp->GetCurrentConnection();

	if(connection)
		connection->sendEvent(event);
}

void RenderFrame::keyReleaseEvent(QKeyEvent *event)
{
	TSDClientConnection* connection=g_clientapp->GetCurrentConnection();

	if(connection)
		connection->sendEvent(event);
}

bool RenderFrame::TransformCoord(int& x,int &y)
{
	if(x<m_GapWidth) return false;

	if(x>m_GapWidth+(float)(m_RCControlledDisplaySize.width())*m_ScaleFactor) return false;

	if(y<m_GapHeight) return false;

	if(y>m_GapHeight+m_RCControlledDisplaySize.height()*m_ScaleFactor) return false;

	x=x-m_GapWidth;
	x=(float)x/m_ScaleFactor;

	y=y-m_GapHeight;
	y=(float)y/m_ScaleFactor;

	return true;
}

void RenderFrame::SetControledDisplaySize(int w,int h)
{
	m_RCControlledDisplaySize.setWidth(w);
	m_RCControlledDisplaySize.setHeight(h);
	ResizeFrame();
}

void RenderFrame::UpdateFrame(QImage * im,QRect rect)
{
	int col=ceil((float)rect.left()/(float)rect.width());
	int row=ceil((float)rect.top()/(float)rect.height());

	if(!m_pImageTiles) return ;

	Q_ASSERT(m_Cols*row+col<m_Cols*m_Rows);

	m_pImageTiles[m_Cols*row+col].SetImage(im,rect);
	m_pRenderArea->update();
}

RenderFrame::~RenderFrame()
{
	delete m_pRenderArea;
}


void RenderFrame::resizeEvent(QResizeEvent *event)
{
	m_pRenderArea->setGeometry(0,0,width(),height());
	ResizeFrame();
}

void RenderFrame::ResizeFrame()
{
	float myaspect=(float)this->width()/(float)this->height();
	float remoteaspect=(float)m_RCControlledDisplaySize.width()/(float)m_RCControlledDisplaySize.height();

	if(myaspect<remoteaspect)
	{
		m_ScaleFactor=(float)this->width()/(float)m_DisplaySize.width();
		m_GapWidth=0;
		m_GapHeight=(float)this->height()-(float)m_ScaleFactor*(float)m_RCControlledDisplaySize.height();
		m_GapHeight=m_GapHeight/2.0;
	}
	else
	{
		m_ScaleFactor=(float)this->height()/(float)m_DisplaySize.height();
		m_GapHeight=0;
		m_GapWidth=(float)this->width()-(float)m_ScaleFactor*(float)m_RCControlledDisplaySize.width();
		m_GapWidth=m_GapWidth/2.0;	
	}
}

