// SpriteSample.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "resource.h"
#include "ScreenDib.h"
#include "Sprite.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HDC hMemDC;
HBITMAP hBufBmp;
RECT rtClient;

BYTE* backgroundBuf;

LARGE_INTEGER startTick;
LARGE_INTEGER endTick;

stSprite user;

LARGE_INTEGER mouseMoveTime;
bool mouseMoveNow;
LARGE_INTEGER freq;

void update() {

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    if (mouseMoveNow == true && now.QuadPart / freq.QuadPart - mouseMoveTime.QuadPart / freq.QuadPart > 300) {
        mouseMoveNow = false;
        user.state = stSprite::STATE::idle;
        user.idle->init();
    }

    if (mouseMoveNow == true) {
        if (user.state == stSprite::STATE::idle) {
            user.move->init();
        }
        user.state = stSprite::STATE::move;
    }

    stAnimation* ani = user.getCurrentAnimation();
    stImage* img = ani->getCurrentImage();
    int* frame = &(img->_currentSpendFrame);
    *frame += 1;
    if (*frame == img->_delayFrame) {
        *frame = 0;
        if (ani->_currentImage == ani->_imageNum - 1 && user.state == stSprite::STATE::attack) {
            user.state = stSprite::STATE::idle;
            user.idle->init();
            return;
        }
        ani->nextImage();
    }
    
}

void render(HWND hWnd) {
    
    BYTE* bmpBuf;
    int bufWidth;
    int bufHeight;
    int bufPitch;
    int bufSize;

    {
        CScreenDib* screen = CScreenDib::getInstance();
        bmpBuf = screen->getDibBuffer();
        bufWidth = screen->getWidth();
        bufHeight = screen->getHeight();
        bufPitch = screen->getPitch();
        bufSize = screen->getBufSize();
    }


    // ----------------------------------------------------------------------
    // 배경 출력
    memcpy(bmpBuf, backgroundBuf, bufSize);
    // ----------------------------------------------------------------------
   
    // ----------------------------------------------------------------------
    // 플레이어 캐릭터 출력
    {
        stImage* img = user.getCurrentAnimation()->getCurrentImage();
        int width = img->_width;
        int height = img->_height;

        DWORD* dwpBmpBuf = (DWORD*)bmpBuf + user.x + (bufPitch / 4 * user.y);
        DWORD* dwpUserBuf = (DWORD*)img->_idleBuf;

        for (int cntHeight = 0; cntHeight < height; cntHeight++) {
            for (int cntWidth = 0; cntWidth < width; cntWidth++, dwpUserBuf++) {
                if ((*dwpUserBuf & 0x00ffffff) == 0x00ffffff) {
                    continue;
                }
                memcpy(dwpBmpBuf + cntWidth, dwpUserBuf, sizeof(DWORD));
            }
            dwpBmpBuf += bufPitch / 4;
        }
    }
    // ----------------------------------------------------------------------


    CScreenDib::getInstance()->flip(hWnd);
}

void input() {

}

BYTE* loadImage(const WCHAR* fileDir, int* width = nullptr, int* height = nullptr) {

    BYTE* buf;

    FILE* bmpFile;
    _wfopen_s(&bmpFile, fileDir, L"rb");

    BITMAPFILEHEADER bmpHead;
    BITMAPINFOHEADER bmpInfo;
    memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
    bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
    fread(&bmpHead, sizeof(BITMAPFILEHEADER), 1, bmpFile);
    fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, bmpFile);
    buf = (BYTE*)malloc(sizeof(BYTE) * bmpInfo.biHeight * bmpInfo.biWidth * 4);
    DWORD pitch = (bmpInfo.biWidth * bmpInfo.biBitCount / 8 + 3) & ~3;
    fread(buf, sizeof(BYTE), bmpInfo.biHeight * bmpInfo.biWidth * 4, bmpFile);

    if (width != nullptr) {
        *width = bmpInfo.biWidth;
        *height = bmpInfo.biHeight;
    }

    return buf;
}

