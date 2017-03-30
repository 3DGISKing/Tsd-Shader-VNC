/********************************************************************
        created:	2013/05/17
        created:	17:5:2013   9:43
        filename: 	d:\TSDClient\TSDClient\TSDClient\eventhandler.cpp
        file path:	d:\TSDClient\TSDClient\TSDClient
        file base:	eventhandler
        file ext:	cpp
        author:		ugi

        purpose:
*********************************************************************/
#include "eventhandler.h"




#ifdef Q_OS_WIN
#include <vector>


WORD EventHandler::convertQEventTypeToWindowsMessage(QEvent::Type type,Qt::MouseButton button,Qt::MouseButtons buttons)
{
        switch(type)
        {
        case QEvent::KeyPress:
                return WM_KEYDOWN;
        case QEvent::KeyRelease:
                return WM_KEYUP;
        }

        if(type==QEvent::MouseButtonPress && button==Qt::LeftButton)
                return WM_LBUTTONDOWN;
        else if(type==QEvent::MouseButtonPress && button==Qt::RightButton)
                return WM_RBUTTONDOWN;
        else if(type==QEvent::MouseButtonPress && button==Qt::MidButton)
                return WM_MBUTTONDOWN;

        else if (type==QEvent::MouseButtonRelease &&  button==Qt::LeftButton)
                return WM_LBUTTONUP;
        else if (type==QEvent::MouseButtonRelease &&  button==Qt::RightButton)
                return WM_RBUTTONUP;
        else if (type==QEvent::MouseButtonRelease &&  button==Qt::MidButton)
                return WM_MBUTTONUP;

        else if(type==QEvent::MouseMove)
                return WM_MOUSEMOVE;

        else if(type==QEvent::MouseButtonDblClick && button==Qt::LeftButton)
                return WM_LBUTTONDBLCLK;
        else if(type==QEvent::MouseButtonDblClick && button==Qt::RightButton)
                return WM_RBUTTONDBLCLK;
        else if(type==QEvent::MouseButtonDblClick && button==Qt::MidButton)
                return WM_MBUTTONDBLCLK;

        return  WM_NULL;
}

UINT EventHandler::convertQKeyboardModifierToWindowsVirtualKeyFlags(Qt::KeyboardModifier modifier)
{
        if(modifier==Qt::ShiftModifier)
                return MK_SHIFT;
        else if(modifier==Qt::ControlModifier)
                return MK_CONTROL;
        return Qt::NoModifier;
}

UINT EventHandler::convertQtKeyToWindowsVirtualKey(int qtkey)
{
        switch(qtkey)
        {
        case Qt::Key_Escape:
                return VK_ESCAPE;
        case Qt::Key_Tab:
                return VK_TAB;
        case Qt::Key_Backspace:
                return VK_BACK;
        case Qt::Key_Return:
        case Qt::Key_Enter:
                return VK_RETURN;
        case Qt::Key_Insert:
                return VK_INSERT;
        case Qt::Key_Delete:
                return VK_DELETE;
        case Qt::Key_Pause:
                return VK_PAUSE;
        case Qt::Key_Print:
                return VK_PRINT;
        case Qt::Key_Home:
                return VK_HOME;
        case Qt::Key_End:
                return VK_END;
        case Qt::Key_Left:
                return VK_LEFT;
        case Qt::Key_Up:
                return VK_UP;
        case Qt::Key_Right:
                return VK_RIGHT;
        case Qt::Key_Down:
                return VK_DOWN;
        case Qt::Key_PageUp:
                return 33;
        case Qt::Key_PageDown:
                return 34;
        case Qt::Key_Shift:
                return VK_SHIFT;
        case Qt::Key_Control:
                return VK_CONTROL;
        case Qt::Key_Alt:
                return VK_TAB;
        case Qt::Key_CapsLock:
                return VK_CAPITAL;
        case Qt::Key_NumLock:
                return VK_NUMLOCK;
        case Qt::Key_F1:
                return VK_F1;
        case Qt::Key_F2:
                return VK_F2;
        case Qt::Key_F3:
                return VK_F3;
        case Qt::Key_F4:
                return VK_F4;
        case Qt::Key_F5:
                return VK_F5;
        case Qt::Key_F6:
                return VK_F6;
        case Qt::Key_F7:
                return VK_F7;
        case Qt::Key_F8:
                return VK_F8;
        case Qt::Key_F9:
                return VK_F9;
        case Qt::Key_F10:
                return VK_F10;
        case Qt::Key_F11:
                return VK_F11;
        case Qt::Key_F12:
                return VK_F12;
        case Qt::Key_Plus:
                return VK_OEM_PLUS;
        case Qt::Key_Comma:
                return VK_OEM_COMMA;
        case Qt::Key_Minus:
                return VK_OEM_MINUS;
        default:
                return qtkey;
        }
}

