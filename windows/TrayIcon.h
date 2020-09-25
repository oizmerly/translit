#pragma once

#include "utils.h"
#include "KbdHook.h"

/*!
Tray icon singleton
*/
class TrayIcon : public KbdHook::KbdEventsListener
{	
	WinAPIThunk<TrayIcon,  
				DWORD (WINAPI TrayIcon::*)(PVOID), 
				LPTHREAD_START_ROUTINE> iconWorkingThreadThunk;
	WinAPIThunk<TrayIcon,  
				LRESULT (CALLBACK TrayIcon::*)(HWND, UINT, WPARAM, LPARAM), 
				WNDPROC> notificationWndProcThunk;
public:	
	struct EventListener;
	TrayIcon(const UINT iconPassiveId, 
			 const UINT iconActiveId,		 
			 EventListener& listener);
	virtual ~TrayIcon();	
	void show();

private:
	enum State { Passive, Active, Hover } state;
	LRESULT CALLBACK notificationWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	NOTIFYICONDATA trayIconData;

	// working thread
private:
	DWORD WINAPI iconWorkingThread(PVOID param);
	DWORD iconWorkingThreadId;
	HANDLE iconWorkingThreadHandle;

	// KbdEventsListener methods
	virtual void switchActiveState();
	virtual void animate();

	// icon handlers
private:
	HICON iconPassive, iconActive;

	// event listener
public:
	struct EventListener 
	{
		enum Event { Passive, Active, Exit, About, CheckForUpdates };
		virtual void event(Event e) = 0;
	};
private:
	EventListener& eventListener;
};