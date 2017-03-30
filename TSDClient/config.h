#ifndef CONFIG_H
#define CONFIG_H
#include <QtCore/QPoint>
#include <QtCore/QSize>

#define MAINWINDOW_TITLE_USER	"Tech Shedar"

#define CONF_SET_ORG			"Tech Shedar"
#define CONF_SET_APP			"TSDClient"

#define CONF_SRVADDR_LEN    256

#define CONF_SET_STARTMODE		"startmode"
#define CONF_REG_PATH			"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run" 


#define CONF_WEBSERVER_PATH		"/TSD_remote_net/" //asd add 2013.5.29
#define CONF_WEBPAGE_CONTROL	"controller.php" //asd add 2013.5.29
#define CONF_WEBPAGE_LOGIN		"login.php" //asd add 2013.5.29
#define CONF_PINCODE_SIZE		8  //asd add 2013.5.29

#define CONF_DEV_DESKTOP		"DesktopPC" //asd add 2013.5.2
#define CONF_DEV_WINMOBILE		"WinMobile" //asd add 2013.5.2
#define CONF_DEV_IPHONE			"iPhone" //asd add 2013.5.2
#define CONF_DEV_IPAD			"iPad" //asd add 2013.5.2
#define CONF_DEV_IPOD			"iPodTouch" //asd add 2013.5.2
#define CONF_DEV_ANDROID		"Android" //asd add 2013.5.2

#define CONF_OS_WINDOWS			"Windows" //asd add 2013.5.2
#define CONF_OS_LINUX			"Linux" //asd add 2013.5.2
#define CONF_OS_MACINTOSH		"Macintosh" //asd add 2013.5.2
#define CONF_OS_WINCE			"WindowsCE" //asd add 2013.5.2
#define CONF_OS_IOS				"iOS" //asd add 2013.5.2
#define CONF_OS_ANDROID			"Android" //asd add 2013.5.2

enum requestKey { //asd add 2013.5.29
	DB_ERROR, 
	SRV_CONNECT,
	REQ_LOGIN,
	REQ_PINCODE,
	REQ_SESSION,
	REQ_START,
	REQ_STOP,
	REQ_DELETE,
	REQ_LOGOUT,
	SRV_ERROR
};

#define LOCAL_TEST

class Config
{
public:
    Config();
    ~Config();

    bool Init();
    void Release();

    void ReadSettings();
    void WriteSettings();

	void SetServer(char *addr, quint16 port,char *webaddr);
	void SetScreenDir(char *capdir,char *shotdir);

	QString getIPAddress(); //asd add 2013.5.9
	QString getClientInfo(); //asd add 2013.5.9
	QString generatePinCode(); //asd add 2013.5.9
	
	QString GetServerIP();//5.16
	QString Config::GetWebServerIP();

public:
    //windows's position
    QPoint  m_wndPos;
    QSize   m_wndSize;


	//View Size
	bool m_fullScreen;

	//login state
	bool m_bLogin;	//login´óÊîËº·² ½Ø,Ê­³£·² °º¼ð

	// start mode
	bool m_bStartmode;

	// screen save
	char		m_dirCature[CONF_SRVADDR_LEN];
	char		m_dirShot[CONF_SRVADDR_LEN];

	//server info
	char		m_srvAddr[CONF_SRVADDR_LEN];
	char		m_webAddr[CONF_SRVADDR_LEN];

	unsigned int	m_posTask;
	unsigned int	m_posUesr;
	unsigned int	m_posChat;
};

#endif // CONFIG_H
