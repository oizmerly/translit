#include "version.h"
#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include "TrayIcon.h"
#include "utils.h"
#include "dbg.h"

void test()
{	
	wchar_t c = L'à';
	DBG_W("====>>> %x", c)

	Sleep(5000);

	INPUT inp = {0};
	inp.type = INPUT_KEYBOARD;
	inp.ki.dwFlags = KEYEVENTF_UNICODE;
	inp.ki.dwExtraInfo = 0;
	//inp.ki.wVk = (short)c;
	if (c == VK_BACK)
		inp.ki.wVk = (short)VK_BACK;
	else
		inp.ki.wScan = (short)c;
	UINT x = SendInput(1, &inp, sizeof(inp));
}