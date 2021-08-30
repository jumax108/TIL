#pragma once
class TextParser{

public:

	TextParser(const wchar_t* fileName);

	bool GetValueByKey(
		const wchar_t* const key, // value를 찾을 key 값
		wchar_t** out // value 출력
	);

	bool GetTextByKey(
		const wchar_t* const key, // value를 찾을 key 값
		wchar_t** out // value 출력
	);

	bool GetNextWord(
		wchar_t** out
	);

	void addIgnoreCharacter(wchar_t addCharacter);
	void addIgnoreCharacter(const wchar_t* addCharacter);

private:

	wchar_t* data;
	int idx = 0;
	int size;

	unsigned char ignoreCharacterNum ;
	wchar_t ignoreCharacter[50];

	void SkipNotValue();

	bool GetNextText(
		wchar_t** out
	);

	bool checkIgnoreCharacter(const wchar_t data);

};