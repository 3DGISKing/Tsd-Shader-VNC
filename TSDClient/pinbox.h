#ifndef PINBOX_H
#define PINBOX_H

#include <QWidget>
#include "ui_pinbox.h"
class MainWindow;
enum PinBox_Type{PINCODE=0, PINMESSAGE};

class PinBox : public QWidget
{
	Q_OBJECT

public:
	PinBox(QWidget *parent = 0,int type = 0);
	~PinBox();
	void Init(QString pinmMessage);

	QString m_targetIP;

private:
	Ui::PinBox ui;
	int m_type;
	MainWindow* m_parent ;

	bool CreateFrame(int type);
};

#endif // PINBOX_H
