#pragma once

#include <stdio.h>

#define DBG_E(format, ...) \
	{ DBG_OUT("err", FOREGROUND_RED | FOREGROUND_INTENSITY, format, __VA_ARGS__); }

#define DBG_W(format, ...) \
	{ DBG_OUT("wrn", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, format, __VA_ARGS__); }

#define DBG_I(format, ...) \
	{ DBG_OUT("inf", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, format, __VA_ARGS__); }

#define DBG_OUT(severity, color, format, ...)									\
{																				\
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);						\
	WORD oldConsoleAttrs;														\
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;										\
	GetConsoleScreenBufferInfo(consoleHandle, &consoleInfo);					\
	oldConsoleAttrs = consoleInfo.wAttributes;									\
	SetConsoleTextAttribute(consoleHandle, color);								\
	printf("<%s %s:%d> " format "\n", severity,									\
		   __FUNCTION__, __LINE__, __VA_ARGS__);								\
	SetConsoleTextAttribute(consoleHandle, oldConsoleAttrs);					\
}


#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC									
	#include <CRTDBG.H>
	#define DETECT_MEMORY_LEAK()												\
		if (_CrtDumpMemoryLeaks())												\
		{																		\
			DBG_E("memory leak detected");										\
			_CrtDbgBreak();														\
		}
#else
	#define DETECT_MEMORY_LEAK()
#endif