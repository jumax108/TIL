#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

const WCHAR* SERVER_IP = L"172.30.1.16";
const USHORT SERVER_PORT = 1008;

int main() {
	
	const char* buf = "123456789qwertyuiopasdf";
	
	WSADATA wsaStartup;
	if (WSAStartup(MAKEWORD(2, 2), &wsaStartup) != NULL) {
		return 0;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		return 0;
	}

	ULONG setNonBlockSocket = 1;
	ioctlsocket(sock, FIONBIO, &setNonBlockSocket);

	int option = true;
	//setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	InetPtonW(AF_INET, SERVER_IP, &sockAddr.sin_addr.S_un.S_addr);
	sockAddr.sin_port = htons(SERVER_PORT);
	int res = connect(sock, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	if (res == SOCKET_ERROR) {
		int error = WSAGetLastError();
		if(error != WSAEWOULDBLOCK)
			return 0;
	}

	int num = 1;
	while (1) {
		FD_SET set;
		FD_ZERO(&set);
		FD_SET(sock, &set);

		timeval delayZero;
		delayZero.tv_sec = 0;
		delayZero.tv_usec = 0;
		int res = select(0, nullptr, &set, nullptr, &delayZero);

		if (res > 0) {

			while (1) {
				int res = send(sock, buf, 23, 0);
				printf("%d %d\n", num++, res);
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK) {

					printf("%s\n", buf);

					system("PAUSE");
					while (1) {
						res = send(sock, buf, 1, 0);
						printf("%d %d\n", num++, res);
						int error = WSAGetLastError();
						system("PAUSE");
						if (error == WSAEWOULDBLOCK) {

						}
					}
				}


				
			}
			

		}


	}
	

	return 0;
}