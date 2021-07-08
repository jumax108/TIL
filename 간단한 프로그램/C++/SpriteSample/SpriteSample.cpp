// SpriteSample.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "resource.h"
#include "ScreenDib.h"
#include "Image.h"
#include "Animation.h"
#include "SpriteData.h"
#include "Sprite.h"

#define MIN(a,b) a>b?b:a
#define MAX(a,b) a>b?a:b

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

const int CLIENT_WIDTH = 640;
const int CLIENT_HEIGHT = 480;

HDC hMemDC;
HBITMAP hBufBmp;
RECT rtClient;

BYTE* backgroundBuf;
CSpriteData* spriteData;
CSprite* user;

LARGE_INTEGER mouseMoveTime;
bool mouseMoveNow;
LARGE_INTEGER freq;

int logicCnt = 0;
int renderCnt = 0;

WCHAR logicFrame[100] = { 0, };
WCHAR renderFrame[100] = { 0, };


enum class INPUT_MESSAGE {
    NONE = -1,
    MOVE_LL,
    MOVE_LU,
    MOVE_UU,
    MOVE_RU,
    MOVE_RR,
    MOVE_RD,
    MOVE_DD,
    MOVE_LD
};

INPUT_MESSAGE msg;

void userUpdate(void* argv) {

    switch (*(INPUT_MESSAGE*)argv) {
    case INPUT_MESSAGE::MOVE_DD:
        user->_y += 2;
        user->setAnimation(2);
        break;
    case INPUT_MESSAGE::MOVE_LD:
        user->_y += 2;
        user->_x -= 3;
        user->_seeRight = false;
        user->setAnimation(2);
        break;
    case INPUT_MESSAGE::MOVE_LL:
        user->_x -= 3;
        user->_seeRight = false;
        user->setAnimation(2);
        break;
    case INPUT_MESSAGE::MOVE_LU:
        user->_x -= 3;
        user->_y -= 2;
        user->_seeRight = false;
        user->setAnimation(2);
        break;
    case INPUT_MESSAGE::MOVE_RD:
        user->_x += 3;
        user->_y += 2;
        user->_seeRight = true;
        user->setAnimation(2);
        break;
    case INPUT_MESSAGE::MOVE_RR:
        user->_x += 3;
        user->_seeRight = true;
        user->setAnimation(2);
        break;
    case INPUT_MESSAGE::MOVE_RU:
        user->_x += 3;
        user->_y -= 2;
        user->_seeRight = true;
        user->setAnimation(2);
        break;
    case INPUT_MESSAGE::MOVE_UU:
        user->_y -= 2;
        user->setAnimation(2);
        break;
    case INPUT_MESSAGE::NONE:
        user->setAnimation(0);
        break;
    }
}

void update() {

    ++logicCnt;

    user->update(&msg);

}

void render(HWND hWnd) {

    renderCnt += 1;
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
   
    user->draw();

    CScreenDib::getInstance()->flip(hWnd);

    //Sleep(60);

}

void input() {


    short upState = GetAsyncKeyState(VK_UP);
    short leftState = GetAsyncKeyState(VK_LEFT);
    short rightState = GetAsyncKeyState(VK_RIGHT);
    short downState = GetAsyncKeyState(VK_DOWN);

    if (upState) {
        if (leftState) {
            msg = INPUT_MESSAGE::MOVE_LU;
        }
        else if (rightState) {
            msg = INPUT_MESSAGE::MOVE_RU;
        }
        else {
            msg = INPUT_MESSAGE::MOVE_UU;
        }
    }
    else if (leftState) {
        if (downState) {
            msg = INPUT_MESSAGE::MOVE_LD;
        }
        else {
            msg = INPUT_MESSAGE::MOVE_LL;
        }
    }
    else if (rightState) {
        if (downState) {
            msg = INPUT_MESSAGE::MOVE_RD;
        }
        else {
            msg = INPUT_MESSAGE::MOVE_RR;
        }
    }
    else if(downState) {
        msg = INPUT_MESSAGE::MOVE_DD;
    }
    else {
        msg = INPUT_MESSAGE::NONE;
    }

}

