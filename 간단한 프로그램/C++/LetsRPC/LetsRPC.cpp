#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <wchar.h>

#include "ringBuffer.h"	
#include "LinkedList.h"
#include "LetsRPC.h"
#include "TextParser.h"

FILE* commonFile;
FILE* recvFile;
FILE* sendFile;

void CLetsRPC::process(const wchar_t* fileName) {

	_parser = new TextParser(fileName);

	_parser->addIgnoreCharacter(L'(');
	_parser->addIgnoreCharacter(L')');
	_parser->addIgnoreCharacter(L',');

	splitData();

	_wfopen_s(&commonFile, L"output\\common.h", L"w");
	makeStruct();	  // common.h
	makeConstVal();   // common.h
	fclose(commonFile);
	
	_wfopen_s(&recvFile, L"output\\recv.h", L"w");
	makeProxyClass(); // recv.h
	fclose(recvFile);

	_wfopen_s(&sendFile, L"output\\send.h", L"w");
	makeStubClass();  // send.h
	fclose(sendFile);

	delete(_parser);
}

void CLetsRPC::splitData() {

	wchar_t* funcName = nullptr;
	wchar_t* typeName = nullptr;
	wchar_t* valName = nullptr;
	_func = new linkedList<stFunc*>();

	int num = 0;

	while (true) {
		funcName = nullptr;
		bool res = _parser->GetNextWord(&funcName);
		if (funcName == nullptr || res == 0) {
			break;
		}
		if (wcscmp(funcName, L"[num]") == 0) {
			wchar_t* nextNum = nullptr;
			_parser->GetNextWord(&nextNum);
			_parser->GetNextWord(&nextNum);
			num = wcstol(nextNum, NULL, 0);
			continue;
		}
		
		stFunc* func = (stFunc*)malloc(sizeof(stFunc));
		func->funcName = funcName;
		func->argu = new linkedList<stArgu*>();
		linkedList<stArgu*>* arguList = func->argu;
		while (1) {
			stArgu* argu = (stArgu*)malloc(sizeof(stArgu));
			typeName = nullptr;
			valName = nullptr;

			_parser->GetNextWord(&typeName);
			if (wcscmp(L"unsigned", typeName) == 0) {
				argu->isSigned = false;
			}
			else {
				argu->isSigned = true;
			}

			if (argu->isSigned == false) {
				_parser->GetNextWord(&typeName);
			}

			if (wcscmp(L";", typeName) == 0) {
				free(argu);
				break;
			}

			_parser->GetNextWord(&valName);

			argu->typeName = typeName;
			argu->valName = valName;
			arguList->push_back(argu);
		}
		func->num = num;
		num += 1;
		_func->push_back(func);
	}
}


void CLetsRPC::makeStruct() {

	fwprintf(commonFile, L"pragma pack(1)\n");
	fwprintf(commonFile, L"struct stHeader{\n");
	fwprintf(commonFile, L"\tchar code;\n");
	fwprintf(commonFile, L"\tchar payloadSize;\n");
	fwprintf(commonFile, L"\tchar payloadType;\n");
	fwprintf(commonFile, L"};\n");


	for (linkedList<stFunc*>::iterator funcIter = _func->begin(); funcIter != _func->end(); ++funcIter) {
		fwprintf(commonFile, L"struct st%s{\n",(*funcIter)->funcName);
		linkedList<stArgu*>* argu = (*funcIter)->argu;
		for (linkedList<stArgu*>::iterator arguIter = argu->begin(); arguIter != argu->end(); ++arguIter) {
			fwprintf(commonFile, L"\t");
			if ((*arguIter)->isSigned == false) {
				fwprintf(commonFile, L"unsigned ");
			}
			fwprintf(commonFile, L"%s %s;\n",(*arguIter)->typeName, (*arguIter)->valName);
		}
		fwprintf(commonFile, L"};\n");
	}

	fwprintf(commonFile, L"pragma pack(0)\n");
}

void CLetsRPC::makeProxyClass() {

	fwprintf(recvFile, L"class CProxyFuncBase{\n");
	fwprintf(recvFile, L"public:\n");
	for (linkedList<stFunc*>::iterator funcIter = _func->begin(); funcIter != _func->end(); ++funcIter) {
		fwprintf(recvFile, L"\tvirtual bool %sProxy(", (*funcIter)->funcName);
		linkedList<stArgu*>* argu = (*funcIter)->argu;
		for (linkedList<stArgu*>::iterator arguIter = argu->begin(); arguIter != argu->end(); ++arguIter) {
			if (arguIter != argu->begin()) {
				fwprintf(recvFile, L", ");
			}
			if ((*arguIter)->isSigned == false) {
				fwprintf(recvFile, L"unsigned ");
			}
			fwprintf(recvFile, L"%s %s", (*arguIter)->typeName, (*arguIter)->valName);
			
		}
		fwprintf(recvFile, L"){ return false; }\n");
	}
	makePacketProc(); // recv.h
	fwprintf(recvFile, L"};\n");

}

