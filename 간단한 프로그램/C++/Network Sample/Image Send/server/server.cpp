#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <locale.h>

#include "ringBuffer.h"

#pragma comment(lib, "ws2_32")


struct stPacketHeader {

	DWORD code;
	WCHAR name[32];
	WCHAR fileName[128];
	int fileSize;

};

constexpr int SERVER_PORT = 9000;
constexpr int BUFFER_SIZE = 1460;
constexpr int RINGBUFFER_SIZE = BUFFER_SIZE * 2;

void printWSAError() {
	int error = WSAGetLastError();
	wprintf(L"wsa error = %d\n", error);
}

void printError() {
	int error = GetLastError();
	wprintf(L"error = %d\n", error);
}

int main() {
	setlocale(LC_ALL, "ko-KR");
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NULL) {
		printWSAError();
		return 1;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		printWSAError();
		return 1;
	}

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVER_PORT);
	int result = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR) {
		printWSAError();
		return 1;
	}
	
	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		printWSAError();
		return 1;
	}

	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen;

	CRingBuffer ringBuffer(RINGBUFFER_SIZE);
	char buf[BUFFER_SIZE + 1];

	stPacketHeader packetHeader;

	bool findHeader = false;

	BYTE* fileData = nullptr;
	BYTE* fileLastData = nullptr;
	int fileDataLen;

	while (1) {

		addrLen = sizeof(clientAddr);
		clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET) {
			printWSAError();
			break;
		}
		InetNtopA(AF_INET, &clientAddr.sin_addr, buf, BUFFER_SIZE);
		printf("클라이언트 접속: IP = %s, PORT = %d\n", buf, ntohs(clientAddr.sin_port));

		stPacketHeader packetHeader;
		ZeroMemory(&packetHeader, sizeof(stPacketHeader));
		while (1) {

			ZeroMemory(buf, BUFFER_SIZE + 1);
			result = recv(clientSocket, buf, BUFFER_SIZE, 0);
			if (result == SOCKET_ERROR || result == 0) {
				printWSAError();
				break;
			}

			ringBuffer.push(result, (const BYTE*)buf);

			if (findHeader == false){
				if (ringBuffer.size() >= sizeof(stPacketHeader)) {

					ringBuffer.front(sizeof(stPacketHeader), (BYTE*)&packetHeader);
					ringBuffer.pop(sizeof(stPacketHeader));
					findHeader = true;
					wprintf(L"[VALUE] %x %s\n", packetHeader.code, packetHeader.name);
				}
			}
			else {

				if (fileData == nullptr) {
					fileData = (BYTE*)malloc(sizeof(BYTE) * packetHeader.fileSize);
					fileLastData = fileData;
					fileDataLen = 0;
				}

				int popDataLen = ringBuffer.size();
				if (popDataLen + fileDataLen > packetHeader.fileSize) {
					popDataLen = packetHeader.fileSize - fileDataLen;
				}

				ringBuffer.front(popDataLen, fileLastData);
				fileLastData += popDataLen;
				fileDataLen += popDataLen;
				ringBuffer.pop(popDataLen);
				wprintf(L"%d Byte 받음, %d Byte 남음, 전체 %d Byte\n", popDataLen, packetHeader.fileSize - fileDataLen, packetHeader.fileSize);

				if (fileDataLen == packetHeader.fileSize) {

					WCHAR* fileName = (WCHAR*)malloc(sizeof(WCHAR) * (wcslen(packetHeader.fileName) + wcslen(packetHeader.name) + 2));
					wsprintf(fileName, L"%s_%s", packetHeader.name, packetHeader.fileName);

					FILE* writeFile;
					_wfopen_s(&writeFile, fileName, L"wb");
					if (writeFile == nullptr) {
						printError();
						break;
					}

					fwrite(fileData, sizeof(BYTE), fileDataLen, writeFile);
					fclose(writeFile);

					wprintf(L"파일저장 !\n");

					free(fileData);
					fileData = nullptr;
					findHeader = false;

					buf[0] = 0xdd;
					buf[1] = 0xdd;
					buf[2] = 0xdd;
					buf[3] = 0xdd;
					buf[4] = '\0';
					send(clientSocket, buf, 4, 0);
				}

			}

		}

		closesocket(clientSocket);
		InetNtopA(AF_INET, &clientAddr.sin_addr, buf, BUFFER_SIZE);
		printf("클라이언트 종료: IP = %s, PORT = %d\n", buf, ntohs(clientAddr.sin_port));

		findHeader = false;
		free(fileData);
		fileData = nullptr;

	}
	
	closesocket(listenSocket);
	WSACleanup();

	return 0;
}