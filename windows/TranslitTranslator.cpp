#include "version.h"
#include <Windows.h>
#include "dbg.h"
#include "utils.h"
#include "TranslitTranslator.h"

using namespace std;

void TranslitTranslator::setTranslitTemplates(const map<wstring, wstring>& t)
{
	translitTemplates = t;
}

TranslitTranslator::TranslationResult TranslitTranslator::translateKeystroke(const VKey vk)
{
    TranslationResult res(L"", 0);
	if (('A' <= vk && vk <= 'Z') || vk == '\'')
	{
start:
		inputSequence += (wchar_t)tolower(vk);
		bool isFound = false;
		for (map<wstring, wstring>::const_iterator it = translitTemplates.begin();
			 it != translitTemplates.end();
			 it ++)
		{
			if (it->first == inputSequence)
            {
                res = TranslationResult(it->second, (int)it->first.length() > 1 ? 1 : 0);
				isFound = true;
				break;
            }
		}
		if (! isFound && ! inputSequence.empty())
		{			
			inputSequence.erase(inputSequence.begin(), inputSequence.end()--);
			goto start;
		}
	}
	else
	{
        DBG_I("drop the current input sequence");
		inputSequence.erase();
	}
	return res;
}