void CLetsRPC::makeStubClass() {

	fwprintf(sendFile, L"class CNetwork;\n");
	fwprintf(sendFile, L"extern CNetwork* network;\n");
	fwprintf(sendFile, L"class CStubFunc{\n");
	for (linkedList<stFunc*>::iterator funcIter = _func->begin(); funcIter != _func->end(); ++funcIter) {
		fwprintf(sendFile, L"\tvirtual bool %sStub(CRingBuffer* sendBuffer", (*funcIter)->funcName);
		linkedList<stArgu*>* argu = (*funcIter)->argu;
		for (linkedList<stArgu*>::iterator arguIter = argu->begin(); arguIter != argu->end(); ++arguIter) {
			fwprintf(sendFile, L", ");
			if ((*arguIter)->isSigned == false) {
				fwprintf(sendFile, L"unsigned ");
			}
			fwprintf(sendFile, L"%s %s", (*arguIter)->typeName, (*arguIter)->valName);

		}
		fwprintf(sendFile, L")\n");
		fwprintf(sendFile, L"\t{\n");
		fwprintf(sendFile, L"\t\tCProtocolBuffer packet(50);\n");
		fwprintf(sendFile, L"\t\tpacket<<(char)0x89;\n");
		fwprintf(sendFile, L"\t\tpacket<<(char)sizeof(st%s);\n",(*funcIter)->funcName);
		fwprintf(sendFile, L"\t\tpacket<<(char)%s;\n",(*funcIter)->funcName);
		for (linkedList<stArgu*>::iterator arguIter = argu->begin(); arguIter != argu->end(); ++arguIter) {
			fwprintf(sendFile, L"\t\tpacket << %s;\n", (*arguIter)->valName);
		}
		fwprintf(sendFile, L"\t\tsendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());\n");
		fwprintf(sendFile, L"\t\tif(network->ableSendPacket == true){\n");
		fwprintf(sendFile, L"\t\t\tnetwork->sendPacket();\n");
		fwprintf(sendFile, L"\t\t}\n");
		fwprintf(sendFile, L"\t}\n");
	}
	fwprintf(sendFile, L"};\n");

}

void CLetsRPC::makeConstVal() {

	for (linkedList<stFunc*>::iterator funcIter = _func->begin(); funcIter != _func->end(); ++funcIter) {
		fwprintf(commonFile, L"constexpr unsigned int %s = %d;\n", (*funcIter)->funcName, (*funcIter)->num);
	}
}

void CLetsRPC::makePacketProc() {

	fwprintf(recvFile, L"\tvoid packetProc(stHeader* header, CProtocolBuffer* payload, CProxyFuncBase* proxy){\n");
	fwprintf(recvFile, L"\t\tswitch(header->payloadType){\n");
	for (linkedList<stFunc*>::iterator funcIter = _func->begin(); funcIter != _func->end(); ++funcIter) {
		fwprintf(recvFile, L"\t\t\tcase %s:\n", (*funcIter)->funcName);
		fwprintf(recvFile, L"\t\t\t{\n");
		linkedList<stArgu*>* arguList = (*funcIter)->argu;
		for (linkedList<stArgu*>::iterator arguIter = arguList->begin(); arguIter != arguList->end(); ++arguIter) {
			fwprintf(recvFile, L"\t\t\t\t");
			if ((*arguIter)->isSigned == false) {
				fwprintf(recvFile, L"unsigned ");
			}
			fwprintf(recvFile, L"%s %s;\n", (*arguIter)->typeName, (*arguIter)->valName);
			fwprintf(recvFile, L"\t\t\t\t*payload >> %s;\n", (*arguIter)->valName);
		}
		fwprintf(recvFile, L"\t\t\t\tproxy->%sProxy(", (*funcIter)->funcName);
		for (linkedList<stArgu*>::iterator arguIter = arguList->begin(); arguIter != arguList->end(); ++arguIter) {
			if (arguIter != arguList->begin()) {
				fwprintf(recvFile, L", ");
			}
			fwprintf(recvFile, L"%s", (*arguIter)->valName);
		}
		fwprintf(recvFile, L");\n");
		fwprintf(recvFile, L"\t\t\t}\n");
		fwprintf(recvFile, L"\t\t\tbreak;\n");
	}
	fwprintf(recvFile, L"\t\t}\n");
	fwprintf(recvFile, L"\t}\n");

}