BYTE* loadImage(const WCHAR* fileDir, int* width = nullptr, int* height = nullptr) {

    BYTE* buf;
    BYTE* reverseBuf;

    FILE* bmpFile;
    _wfopen_s(&bmpFile, fileDir, L"rb");

    BITMAPFILEHEADER bmpHead;
    BITMAPINFOHEADER bmpInfo;
    memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
    bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
    fread(&bmpHead, sizeof(BITMAPFILEHEADER), 1, bmpFile);
    fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, bmpFile);
    //bmpInfo.biHeight = -bmpInfo.biHeight;
    DWORD bufSize = sizeof(BYTE) * bmpInfo.biHeight * bmpInfo.biWidth * 4;
    buf = (BYTE*)malloc(bufSize);
    reverseBuf = (BYTE*)malloc(bufSize);
    DWORD pitch = (bmpInfo.biWidth * bmpInfo.biBitCount / 8 + 3) & ~3;
    fread(buf, sizeof(BYTE), bmpInfo.biHeight * bmpInfo.biWidth * 4, bmpFile);

    BYTE* pBuf = buf;
    BYTE* pReverseBuf = reverseBuf + bufSize - pitch;

    for (DWORD heightCnt = 0; heightCnt < bmpInfo.biHeight; heightCnt++) {
        memcpy(pReverseBuf, pBuf,
            sizeof(BYTE) * bmpInfo.biWidth * 4);
        pReverseBuf -= pitch;
        pBuf += pitch;
    }

    if (width != nullptr) {
        *width = bmpInfo.biWidth;
        *height = bmpInfo.biHeight;
    }

    fclose(bmpFile);

    free(buf);
    return reverseBuf;
}

