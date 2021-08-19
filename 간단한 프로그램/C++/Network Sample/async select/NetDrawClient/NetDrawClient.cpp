// NetDrawClient.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include <stdio.h>
#include <windowsx.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "ringBuffer.h"

#pragma comment(lib, "ws2_32")

#include "framework.h"
#include "NetDrawClient.h"

#define MAX_LOADSTRING 100

#define UM_SOCKET (WM_USER+1)

#define printWSAError(str, error) printWSAErrorW(str, error, __FILEW__, __LINE__);
void printWSAErrorW(const WCHAR* str, int* error, const WCHAR* file, int line);
bool networkInit(HWND hWnd);
void socketProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void sendPacket();

const WCHAR* SERVER_IP = L"127.0.0.1";
constexpr USHORT SERVER_PORT = 25000;
constexpr USHORT SENDBUF_SIZE = 3000;
constexpr USHORT RECVBUF_SIZE = 3000;

struct stMsgHeader {
    USHORT len;
};

struct stMsgDraw {
    int startX;
    int startY;
    int endX;
    int endY;
};

struct stUser {
    USHORT id;
    WCHAR ip[16];
    USHORT port;
};

stUser user[1000];
SOCKET sock;
CRingBuffer sendBuf(SENDBUF_SIZE);
CRingBuffer recvBuf(RECVBUF_SIZE);

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_NETDRAWCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NETDRAWCLIENT));

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
//  함수: MyRegisterClass()
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NETDRAWCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NETDRAWCLIENT);
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

bool mouseDown = false;
UINT lastX;
UINT lastY;

HDC hdc;

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
        hdc = GetDC(hWnd);
        networkInit(hWnd);

        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_LBUTTONDOWN: 
        mouseDown = true;
        lastX = GET_X_LPARAM(lParam);
        lastY = GET_Y_LPARAM(lParam);
        break;
    case WM_MOUSEMOVE: 
        {
            if (mouseDown == false) {
                break;
            }
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            stMsgDraw packet;
            packet.startX = lastX;
            packet.startY = lastY;
            packet.endX = x;
            packet.endY = y;

            stMsgHeader header;
            header.len = sizeof(stMsgDraw);
            sendBuf.push(sizeof(header), (BYTE*)&header);
            sendBuf.push(sizeof(packet), (BYTE*)&packet);
            sendPacket();

            lastX = x;
            lastY = y;
        }
        break;
    case WM_LBUTTONUP:
        mouseDown = false;
        break;
    case UM_SOCKET:
        socketProc(hWnd, message, wParam, lParam);
        break;
    case WM_DESTROY:
        ReleaseDC(hWnd, hdc);
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

bool networkInit(HWND hWnd) {

    WSADATA wsaData;
    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    int wsaStartupError;
    if (wsaStartupResult != 0) {
        printWSAError(L"WSA startup error", &wsaStartupError);
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    int socketError;
    if (sock == INVALID_SOCKET) {
        printWSAError(L"socket error", &socketError);
        return false;
    }

    u_long setNonBlockingSocket = 1;
    int ioctlResult = ioctlsocket(sock, FIONBIO, &setNonBlockingSocket);
    int ioctlError;
    if (ioctlResult == SOCKET_ERROR) {
        printWSAError(L"ioctl Error", &ioctlError);
        return false;
    }

    WSAAsyncSelect(sock, hWnd, UM_SOCKET, FD_READ | FD_WRITE);

    SOCKADDR_IN serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    InetPtonW(AF_INET, SERVER_IP, &serverAddr.sin_addr.S_un.S_addr);
    serverAddr.sin_port = htons(SERVER_PORT);
    int connectResult = connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    int connectError;
    if (connectResult == SOCKET_ERROR) {
        printWSAError(L"connect error", &connectError);
        if (connectError != 10035) {
            return false;
        }
    }

    return true;
}

void printWSAErrorW(const WCHAR* str, int* error, const WCHAR* file, int line) {

    *error = WSAGetLastError();
    wprintf(L"%s\n", str);
    wprintf(L"file: %s, line: %d\n", file, line);
    wprintf(L"error: %d\n", *error);

}

void recvPacket() {

    char buf[RECVBUF_SIZE];

    UINT bufFreeSize;
    recvBuf.getFreeSize(&bufFreeSize);
    UINT recvSize = min(RECVBUF_SIZE, bufFreeSize);

    UINT recvedSize = recv(sock, buf, recvSize, 0);

    recvBuf.push(recvedSize, (const BYTE*)buf);

}

void sendPacket() {

    char buf[SENDBUF_SIZE];

    UINT bufUseSize;
    sendBuf.getUsedSize(&bufUseSize);
    UINT sendSize = min(SENDBUF_SIZE, bufUseSize);

    sendBuf.front(sendSize, (BYTE*)buf);
    sendBuf.pop(sendSize);

    send(sock, buf, sendSize, 0);

}

void packetProc() {

    UINT bufUseSize;

    for (;;) {
        recvBuf.getUsedSize(&bufUseSize);

        if (bufUseSize < sizeof(stMsgHeader)) {
            break;
        }

        USHORT header;
        recvBuf.front(sizeof(stMsgHeader), (BYTE*)&header);

        if (bufUseSize < sizeof(stMsgHeader) + header) {
            break;
        }

        recvBuf.pop(2);
        stMsgDraw packet;
        recvBuf.front(header, (BYTE*)&packet);
        recvBuf.pop(header);
        MoveToEx(hdc, packet.startX, packet.startY, NULL);
        LineTo(hdc, packet.endX, packet.endY);
        
    }

}

void socketProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    int selectError;
    if (WSAGETSELECTERROR(lParam)) {
        printWSAError(L"select error", &selectError);
        return;
    }

    switch (WSAGETSELECTEVENT(lParam)) {
    case FD_READ:
        printf("FD_READ\n");
        recvPacket();
        packetProc();
        break;
    case FD_WRITE:
        printf("FD_WRITE\n");
        sendPacket();
        break;
    }

}