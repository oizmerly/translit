#include "version.h"
#include <Windows.h>
#include "resource.h"
#include "dbg.h"
#include "utils.h"
#include "CfgManager.h"
#include "TrayIcon.h"
#include "KbdHook.h"
#include "TranslitTranslator.h"

class Application : public TrayIcon::EventListener 
{
public:
	Application() : kbdHook(translator, trayIcon), trayIcon(IDI_APP_ICON_PASSIVE, IDI_APP_ICON_ACTIVE, *this)
	{}

	void run()
	{
		translator.setTranslitTemplates(cfg.getTranslitTemplates());
		kbdHook.setSwitchKeys(cfg.getSwitchKeys());
		wndProcThreadId = GetCurrentThreadId();		
		trayIcon.show();
		// application message loop
		MSG msg;
		while (GetMessage(&msg, 0, 0, 0))
		{
			TranslateMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				DBG_I("exit application");
				break;
			}
			DispatchMessage(&msg);
		}
	}

private:
	void event(Event e)
	{
		DBG_I("event = %d", e);
		switch(e)
		{
        case EventListener::About :			
            OpenBrowser(APPLICATION_ABOUT_URL);
            break;
        case EventListener::CheckForUpdates :			
            OpenBrowser(APPLICATION_CHECK_FOR_UPDATES_URL);
            break;
		case EventListener::Active :			
            kbdHook.setActive(true);
			break;
		case EventListener::Passive :
            kbdHook.setActive(false);
			break;
		case EventListener::Exit :
			PostThreadMessage(wndProcThreadId, WM_QUIT, 0, 0);
			break;
		default:
			DBG_E("wrong event id");
		    break;
		}
	}	

private:
	DWORD wndProcThreadId;
	TrayIcon trayIcon;
	KbdHook kbdHook;
	CfgManager cfg;
	TranslitTranslator translator;
};

void test(); // \todo - remove

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR szCmdLine, int)
{
	// \todo - ensure one instance only
    if (strcmp(szCmdLine, "-debug") == 0)
	    RedirectIoToConsole();
	Application app;	
	app.run(); 	
	return 0;
}