// Execute the keyboard command
void EventHandler::winSetKBMessage(WORD wMM,UINT nChar,UINT nRepCnt,UINT nFlags)
{
        // Create the maximum number of user inputs
        std::vector<INPUT> UserInput(nRepCnt);
        INPUT * pUserInput = &UserInput[0];

        // Build up the keyboard input
        for (UINT iInput = 0;iInput < nRepCnt;++iInput)
        {
                // Get the keyboard input
                INPUT & KeyBoardInput = pUserInput[iInput];

                // Set the input type
                KeyBoardInput.type = INPUT_KEYBOARD;

                // Get the keyboard input structure
                KEYBDINPUT & KeyBoard = KeyBoardInput.ki;

                // Set the scan code
                KeyBoard.wScan = 0;

                // Set the ignored fields
                KeyBoard.dwExtraInfo = 0;
                KeyBoard.time = 0;

                // Test for the key being pressed or released
                KeyBoard.dwFlags = wMM == WM_KEYDOWN ? 0 : KEYEVENTF_KEYUP;

                // Set the character
                KeyBoard.wVk = nChar;
        }

        // Send the user input for the keyboard
        SendInput(nRepCnt,pUserInput,sizeof(INPUT));
}

// Set the virtual keycode for the mouse message
void EventHandler::winSetMouseKB(INPUT & KeyBoardInput,WORD wVk,bool bDown)
{
        // Set the input type
        KeyBoardInput.type = INPUT_KEYBOARD;

        // Get the keyboard input structure
        KEYBDINPUT & KeyBoard = KeyBoardInput.ki;

        // Set the scan code
        KeyBoard.wScan = 0;

        // Set the ignored fields
        KeyBoard.dwExtraInfo = 0;
        KeyBoard.time = 0;

        // Test for the key being pressed or released
        KeyBoard.dwFlags = bDown ? 0 : KEYEVENTF_KEYUP;

        // Set the virtual key
        KeyBoard.wVk = wVk;
}

