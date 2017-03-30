#ifndef TSDPROTOCOL_H
#define TSDPROTOCOL_H

#define Packet_DisplayInfo				0
#define Packet_TessellationInfo			1
#define Packet_RequestScreenShare		2
#define Packet_Image					3
#define Packet_MouseEvent				4
#define Packet_MouseWheelEvent			5
#define Packet_KeyPressEvent			6
#define Packet_KeyReleaseEvent			7

#define Packet_StopScreenCapture		8
#define Packet_StartScreenCapture       9
#define Packet_FileListRequest          10
#define Packet_FileListData             11
#define Packet_FileDownloadRequest      12
#define Packet_FileDownloadData         13
#define Packet_FileUploadRequest        14
#define Packet_FileDownloadFailed       141
#define Packet_FileDownloadCancel       1411
#define Packet_FileUploadData           15
#define Packet_FileUploadCancel         16
#define Packet_FileUploadFailed         17
#define Packet_HighLighterStart         18
#define Packet_HighLighterEnd           19
#define Packet_HighLighterData          20
#define Packet_Chat                     21
#define Packet_StartSessionRequest      22
#define Packet_StartSessionRefuse       23
#define Packet_StartSessionAgree        24
#define Packet_StopSessionRequest       25
#define Packet_DeleteSessionRequest     26
#define Packet_Reboot                   27

typedef struct tagDisplayInfoPacket
{
	int type;
	int width;
	int height;
	int depth;
} DisplayInfoPacket;

typedef struct tagTessellationInfoPacket
{
	int type;
	int xgridnumber;
	int ygridnumber;

}TessellationInfoPacket;


typedef struct tagRequestScreenSharePacket
{
	int type;
}RequestScreenSharePacket;

typedef struct tagImagePacket
{
	int    type;
	int    left;
	int    top;
	int    width;
	int    height;
	int    size;
	/**Followed by char [size] jpg raw data*/
}ImagePacket;

typedef struct tagMouseEventPacket
{
	int  type;
	char eventtype;
	char keyboardmodifiers ;
	char button;
	char buttons;
	int globalx;
	int globaly;
	int x;
	int y;
}MouseEventPacket;

typedef struct tagMouseWheelEventPacket
{
	int type;
	int keyboardmodifiers;
	int buttons;
	int globalx;
	int globaly;
	int x;
	int y;
	int delta;
}MouseWheelEventPacket;

typedef struct tagKeyEventPacket
{
	int  type;
	int  keyboardmodifiers;
	int  nativemodifiers;
	int  nativescancode;
	int  nativevirtualkey;
	int  key;
	int  count;
}KeyEventPacket;

typedef struct tagStopScreenCapturePacket
{
	int  type;
	
}StopScreenCapturePacket;

typedef struct tagStartScreenCapturePacket
{
	int  type;

}StartScreenCapturePacket;

typedef struct tagFileListRequestPacket
{
	int type;
	int flags;
	int dirNameSize;
	/* Followed by char Dirname[dirNameSize] */
} FileListRequestPacket;

typedef struct tagFileListDataPacket
{
	int type;
	int flags;
	int numFiles;
	int dataSize;
	int compressedSize;
	/* Followed by SizeData[numFiles] */
	/* Followed by Filenames[compressedSize] */
} FileListDataPacket;

typedef struct tagFileDownloadRequestPacket
{
	int type;
	int compressedLevel;
	int fNameSize;
	int position;
	/* Followed by char Filename[fNameSize] */
} FileDownloadRequestPacket;

typedef struct tagFileDownloadFailedPacket
{
	int type;
	int reasonLen;
	/* Followed by reason[reasonLen] */
} FileDownloadFailedPacket;

typedef struct tagFileDownloadCancelPacket
{
	int type;
	int reasonLen;
	/* Followed by reason[reasonLen] */
} FileDownloadCancelPacket;

typedef struct tagFileDownloadDataPacket
{
	int type;
	int compressLevel;
	int realSize;
	int compressedSize;
	/* Followed by File[copressedSize], 
	but if (realSize = compressedSize = 0) followed by CARD32 modTime  */
} FileDownloadDataPacket;

typedef struct tagFileUploadRequestPacket
{
	int type;
	int compressedLevel;
	int fNameSize;
	int position;
	/* Followed by char Filename[fNameSize] */
} FileUploadRequestPacket;

typedef struct _tagFileUploadDataPacket
{
	int type;
	int compressedLevel;
	int realSize;
	int compressedSize;
	/* Followed by File[compressedSize], 
	but if (realSize = compressedSize = 0) followed by CARD32 modTime  */
} FileUploadDataPacket;


typedef struct tagFileUploadCancelPacket
{
	int type;
	int unused;
	int reasonLen;
	/* Followed by reason[reasonLen] */
} FileUploadCancelPacket;

typedef struct tagFileUploadFailedPacket
{
	int type;
	int unused;
	int reasonLen;
	/* Followed by reason[reasonLen] */
} FileUploadFailedPacket;

typedef struct tagHighLighterStartPacket
{
	int type;
} HighLighterStartPacket;

typedef struct tagHighLighterEndPacket
{
	int type;
} HighLighterEndPacket;

typedef struct tagHighLighterDataPacket
{
	int type;
	int pointnum;
	/* Followed by QPoint[pointnum] */
} HighLighterDataPacket;

typedef struct tagChatPacket
{
	int  type;
	int  usernamesize;
	int  msgsize;
	
	uint time;
	/* Followed by char[usernamesize] 
	               char[msgsize]*/
} ChatPacket;

typedef struct tagStartSessionRequestPacket
{
	int  type;

} StartSessionRequestPacket;

typedef struct tagStartSessionRefusePacket
{
	int  type;

} StartSessionRefusePacket;

typedef struct tagStartSessionAgreePacket
{
	int  type;

} StartSessionAgreePacket;

typedef struct tagStopSessionRequestPacket
{
	int  type;

} StopSessionRequestPacket;

typedef struct tagDeleteSessionPacket
{
	int  type;

} DeleteSessonPacket;

typedef struct tagRebootPacket
{
	int  type;

} RebootPacket;







#endif