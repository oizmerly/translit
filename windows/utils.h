#pragma once

/// Open a console and redirect all in/out to it
void RedirectIoToConsole();

/// Navigate the default browser to url
void OpenBrowser(wchar_t* url);

/// Compares strings using DOS wildcards
int xwstrcmp(const wchar_t* mask, const wchar_t* s);

template<typename To, typename From> inline To 
union_cast(From fr) throw()
{
	union
	{
		From f;
		To t;
	} uc;
	uc.f = fr;
	return uc.t;
}

/// A helper for callback methods invocation
#ifndef _M_IX86
#pragma message("WinAPIThunk is implemented for X86 only")
#endif

#pragma warning(disable: 4355) // 'this' : used in base member initializer list (we explicitly want to do this)

#pragma pack(push)
#pragma pack(push, 1)
template<typename ClassName, typename typename Method, typename Callback>
class WinAPIThunk
{		
	BYTE push_dword_ptr_esp[3];
	DWORD mov_dword_ptr_esp_plus_4_this;
	ClassName* this_ptr;
	BYTE jmp_func; 
	ptrdiff_t jmp_func_offset;  
public:	
	WinAPIThunk(Method method, ClassName* self) 
	{
		push_dword_ptr_esp[0] = 0xFF; push_dword_ptr_esp[1] = 0x34; push_dword_ptr_esp[2] = 0x24;
		mov_dword_ptr_esp_plus_4_this = 0x042444C7;
		this_ptr = self;
		jmp_func = 0xE9;
		jmp_func_offset = 
			union_cast<char*>(method) - reinterpret_cast<char*>(this) - sizeof(WinAPIThunk);
		FlushInstructionCache(GetCurrentProcess(), this, sizeof(WinAPIThunk));
	}

	operator Callback() const
	{
		return reinterpret_cast<Callback>(this);
	}
};
#pragma pack(pop)
