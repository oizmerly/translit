#pragma once

#include <string>
#include <map>
#include "KbdHook.h"

class TranslitTranslator : public KbdHook::KbdEventTranslator
{
public:
	void setTranslitTemplates(const std::map<std::wstring, std::wstring>& t);	
private:
	virtual TranslationResult translateKeystroke(const VKey vk);

	std::map<std::wstring, std::wstring> translitTemplates;
	std::wstring inputSequence;
};