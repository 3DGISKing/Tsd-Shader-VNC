/********************************************************************
	created:	2013/05/08
	created:	8:5:2013   20:59
	filename: 	\address.cpp
	file path:	e:\AddressBar\AddressBar
	file base:	address
	file ext:	cpp
	author:		ugi
	
	purpose:	
*********************************************************************/

#include "addressbar.h"
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QFileDialog>
#include <QtGui/QResizeEvent>
#include "explolerclient.h"
#include "filemanager.h"

#define DEFAULT_HEIGHT  30
#define BUTTON_WIDTH    80
#define TEXTEDIT_WIDTH  500

AddressBar::AddressBar(QWidget* parent/* =NULL */)
:QWidget(parent)
{
	m_pAddressButton=new QPushButton(this);
	m_pAddressButton->setGeometry(QRect(0, 0, BUTTON_WIDTH, DEFAULT_HEIGHT));

	m_pAddressTextEdit=new QLineEdit(this);
	m_pAddressTextEdit->setGeometry(QRect(BUTTON_WIDTH, 0, TEXTEDIT_WIDTH, DEFAULT_HEIGHT));

	this->setFixedHeight(DEFAULT_HEIGHT);
	this->setWindowFlags(Qt::Tool);
	this->setAddressTitle("Local Site:");

	m_pAddressTextEdit->setText(QDir::homePath());
	connect(m_pAddressButton,SIGNAL(clicked()),this,SLOT(onChangeAddressFromButton()));
	connect(m_pAddressTextEdit,SIGNAL(textEdited(const QString& )),this,SLOT(onChangeAddressFromTextEdit(const QString& )));
}

AddressBar::~AddressBar()
{
	delete m_pAddressButton;
	delete m_pAddressTextEdit;
}

void AddressBar::resizeEvent(QResizeEvent * event)
{
	int newwidth=event->size().width();	
	m_pAddressTextEdit->setGeometry(BUTTON_WIDTH,0, newwidth-BUTTON_WIDTH,DEFAULT_HEIGHT);
}

void AddressBar::setAddressTitle(QString title)
{
	m_pAddressButton->setText(title);
}

void AddressBar::setCurrentPath(QString path)
{
	m_pAddressTextEdit->setText(path);
}
void AddressBar::onChangeAddressFromButton()
{
	QFileDialog dlg;

	dlg.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	

	QString dir = QFileDialog::getExistingDirectory(this,
													tr("Select Directory"),
													QDir::homePath(),
													QFileDialog::ShowDirsOnly
													| QFileDialog::DontResolveSymlinks);

	if(dir.right(1)!="/")	//check last character
		dir=dir+"/";

	m_pAddressTextEdit->setText(dir);
	m_pClient->setCurrentPath(dir);

	g_FileManager->setClientPath(dir);
}

void AddressBar::onChangeAddressFromTextEdit(const QString& path)
{
	QFileInfo info(path);

	if(info.isDir())
		m_pClient->setCurrentPath(path);
}