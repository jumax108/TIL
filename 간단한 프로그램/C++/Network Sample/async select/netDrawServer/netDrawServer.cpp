// netDrawServer.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <new>

#pragma comment(lib, "ws2_32")

#include "queue.h"
#include "ringBuffer.h"
#include "framework.h"
#include "netDrawServer.h"

#define MAX_LOADSTRING 100
#define UM_SOCKET (WM_USER+1)

const WCHAR* SERVER_IP = L"0.0.0.0";
constexpr USHORT SERVER_PORT = 25000;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

SOCKET listenSocket;
struct stMsgHeader {
    USHORT len;
};

struct stMsgDraw {
    int startX;
    int startY;
    int endX;
    int endY;
};
constexpr UINT RECVBUF_SIZE = 3000;
constexpr UINT SENDBUF_SIZE = 3000;
struct stUser {

    UINT id;
    SOCKET sock;
    WCHAR* ip[20];
    USHORT port;

    CRingBuffer recvBuffer;
    CRingBuffer sendBuffer;
};

UINT idBase = 1;

constexpr USHORT USER_NUM = 1000;
stUser users[USER_NUM];
CQueue<USHORT> usersIndex(USER_NUM);

void networkInit(HWND hWnd);
#define printWSAError(str, error) printWSAErrorW(str, error, __FILEW__, __LINE__);
void printWSAErrorW(const WCHAR* str, int* error, const WCHAR* file, int line);
void socketProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void init();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NETDRAWSERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NETDRAWSERVER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