void EventHandler::winSetMouseMessage(WORD wMM,QPoint MousePosition,UINT nFlags,short zDelta)
{
        // Read the mouse packet
        int mx = MousePosition.x();
        int my = MousePosition.y();

          // The time stamp of the operation in milli-seconds
        DWORD dwTime = 0;

        // Extract the flags
        bool bCtrl = nFlags & 1 ? true : false;
        bool bShift = nFlags & 2 ? true : false;

        // Set the number of commands in the input
        int nInputs = 0;

        // Test for the virtual keys
        if (bCtrl)
                nInputs += 2;
        if (bShift)
                nInputs += 2;

        // For the loop termination
        int nHalf = nInputs / 2;

        // Test for the mouse message
        nInputs += 1;
        if (wMM == WM_LBUTTONDBLCLK || wMM == WM_MBUTTONDBLCLK || wMM == WM_RBUTTONDBLCLK)
                nInputs += 3;

        // Create the maximum number of user inputs
        std::vector<INPUT> UserInput(nInputs);
        INPUT * pUserInput = &UserInput[0];

        // Set the starting input
        int nStart = 0;

        // Set the virtual keydown(s) for the mouse message
        if (bCtrl)
        {
                INPUT & KeyBoardInput = *(pUserInput + (nStart++));
                winSetMouseKB(KeyBoardInput,VK_CONTROL,true);
        }
        if (bShift)
        {
                INPUT & KeyBoardInput = *(pUserInput + (nStart++));
                winSetMouseKB(KeyBoardInput,VK_SHIFT,true);
        }

        // Build up the mouse input
        int iInput = nStart;
        for (;iInput < (nInputs - nHalf);iInput++,dwTime += 100)
        {
                // Get the mouse input
                INPUT & MouseInput = pUserInput[iInput];

                // Set the input type
                MouseInput.type = INPUT_MOUSE;

                // Get the mouse input structure
                MOUSEINPUT & Mouse = MouseInput.mi;

                // Set the absolute coordinates
                Mouse.dx = mx;
                Mouse.dy = my;

                // Set the mouse data
                Mouse.mouseData = zDelta;

                // Set the flags
                Mouse.dwFlags = MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;

                // Set the button(s)
                if (wMM == WM_LBUTTONDBLCLK)
                {
                        if (iInput == 0 || iInput == 2)
                                Mouse.dwFlags |= MOUSEEVENTF_LEFTDOWN;
                        else if (iInput == 1 || iInput == 3)
                                Mouse.dwFlags |= MOUSEEVENTF_LEFTUP;
                }
                else if (wMM == WM_LBUTTONDOWN)
                        Mouse.dwFlags |= MOUSEEVENTF_LEFTDOWN;
                else if (wMM == WM_LBUTTONUP)
                        Mouse.dwFlags |= MOUSEEVENTF_LEFTUP;
                else if (wMM == WM_MBUTTONDBLCLK)
                {
                        if (iInput == 0 || iInput == 2)
                                Mouse.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
                        else if (iInput == 1 || iInput == 3)
                                Mouse.dwFlags |= MOUSEEVENTF_MIDDLEUP;
                }
                else if (wMM == WM_MBUTTONDOWN)
                        Mouse.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
                else if (wMM == WM_MBUTTONUP)
                        Mouse.dwFlags |= MOUSEEVENTF_MIDDLEUP;
                else if (wMM == WM_MOUSEWHEEL)
                        Mouse.dwFlags |= MOUSEEVENTF_WHEEL;
                else if (wMM == WM_MOUSEMOVE)
                        Mouse.dwFlags |= MOUSEEVENTF_MOVE;
                else if (wMM == WM_RBUTTONDBLCLK)
                {
                        if (iInput == 0 || iInput == 2)
                                Mouse.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
                        else if (iInput == 1 || iInput == 3)
                                Mouse.dwFlags |= MOUSEEVENTF_RIGHTUP;
                }
                else if (wMM == WM_RBUTTONDOWN)
                        Mouse.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
                else if (wMM == WM_RBUTTONUP)
                        Mouse.dwFlags |= MOUSEEVENTF_RIGHTUP;

                // Set the time
                Mouse.time = dwTime;
        }

        // Set the new starting place
        nStart = iInput;

        // Set the virtual keydown(s) for the mouse message
        if (bCtrl)
        {
                INPUT & KeyBoardInput = *(pUserInput + (nStart++));
                winSetMouseKB(KeyBoardInput,VK_CONTROL,false);
        }
        if (bShift)
        {
                INPUT & KeyBoardInput = *(pUserInput + (nStart++));
                winSetMouseKB(KeyBoardInput,VK_SHIFT,false);
        }

        // Send the user inputs for the mouse
        SendInput(nInputs,pUserInput,sizeof(INPUT));
}
#endif

#ifdef Q_OS_LINUX

bool EventHandler::bXTestSupported;

void EventHandler::checkXTestSupport()
{
    bXTestSupported = false;

    Display *display = XOpenDisplay(NULL);
    if(display == NULL) exit(EXIT_FAILURE);

    int major_version, minor_version,dummy;

    Bool success = XTestQueryExtension(display, &dummy, &dummy, &major_version, &minor_version);
    if(success == False || major_version < 2 || (major_version <= 2 && minor_version < 2))
        bXTestSupported = false;
    else
        bXTestSupported = true;

    // disconnect to X-server
    XCloseDisplay(display);
}

int EventHandler::convertQEventTypeToLinuxMessage(QEvent::Type type)
{
        switch(type)
        {
        case 6://QEvent::KeyPress:
                return KeyPress;
        case 7://QEvent::KeyRelease:
                return KeyRelease;
        case QEvent::MouseButtonPress:
                return ButtonPress;
        case QEvent::MouseButtonRelease:
                return ButtonRelease;
        case QEvent::MouseButtonDblClick:
                return ButtonDblClick;
        case 5://QEvent::MouseMove:
                return MouseMove;
        case QEvent::Wheel:
                return WheelMove;
        default:
                return 0;
        }
}

