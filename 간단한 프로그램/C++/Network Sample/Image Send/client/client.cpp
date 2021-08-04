#include <stdio.h>
#include <locale>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32")

const char* SERVER_IP = "127.0.0.1";
constexpr int SERVER_PORT = 9000;
constexpr int BUFFER_SIZE = 1000;

struct stPacketHeader {

	DWORD code;
	WCHAR name[32];
	WCHAR fileName[128];
	int fileSize;

};

void printWSAError() {
	int error = WSAGetLastError();
	wprintf(L"wsa error = %d\n", error);
}
void printError() {
	int error = GetLastError();
	wprintf(L"error = %d\n", error);
}


int main() {

	SOCKET sock = NULL;

	setlocale(LC_ALL, "ko-KR");
	do {

		FILE* imgFile;
		_wfopen_s(&imgFile, L"냥이.jpg", L"rb");
		if (imgFile == nullptr) {
			printError();
			break;
		}
		int fileSize;
		fseek(imgFile, 0, SEEK_END);
		fileSize = ftell(imgFile);
		rewind(imgFile);

		BYTE* fileBuffer = (BYTE*)malloc(sizeof(BYTE) * fileSize);
		fread_s(fileBuffer, sizeof(BYTE) * fileSize, sizeof(BYTE), fileSize, imgFile);

		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != NULL) {
			printWSAError();
			break;
		}

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) {
			printWSAError();
			break;
		}

		SOCKADDR_IN serverAddr;
		ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
		serverAddr.sin_family = AF_INET;
		InetPtonA(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr);
		serverAddr.sin_port = htons(SERVER_PORT);
		int result = connect(sock, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN));
		if (result == SOCKET_ERROR) {
			printWSAError();
			break;
		}

		stPacketHeader packetHeader;
		packetHeader.code = 0x11223344;
		packetHeader.fileSize = fileSize;
		wcscpy_s(packetHeader.name, 32, L"최주환");
		wcscpy_s(packetHeader.fileName, 128, L"저희집냥이에요.jpg");
		result = send(sock, (const char*)&packetHeader, sizeof(stPacketHeader), 0);
		if (result == SOCKET_ERROR) {
			printWSAError();
			break;
		}

		while (fileSize > 0) {

			int sendLen = 1000;
			if (sendLen > fileSize) {
				sendLen = fileSize;
			} 

			result = send(sock, (const char*)fileBuffer, sendLen, 0);
			if (result == SOCKET_ERROR){
				printWSAError();
				break;
			}
			fileBuffer += sendLen;
			fileSize -= sendLen;

			wprintf(L"%d Byte 보냄, %d Byte 남음\n", sendLen, fileSize);

		}

		char buf[255];
		result = recv(sock, buf, 255, 0);
		if (result == SOCKET_ERROR) {
			printWSAError();
			break;
		}
		wprintf(L"recv %x%x%x%x\n", buf[0], buf[1], buf[2], buf[3]);
		
	} while (false);


	closesocket(sock);
	WSACleanup();

	system("PAUSE>NUL");
	return 0;
}