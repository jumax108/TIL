#pragma once

#define errorOutputFunc printWsaError
#define printWsaError(errorMsg, errorCode) printWsaErrorW(errorMsg, errorCode, __FILEW__, __LINE__);
#define msgBoxWsaError(errorMsg, errorCode) msgBoxWsaErrorW(errorMsg, errorCode, __FILEW__, __LINE__);
#define UM_SOCKET (WM_USER+1)

extern SOCKET sock;

static const WCHAR* SERVER_IP = L"172.30.1.16";
static const USHORT SERVER_PORT = 5000;

constexpr UINT NETWORK_BUFFER_SIZE = 5000;
static CRingBuffer recvBuffer;
static CRingBuffer sendBuffer;

static bool ableSendPacket;

void printWsaErrorW(const WCHAR* errorMsg, int* errorCode, const WCHAR* file, int line);
void msgBoxWsaErrorW(const WCHAR* errorMsg, int* errorCode, const WCHAR* file, int line);
bool networkInit(HWND);
bool socketMessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool recvPacket();
void packetProc();
bool sendPacket();

static void (*packetFunc[31])(char*);