int EventHandler::convertQtMouseButtonToLinuxButton(Qt::MouseButton button)
{
        switch(button)
        {
        case Qt::LeftButton:
                return Button1;
        case Qt::MidButton:
                return Button2;
        case Qt::RightButton:
                return Button3;
        case Qt::XButton1:
                return Button4;
        case Qt::XButton2:
                return Button5;
        }
}

int EventHandler::convertQKeyboardModifierToLinuxVirtualKeyFlags(Qt::KeyboardModifier modifier)
{
        if(modifier==Qt::ShiftModifier)
                return ShiftMask;
        else if(modifier==Qt::ControlModifier)
                return ControlMask;
        return 0;
}

int convertQtKeyToLinuxKey(int key)
{
	int nLinuxKey = (key > 0xfff) ? XK_space : key;
	switch(key)
	{
	case Qt::Key_Escape: nLinuxKey = XK_Escape; break;
	case Qt::Key_Tab: nLinuxKey = XK_Tab; break;
	case Qt::Key_Backtab: nLinuxKey = XK_BackSpace; break;
	case Qt::Key_Backspace: nLinuxKey = XK_BackSpace; break;
	case Qt::Key_Return: nLinuxKey = XK_Return; break;
	case Qt::Key_Enter: nLinuxKey = XK_Return; break;
	case Qt::Key_Insert: nLinuxKey = XK_Insert; break;
	case Qt::Key_Delete: nLinuxKey = XK_Delete; break;
	case Qt::Key_Pause: nLinuxKey = XK_Pause; break;
	case Qt::Key_Print: nLinuxKey = XK_Print; break;
	case Qt::Key_SysReq: nLinuxKey = XK_Sys_Req; break;
	case Qt::Key_Clear: nLinuxKey = XK_Clear; break;
	case Qt::Key_Home: nLinuxKey = XK_Home; break;
	case Qt::Key_End: nLinuxKey = XK_End; break;
	case Qt::Key_Left: nLinuxKey = XK_End; break;
	case Qt::Key_Up: nLinuxKey = XK_Up; break;
	case Qt::Key_Right: nLinuxKey = XK_Right; break;
	case Qt::Key_Down: nLinuxKey = XK_Down; break;
	case Qt::Key_PageUp: nLinuxKey = XK_Page_Up; break;
	case Qt::Key_PageDown: nLinuxKey = XK_Page_Down; break;
	case Qt::Key_Shift: nLinuxKey = XK_Shift_L; break;
	case Qt::Key_Control: nLinuxKey = XK_Control_L; break;
	case Qt::Key_Meta: nLinuxKey = XK_Meta_L; break;
	case Qt::Key_Alt: nLinuxKey = XK_Alt_L; break;
	case Qt::Key_AltGr: nLinuxKey = XK_Alt_R; break;
	case Qt::Key_CapsLock: nLinuxKey = XK_Caps_Lock; break;
	case Qt::Key_NumLock: nLinuxKey = XK_Num_Lock; break;
	case Qt::Key_ScrollLock: nLinuxKey = XK_Scroll_Lock; break;
	case Qt::Key_F1: nLinuxKey = XK_F1; break;
	case Qt::Key_F2: nLinuxKey = XK_F2; break;
	case Qt::Key_F3: nLinuxKey = XK_F3; break;
	case Qt::Key_F4: nLinuxKey = XK_F4; break;
	case Qt::Key_F5: nLinuxKey = XK_F5; break;
	case Qt::Key_F6: nLinuxKey = XK_F6; break;
	case Qt::Key_F7: nLinuxKey = XK_F7; break;
	case Qt::Key_F8: nLinuxKey = XK_F8; break;
	case Qt::Key_F9: nLinuxKey = XK_F9; break;
	case Qt::Key_F10: nLinuxKey = XK_F10; break;
	case Qt::Key_F11: nLinuxKey = XK_F11; break;
	case Qt::Key_F12: nLinuxKey = XK_F12; break;
	case Qt::Key_F13: nLinuxKey = XK_F13; break;
	case Qt::Key_F14: nLinuxKey = XK_F14; break;
	case Qt::Key_F15: nLinuxKey = XK_F15; break;
	case Qt::Key_F16: nLinuxKey = XK_F16; break;
	case Qt::Key_F17: nLinuxKey = XK_F17; break;
	case Qt::Key_F18: nLinuxKey = XK_F18; break;
	case Qt::Key_F19: nLinuxKey = XK_F19; break;
	case Qt::Key_F20: nLinuxKey = XK_F20; break;
	case Qt::Key_F21: nLinuxKey = XK_F21; break;
	case Qt::Key_F22: nLinuxKey = XK_F22; break;
	case Qt::Key_F23: nLinuxKey = XK_F23; break;
	case Qt::Key_F24: nLinuxKey = XK_F24; break;
	case Qt::Key_F25: nLinuxKey = XK_F25; break;
	case Qt::Key_F26: nLinuxKey = XK_F26; break;
	case Qt::Key_F27: nLinuxKey = XK_F27; break;
	case Qt::Key_F28: nLinuxKey = XK_F28; break;
	case Qt::Key_F29: nLinuxKey = XK_F29; break;
	case Qt::Key_F30: nLinuxKey = XK_F30; break;
	case Qt::Key_F31: nLinuxKey = XK_F31; break;
	case Qt::Key_F32: nLinuxKey = XK_F32; break;
	case Qt::Key_F33: nLinuxKey = XK_F33; break;
	case Qt::Key_F34: nLinuxKey = XK_F34; break;
	case Qt::Key_F35: nLinuxKey = XK_F35; break;
	case Qt::Key_Super_L: nLinuxKey = XK_Super_L; break;
	case Qt::Key_Super_R: nLinuxKey = XK_Super_R; break;
	case Qt::Key_Menu: nLinuxKey = XK_Menu; break;
	case Qt::Key_Hyper_L: nLinuxKey = XK_Hyper_L; break;
	case Qt::Key_Hyper_R: nLinuxKey = XK_Hyper_R; break;
	case Qt::Key_Help: nLinuxKey = XK_Help; break;
		//        case Qt::Key_Direction_L: nLinuxKey = XK_Direction_L; break;
		//        case Qt::Key_Direction_R: nLinuxKey = XK_Direction_R; break;
	default: break;
	}
	Display *dpy = XOpenDisplay(NULL);
	nLinuxKey = XKeysymToKeycode(dpy, nLinuxKey);
	XCloseDisplay(dpy);

	return nLinuxKey;
}