void init(HWND hWnd) {
    
    QueryPerformanceFrequency(&freq);
    freq.QuadPart = freq.QuadPart / 1000000;
    CScreenDib::setInstance(640, 480, 32);

    // ----------------------------------------------------------------------
    // 배경 로드
    backgroundBuf = loadImage(L"Sprite_Data\\_Map.bmp");
    // ----------------------------------------------------------------------

    DWORD aniNum = 4;
    DWORD* imgNums = (DWORD*)malloc(sizeof(DWORD) * aniNum);
    imgNums[0] = 3;
    imgNums[1] = 3;
    imgNums[2] = 12;
    imgNums[3] = 12;
    spriteData = new CSpriteData(aniNum, imgNums);
    // ----------------------------------------------------------------------
    // user idle 애니메이션 로드
    (*spriteData)[0]->loadImage(0, L"Sprite_Data\\Stand_L_01.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[0]->loadImage(1, L"Sprite_Data\\Stand_L_02.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[0]->loadImage(2, L"Sprite_Data\\Stand_L_03.bmp", 5, 71, 90, 0xffffffff);

    (*spriteData)[1]->loadImage(0, L"Sprite_Data\\Stand_R_01.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[1]->loadImage(1, L"Sprite_Data\\Stand_R_02.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[1]->loadImage(2, L"Sprite_Data\\Stand_R_03.bmp", 5, 71, 90, 0xffffffff);
    // ----------------------------------------------------------------------
    // 
    // ----------------------------------------------------------------------
    // user move 애니메이션 로드
    (*spriteData)[2]->loadImage(0, L"Sprite_Data\\Move_L_01.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(1, L"Sprite_Data\\Move_L_02.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(2, L"Sprite_Data\\Move_L_03.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(3, L"Sprite_Data\\Move_L_04.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(4, L"Sprite_Data\\Move_L_05.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(5, L"Sprite_Data\\Move_L_06.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(6, L"Sprite_Data\\Move_L_07.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(7, L"Sprite_Data\\Move_L_08.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(8, L"Sprite_Data\\Move_L_09.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(9, L"Sprite_Data\\Move_L_10.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(10, L"Sprite_Data\\Move_L_11.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[2]->loadImage(11, L"Sprite_Data\\Move_L_12.bmp", 5, 71, 90, 0xffffffff);

    (*spriteData)[3]->loadImage(0, L"Sprite_Data\\Move_R_01.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(1, L"Sprite_Data\\Move_R_02.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(2, L"Sprite_Data\\Move_R_03.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(3, L"Sprite_Data\\Move_R_04.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(4, L"Sprite_Data\\Move_R_05.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(5, L"Sprite_Data\\Move_R_06.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(6, L"Sprite_Data\\Move_R_07.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(7, L"Sprite_Data\\Move_R_08.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(8, L"Sprite_Data\\Move_R_09.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(9, L"Sprite_Data\\Move_R_10.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(10, L"Sprite_Data\\Move_R_11.bmp", 5, 71, 90, 0xffffffff);
    (*spriteData)[3]->loadImage(11, L"Sprite_Data\\Move_R_12.bmp", 5, 71, 90, 0xffffffff);
    // ----------------------------------------------------------------------

    user = new CSprite(spriteData, userUpdate);

    user->_x = 100;
    user->_y = 200;

    return;
}

int sleep = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
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
        0, 0, CLIENT_WIDTH, CLIENT_HEIGHT, nullptr, nullptr, hInstance, nullptr);

    RECT rt = {0,};
    rt.right = CLIENT_WIDTH;
    rt.bottom = CLIENT_HEIGHT;

    AdjustWindowRectEx(&rt, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL, GetWindowExStyle(hWnd));

    MoveWindow(hWnd, 0, 0, rt.right, rt.bottom, false);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    init(hWnd);

    MSG msg;

    AllocConsole();
    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w+", stdout);

    //QueryPerformanceFrequency
    // 기본 메시지 루프입니다:
    //QueryPerformanceCounter(&startTick);


    LARGE_INTEGER startTick = {0, };
    LARGE_INTEGER endTick = {0, };
    
    LARGE_INTEGER fpsStart = {0, };
    LARGE_INTEGER fpsEnd = {0, };
    __int64 fpsStartTime = fpsStart.QuadPart / freq.QuadPart;
    QueryPerformanceCounter(&fpsStart);
    
    
    timeBeginPeriod(1);

    __int64 startTime = 0;
    __int64 endTime;
    __int64 frameTime;
    int delayTime;
    int intervalTime = 20 * 1000;

    __int64 remainDelayTime = 0; 

    bool renderSkip = false;

    QueryPerformanceCounter(&fpsStart);
    fpsStartTime = fpsStart.QuadPart / freq.QuadPart;
    QueryPerformanceCounter(&startTick);
    startTime = startTick.QuadPart / freq.QuadPart;
    do {

        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else {
            input();
            update();
            renderSkip = false;
            QueryPerformanceCounter(&endTick);
            QueryPerformanceCounter(&startTick);
            endTime = endTick.QuadPart / freq.QuadPart;
            frameTime = endTime - startTime;
            startTime = startTick.QuadPart / freq.QuadPart;
            remainDelayTime += intervalTime - frameTime;
            
            if (remainDelayTime > 0) {
                delayTime = remainDelayTime / 1000;
                remainDelayTime -= delayTime * 1000;
                Sleep(delayTime);
                startTime += delayTime * 1000;
            }
            else {
                renderSkip = true;
            }

            printf("%lld %lld %d %d %lld\n", startTime, endTime, frameTime, delayTime, remainDelayTime);


            if (renderSkip == false) {
                render(hWnd);
            }

            QueryPerformanceCounter(&fpsEnd);
            __int64 fpsEndTime = fpsEnd.QuadPart / freq.QuadPart;
            if (fpsEndTime - fpsStartTime >= (__int64)1000 * 1000) {
                //printf("%d %d %d\n", fpsEndTime, fpsStartTime, fpsEndTime - fpsStartTime);
                QueryPerformanceCounter(&fpsStart);
                fpsStartTime = fpsStart.QuadPart / freq.QuadPart;
                
                wsprintfW(logicFrame, L"Logic FPS: %d", logicCnt);
                wsprintfW(renderFrame, L"render FPS: %d", renderCnt);
                logicCnt = 0;
                renderCnt = 0;


            }
            HDC hdc = GetDC(hWnd);
            TextOutW(hdc, 0, 0, logicFrame, wcslen(logicFrame));
            TextOutW(hdc, 0, 20, renderFrame, wcslen(renderFrame));
            ReleaseDC(hWnd, hdc);

                
        }

    } while (msg.message != WM_QUIT);

    return (int) msg.wParam;
}

