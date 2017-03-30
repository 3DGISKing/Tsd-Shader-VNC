/********************************************************************
	created:	2013/05/08
	created:	8:5:2013   20:57
	filename: 	\address.h
	file path:	
	file base:	address
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/
#ifndef ADDRESSBAR_H
#define ADDRESSBAR_H

#include <QtGui/QWidget>

class QLineEdit;
class QPushButton;
class ExplolerClient;

class AddressBar :public QWidget
{
	Q_OBJECT

public:
	AddressBar(QWidget* parent=NULL);
	~AddressBar();

	void setAddressTitle(QString title);
	void setCurrentPath(QString path);
	void setClient(ExplolerClient* ex){m_pClient=ex;}
protected:
	virtual void resizeEvent(QResizeEvent * event);

private slots:
	void onChangeAddressFromButton();
	void onChangeAddressFromTextEdit(const QString& );
private:
	QPushButton *       m_pAddressButton;
	QLineEdit *         m_pAddressTextEdit;
	ExplolerClient*     m_pClient;
};
#endif