void EventHandler::linuxSetMouseMessage(int nMessageType, int nMouseButton, QPoint MousePosition, int nFlags, int zDelta)
{
        // connect X-server
        Display *display = XOpenDisplay(NULL);

        if(display == NULL)
        {
                fprintf(stderr, "Error Display !!!\n");
                exit(EXIT_FAILURE);
        }

        Window root = RootWindow(display, DefaultScreen(display));

        // prepare mouse event structure
        XEvent event;
        memset(&event, 0x00, sizeof(event));
        event.xbutton.button = nMouseButton;
        event.xbutton.same_screen = True;

	switch(nMessageType)
	{
	case ButtonPress:
            if(bXTestSupported)
            {
                XTestFakeButtonEvent(display, nMouseButton, true, 0);
            }
            else
            {
                event.type = ButtonPress;
		// get last child window that pointer is in
		XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root,
			&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
		event.xbutton.subwindow = event.xbutton.window;
		while(event.xbutton.subwindow)
		{
			event.xbutton.window = event.xbutton.subwindow;

			XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root,
				&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
		}
		event.xbutton.state = nFlags;
		// send mouse event
		if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
            }
            break;
	case ButtonRelease:
            if(bXTestSupported)
            {
                XTestFakeButtonEvent(display, nMouseButton, false, 0);
            }
            else
            {
                event.type = ButtonRelease;
		// get last child window that pointer is in
		XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root,
			&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
		event.xbutton.subwindow = event.xbutton.window;
		while(event.xbutton.subwindow)
		{
			event.xbutton.window = event.xbutton.subwindow;

			XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root,
				&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
		}
		event.xbutton.state = nFlags;
		event.xbutton.state |= 0x100;
		// send mouse event
		if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
            }
            break;
	case MouseMove:
            if(bXTestSupported)
            {
                XTestFakeMotionEvent(display, -1, MousePosition.x(), MousePosition.y(), 0);
            }
            else
            {
                // move mouse position
		XWarpPointer(display, None, root, 0, 0, 0, 0, MousePosition.x(), MousePosition.y());
            }
            break;
	case ButtonDblClick:
            if(bXTestSupported)
            {
                 XTestFakeButtonEvent(display, nMouseButton, true, 1);
                 XTestFakeButtonEvent(display, nMouseButton, true, 1);
                 XTestFakeButtonEvent(display, nMouseButton, true, 1);
                 XTestFakeButtonEvent(display, nMouseButton, true, 1);
            }
            else
            {
                event.type = ButtonPress;
		// get last child window that pointer is in
		XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root,
			&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
		event.xbutton.subwindow = event.xbutton.window;
		while(event.xbutton.subwindow)
		{
			event.xbutton.window = event.xbutton.subwindow;

			XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root,
				&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
		}
		event.xbutton.state = nFlags;
		// send mouse event
		if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
		usleep(50000);
		// update display
		XFlush(display);
		// send mouse event
		if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
		usleep(50000);
		XFlush(display);
		event.xbutton.state |= 0x100;
		event.type = ButtonRelease;
		// send mouse event
		if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
            }
            break;
	case WheelMove:
            if(bXTestSupported)
            {
                nMouseButton = (zDelta < 0) ? Button5 : Button4;
                XTestFakeButtonEvent(display, nMouseButton, true, 1);
                XTestFakeButtonEvent(display, nMouseButton, false, 1);
            }
            else
            {
                event.type = ButtonPress;
                if(zDelta < 0) event.xbutton.button = Button4;
                else event.xbutton.button = Button5;
                // get last child window that pointer is in
                XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root,
                        &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
                event.xbutton.subwindow = event.xbutton.window;
                while(event.xbutton.subwindow)
                {
                        event.xbutton.window = event.xbutton.subwindow;

                        XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root,
                                &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
                }
                event.xbutton.state = nFlags;
                // send mouse event
                if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
                usleep(5000);
                event.xbutton.state |= 0x100;
                event.type = ButtonRelease;
                // send mouse event
                if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
            }
            break;
	}

	// update display
	XFlush(display);
	XCloseDisplay(display);
}

