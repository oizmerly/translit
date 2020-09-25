#include "version.h"
#include <Windows.h>
#include <string>
#include "dbg.h"
#include "utils.h"
#include "KbdHook.h"

using namespace std;

KbdHook::KbdHook(KbdEventTranslator& t, KbdEventsListener& listener)
	: translator(t),
	  eventsListener(listener),
	  hookWndProcThreadThunk(&KbdHook::hookWndProcThread, this),
	  hookHandlerThunk(&KbdHook::hookHandler, this),
      isActive(false)
{
	hookWndProcThreadHandle = 
		CreateThread(0, 0, hookWndProcThreadThunk, 0, 0, &hookWndProcThreadId);
}

KbdHook::~KbdHook()
{
	if (hookWndProcThreadHandle)
	{
		// close the working thread
		PostThreadMessage(hookWndProcThreadId, WM_QUIT, 0, 0);
		if (WaitForSingleObject(hookWndProcThreadHandle, 10000) == WAIT_ABANDONED)
		{
			DBG_E("close working thread - timeout");
		}
		CloseHandle(hookWndProcThreadHandle);
	}
}

void KbdHook::setActive(bool isActive)
{
    DBG_I("set active %d", isActive);
    this->isActive = isActive;
}

void KbdHook::setSwitchKeys(const std::set<VKey>& switchKeys)
{
	this->switchKeys = switchKeys;
}

DWORD WINAPI KbdHook::hookWndProcThread(PVOID param)
{	
	DBG_I("start");		
	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, hookHandlerThunk, GetModuleHandle(0), 0);
	if (! hook)
	{
		DBG_E("cannot install keyboard hook %x", GetLastError());
	}
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (msg.message == WM_QUIT)
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}	
	UnhookWindowsHookEx(hook);
	DBG_I("exit");
	return 0;
}

LRESULT CALLBACK KbdHook::hookHandler(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT kbd = (PKBDLLHOOKSTRUCT)lParam;
	VKey vk = (VKey)kbd->vkCode;   
	bool callNextHook = true;		
	if ((kbd->flags & LLKHF_INJECTED) == 0) // skip the synthetic keystrokes (most
	{                                       // probably produced by ourself) 		
		// check if the active state changed 		
		if (! checkSwitchKeys(wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN, vk) &&
			isActive &&
			nCode >= 0 && 
			wParam == WM_KEYDOWN) // \todo - keyup	
		{       			
			if (! isControlKey())
			{									
				SHORT shiftDown = (GetKeyState(VK_SHIFT) & 0xFF00) != 0;				
				bool upperCase = (shiftDown ^ GetKeyState(VK_CAPITAL)) != 0;
				static bool upperCasePrev = upperCase;
				DBG_I("vk = %x, upper case = %s, upper case prev = %s", 
					vk, upperCase ? "yes" : "no", upperCasePrev ? "yes" : "no");
				if (vk == VK_OEM_7 && ! shiftDown) // \todo - I hate this hard coded :(
				{
					vk = '\'';
					upperCase = upperCasePrev;
				}
				else if (vk == '\'')
				{
					vk = 0;		
				}
				if (('A' <= vk && vk <= 'Z') || vk == '\'')
				{
					DBG_I("key down %c", upperCase ? toupper(vk) : tolower(vk));
				}
				KbdEventTranslator::TranslationResult translation = translator.translateKeystroke(vk);
				kbdEvent(VK_BACK, false, translation.second);
				if (! translation.first.empty())
				{				
					if (translation.second >= 1)
						upperCase = upperCasePrev; // special case in multicharacter combinations
					for (wstring::const_iterator it = translation.first.begin();
						 it != translation.first.end();
						 it ++)
					{				
						kbdEvent(*it, upperCase);
					}
					callNextHook = false;
				}
				upperCasePrev = upperCase;
			}
		}          
	}
	return callNextHook ? CallNextHookEx(0, nCode, wParam, lParam) : TRUE;
}        

void KbdHook::kbdEvent(wchar_t c, bool upperCase /*=false*/, int count /*=1*/) const
{	// send a unicode keyboard event
	INPUT inp = {0};
	inp.type = INPUT_KEYBOARD;
	inp.ki.dwFlags = KEYEVENTF_UNICODE;
	inp.ki.dwExtraInfo = 0;
	//inp.ki.wVk = (short)c;
	if (c == VK_BACK)
		inp.ki.wVk = (short)VK_BACK;
	else
        inp.ki.wScan = upperCase ? c - 32  : c; // \todo - need a normal unicode toupper
    for (int i = 0; i < count; i ++)
	    SendInput(1, &inp, sizeof(inp));
}

bool KbdHook::isControlKey() const
{   // detect if some control key is down (and the current key should not be traslitelated)	
    return ((GetKeyState(VK_CONTROL) & 0xFF00) != 0) || 
           ((GetKeyState(VK_MENU) & 0xFF00) != 0)    || 
           ((GetKeyState(VK_LWIN) & 0xFF00) != 0)	 || 
           ((GetKeyState(VK_RWIN) & 0xFF00) != 0)	 || 
           ((GetKeyState(VK_APPS) & 0xFF00) != 0);
}

bool KbdHook::checkSwitchKeys(bool isKeyDown, VKey vk)
{
	static set<VKey>switchKeysDownSet;
	bool trigger = false;
	if (isKeyDown)
	{		
		if (switchKeys.find(vk) != switchKeys.end())
			switchKeysDownSet.insert(vk);
		else
			switchKeysDownSet.clear();
	}
	else
	{
		trigger = switchKeysDownSet.size() == switchKeys.size();
		switchKeysDownSet.clear();
	}
	if (trigger)
	{
		DBG_W("trigger the active state");
		eventsListener.switchActiveState();
	}
	return trigger;
}