void init(HWND hWnd) {
    
    QueryPerformanceFrequency(&freq);
    freq.QuadPart = freq.QuadPart / 1000;
    CScreenDib::setInstance(640, 480, 32);

    // ----------------------------------------------------------------------
    // 배경 로드
    backgroundBuf = loadImage(L".\\Sprite_Data\\_Map.bmp");
    // ----------------------------------------------------------------------


    // ----------------------------------------------------------------------
    // user idle 애니메이션 로드
    {
        constexpr int imgNum = 3;
        user.idle = new stAnimation(imgNum);
        stImage* imgs = user.idle->_images;
        int delay[imgNum] = { 5,5,5 };
        for (int imgCnt = 0; imgCnt < imgNum; imgCnt++) {
            new(&(imgs[imgCnt])) stImage(delay[imgCnt]);
            WCHAR* fileDir = new WCHAR[255];
            wsprintfW(fileDir, L".\\Sprite_Data\\Stand_L_%02d.bmp", imgCnt + 1);
            imgs[imgCnt]._idleBuf = loadImage(fileDir, &(imgs[imgCnt]._width), &(imgs[imgCnt]._height));
            delete fileDir;
        }
    }
    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    // user move 애니메이션 로드
    {
        constexpr int imgNum = 12;
        user.move = new stAnimation(imgNum);
        stImage* imgs = user.move->_images;
        int delay[imgNum] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
        for (int imgCnt = 0; imgCnt < imgNum; imgCnt++) {
            new(&(imgs[imgCnt])) stImage(delay[imgCnt]);
            WCHAR* fileDir = new WCHAR[255];
            wsprintfW(fileDir, L".\\Sprite_Data\\Move_L_%02d.bmp", imgCnt + 1);
            imgs[imgCnt]._idleBuf = loadImage(fileDir, &(imgs[imgCnt]._width), &(imgs[imgCnt]._height));
            delete fileDir;
        }
    }
    // ----------------------------------------------------------------------

     // ----------------------------------------------------------------------
    // user attack 애니메이션 로드
    {
        constexpr int imgNum = 4;
        user.attack = new stAnimation(imgNum);
        stImage* imgs = user.attack->_images;
        int delay[imgNum] = { 3, 3, 3, 3 };
        for (int imgCnt = 0; imgCnt < imgNum; imgCnt++) {
            new(&(imgs[imgCnt])) stImage(delay[imgCnt]);
            WCHAR* fileDir = new WCHAR[255];
            wsprintfW(fileDir, L".\\Sprite_Data\\Attack1_L_%02d.bmp", imgCnt + 1);
            imgs[imgCnt]._idleBuf = loadImage(fileDir, &(imgs[imgCnt]._width), &(imgs[imgCnt]._height));
            delete fileDir;
        }
    }
    // ----------------------------------------------------------------------

    user.x = 100;
    user.y = 100;

    return;
}

int sleep = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEMOVE:
        user.x = GET_X_LPARAM(lParam) - user.idle->_images[0]._width / 2;
        user.y = 480 - (GET_Y_LPARAM(lParam) + user.idle->_images[1]._height / 2);
        mouseMoveNow = true;
        QueryPerformanceCounter(&mouseMoveTime);
        break;
    case WM_LBUTTONDOWN:
        {
            if (user.state == stSprite::STATE::idle) {
                user.state = stSprite::STATE::attack;
                user.attack->init();
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
    case WM_DESTROY:
        ReleaseDC(hWnd, hMemDC);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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
    LoadStringW(hInstance, IDC_SPRITESAMPLE, szWindowClass, MAX_LOADSTRING);

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPRITESAMPLE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU,
        0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    init(hWnd);

    MSG msg;

    // 기본 메시지 루프입니다:

    do {
        QueryPerformanceCounter(&startTick);

        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else {
            input();
            update();
            render(hWnd);
            QueryPerformanceCounter(&endTick);
            Sleep(20 - (endTick.QuadPart / freq.QuadPart - startTick.QuadPart / freq.QuadPart));
        }

    } while (msg.message != WM_QUIT);

    return (int) msg.wParam;
}

