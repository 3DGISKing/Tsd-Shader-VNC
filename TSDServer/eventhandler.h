/********************************************************************
	created:	2013/05/17
	created:	17:5:2013   9:42
	filename: 	d:\TSDClient\TSDClient\TSDClient\eventhandler.h
	file path:	d:\TSDClient\TSDClient\TSDClient
	file base:	eventhandler
	file ext:	h
	author:		ugi
	
	purpose:	
*********************************************************************/

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QtCore/qglobal.h>
#include <QtGui/QtEvents>

#ifdef Q_OS_WIN
#define _WIN32_WINNT 0x0501
#include <windows.h>
#endif


#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <linux/input.h>
#include <X11/extensions/XTest.h>

// message definition for linux mouse
#define MouseMove       50
#define ButtonDblClick      51
#define WheelMove       52

#endif

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#endif

class EventHandler
{
public:

#ifdef Q_OS_WIN
	static WORD convertQEventTypeToWindowsMessage(QEvent::Type type,Qt::MouseButton button,Qt::MouseButtons buttons);
	static UINT convertQKeyboardModifierToWindowsVirtualKeyFlags(Qt::KeyboardModifier modifier);
	static UINT convertQtKeyToWindowsVirtualKey(int qtkey);

	static void winSetMouseMessage(WORD wMM,QPoint MousePosition,UINT nFlags,short zDelta);
	static void winSetKBMessage   (WORD wMM,UINT nChar,UINT nRepCnt,UINT nFlags);
	static void winSetMouseKB     (INPUT & KeyBoardInput,WORD wVk,bool bDown);
#endif

#ifdef Q_OS_MAC
	static int convertQEventTypeToMacEventType(QEvent::Type type,Qt::MouseButton button,Qt::MouseButtons buttons);
	static uint convertQtKeyToMacVirtualKey(int qtkey);
	static uint convertQKeyboardModifierToMacEventFlagMask(Qt::KeyboardModifier modifier);

	static void macSetMouseLButtonDblClick(int x,int y);
	static void macSetMouseRButtonDblClick(int x,int y);
	static void macSetMouseMButtonDblClick(int x,int y);

#endif

#ifdef Q_OS_LINUX
        static bool bXTestSupported;
        static void checkXTestSupport();
        static int convertQEventTypeToLinuxMessage(QEvent::Type type);
        static int convertQtMouseButtonToLinuxButton(Qt::MouseButton button);
        static int convertQKeyboardModifierToLinuxVirtualKeyFlags(Qt::KeyboardModifier modifier);
        static int convertQtKeyToLinuxKey(int key);
	static void linuxSetMouseMessage(int nMessageType, int nMouseButton,  QPoint MousePosition, int nFlags, int zDelta);
	static void linuxSetKBMessage   (int nMessageType, int nChar, int nRepCnt, int nFlags);
#endif

};


#endif