#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "TextParser.h"

TextParser::TextParser(const wchar_t* fileName) {

	FILE* dataFile;
	_wfopen_s(&dataFile, fileName, L"r");

	fseek(dataFile, 0, SEEK_END);
	size = ftell(dataFile);
	rewind(dataFile);

	data = new wchar_t[size + 1];
	
	//fread(data, size, 1, dataFile);
	wchar_t* dataIndex = data;
	wchar_t* fgetwsResult;
	do {
		fgetwsResult = fgetws(dataIndex, size, dataFile);
		dataIndex += wcslen(dataIndex);
	} while (fgetwsResult != NULL);

	fclose(dataFile);

	addIgnoreCharacter(L' ');
	addIgnoreCharacter(0x000d);
	addIgnoreCharacter(0x000a);
	addIgnoreCharacter(0x0009);
	addIgnoreCharacter(L'\0');

}

// °ø¹é ÁÙ¹Ù²Þ ÅÇ Ã¼Å©
bool TextParser::checkIgnoreCharacter(const wchar_t data) {

	wchar_t* listEnd = ignoreCharacter + ignoreCharacterNum;

	for (wchar_t* ignoreCharIter = ignoreCharacter; ignoreCharIter != listEnd; ++ignoreCharIter) {

		if (data == *ignoreCharIter) {
			return true;
		}

	}
		
	return false;
}

void TextParser::SkipNotValue() {

	while (true) {
		if ( checkIgnoreCharacter(data[idx]) == true ) {
			idx += 1;
			continue;
		}

		if (data[idx] == '/' && data[idx + 1] == '/') {
			while (data[idx++] != 0x0d);
			continue;
		}

		if (data[idx] == '/' && data[idx + 1] == '*') {
			while ((data[idx] == '*' && data[idx+1] == '/') == false) {
				idx += 1;
			}
			idx = idx + 2;
			continue;
		}

		return;
	}

}

bool TextParser::GetNextWord(wchar_t** out) {
	
	free(*out);

	SkipNotValue();

	int start = idx;

	while (true) {
		if ( checkIgnoreCharacter(data[idx]) == false ) {
			idx += 1;
			continue;
		}

		break;
	}

	int end = idx;
	int len = end - start;

	if (end > size) {
		return false;
	}

	*out = (wchar_t*)malloc(sizeof(wchar_t) * (len + 1));

	for (int cnt = start; cnt < end; ++cnt) {
		(*out)[cnt - start] = data[cnt];
	}

	(*out)[len] = '\0';

	return true;
}

bool TextParser::GetNextText(wchar_t** out) {

	free(*out);

	SkipNotValue();

	idx += 1;
	int start = idx;
	 
	while (true) {
		if (data[idx] != '"') {
			idx += 1;
			continue;
		}

		break;
	}

	int end = idx;
	int len = end - start;

	if (end > size) {
		return false;
	}

	*out = (wchar_t*)malloc(len + 1);

	for (int cnt = start; cnt < end; ++cnt) {
		(*out)[cnt - start] = data[cnt];
	}

	(*out)[len] = '\0';

	return true;
}

bool TextParser::GetValueByKey(const wchar_t* const key, wchar_t** out) {
	
	idx = 0;
	wchar_t* strTemp = nullptr;
	while (GetNextWord(&strTemp)) {
		if (wcscmp(key, strTemp) == 0) {
			bool result = GetNextWord(&strTemp);
			if (result == false) {
				return false;
			}

			if (wcscmp(L":", strTemp) == 0) {
				GetNextWord(&strTemp);
				if (result == false) {
					return false;
				}

				free(*out);
				int len = wcslen(strTemp);
				*out = (wchar_t*)malloc(len + 1);
				wcscpy_s(*out, len + 1, strTemp);
				(*out)[len] = '\0';
				return true;

			}
		}
	}

	return false;
}


bool TextParser::GetTextByKey(const wchar_t* const key, wchar_t** out) {

	idx = 0;
	wchar_t* strTemp = nullptr;
	while (GetNextWord(&strTemp)) {
		if (wcscmp(key, strTemp) == 0) {
			bool result = GetNextWord(&strTemp);
			if (result == false) {
				return false;
			}

			if (wcscmp(L":", strTemp) == 0) {

				GetNextText(&strTemp);
				
				if (result == false) {
					return false;
				}

				free(*out);
				int len = wcslen(strTemp);
				*out = (wchar_t*)malloc(len + 1);
				wcscpy_s(*out, len + 1, strTemp);
				(*out)[len] = '\0';
				return true;

			}
		}
	}

	return false;

}

void TextParser::addIgnoreCharacter(wchar_t addCharacter) {

	ignoreCharacter[ignoreCharacterNum] = addCharacter;
	ignoreCharacterNum += 1;

}

void TextParser::addIgnoreCharacter(const wchar_t* addCharater) {

	ignoreCharacter[ignoreCharacterNum] = *addCharater;
	ignoreCharacterNum += 1;

}