#include "version.h"
#include <Windows.h>
#include <fstream>
#include <string>
#include "dbg.h"
#include "utils.h"
#include "CfgManager.h"

using namespace std;

CfgManager::CfgManager()
{
	DBG_I("");
	wifstream cfgFile(CFG_FILE_NAME);
	if (! cfgFile)
	{
		DBG_E("cannot open cfg file");
	}
	else
	{
		enum {None, Settings, Templates} secton = None;
		wchar_t line[256];
		while (cfgFile.getline(line, sizeof(line)))
		{			
			// fix Russian unicode chars 
			for (size_t i = 0; i < wcslen(line); i ++)
			{	
				wchar_t& c = line[i];
				if (0xE0 <= c && c <= 0xE0 + 33)
					c = c - 0xE0 + 0x430;
			}			
			if (xwstrcmp(L"[SETTINGS]*", line))
			{
				DBG_I("settings section start");
				secton = Settings;
				continue;
			}
			if (xwstrcmp(L"[TEMPLATES]*", line))
			{
				DBG_I("templates section start");
				secton = Templates;
				continue;
			}
			switch (secton)
			{
			case Settings :
				{
					wchar_t* optionName = NULL;
					wchar_t* nextToken = NULL;
					optionName = wcstok_s(line, L" =\t\n\r", &nextToken);
					if (optionName)
					{
						wcstok_s(NULL, L" \t", &nextToken); // skip '='
						wchar_t* optionValue = NULL;
						optionValue = wcstok_s(NULL, L"\n\r", &nextToken);
						DBG_I("option %ws = %ws", optionName, optionValue);
						if (wcscmp(optionName, L"switch_keys") == 0)
						{
							setSwitchKeys(optionValue);
						}
					}					
				}
				break;
			case Templates:
				{
					wchar_t* inp = NULL;
					wchar_t* outp = NULL;
					wchar_t* nextToken = NULL;
					inp = wcstok_s(line, L" \t\n\r", &nextToken);
					outp = wcstok_s(NULL, L" \t\n\r", &nextToken);
					translitTemplates[inp] = outp;
				}
			    break;
			default :
				DBG_E("unknown section");
			}			
		}
		cfgFile.close();
	}
}

const map<wstring, wstring>& CfgManager::getTranslitTemplates() const
{
	return translitTemplates;
}

const std::set<VKey>& CfgManager::getSwitchKeys() const
{
	return switchKeys;
}

void CfgManager::setSwitchKeys(wchar_t* keys)
{
	DBG_I("switch keys: %ws", keys);	
	for (wchar_t *next = NULL, *key = wcstok_s(keys, L" \t\n\r", &next);
		 key;
		 key = wcstok_s(NULL, L" \t\n\r", &next))
	{
		DBG_I("key %ws", key);
		VKey vk = textKeyToVK(key);
		if (! vk)
		{
			DBG_E("%ws is underfined", key);
		}
		else
		{
			DBG_I("vk = %02x", vk);
			switchKeys.insert(vk);
		}
	}
}

VKey CfgManager::textKeyToVK(wchar_t* key) const
{
	VKey retval = 0;
	struct TextKey { wchar_t* str; VKey vk; };
	TextKey textKeys[] = 
	{
		{L"F1",VK_F1},{L"F2",VK_F2},{L"F3",VK_F3},{L"F4",VK_F4},{L"F5",VK_F5},{L"F6",VK_F6},
		{L"F7",VK_F7},{L"F8",VK_F8},{L"F9",VK_F9},{L"F10",VK_F10},{L"F11",VK_F11},{L"F12",VK_F12},
		{L"LMENU",VK_LMENU},{L"RMENU",VK_RMENU},{L"MENU",VK_MENU},
		{L"LCONTROL",VK_LCONTROL},{L"RCONTROL",VK_RCONTROL},{L"CONTROL",VK_CONTROL},
		{L"LSHIFT",VK_LSHIFT},{L"RSHIFT",VK_RSHIFT},{L"SHIFT",VK_SHIFT},
		{L"LWIN",VK_LWIN},{L"RWIN",VK_RWIN},
		{L"APPS",VK_APPS}
	};
	if (wcslen(key) == 1)
	{
		retval = (VKey)key;
	}	
	else
	{
		for (int i = 0; i < _countof(textKeys); i ++)
		{
			if (wcscmp(key, textKeys[i].str) == 0)
			{
				retval = textKeys[i].vk;
				break;
			}
		}
	}
	DBG_I("%ws --> %02x", key, retval);
	return retval;
}