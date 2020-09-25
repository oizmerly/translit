#include "version.h"
#include <Windows.h>
#include "resource.h"
#include "dbg.h"
#include "TrayIcon.h"

static const UINT WM_TRAY_MESSAGE = RegisterWindowMessageA("TRANSLITA_WM_TRAY_MESSAGE");
static const UINT WM_KBD_SWITCH_STATE = RegisterWindowMessageA("WM_KBD_SWITCH_STATE");
static const WCHAR TrayIconNotificationWndClass[] = L"TRANSLITA_TRAY_ICON_NOTIFICATION_WND_CLASS";


TrayIcon::TrayIcon(const UINT iconPassiveId, 
				   const UINT iconActiveId, 				   
				   EventListener& listener) 
	: eventListener(listener), 
	  iconWorkingThreadId(0), iconWorkingThreadHandle(NULL), 
	  state(Passive),
	  iconWorkingThreadThunk(&TrayIcon::iconWorkingThread, this),
	  notificationWndProcThunk(&TrayIcon::notificationWndProc, this)
{
	DBG_I("%d %d %d %p", iconPassiveId, iconActiveId, listener);
	iconPassive = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(iconPassiveId));
	iconActive = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(iconActiveId));		
}

TrayIcon::~TrayIcon()
{
	DBG_I("");
	if (iconWorkingThreadHandle)
	{
		// close the working thread
		PostThreadMessage(iconWorkingThreadId, WM_QUIT, 0, 0);
		if (WaitForSingleObject(iconWorkingThreadHandle, 10000) == WAIT_ABANDONED)
		{
			DBG_E("close working thread - timeout");
		}
		CloseHandle(iconWorkingThreadHandle);
	}
}

void TrayIcon::show()
{
	iconWorkingThreadHandle = 
		CreateThread(0, 0, iconWorkingThreadThunk, this, 0, &iconWorkingThreadId);
}

DWORD TrayIcon::iconWorkingThread(PVOID param)
{	
	DBG_I("start");		
	WNDCLASS notificationWndClass;
	ZeroMemory(&notificationWndClass, sizeof(WNDCLASS));
	notificationWndClass.lpszClassName = TrayIconNotificationWndClass;
	notificationWndClass.lpfnWndProc = notificationWndProcThunk;	
	RegisterClass(&notificationWndClass);	
	HWND trayIconNotificationWnd = 
		CreateWindow(TrayIconNotificationWndClass, TrayIconNotificationWndClass, 
					 0, 0, 0, 0, 0, 0, 0, 0, 0);	
	ZeroMemory(&trayIconData, sizeof(NOTIFYICONDATA));
	trayIconData.cbSize = sizeof(NOTIFYICONDATA);
	trayIconData.hWnd = trayIconNotificationWnd;
	trayIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	trayIconData.uCallbackMessage = WM_TRAY_MESSAGE;
	trayIconData.hIcon = iconPassive;
	wcscpy_s(trayIconData.szTip, _countof(trayIconData.szTip), L"TranslitA");
	BOOL b = Shell_NotifyIcon(NIM_ADD, &trayIconData);
	Shell_NotifyIcon(NIM_ADD, &trayIconData);
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (msg.message == WM_QUIT)
			break;
		else if (msg.message == WM_KBD_SWITCH_STATE)
		{
			PostMessage(trayIconNotificationWnd, WM_TRAY_MESSAGE, 0, WM_LBUTTONDOWN);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}	
	Shell_NotifyIcon(NIM_DELETE, &trayIconData);
	DBG_I("exit");
	return 0;
}

LRESULT CALLBACK TrayIcon::notificationWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{		
	if (msg == WM_TRAY_MESSAGE)
	{		
		switch(lParam)
		{
		case WM_MOUSEMOVE :	
			// left for the future implementation of hover
			break;
		case WM_LBUTTONDOWN:
			// change the tray icon + notify the listener
			state = state == Passive ? Active : Passive;
			DBG_I("new state %d", state);
			trayIconData.hIcon = state == Passive ? iconPassive : iconActive;
			Shell_NotifyIcon(NIM_MODIFY, &trayIconData);
			eventListener.event(state == Passive ? EventListener::Passive : EventListener::Active);
			break;
		case WM_RBUTTONDOWN :
			{	
				DBG_I("display tray menu");
				HMENU menu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_TRAY_MENU));
				HMENU hmenuTrackPopup = GetSubMenu(menu, 0);
				POINT mousePos = {0};
				GetCursorPos(&mousePos);
				SetForegroundWindow(hWnd);
				TrackPopupMenu(hmenuTrackPopup, TPM_RIGHTBUTTON, mousePos.x, mousePos.y, 0, hWnd, NULL);
				PostMessage(hWnd, WM_NULL, 0, 0);
				DestroyMenu(menu);				
			}
			break;
		default :			
			break;
		}		
	}
	else if (msg == WM_COMMAND)
	{
		switch (wParam)
		{
		case IDC_TRAY_ABOUT:
			DBG_I("cmd - about");
            eventListener.event(EventListener::About);
			break;
		case IDC_TRAY_CHECK_FOR_UPDATE :
			DBG_I("cmd - check for updates");
            eventListener.event(EventListener::CheckForUpdates);
			break;
		case IDC_TRAY_EXIT :
			DBG_I("cmd - exit");
			eventListener.event(EventListener::Exit);
			break;
		default:
			break;
		}
	}
	else
	{		
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}    
	return 0;
}

void TrayIcon::switchActiveState()
{
	DBG_I("");
	PostThreadMessage(iconWorkingThreadId, WM_KBD_SWITCH_STATE, 0, 0);
}

void TrayIcon::animate()
{
	// \todo 
}