void EventHandler::linuxSetKBMessage(int nMessageType, int nChar, int nRepCnt, int nFlags)
{
	// connect X-server
	Display *display = XOpenDisplay(NULL);

        if(display == NULL) exit(EXIT_FAILURE);

        if(bXTestSupported)
        {
           if(nMessageType == KeyPress) XTestFakeKeyEvent(display, nChar+8, true, 0);
           if(nMessageType == KeyRelease) XTestFakeKeyEvent(display, nChar+8, false, 0);
        }
        else
        {
            // prepare key event structure
            XEvent event;
            memset(&event, 0x00, sizeof(event));
            event.type = nMessageType;

            event.xkey.state = 0;//ShiftMask;
            event.xkey.keycode = nChar+8;
            event.xkey.same_screen = True;
            event.xkey.state = nFlags;

            int nState = 0;
            XGetInputFocus(display, &event.xkey.window, &nState);
            // send KB event
            if(XSendEvent(display, InputFocus, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
        }

	// update display
	XFlush(display);
	// disconnect to X-server
	XCloseDisplay(display);
}


#endif


#ifdef Q_OS_MAC

int EventHandler::convertQEventTypeToMacEventType(QEvent::Type type,Qt::MouseButton button,Qt::MouseButtons buttons)
{

        if(type==QEvent::MouseButtonPress && button==Qt::LeftButton)
                return kCGEventLeftMouseDown;
        else if(type==QEvent::MouseButtonPress && button==Qt::RightButton)
                return kCGEventRightMouseDown;
        else if(type==QEvent::MouseButtonPress && button==Qt::MidButton)
                return kCGEventOtherMouseDown;

        else if (type==QEvent::MouseButtonRelease &&  button==Qt::LeftButton)
                return kCGEventLeftMouseUp;
        else if (type==QEvent::MouseButtonRelease &&  button==Qt::RightButton)
                return kCGEventRightMouseUp;
        else if (type==QEvent::MouseButtonRelease &&  button==Qt::MidButton)
                return kCGEventOtherMouseUp;
        else if(type==QEvent::MouseMove)
                return kCGEventMouseMoved;
        else if(type==QEvent::MouseMove && button==Qt::LeftButton)
                return kCGEventLeftMouseDragged;
        else if(type==QEvent::MouseMove && button==Qt::RightButton)
                return kCGEventRightMouseDragged;
        /*
        else if(type==QEvent::MouseButtonDblClick && button==Qt::LeftButton)
        return WM_LBUTTONDBLCLK;
        else if(type==QEvent::MouseButtonDblClick && button==Qt::RightButton)
        return WM_RBUTTONDBLCLK;
        else if(type==QEvent::MouseButtonDblClick && button==Qt::MidButton)
        return WM_MBUTTONDBLCLK;
        */
        return  kCGEventNull;
}

uint EventHandler::convertQKeyboardModifierToMacEventFlagMask(Qt::KeyboardModifier modifier)
{
        if(modifier==Qt::ShiftModifier)
                return kCGEventFlagMaskShift;
        else if(modifier==Qt::ControlModifier)
                return kCGEventFlagMaskControl;
        /*There are some more ModifierVirtualKeys in MAC as follow
        *  kCGEventFlagMaskAlternate
        *  kCGEventFlagMaskCommand
        *  kCGEventFlagMaskAlphaShift
        *  kCGEventFlagMaskHelp
        *  kCGEventFlagMaskNumericPad
        *  kCGEventFlagMaskSecondaryFn
        *  kCGEventFlagMaskNonCoalesced
        *
        */
        return kCGEventNull;
}

uint EventHandler::convertQtKeyToMacVirtualKey(int qtkey)
{
        switch(qtkey)
        {
            case Qt::Key_0:
                 return 29;
            case Qt::Key_1:
                 return 18;
            case Qt::Key_2:
                return 19;
            case Qt::Key_3:
                return 20;
            case Qt::Key_4:
                return 21;
            case Qt::Key_5:
                return 23;
            case Qt::Key_6:
                return 22;
            case Qt::Key_7:
                return 23;
            case Qt::Key_8:
                return 28;
            case Qt::Key_9:
                return 25;
            case Qt::Key_A:
                return 0;
            case Qt::Key_S:
                return 1;
            case Qt::Key_D:
                return 2;
            case Qt::Key_F:
                return 3;
            case Qt::Key_H:
                return 4;
            case Qt::Key_G:
                return 5;
            case Qt::Key_Z:
                return 6;
    case Qt::Key_X:
        return 7;
    case Qt::Key_C:
        return 8;
    case Qt::Key_V:
        return 9;
    case Qt::Key_B:
        return 11;
    case Qt::Key_Q:
        return 12;
    case Qt::Key_W:
        return 13;
    case Qt::Key_E:
        return 14;
    case Qt::Key_R:
        return 15;
    case Qt::Key_Y:
        return 16;
    case Qt::Key_T:
        return 17;
    case Qt::Key_Equal:
        return 24;
    case Qt::Key_Minus:
        return 27;
    case Qt::Key_BraceRight:
        return 30;
    case Qt::Key_O:
        return 31;
    case Qt::Key_U:
        return 32;
    case Qt::Key_BraceLeft:
        return 33;
    case Qt::Key_I:
        return 34;
    case Qt::Key_P:
        return 35;


    case Qt::Key_L:
        return 37;
    case Qt::Key_J:
        return 38;
    case Qt::Key_Apostrophe:
        return 39;
    case Qt::Key_K:
        return 40;
    case Qt::Key_Semicolon:
        return 41;


        case Qt::Key_Escape:
                return 53;
        case Qt::Key_Tab:
                return 48;
        case Qt::Key_Backspace:
                return 71;

        case Qt::Key_Enter:
                return 52;
                //    case Qt::Key_Insert:
                //     return VK_INSERT;
        case Qt::Key_Delete:
                return 117;
                //   case Qt::Key_Pause:
                //      return VK_PAUSE;
                //   case Qt::Key_Print:
                //      return VK_PRINT;
        case Qt::Key_Home:
                return 115;
        case Qt::Key_End:
                return 119;
        case Qt::Key_Left:
                return 123;
        case Qt::Key_Up:
                return 126;
        case Qt::Key_Right:
                return 124;
        case Qt::Key_Down:
                return 125;
        case Qt::Key_PageUp:
                return 116;
        case Qt::Key_PageDown:
                return 121;
                /*
                case Qt::Key_Shift:
                return VK_SHIFT;
                case Qt::Key_Control:
                return VK_CONTROL;
                case Qt::Key_Alt:
                return VK_TAB;
                case Qt::Key_CapsLock:
                return VK_CAPITAL;
                case Qt::Key_NumLock:
                return VK_NUMLOCK;
                //  In MAC shift and control is handled in the other way.
                //
                //
                //

                */
        case Qt::Key_F1:
                return 122;
        case Qt::Key_F2:
                return 120;
        case Qt::Key_F3:
                return 99;
        case Qt::Key_F4:
                return 118;
        case Qt::Key_F5:
                return 96;
        case Qt::Key_F6:
                return 97;
        case Qt::Key_F7:
                return 98;
        case Qt::Key_F8:
                return 100;
        case Qt::Key_F9:
                return 101;
        case Qt::Key_F10:
                return 109;
        case Qt::Key_F11:
                return 103;
        case Qt::Key_F12:
                return 111;
        case Qt::Key_Plus:
                return 69;
        case Qt::Key_Comma:
                return 43;

        default:
                return qtkey;
        }
}
//*/

void EventHandler::macSetMouseLButtonDblClick(int x,int y)
{
        CGEventRef mouseDown=CGEventCreateMouseEvent(NULL,kCGEventLeftMouseDown , CGPointMake(x,y), 0);
        CGEventRef mouseUp=CGEventCreateMouseEvent(NULL,kCGEventLeftMouseUp , CGPointMake(x,y), 0);
        CGEventSetIntegerValueField(mouseDown, kCGMouseEventClickState, 1);
        CGEventSetIntegerValueField(mouseUp, kCGMouseEventClickState, 1);
        CGEventPost(kCGHIDEventTap, mouseDown);
        CGEventPost(kCGHIDEventTap, mouseUp);
        CGEventSetIntegerValueField(mouseDown, kCGMouseEventClickState, 2);
        CGEventSetIntegerValueField(mouseUp, kCGMouseEventClickState, 2);
        CGEventPost(kCGHIDEventTap, mouseDown);
        CGEventPost(kCGHIDEventTap, mouseUp);
        CFRelease(mouseDown);
        CFRelease(mouseUp);
}

void EventHandler::macSetMouseRButtonDblClick(int x,int y)
{
        CGEventRef mouseDown=CGEventCreateMouseEvent(NULL,kCGEventRightMouseDown , CGPointMake(x,y), 0);
        CGEventRef mouseUp=CGEventCreateMouseEvent(NULL,kCGEventRightMouseUp , CGPointMake(x,y), 0);
        CGEventSetIntegerValueField(mouseDown, kCGMouseEventClickState, 1);
        CGEventSetIntegerValueField(mouseUp, kCGMouseEventClickState, 1);
        CGEventPost(kCGHIDEventTap, mouseDown);
        CGEventPost(kCGHIDEventTap, mouseUp);
        CGEventSetIntegerValueField(mouseDown, kCGMouseEventClickState, 2);
        CGEventSetIntegerValueField(mouseUp, kCGMouseEventClickState, 2);
        CGEventPost(kCGHIDEventTap, mouseDown);
        CGEventPost(kCGHIDEventTap, mouseUp);
        CFRelease(mouseDown);
        CFRelease(mouseUp);
}

void EventHandler::macSetMouseMButtonDblClick(int x,int y)
{
        CGEventRef mouseDown=CGEventCreateMouseEvent(NULL,kCGEventOtherMouseDown , CGPointMake(x,y), 0);
        CGEventRef mouseUp=CGEventCreateMouseEvent(NULL,kCGEventOtherMouseUp , CGPointMake(x,y), 0);
        CGEventSetIntegerValueField(mouseDown, kCGMouseEventClickState, 1);
        CGEventSetIntegerValueField(mouseUp, kCGMouseEventClickState, 1);
        CGEventPost(kCGHIDEventTap, mouseDown);
        CGEventPost(kCGHIDEventTap, mouseUp);
        CGEventSetIntegerValueField(mouseDown, kCGMouseEventClickState, 2);
        CGEventSetIntegerValueField(mouseUp, kCGMouseEventClickState, 2);
        CGEventPost(kCGHIDEventTap, mouseDown);
        CGEventPost(kCGHIDEventTap, mouseUp);
        CFRelease(mouseDown);
        CFRelease(mouseUp);
}




#endif


