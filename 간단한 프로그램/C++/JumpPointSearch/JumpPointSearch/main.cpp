// JumpPointSearch.cpp : ���ø����̼ǿ� ���� �������� �����մϴ�.
//

#include "framework.h"
#include "Resource.h"
#include "myLinkedList.h"
#include "JumpPointSearch.h"

#define MAX_LOADSTRING 100

CJumpPointSearch* jps;

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ���Ե� �Լ��� ������ �����մϴ�:
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

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_JUMPPOINTSEARCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���ø����̼� �ʱ�ȭ�� �����մϴ�:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JUMPPOINTSEARCH));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  �뵵: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JUMPPOINTSEARCH));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(43, 43, 43));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_JUMPPOINTSEARCH);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   �뵵: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   �ּ�:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �뵵: �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���ø����̼� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//

int mapWidth;
int mapHeight;

int mouseX;
int mouseY;
int blockSize;
int timerDelay;

bool mouseDown;
CJumpPointSearch::MAP_STATE changeThisState;

CJumpPointSearch::stNode* endNode;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:

        mapWidth = 50;
        mapHeight = 25;
        blockSize = 25;
        timerDelay = 200;
        mouseDown = false;
        endNode = nullptr;

        jps = new CJumpPointSearch(mapWidth, mapHeight);
        new (&jps->_end) CJumpPointSearch::stCoord(-1, -1);
        new (&jps->_start) CJumpPointSearch::stCoord(-1, -1);
        break;
    case WM_TIMER:
    {
        void* result = jps->pathFindSingleLoop();
        if ((int)result != 1) {
            KillTimer(hWnd, 1);
            if (result == nullptr) {
                // ��ã��
            }
            else {
                // ã��
                endNode = (CJumpPointSearch::stNode*)result;
            }
        }
        InvalidateRect(hWnd, nullptr, true);
    }
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // �޴� ������ ���� �м��մϴ�:
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
    case WM_MOUSEMOVE:
        {
            mouseX = GET_X_LPARAM(lParam);
            mouseY = GET_Y_LPARAM(lParam);
            do {
                if (mouseDown == false) {
                    break;
                }
                int blockY = mouseY / blockSize;
                int blockX = mouseX / blockSize;
                *(jps->map(blockY, blockX)) = changeThisState;
                RECT rt;
                rt.left = blockX * blockSize;
                rt.right = rt.left + blockSize;
                rt.top = blockY * blockSize;
                rt.bottom = rt.top + blockSize;
                InvalidateRect(hWnd, &rt, true);
            } while (false);
        }
        break;

    case WM_MBUTTONDOWN:
        break;
    case WM_RBUTTONDOWN:
        endNode = nullptr;
        jps->pathFindInit();
        SetTimer(hWnd, 1, timerDelay, nullptr);
        break;
    case WM_LBUTTONDOWN: {
        mouseDown = true;
        int blockY = mouseY / blockSize;
        int blockX = mouseX / blockSize;
        switch (*(jps->map(blockY, blockX))) {
            case CJumpPointSearch::MAP_STATE::ROAD:
                changeThisState = CJumpPointSearch::MAP_STATE::WALL;
                break;
            case CJumpPointSearch::MAP_STATE::WALL:
                changeThisState = CJumpPointSearch::MAP_STATE::ROAD;
                break;
        }
        break;
    }
    case WM_LBUTTONUP:
        mouseDown = false;
        break;
    case WM_CHAR:
    {
        switch (wParam) {
            case 's':
            case 'e':
            {
                int blockY = mouseY / blockSize;
                int blockX = mouseX / blockSize;
                if (blockY < 0 || blockX < 0 || blockY >= jps->height() || blockX >= jps->width()) {
                    break;
                }

                CJumpPointSearch::stCoord* target;
                if (wParam == 's') {
                    target = &jps->_start;
                }
                else {
                    target = &jps->_end;
                }

                new (target) CJumpPointSearch::stCoord(blockY, blockX);
                InvalidateRect(hWnd, nullptr, true);
                break;
            }
            case 'c':
            {

                endNode = nullptr;
                KillTimer(hWnd, 1);
                jps->pathFindInit();
                InvalidateRect(hWnd, nullptr, true);
                break;
            }
            case 'z':
            {
                endNode = nullptr;
                KillTimer(hWnd, 1);
                endNode = nullptr;
                delete(jps);
                jps = new CJumpPointSearch(mapWidth, mapHeight);
                InvalidateRect(hWnd, nullptr, true);
                break;
            }
            case 't':
            {
                CJumpPointSearch::test();
                MessageBoxW(hWnd, L"�׽�Ʈ �� !", L"JumpPointSearch", MB_OK);
            }
                break;
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�...
        jps->print(hdc, blockSize, endNode);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_MOUSEWHEEL: {
        short wheelMove = HIWORD(wParam);
        if (wheelMove > 0) {
            blockSize += 1;
        }
        else {
            if (blockSize - 1 > 0) {
                blockSize -= 1;
            }
        }
        InvalidateRect(hWnd, nullptr, true);
    }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
