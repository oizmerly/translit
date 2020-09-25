#pragma once

#include <string>
#include <set>
#include "utils.h"

/*!
Low level keyboard hook
*/
class KbdHook
{
public:	
	struct KbdEventTranslator 
	{
        typedef std::pair<std::wstring, int> TranslationResult; // chars, backspaces
		virtual TranslationResult translateKeystroke(const VKey vk) = 0;
	};
	struct KbdEventsListener
	{
		virtual void switchActiveState() = 0;
		virtual void animate() = 0;
	};

	KbdHook(KbdEventTranslator& translator, KbdEventsListener& listener);
	virtual ~KbdHook();
	void setSwitchKeys(const std::set<VKey>& switchKeys);
    void setActive(bool isActive);
	
private:
	DWORD WINAPI hookWndProcThread(PVOID param);
	HANDLE hookWndProcThreadHandle;
	DWORD hookWndProcThreadId;
	WinAPIThunk<KbdHook,  
			    DWORD (WINAPI KbdHook::*)(PVOID), 
				LPTHREAD_START_ROUTINE> hookWndProcThreadThunk;

	LRESULT CALLBACK hookHandler(int nCode, WPARAM wParam, LPARAM lParam);
	WinAPIThunk<KbdHook,  
				LRESULT (CALLBACK KbdHook::*)(int, WPARAM, LPARAM), 
				HOOKPROC> hookHandlerThunk;

	void kbdEvent(wchar_t c, bool upperCase = false, int count = 1) const;
    bool isControlKey() const;
	bool checkSwitchKeys(bool isKeyDown, VKey vk);

	KbdEventTranslator& translator;
	KbdEventsListener& eventsListener;
	std::set<VKey> switchKeys;
    bool isActive;
};
