#include "framework.h"
#include "ringBuffer.h"
#include "network.h"
#include "networkMessage.h"

void printWsaErrorW(const WCHAR* errorMsg, int* errorCode, const WCHAR* file, int line) {

	*errorCode = WSAGetLastError();

	wprintf(L"%s\n",errorMsg);
	wprintf(L"file: %s\n", file);
	wprintf(L"line: %d\n", line);
	wprintf(L"error: %d\n", *errorCode);

}

void msgBoxWsaErrorW(const WCHAR* errorMsg, int* errorCode, const WCHAR* file, int line) {

	WCHAR errorText[500];
	size_t index = 0;

	*errorCode = WSAGetLastError();

	index += swprintf_s(errorText		 , 500        , L"msg : %s\n", errorMsg);
	index += swprintf_s(errorText + index, 500 - index, L"file : %s\n", file);
	index += swprintf_s(errorText + index, 500 - index, L"line : %d\n", line);
	index += swprintf_s(errorText + index, 500 - index, L"error : %d\n", *errorCode);

	MessageBox(NULL, errorText, L"error", MB_OK);
}


bool networkInit(HWND hWnd) {

	WSAData wsaData;
	int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	int wsaStartupError;
	if (wsaStartupResult != NULL) {
		errorOutputFunc(L"startup error", &wsaStartupError);
		return false;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	int socketError;
	if (sock == INVALID_SOCKET) {
		errorOutputFunc(L"socket error", &socketError);
		return false;
	}

	ULONG setNonBlockingSocket = 1;
	int ioctlResult = ioctlsocket(sock, FIONBIO, &setNonBlockingSocket);
	int ioctlError;
	if (ioctlResult == SOCKET_ERROR) {
		errorOutputFunc(L"ioctl error", &ioctlError);
		return false;
	}

	WSAAsyncSelect(sock, hWnd, UM_SOCKET, FD_READ | FD_WRITE);

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	InetPtonW(AF_INET, SERVER_IP, &sockAddr.sin_addr.S_un.S_addr);
	sockAddr.sin_port = htons(SERVER_PORT);
	int connectResult = connect(sock, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	int connectError;
	if (connectResult == SOCKET_ERROR) {
		errorOutputFunc(L"connect error", &connectError);
		if (connectError != WSAEWOULDBLOCK) {
			return false;
		}
	}

	new (&recvBuffer) CRingBuffer(NETWORK_BUFFER_SIZE);
	new (&sendBuffer) CRingBuffer(NETWORK_BUFFER_SIZE);

	packetFunc[SC_CREATE_MY_CHARACTER] = createMyCharacter;
	packetFunc[SC_CREATE_OTHER_CHARACTER] = createOtherCharacter;
	packetFunc[SC_DELETE_CHARACTER] = deleteCharacter;
	packetFunc[SC_MOVE_START] = moveStart;
	packetFunc[SC_MOVE_STOP] = moveStop;
	packetFunc[SC_ATTACK1] = attack1;
	packetFunc[SC_ATTACK2] = attack2;
	packetFunc[SC_ATTACK3] = attack3;
	packetFunc[SC_DAMAGE] = damage;

	long setNagleOff = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&setNagleOff, sizeof(setNagleOff));

	return true;

}

bool socketMessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	USHORT selectResult = WSAGETSELECTERROR(lParam);
	int selectError;
	if (selectResult != 0) {
		errorOutputFunc(L"select Error", &selectError);
		return false;
	}

	switch (WSAGETSELECTEVENT(lParam)) {
	case FD_READ:
		recvPacket();
		packetProc();
		break;
	case FD_WRITE:
		ableSendPacket = true;
		sendPacket();
		break;
	}
	
	return true;
}

bool recvPacket() {

	constexpr short bufSize = 1460;
	char buf[bufSize];

	UINT recvBufFreeSize;
	recvBuffer.getFreeSize(&recvBufFreeSize);

	short readSize = min(bufSize, recvBufFreeSize);

	int recvResult = recv(sock, buf, readSize, 0);
	int recvError;
	if (recvResult == SOCKET_ERROR) {
		errorOutputFunc(L"recv error", &recvError);
		return false;
	}

	recvBuffer.push(recvResult , (const BYTE*)buf);

	return true;

}

bool sendPacket() {

	while (1) {
		constexpr short bufSize = 1460;
		char buf[bufSize];

		UINT sendBufUsedSize;
		sendBuffer.getUsedSize(&sendBufUsedSize);

		short sendSize = min(bufSize, sendBufUsedSize);
		if (sendSize == 0) {
			break;
		}

		sendBuffer.front(sendSize, (BYTE*)buf);

		int sendResult = send(sock, buf, sendSize, 0);
		int sendError;
		if (sendResult == SOCKET_ERROR) {
			errorOutputFunc(L"send error", &sendError);
			if (sendError != WSAEWOULDBLOCK) {
				return false;
			}
			else {
				ableSendPacket = false;
				break;
			}
		}

		sendBuffer.pop(sendSize);
	}

	return true;

}

void packetProc() {

	UINT recvBufUsedSize;
	recvBuffer.getUsedSize(&recvBufUsedSize);
	while (recvBufUsedSize > 3) {

		stPacketHeader header;
		recvBuffer.front(sizeof(header), (BYTE*)&header);

		if (header.code != 0x89) {
			// disconnect
			return;
		}

		if (packetFunc[header.type] == nullptr) {
			//disconnect
			return;
		}

		if (recvBufUsedSize < 3 + header.size) {
			return;
		}

		recvBuffer.pop(3);

		char buf[50];
		recvBuffer.front(header.size, (BYTE*)buf);

		printf("%x %d %d\n", header.code, header.size, header.type);

		packetFunc[header.type](buf);

		recvBuffer.pop(header.size);

		recvBuffer.getUsedSize(&recvBufUsedSize);

	}

}