#ifndef MSGDATASERVER_H
#define MSGDATASERVER_H
	#define TR	g_clientapp->Trans
//for Application
#define MSG_APP_TITLE		TR("TechShedar Client")

#define MSG_OK				TR("OK")
#define MSG_CANCEL			TR("Cancel")
#define MSG_YES				TR("Yes")
#define MSG_NO				TR("No")

//for success
#define MSG_SUCCESS_TITLE			TR("Success") //asd add 2013.5.4
#define MSG_SUCCESS_LOGINED			TR("Successful Login") //asd add 2013.5.15

//for failed

#define MSG_FAILED_USERSEARCH			TR("No selected customer user!!!") //rikr add 2013.5.4
#define MSG_FAILED_TITLE							TR("Failed") //asd add 2013.5.4
#define MSG_FAILED_NEWSESSION			TR("Failed new session") //asd add 2013.5.4
#define MSG_REFUSE_NEWSESSION		TR("Refused request connection") //asd add 2013.5.4
#define MSG_SESSION_DISCONNECT		TR("Disconnected current session") //asd add 2013.5.12
#define MSG_FAILED_SERVER_DATABASE	TR("Occured error in DataBase of Server") //asd add 2013.5.15
#define MSG_FAILED_SERVER_PARAMETER	TR("Sent missing infomation to Server") //asd add 2013.5.15
#define MSG_FAILED_LOGIN_USERNO		TR("There is no this User") //asd add 2013.5.15
#define MSG_FAILED_LOGIN_DUPLICATE	TR("This User is already Logined") //asd add 2013.5.15
#define MSG_FAILED_DOUBLE_CONNECT	TR("Customer is already controled by other User") //asd add 2013.5.15
#define MSG_FAILED_RING_CONNECT		TR("Now you are controling by this Customer") //asd add 2013.5.15

//for warning
#define MSG_WARNING_TITLE			TR("Warning") //asd add 2013.5.4
#define MSG_WARNING_LOGIN_USER		TR("Input your UserName") //asd add 2013.5.4
#define MSG_WARNING_LOGIN_PASS		TR("Input your Password") //asd add 2013.5.4
#define MSG_WARNING_SELECT_USER_DELETE		TR("Do you delete current selected user?") //asd add 2013.5.4

//for missing
#define MSG_MISSING_TITLE			TR("Missing") //asd add 2013.5.4

//for request
#define MSG_REQUEST_TITLE			TR("Request") //asd add 2013.5.11

//for error
#define MSG_ERR_TITLE				TR("Error")
#define MSG_ERR_INITERROR			TR("Occured error in initialization")
#define MSG_ERR_INITERROR_TRAYICON	TR("Occured error in TrayIcon initialization")
#define MSG_ERR_INITERROR_LOGIN		TR("Occured error in Login initialization")
#define MSG_ERR_NETWORK_CONNECTION	TR("Network disconnect or no server")
#define MSG_ERR_SERVER_DATABASE		TR("Occured error in DataBase of Server")

#define MSG_ERR_SEARCH		TR("Search Error")
#define	MSG_ERR_LINK_SEARCH	TR("Can't Search")
#endif // MSGDATASERVER_H