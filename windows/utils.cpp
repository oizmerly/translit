#include "version.h"
#include <Windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#pragma warning(disable: 4311)

void RedirectIoToConsole()
{
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	const unsigned int MAX_CONSOLE_LINES = 10000;

	// allocate a console for this app
	if (!AllocConsole())
		return;

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
		&coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
		coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
}

int xwstrcmp(const wchar_t* mask, const wchar_t* s)
{
	while (*mask)
	{
		switch (*mask)
		{
		case '?':
			if (!*s)
				return (0);
			s++;
			mask++;
			break;
		case L'*':
			while (*mask == L'*')
				mask++;
			if (!*mask)
				return ( 1 );
			if (*mask == L'?')
				break;
			while (*s != *mask)
			{
				if (!*s)
					return (0);
				s++;
			}
			s++;
			mask++;
			break;
		default:
			if (*s != *mask)
				return (0);
			s++;
			mask++;
		}
	}
	if (!*s && *mask)
		return (0);
	return (1);
}

void OpenBrowser(wchar_t* url)
{
    ShellExecute(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
}