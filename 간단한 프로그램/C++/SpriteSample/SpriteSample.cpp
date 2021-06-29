// SpriteSample.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "resource.h"
#include "ScreenDib.h"
#include "Sprite.h"

#define MIN(a,b) a>b?b:a
#define MAX(a,b) a>b?a:b

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HDC hMemDC;
HBITMAP hBufBmp;
RECT rtClient;

BYTE* backgroundBuf;

stSprite user;

LARGE_INTEGER mouseMoveTime;
bool mouseMoveNow;
LARGE_INTEGER freq;

int logicCnt = 0;
int renderCnt = 0;

WCHAR logicFrame[100] = { 0, };
WCHAR renderFrame[100] = { 0, };

void update() {
    Sleep(5);
    logicCnt += 1;
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
   
    // ----------------------------------------------------------------------
    // 플레이어 캐릭터 출력
    {
        stImage* img = user.getCurrentAnimation()->getCurrentImage();
        int width = img->_width;
        int height = img->_height;

        int left = user.x - width / 2;
        int right = left + width;
        int top = user.y - height;
        int bottom = user.y;


        DWORD* dwpUserBuf = (DWORD*)img->_idleBuf;
        if (left < 0) {
            dwpUserBuf -= left;
            left = 0;
        }
        right = MIN(640, right);

        if (top < 0) {
            dwpUserBuf += -top * width;
            top = 0;
        }
        bottom = MIN(480, bottom);

        DWORD* dwpBmpBuf = (DWORD*)bmpBuf + left + (bufPitch / 4 * top);

        for (int cntY = top; cntY < bottom; cntY++) {
            for (int cntX = left; cntX < right; cntX++) {
                /*
                if ((*(dwpUserBuf + cntX - left) & 0x00ffffff) == 0x00ffffff) {
                    continue;
                }
                */
                memcpy(dwpBmpBuf + cntX - left, dwpUserBuf + cntX - left, sizeof(DWORD));
            }
            dwpUserBuf += width;
            dwpBmpBuf += bufPitch / 4;
        }
    }
    // ----------------------------------------------------------------------

    CScreenDib::getInstance()->flip(hWnd);

    //Sleep(60);

}

void input() {

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
    case WM_CREATE:
        //setTimer(hWnd, 0, 1000, NULL);
        break;
    case WM_TIMER: 
        {
        }
        break;
    case WM_MOUSEMOVE:
        user.x = GET_X_LPARAM(lParam);
        user.y = GET_Y_LPARAM(lParam);
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

