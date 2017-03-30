#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QLabel>
#include <QVector>

class HighLighter : public QLabel
{
	Q_OBJECT

public:
	HighLighter(QWidget *parent);
	~HighLighter();

	void addPointVector(QVector<QPoint> pointvec);
	void start();
	void end();
	inline bool started() {return m_bStarted;}

protected:
	virtual void paintEvent(QPaintEvent *event);
private:

	QVector<QVector<QPoint>> m_StrokeVector;
	QPixmap                  m_pixBackground;
	int                      m_nWidth, m_nHeight;
	bool                     m_bStarted;

};

#endif // HIGHLIGHTER_H
