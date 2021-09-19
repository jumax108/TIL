// Astar.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "resource.h"

#include "SimpleProfiler.h"
#include "myLinkedList.h"
#include "Astar.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

CAstar* aStar;

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
    LoadStringW(hInstance, IDC_ASTAR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASTAR));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTAR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(43, 43 ,43));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ASTAR);
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

int mouseX;
int mouseY;

int oldMouseX;
int oldMouseY;

bool mouseDown = false;

CAstar::stNode* endNode = nullptr;

int width = 40;
int height = 20;

int blockSize = 30;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:

        CAstar::stNode* node;
        node = aStar->pathFindSingleLoop();
        if (node == nullptr) {
            KillTimer(hWnd, 0);
        }
        else if ((int)node != 1) {
            endNode = node;
            KillTimer(hWnd, 0);
        }
        InvalidateRect(hWnd, nullptr, true);

        break;
    case WM_CREATE:
        aStar = new CAstar(width, height, blockSize); 
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
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
            aStar->print(hdc); 

            HPEN hPen = CreatePen(PS_SOLID, 5, RGB(45, 20, 200));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            if (endNode != nullptr) {
                while (endNode->_parent != nullptr) {
                    CAstar::stNode* parent = endNode->_parent;
                    MoveToEx(hdc, endNode->_coord->_x * blockSize + (blockSize / 2), endNode->_coord->_y * blockSize + (blockSize / 2), nullptr);
                    LineTo(hdc, parent->_coord->_x * blockSize + (blockSize / 2), parent->_coord->_y * blockSize + (blockSize / 2));
                    endNode = parent;
                }
            }
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_MOUSEMOVE:
        {
            mouseX = GET_X_LPARAM(lParam) / blockSize * blockSize;
            mouseY = GET_Y_LPARAM(lParam) / blockSize * blockSize;

            if (mouseDown == true) {
                int x = mouseX / blockSize;
                int y = mouseY / blockSize;
                int oldX = oldMouseX / blockSize;
                int oldY = oldMouseY / blockSize;

                if (0 > x || 0 > y || aStar->width() <= x || aStar->height() <= y) {
                    break;
                }

                if (x == oldX && y == oldY) {
                    break;
                }

                oldMouseX = mouseX;
                oldMouseY = mouseY;

                if (aStar->map(y, x) == CAstar::MAP_STATE::ROAD) {
                    aStar->map(y, x) = CAstar::MAP_STATE::WALL;
                }
                else {
                    aStar->map(y, x) = CAstar::MAP_STATE::ROAD;
                }

                InvalidateRect(hWnd, nullptr, true);
                
            }

        }
        break;
    case WM_RBUTTONDOWN:
        {
            aStar->listClear();
            endNode = nullptr;
            aStar->pathFindInit();
            SetTimer(hWnd, 0, 100, nullptr);            
            InvalidateRect(hWnd, nullptr, true);
        }
        break;
    case WM_LBUTTONDOWN:
        {
            mouseDown = true;
        }
        break;
    case WM_LBUTTONUP:
        {
            oldMouseX = -1;
            oldMouseY = -1;

            mouseDown = false;
        }
        break;
    case WM_CHAR: 
        {
            switch (wParam) {
            case 's':
                aStar->startPoint(mouseY / blockSize, mouseX / blockSize);
                InvalidateRect(hWnd, nullptr, true);
                break;
            case 'e':
                aStar->endPoint(mouseY / blockSize, mouseX / blockSize);
                InvalidateRect(hWnd, nullptr, true);
                break;
            case 'c':
                aStar->listClear();
                InvalidateRect(hWnd, nullptr, true);
                endNode = nullptr;
                KillTimer(hWnd, 0);
                break;
            case 'x':
                delete(aStar);
                aStar = new CAstar(width, height, blockSize);
                KillTimer(hWnd, 0);
                endNode = nullptr;
                InvalidateRect(hWnd, nullptr, true);
                break;
            case 'f':
                endNode = aStar->pathFind();
                InvalidateRect(hWnd, nullptr, true);
                break;
            case 't':
                srand(1000);
                for (int loopCnt = 0; loopCnt < 1; ++loopCnt) {
                    printf("loopCnt: %d\n", loopCnt);
                    WCHAR fileName[50];
                    swprintf_s(fileName, 50, L".\\testImage\\result%04d.bmp", loopCnt);

                    CAstar::test(blockSize, fileName);
                }
                break;
            case 'p':
                //aStar->printMapToBitmap(L"result.bmp");
                break;
            }

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
