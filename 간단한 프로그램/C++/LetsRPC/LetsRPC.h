#pragma once

class TextParser;

class CLetsRPC {
public:

	struct stArgu {
		bool isSigned;
		wchar_t* typeName;
		wchar_t* valName;
	};

	struct stFunc {
		wchar_t* funcName;
		linkedList<stArgu*>* argu;
		int num;
	};

public:

	void process(const wchar_t* fileName);

private:

	TextParser* _parser;
	wchar_t* _fileData;
	unsigned int _fileDataSize;

	linkedList<stFunc*>* _func;

	void splitData();
	void makeStruct();
	void makeProxyClass();
	void makeStubClass();
	void makeConstVal();
	void makePacketProc();
	void makeRecvProc();
	void makeSendProc();

};
