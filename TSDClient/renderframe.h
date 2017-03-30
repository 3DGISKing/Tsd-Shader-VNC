#ifndef RENDERFRAME_H
#define RENDERFRAME_H

#include <QScrollArea>
#include <QtGui/QLabel>
#include <QtCore/QRect>
#include <QtGui/QImage>

class RenderFrame;

class Tile 
{
public:
	Tile()  {m_pImage=new QImage();}
	~Tile() {delete m_pImage;}
	
	inline void		SetOriginalRect(QRect r){m_OriginalRect=r;}
	inline void		SetInfo(int x,int y){m_XNumber=x;m_YNumber=y;}

	inline QRect    GetOriginalRect(){return m_OriginalRect;}
	inline int		GetXGridNumber(){return m_XNumber;}
	inline int		GetYGridNumber(){return m_YNumber;}
	inline QImage*  GetImage()       {return m_pImage;}
	inline void     SetImage(QImage *im)       {*m_pImage=*im;} 
	inline void     SetImage(QImage *im,QRect r){*m_pImage=*im;m_OriginalRect=r;} 
private:
	int				m_XNumber, m_YNumber;
	QRect			m_OriginalRect;
	QImage *		m_pImage;
};

class RenderArea : public QLabel
{
	Q_OBJECT
public:
	RenderArea(QWidget *parent = 0);
	
	void				EndHighlighter();
	void				StartHighlighter();

	inline bool			GetHighlighter()                {	return m_bHighLighter;}
	inline void			SetHighlighter(bool highlighter){m_bHighLighter = highlighter;}

protected:
	virtual void		paintEvent(QPaintEvent *event);
	virtual void		mouseMoveEvent(QMouseEvent *event);
	virtual void		mousePressEvent(QMouseEvent *event);
	virtual void		mouseReleaseEvent(QMouseEvent *event);
	virtual void		mouseDoubleClickEvent(QMouseEvent *event);
	virtual void		wheelEvent(QWheelEvent *event);

private:
	RenderFrame*	    	 m_pParent; 

	bool					 m_bHighLighter;
	bool                     m_bLeftButtonPress;

	QVector<QVector<QPoint>> m_StrokeVector;
	QVector<QPoint>          m_PointVector;
	
	friend class RenderFrame;
};

class RenderFrame : public QScrollArea
{
	Q_OBJECT

public:
	RenderFrame(QWidget *parent = 0);
	~RenderFrame();

	inline RenderArea*  GetRenderArea(){return m_pRenderArea;}
	
	inline float	    GetScaleFactor() {return m_ScaleFactor;}
	inline void		    SetScaleFactor(float val) {m_ScaleFactor=val;ResizeRenderArea();}

	void				ResizeFrame();
	void				UpdateFrame(QImage * im,QRect rect);

	bool				TransformCoord(int& x,int &y);

	void				SetControledDisplaySize(int w,int h);
	bool				CreateImageTiles(int x,int y);
	void				DeleteImageTiles();
	void                RenderString(QString info);

protected:
	virtual void	    keyPressEvent(QKeyEvent *event);
	virtual void	    keyReleaseEvent(QKeyEvent *event);
	virtual void        resizeEvent(QResizeEvent *event);

private:
	void				ResizeRenderArea();

	RenderArea *		m_pRenderArea;
	QSize				m_DisplaySize,m_RCControlledDisplaySize;
            
	float				m_ScaleFactor;   
	float               m_GapWidth,m_GapHeight;

	Tile*				m_pImageTiles;   
	int					m_Cols,m_Rows;
	QString				m_RenderedString;

    

	friend class RenderArea;
};

#endif // RENDERFRAME_H
