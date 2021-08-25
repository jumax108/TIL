#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

const WCHAR* SERVER_IP = L"0.0.0.0";
const USHORT SERVER_PORT = 1008;

int main() {

	WSADATA wsaStartup;
	if (WSAStartup(MAKEWORD(2, 2), &wsaStartup) != NULL) {
		return 0;
	}

	SOCKET listensocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listensocket == INVALID_SOCKET) {
		return 0;
	}


	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	InetPtonW(AF_INET, SERVER_IP, &sockAddr.sin_addr.S_un.S_addr);
	sockAddr.sin_port = htons(SERVER_PORT);
	int res = bind(listensocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	if (res == SOCKET_ERROR) {
		printf("bind error");
		return 0;
	}
	
	listen(listensocket, SOMAXCONN);
	int out = sizeof(sockAddr);
	SOCKET sock = accept(listensocket, (SOCKADDR*)&sockAddr, &out);

	system("PAUSE");

	ULONG setNonBlockSocket = 1;
	ioctlsocket(sock, FIONBIO, &setNonBlockSocket);

	int num = 1;
	
	while (1) {
		char* buf = new char[100];
		ZeroMemory(buf, 100);
		int a = recv(sock, buf, 23, 0);
		printf("%d: %s\n", num++, buf);
		if (strcmp(buf, "123456789qwertyuiopasdf") != 0) {
			system("PAUSE");
		}
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK)
			system("PAUSE");
	}
		
	return 0;

}