//
//  함수: MyRegisterClass()9
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NETDRAWSERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NETDRAWSERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:

        AllocConsole();
        freopen("CONOUT$", "wb", stdout);
        networkInit(hWnd);
        init();
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case UM_SOCKET:
        socketProc(hWnd, message, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void networkInit(HWND hWnd) {

    WSADATA wsaData;
    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    int wsaStartupError;
    if (wsaStartupResult != 0) {
        printWSAError(L"wsa startup error", &wsaStartupError);
        return;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    int listenSocketError;
    if (listenSocket == INVALID_SOCKET) {
        printWSAError(L"listen socket error", &listenSocketError);
        return;
    }

    SOCKADDR_IN serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    InetPtonW(AF_INET, SERVER_IP, &serverAddr.sin_addr.S_un.S_addr);
    serverAddr.sin_port = htons(SERVER_PORT);

    int bindResult = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    int bindError;
    if (bindResult == SOCKET_ERROR) {
        printWSAError(L"bind Error", &bindError);
        return;
    }

    int listenResult = listen(listenSocket, SOMAXCONN);
    int listenError;
    if (listenResult == SOCKET_ERROR) {
        printWSAError(L"listen error", &listenError);
        return;
    }

    u_long setNonBlockingSocket = 1;
    int ioctlResult = ioctlsocket(listenSocket, FIONBIO, &setNonBlockingSocket);
    int ioctlError;
    if (ioctlResult == SOCKET_ERROR) {
        printWSAError(L"ioctl error", &ioctlError);
        return;
    }

    WSAAsyncSelect(listenSocket, hWnd, UM_SOCKET, FD_ACCEPT);
}

void printWSAErrorW(const WCHAR* str, int* error, const WCHAR* file, int line) {

    *error = WSAGetLastError();
    wprintf(L"%s\n", str);
    wprintf(L"FILE: %s\n, line: %d\n", file, line);
    wprintf(L"error: %d\n", *error);
} 

void recvPacket(stUser* user) {

    char buf[RECVBUF_SIZE];

    UINT bufFreeSize;
    user->recvBuffer.getFreeSize(&bufFreeSize);
    UINT recvSize = min(RECVBUF_SIZE, bufFreeSize);
    UINT recvedSize = recv(user->sock, buf, recvSize, 0);
    user->recvBuffer.push(recvedSize, (const BYTE*)buf);

}
void sendPacket(stUser* user) {

    char buf[SENDBUF_SIZE];

    UINT bufUseSize;
    user->sendBuffer.getUsedSize(&bufUseSize);
    UINT sendSize = min(SENDBUF_SIZE, bufUseSize);

    user->sendBuffer.front(sendSize, (BYTE*)buf);
    user->sendBuffer.pop(sendSize);

    send(user->sock, buf, sendSize, 0);

}

void packetProc(stUser* user){

    UINT bufUseSize;

    for (;;) {
        user->recvBuffer.getUsedSize(&bufUseSize);

        if (bufUseSize < sizeof(stMsgHeader)) {
            break;
        }

        USHORT header;
        user->recvBuffer.front(sizeof(stMsgHeader), (BYTE*)&header);

        if (bufUseSize < sizeof(stMsgHeader) + header) {
            break;
        }

        user->recvBuffer.pop(2);
        stMsgDraw packet;
        user->recvBuffer.front(header, (BYTE*)&packet);
        user->recvBuffer.pop(header);

        for (UINT userCnt = 0; userCnt < USER_NUM; ++userCnt) {

            if (users[userCnt].id == 0) {
                continue;
            }

            users[userCnt].sendBuffer.push(2, (const BYTE*)&header);
            users[userCnt].sendBuffer.push(header, (const BYTE*)&packet);


            sendPacket(&users[userCnt]);

        }

    }

}
void socketProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    int selectError;
    if (WSAGETSELECTERROR(lParam)) {
        printWSAError(L"select error", &selectError);
        return;
    }

    SOCKADDR_IN clientAddr;
    int addrLen;

    switch (WSAGETSELECTEVENT(lParam)) {
    case FD_ACCEPT:
        {
            addrLen = sizeof(clientAddr);
            USHORT userIndex;
            bool haveUserIndex = usersIndex.front((USHORT*)&userIndex);
            if (haveUserIndex == false) {
                break;
            }
            usersIndex.pop();

            stUser* user = &users[userIndex];
            SOCKET* clientSock = &user->sock;
            *clientSock = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
            int acceptError;
            if (*clientSock == INVALID_SOCKET) {
                printWSAError(L"accept error", &acceptError);
                break;
            }

            int asyncSelectResult = WSAAsyncSelect(*clientSock, hWnd, UM_SOCKET, FD_READ | FD_WRITE);
            int asyncSelectError;
            if (asyncSelectResult == SOCKET_ERROR) {
                printWSAError(L"async select error", &asyncSelectError);
                break;
            }

            user->id = idBase++;

        }
        break;
    case FD_READ:
        /* recv */
        printf("FD_READ\n");
        for (USHORT userCnt = 0; userCnt < USER_NUM; ++userCnt) {

            if (wParam == users[userCnt].sock) {

                recvPacket(&users[userCnt]);
                packetProc(&users[userCnt]);

            }

        }
        break; 
    case FD_WRITE:
        /* send */
        printf("FD_WRITE\n");
        for (USHORT userCnt = 0; userCnt < USER_NUM; ++userCnt) {

            if (wParam == users[userCnt].sock) {

                sendPacket(&users[userCnt]);

            }

        }
        break;
    case FD_CLOSE:
        {
            printf("FD_CLOSE\n");
            for (USHORT userCnt = 0; userCnt < USER_NUM; ++userCnt) {

                if (wParam == users[userCnt].sock) {


                    users[userCnt].id = 0;
                    usersIndex.push(userCnt);
                    closesocket(users[userCnt].sock);

                }

            }   
        }
        break;
    }

}

void init() {

    for (USHORT userCnt = 0; userCnt < USER_NUM; userCnt++) {
        usersIndex.push(userCnt);
        new (&users[userCnt].recvBuffer) CRingBuffer(RECVBUF_SIZE);
        new (&users[userCnt].sendBuffer) CRingBuffer(SENDBUF_SIZE);
    }

}