void CLetsRPC::makeRecvProc() {
	fwprintf(recvFile, L"void recvFunc(SOCKET sock, CRingBuffer* recvBuf){\n");
	fwprintf(recvFile, L"\twhile(1){\n");
	fwprintf(recvFile, L"\t\tunsigned int readSize = recvBuffer.getDirectFreeSize();\n");
	fwprintf(recvFile, L"\t\tint recvResult = recv(sock, recvBuffer.getRearPtr(), readSize, 0);\n");
	fwprintf(recvFile, L"\t\tif(recvResult == SOCKET_ERROR){\n");
	fwprintf(recvFile, L"\t\t\tint recvError = WSAGetLastError();\n");
	fwprintf(recvFile, L"\t\t\tif(recvError == WSAEWOULDBLOCK){\n");
	fwprintf(recvFile, L"\t\t\t\tbreak;\n");
	fwprintf(recvFile, L"\t\t\t}\n");
	fwprintf(recvFile, L"\t\t\treturn ;\n");
	fwprintf(recvFile, L"\t\t}\n");
	fwprintf(recvFile, L"\t\trecvBuffer.moveRear(recvResult);\n");
	fwprintf(recvFile, L"\t}\n");
	fwprintf(recvFile, L"\tunsigned int usedSize\n");
	fwprintf(recvFile, L"\trecvBuf->getUsedSize(&usedSize);\n");
	fwprintf(recvFile, L"\twhile(usedSize >= sizeof(stHeader)){\n");
	fwprintf(recvFile, L"\t\tstHeader header;\n");
	fwprintf(recvFile, L"\t\trecvBuf->front(sizeof(header), (char*)&header);\n");
	fwprintf(recvFile, L"\t\tif(usedSize < sizeof(stHeader) + header.payloadSize){\n");
	fwprintf(recvFile, L"\t\t\treturn ;\n");
	fwprintf(recvFile, L"\t\t}\n");
	fwprintf(recvFile, L"\t\trecvBuf->pop(sizeof(header));\n");
	fwprintf(recvFile, L"\t\tCProtocolBuffer protocolBuffer(sizeof(header));\n");
	fwprintf(recvFile, L"\t\trecvBuffer->front(header.size, protocolBuffer.getRearPtr());\n");
	fwprintf(recvFile, L"\t\tprotocolBuffer.moveRear(header.size);\n");
	fwprintf(recvFile, L"\t\tmakePacketProc(&header, &protocolBuffer);\n");
	fwprintf(recvFile, L"\t\trecvBuf->getUsedSize(&usedSize);\n");
	fwprintf(recvFile, L"\t}\n");
	fwprintf(recvFile, L"}\n");
}

void CLetsRPC::makeSendProc() {
	fwprintf(sendFile, L"void sendProc(SOCKET sock, CRingBuffer* sendBuf)\n");
	fwprintf(sendFile, L"{\n");
	fwprintf(sendFile, L"\twhile(1){\n");
	fwprintf(sendFile, L"\t\tunsigned int sendBufUsedSize;\n");
	fwprintf(sendFile, L"\t\tsendBuf->getUsedSize(&sendBufUsedSize);\n");
	fwprintf(sendFile, L"\t\tif(sendBufUsedSize == 0){\n");
	fwprintf(sendFile, L"\t\t\treturn;\n");
	fwprintf(sendFile, L"\t\t}\n");
	fwprintf(sendFile, L"\t\tunsigned int sendSIze = min(sendBufUsedSize, sendBuf->getDirectSendSize());\n");
	fwprintf(sendFile, L"\t\tint sendResult = send(sock, sendBuffer->GetFrontPtr(), sendSize, 0);\n");
	fwprintf(sendFile, L"\t\tif(sendResult == SOCKET_ERROR){\n");
	fwprintf(sendFile, L"\t\t\tint sendError = WSAGetLastError();\n");
	fwprintf(sendFile, L"\t\t\tif(sendError == WSAEWOULDBLOCK){\n");
	fwprintf(sendFile, L"\t\t\t\tbreak;\n");
	fwprintf(sendFile, L"\t\t\t}\n");
	fwprintf(sendFile, L"\t\t\treturn;\n");
	fwprintf(sendFile, L"\t\t}\n");
	fwprintf(sendFile, L"\t\tsendBuf->moveFront(sendResult);\n");
	fwprintf(sendFile, L"\t}\n");
	fwprintf(sendFile, L"}\n");
}