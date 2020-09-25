#pragma once

#include <map>
#include <string>
#include <set>

class CfgManager
{
public:	
	CfgManager();	
	operator bool() const { return true; }
	const std::map<std::wstring, std::wstring>& getTranslitTemplates() const;
	const std::set<VKey>& getSwitchKeys() const;
private: 
	void setSwitchKeys(wchar_t* keys);
	USHORT textKeyToVK(wchar_t* key) const;
	std::map<std::wstring, std::wstring> translitTemplates;
	std::set<VKey> switchKeys;
};