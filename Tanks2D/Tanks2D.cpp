// Tanks2D.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Tanks2D.h"
#include <windows.h>
#include <objidl.h>
#include "gdiplus.h"
#include <string>
#include <sstream>
#include <malloc.h>
#pragma comment(lib, "Msimg32.lib")

#define HEADER_HEIGHT 59
#define HEADER_WIDTH 16
#define MY_HEADER_HEIGHT 50
#define MAX_LOADSTRING 100
#define WINDOW_WIDTH 840 + HEADER_WIDTH
#define WINDOW_HEIGHT 600 + HEADER_HEIGHT
#define CELL_SIZE 60
#define MAP_HEIGHT 10
#define MAP_WIDTH 14
#define SHOT_SIZE 6
#define TO_REAL_SIZE 14

struct shot {
    int x;
    int y;
    BOOL* dir;
    BOOL isPlayer;
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HBITMAP hBmpPlayer, hBmpEnemy, hBmpShoot, hBmpBack, hBmpBlack, hBmpHp;
RECT cRect;
RECT cellRect;
PAINTSTRUCT ps;
HDC hdc;
HDC hCompatibleDC;
HANDLE hOldBitmap;
POINT posPlayer, posEnemy;
BOOL dir[4] = { true, false, false, false };
BOOL dirEnemy[4] = { true, false, false, false };
int map[MAP_HEIGHT][MAP_WIDTH] =
{ 
    { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 1, 0, 0, 0, 0, 1, 0, 0, 0 },
    { 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }
};
shot* shots;
int shotsCount = 0;
int shotDelay = 0, moveDelay = 0, turnDelay = 0, botShotDelay = 0, botMoveDelay = 0, botTurnDelay = 0, botSpeed = 0;
int hpPlayer = 3, hpEnemy = 3;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID                TurnRight();
VOID                TurnLeft();
VOID                TurnRightEnemy();
VOID                TurnLeftEnemy();
VOID                MoveSprite(BOOL, int*, int*, POINT*, int*, int*, BOOL*);
VOID                Shoot(int, int, BOOL*, BOOL);
VOID                Unshoot(int);
BOOL                IsStuck(int, int, int);
BOOL                IsEnemy(int, int, int, BOOL);
VOID                ShootPlayerNearby();
VOID                MoveEnemy();
VOID                DrawBmp(HBITMAP, int, int, int, int, int, int, int, int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.
    hBmpPlayer = (HBITMAP)LoadImageW(hInst, L"tankBlue.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBmpPlayer) {
        MessageBox(NULL, L"ÛÀÛ!", L"ÛÀÛ!", MB_OK);
        return 0;
    }

    hBmpEnemy = (HBITMAP)LoadImageW(hInst, L"tankRed.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBmpEnemy) {
        MessageBox(NULL, L"ÛÀÛ!", L"ÛÀÛ!", MB_OK);
        return 0;
    }
    
    hBmpShoot = (HBITMAP)LoadImageW(hInst, L"shot.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBmpShoot) {
        MessageBox(NULL, L"ÛÀÛ!", L"ÛÀÛ!", MB_OK);
        return 0;
    }

    hBmpBack = (HBITMAP)LoadImageW(hInst, L"background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBmpBack) {
        MessageBox(NULL, L"ÛÀÛ!", L"ÛÀÛ!", MB_OK);
        return 0;
    }

    hBmpBlack = (HBITMAP)LoadImageW(hInst, L"BLACK.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBmpBack) {
        MessageBox(NULL, L"ÛÀÛ!", L"ÛÀÛ!", MB_OK);
        return 0;
    }

    hBmpHp = (HBITMAP)LoadImageW(hInst, L"hp.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBmpBack) {
        MessageBox(NULL, L"ÛÀÛ!", L"ÛÀÛ!", MB_OK);
        return 0;
    }

    posPlayer.x = 0;
    posPlayer.y = 9;
    posEnemy.x = 12;
    posEnemy.y = 1;

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TANKS2D, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TANKS2D));

    MSG msg;

    // Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TANKS2D));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreatePatternBrush((HBITMAP)LoadImage(NULL, TEXT("background.bmp"), 0, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TANKS2D);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_BORDER | WS_SYSMENU,
               0, 0, WINDOW_WIDTH, WINDOW_HEIGHT + MY_HEADER_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   SetTimer(hWnd, 1, 25, NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
int baseX = 0, baseY = WINDOW_HEIGHT - 59 - CELL_SIZE;
int enemyX = WINDOW_WIDTH - 2 * CELL_SIZE - HEADER_WIDTH, enemyY = CELL_SIZE;
int const moveSpeed = CELL_SIZE;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
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
        hdc = BeginPaint(hWnd, &ps);
        hCompatibleDC = CreateCompatibleDC(hdc);
        int newX, newY;

        int i = 0;
        while ( i < shotsCount)
        {
            newX = shots[i].x + CELL_SIZE / 2 - SHOT_SIZE / 2;
            newY = shots[i].y + CELL_SIZE / 2 - SHOT_SIZE / 2, SHOT_SIZE, SHOT_SIZE;
            DrawBmp(hBmpShoot, newX, newY, SHOT_SIZE, SHOT_SIZE, 0, 0, SHOT_SIZE, SHOT_SIZE);
            if (shots[i].dir[0] && newY > 0)
                shots[i].y--;
            if (shots[i].dir[1] && newX < WINDOW_WIDTH - SHOT_SIZE)
                shots[i].x++;
            if (shots[i].dir[2] && newY < WINDOW_HEIGHT - SHOT_SIZE)
                shots[i].y++;
            if (shots[i].dir[3] && newX > 0)
                shots[i].x--;

            if (IsEnemy(newX, newY, SHOT_SIZE, shots[i].isPlayer))
            {
                if (shots[i].isPlayer)
                    hpEnemy--;
                else
                    hpPlayer--;

                if (hpEnemy == 0 || hpPlayer == 0)
                {
                    PostMessage(FindWindow(NULL, L"Tanks2D"), WM_QUIT, 0, 0);
                }
                Unshoot(i);        
            }
            else
            {
                if (IsStuck(newX, newY, SHOT_SIZE))
                    Unshoot(i);
                else
                    i++;
            }
        }

        DrawBmp(hBmpPlayer, baseX + TO_REAL_SIZE, baseY + TO_REAL_SIZE, CELL_SIZE - TO_REAL_SIZE, CELL_SIZE - TO_REAL_SIZE, TO_REAL_SIZE, TO_REAL_SIZE, CELL_SIZE - TO_REAL_SIZE, CELL_SIZE - TO_REAL_SIZE);

        DrawBmp(hBmpEnemy, enemyX + TO_REAL_SIZE, enemyY + TO_REAL_SIZE, CELL_SIZE - TO_REAL_SIZE, CELL_SIZE - TO_REAL_SIZE, TO_REAL_SIZE, TO_REAL_SIZE, CELL_SIZE - TO_REAL_SIZE, CELL_SIZE - TO_REAL_SIZE);

        hOldBitmap = SelectObject(hCompatibleDC, hBmpBlack);
        BitBlt(hdc, 0, 600, WINDOW_WIDTH, 600 + MY_HEADER_HEIGHT, hCompatibleDC, 0, 0, SRCCOPY);
        SelectObject(hCompatibleDC, hOldBitmap);

        for (int i = 0; i < hpPlayer; i++)
        {
            DrawBmp(hBmpHp, 16 + i * 20, 600 + 16, 18, 18, 0, 0, 18, 18);
        }

        for (int i = 0; i < hpEnemy; i++)
        {
            DrawBmp(hBmpHp, 824 - 18 - i * 20, 600 + 16, 18, 18, 0, 0, 18, 18);
        }

        EndPaint(hWnd, &ps);
        DeleteDC(hdc);
        DeleteDC(hCompatibleDC);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN: {
        switch (wParam) {
        case VK_UP: {
            if (moveDelay == 0)
            {
                moveDelay = 10;
                MoveSprite(true, &baseX, &baseY, &posPlayer, &hpPlayer, &moveDelay, dir);
            }
            break;
        }
        case VK_DOWN: {
            if (moveDelay == 0)
            {
                moveDelay = 10;
                MoveSprite(false, &baseX, &baseY, &posPlayer, &hpPlayer, &moveDelay, dir);
            }
            break;
        }
        case VK_LEFT: {
            if (turnDelay == 0)
            {
                turnDelay = 10;
                TurnLeft();
            }
            break;
        }
        case VK_RIGHT: {
            if (turnDelay == 0)
            {
                turnDelay = 10;
                TurnRight();
            }
            break;
        }
        case VK_SPACE: {
            if (shotDelay == 0)
            {
                shotDelay = 40;
                Shoot(baseX, baseY, dir, true);
            }
        }
        }
        break;
    }
    case WM_GETMINMAXINFO: {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.y = WINDOW_HEIGHT;
        lpMMI->ptMinTrackSize.y = WINDOW_HEIGHT;
        lpMMI->ptMinTrackSize.x = WINDOW_WIDTH;
        lpMMI->ptMinTrackSize.x = WINDOW_WIDTH;
        break;
    }
    case WM_TIMER: {
        InvalidateRect(hWnd, NULL, TRUE);
        if (shotDelay != 0)
            shotDelay--;

        if (moveDelay != 0)
            moveDelay--;

        if (turnDelay != 0)
            turnDelay--;

        if (botTurnDelay != 0)
            botTurnDelay--;

        if (botShotDelay != 0)
            botShotDelay--;
        else
            ShootPlayerNearby();

        if (botMoveDelay != 0)
            botMoveDelay--;
        else
            if (botTurnDelay == 0)
                MoveEnemy();

        break;
    }
    //case WM_ERASEBKGND:
    //    return (LRESULT)1; // Say we handled it.
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
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

VOID TurnRight()
{
    long lpvBits[CELL_SIZE * CELL_SIZE], result[CELL_SIZE * CELL_SIZE];
    int x, y, res;
    GetBitmapBits(hBmpPlayer, sizeof(long) * CELL_SIZE * CELL_SIZE, &lpvBits);
    for (int i = 0; i < CELL_SIZE * CELL_SIZE; i++)
    {
        x = i % CELL_SIZE;
        y = i / CELL_SIZE;
        res = (CELL_SIZE - y - 1) + (x) *CELL_SIZE;
        result[res] = lpvBits[i];
    }
    SetBitmapBits(hBmpPlayer, sizeof(long) * CELL_SIZE * CELL_SIZE, &result);

    for (int i = 0; i < 4; i++)
    {
        if (dir[i])
        {
            dir[i] = false;
            dir[(i + 1) % 4] = true;
            break;
        }
    }
}

VOID TurnLeft()
{
    long lpvBits[CELL_SIZE * CELL_SIZE], result[CELL_SIZE * CELL_SIZE];
    int x, y, res;
    GetBitmapBits(hBmpPlayer, sizeof(long) * CELL_SIZE * CELL_SIZE, &lpvBits);
    for (int i = 0; i < CELL_SIZE * CELL_SIZE; i++)
    {
        x = i % CELL_SIZE;
        y = i / CELL_SIZE;
        res = (CELL_SIZE - y - 1) + (x) * CELL_SIZE;
        result[i] = lpvBits[res];
    }
    SetBitmapBits(hBmpPlayer, sizeof(long) * CELL_SIZE * CELL_SIZE, &result);

    for (int i = 0; i < 4; i++)
    {
        if (dir[i])
        {
            dir[i] = false;
            dir[(i + 3) % 4] = true;
            break;
        }
    }
}

VOID TurnRightEnemy()
{
    long lpvBits[CELL_SIZE * CELL_SIZE], result[CELL_SIZE * CELL_SIZE];
    int x, y, res;
    GetBitmapBits(hBmpEnemy, sizeof(long) * CELL_SIZE * CELL_SIZE, &lpvBits);
    for (int i = 0; i < CELL_SIZE * CELL_SIZE; i++)
    {
        x = i % CELL_SIZE;
        y = i / CELL_SIZE;
        res = (CELL_SIZE - y - 1) + (x)*CELL_SIZE;
        result[res] = lpvBits[i];
    }
    SetBitmapBits(hBmpEnemy, sizeof(long) * CELL_SIZE * CELL_SIZE, &result);

    for (int i = 0; i < 4; i++)
    {
        if (dirEnemy[i])
        {
            dirEnemy[i] = false;
            dirEnemy[(i + 1) % 4] = true;
            break;
        }
    }
}

VOID TurnLeftEnemy()
{
    long lpvBits[CELL_SIZE * CELL_SIZE], result[CELL_SIZE * CELL_SIZE];
    int x, y, res;
    GetBitmapBits(hBmpEnemy, sizeof(long) * CELL_SIZE * CELL_SIZE, &lpvBits);
    for (int i = 0; i < CELL_SIZE * CELL_SIZE; i++)
    {
        x = i % CELL_SIZE;
        y = i / CELL_SIZE;
        res = (CELL_SIZE - y - 1) + (x)*CELL_SIZE;
        result[i] = lpvBits[res];
    }
    SetBitmapBits(hBmpEnemy, sizeof(long) * CELL_SIZE * CELL_SIZE, &result);

    for (int i = 0; i < 4; i++)
    {
        if (dirEnemy[i])
        {
            dirEnemy[i] = false;
            dirEnemy[(i + 3) % 4] = true;
            break;
        }
    }
}

VOID MoveSprite(BOOL flag, int *adrX, int* adrY, POINT *posPlayer, int* hp, int* delay, BOOL* dir)
{
    if (flag)
    {
        if (dir[0] && *adrY - CELL_SIZE >= 0)
            if (map[*adrY / CELL_SIZE - 1][*adrX / CELL_SIZE] != 1 && (*adrY / CELL_SIZE - 1 != posEnemy.y || *adrX / CELL_SIZE != posEnemy.x))
            {
                *adrY -= CELL_SIZE;
                posPlayer->y--;
            }
        if (dir[1] && *adrX + CELL_SIZE <= WINDOW_WIDTH - CELL_SIZE)
            if (map[*adrY / CELL_SIZE][*adrX / CELL_SIZE + 1] != 1 && (*adrY / CELL_SIZE != posEnemy.y || *adrX / CELL_SIZE + 1 != posEnemy.x))
            {
                *adrX += CELL_SIZE;
                posPlayer->x++;
            }
        if (dir[2] && *adrY + CELL_SIZE <= WINDOW_HEIGHT - CELL_SIZE)
            if (map[*adrY / CELL_SIZE + 1][*adrX / CELL_SIZE] != 1 && (*adrY / CELL_SIZE + 1 != posEnemy.y || *adrX / CELL_SIZE != posEnemy.x))
            {
                *adrY += CELL_SIZE;
                posPlayer->y++;
            }
        if (dir[3] && *adrX - CELL_SIZE >= 0)
            if (map[*adrY / CELL_SIZE][*adrX / CELL_SIZE - 1] != 1 && (*adrY / CELL_SIZE != posEnemy.y || *adrX / CELL_SIZE - 1 != posEnemy.x))
            {
                *adrX -= CELL_SIZE;
                posPlayer->x--;
            }
    }
    else
    {
        if (dir[0] && *adrY + CELL_SIZE <= WINDOW_HEIGHT - CELL_SIZE)
            if (map[*adrY / CELL_SIZE + 1][*adrX / CELL_SIZE] != 1 && (*adrY / CELL_SIZE + 1 != posEnemy.y || *adrX / CELL_SIZE != posEnemy.x))
            {
                *adrY += CELL_SIZE;
                posPlayer->y++;
            }
        if (dir[1] && *adrX - CELL_SIZE >= 0)
            if (map[*adrY / CELL_SIZE][*adrX / CELL_SIZE - 1] != 1 && (*adrY / CELL_SIZE != posEnemy.y || *adrX / CELL_SIZE - 1 != posEnemy.x))
            {
                *adrX -= CELL_SIZE;
                posPlayer->x--;
            }
        if (dir[2] && *adrY - CELL_SIZE >= 0)
            if (map[*adrY / CELL_SIZE - 1][*adrX / CELL_SIZE] != 1 && (*adrY / CELL_SIZE - 1 != posEnemy.y || *adrX / CELL_SIZE != posEnemy.x))
            {
                *adrY -= CELL_SIZE;
                posPlayer->y--;
            }
        if (dir[3] && *adrX + CELL_SIZE <= WINDOW_WIDTH - CELL_SIZE)
            if (map[*adrY / CELL_SIZE][*adrX / CELL_SIZE + 1] != 1 && (*adrY / CELL_SIZE != posEnemy.y || *adrX / CELL_SIZE + 1 != posEnemy.x))
            {
                *adrX += CELL_SIZE;
                posPlayer->x++;
            }
    }
    if (map[posPlayer->y][posPlayer->x] == 2)
    {
        (*hp)--;
    }

    if (hpEnemy == 0 || hpPlayer == 0)
    {
        PostMessage(FindWindow(NULL, L"Tanks2D"), WM_QUIT, 0, 0);
    }

    if (map[posPlayer->y][posPlayer->x] == 3)
    {
        *delay = 120;
    }
}

VOID Shoot(int x, int y, BOOL* dir, BOOL isPlayer)
{
    shotsCount++;
    shot* arr = (shot*)malloc(shotsCount * sizeof(shot));
    for (int i = 0; i < shotsCount - 1; i++)
    {
        arr[i] = shots[i];
    }

    shot shot;
    shot.x = x;
    shot.y = y;
    shot.dir = (BOOL*)malloc(4 * sizeof(BOOL));
    shot.dir[0] = dir[0];
    shot.dir[1] = dir[1];
    shot.dir[2] = dir[2];
    shot.dir[3] = dir[3];
    shot.isPlayer = isPlayer;
    arr[shotsCount - 1] = shot;
    free(shots);
    shots = arr;
}

VOID Unshoot(int pos)
{
    shotsCount--;
    shot* arr = (shot*)malloc(shotsCount * sizeof(shot));
    int position = 0;
    while (position < pos)
    {
        arr[position] = shots[position];
        position++;
    }
    while (position < shotsCount)
    {
        arr[position] = shots[position + 1];
        position++;
    }
    free(shots[pos].dir);
    free(shots);
    shots = arr;
}

BOOL IsStuck(int x, int y, int size)
{
    if (x + size > WINDOW_WIDTH || x <= 0 || y + size> WINDOW_HEIGHT || y <= 0
        || map[y / CELL_SIZE][x / CELL_SIZE] == 1 || map[(y + size) / CELL_SIZE][(x + size) / CELL_SIZE] == 1)
        return true;
    else
        return false;
}

BOOL IsEnemy(int x, int y, int size, BOOL isPlayer)
{
    if (isPlayer)
    {
        if ((x + TO_REAL_SIZE <= enemyX + CELL_SIZE && y + TO_REAL_SIZE <= enemyY + CELL_SIZE &&
            x - TO_REAL_SIZE >= enemyX && y - TO_REAL_SIZE >= enemyY) ||
            ((x + size) + TO_REAL_SIZE <= enemyX + CELL_SIZE && (y + size) + TO_REAL_SIZE <= enemyY + CELL_SIZE &&
                (x + size) - TO_REAL_SIZE >= enemyX && (y + size) - TO_REAL_SIZE >= enemyY))
            return true;
        else
            return false;
    }
    else
    {
        if ((x + TO_REAL_SIZE <= baseX + CELL_SIZE && y + TO_REAL_SIZE <= baseY + CELL_SIZE &&
            x - TO_REAL_SIZE >= baseX && y - TO_REAL_SIZE >= baseY) ||
            ((x + size) + TO_REAL_SIZE <= baseX + CELL_SIZE && (y + size) + TO_REAL_SIZE <= baseY + CELL_SIZE &&
                (x + size) - TO_REAL_SIZE >= baseX && (y + size) - TO_REAL_SIZE >= baseY))
            return true;
        else
            return false;
    }
}

VOID ShootPlayerNearby()
{
    if (posPlayer.x == posEnemy.x)
    {
        if (posPlayer.y < posEnemy.y)
        {
            for (int i = posEnemy.y; i <= posPlayer.y; i++)
            {
                if (map[i][posPlayer.x] == 1)
                    return;
            }
            if (!dirEnemy[0])
            {
                if (botTurnDelay == 0)
                {
                    if (dirEnemy[1])
                        TurnLeftEnemy();
                    else
                        TurnRightEnemy();
                    botTurnDelay = 20;
                }
            }
            else
            {
                Shoot(enemyX, enemyY, dirEnemy, false);
                botShotDelay = 20;
            }
        }
        else
        {
            for (int i = posPlayer.y; i <= posEnemy.y; i++)
            {
                if (map[i][posPlayer.x] == 1)
                    return;
            }
            if (!dirEnemy[2])
            {
                if (botTurnDelay == 0)
                {
                    if (dirEnemy[3])
                        TurnLeftEnemy();
                    else
                        TurnRightEnemy();
                    botTurnDelay = 20;
                }
            }
            else
            {
                Shoot(enemyX, enemyY, dirEnemy, false);
                botShotDelay = 20;
            }
        }
    }
    else
    {
        if (posPlayer.y == posEnemy.y)
        {
            if (posPlayer.x > posEnemy.x)
            {
                for (int i = posEnemy.x; i <= posPlayer.x; i++)
                {
                    if (map[posPlayer.x][i] == 1)
                        return;
                }
                if (!dirEnemy[1])
                {
                    if (botTurnDelay == 0)
                    {
                        if (dirEnemy[2])
                            TurnLeftEnemy();
                        else
                            TurnRightEnemy();
                        botTurnDelay = 20;
                    }
                }
                else
                {
                    Shoot(enemyX, enemyY, dirEnemy, false);
                    botShotDelay = 20;
                }
            }
            else
            {
                for (int i = posPlayer.x; i <= posEnemy.x; i++)
                {
                    if (map[posPlayer.x][i] == 1)
                        return;
                }
                if (!dirEnemy[3])
                {
                    if (botTurnDelay == 0)
                    {
                        if (dirEnemy[0])
                            TurnLeftEnemy();
                        else
                            TurnRightEnemy();
                        botTurnDelay = 20;
                    }
                }
                else
                {
                    Shoot(enemyX, enemyY, dirEnemy, false);
                    botShotDelay = 20;
                }
            }
        }
        return;
    }
}

VOID MoveEnemy()
{
    if (rand() % 3)
    {
        if (
            !(
                (dirEnemy[0] && !(posEnemy.y - dirEnemy[0] >= 0)) ||
                (dirEnemy[1] && !(posEnemy.x + dirEnemy[1] <= 13)) ||
                (dirEnemy[2] && !(posEnemy.y + dirEnemy[2] <= 9)) ||
                (dirEnemy[3] && !(posEnemy.x - dirEnemy[3] >= 0))
             )
           )
        {
            if (map[posEnemy.y - dirEnemy[0] + dirEnemy[2]][posEnemy.x - dirEnemy[3] + dirEnemy[1]] != 1 &&
                posEnemy.y - dirEnemy[0] + dirEnemy[2] != posPlayer.y &&
                posEnemy.x - dirEnemy[3] + dirEnemy[1] != posPlayer.x)
            {
                botMoveDelay = 20;
                botTurnDelay = 20;
                MoveSprite(true, &enemyX, &enemyY, &posEnemy, &hpEnemy, &botMoveDelay, dirEnemy);
            }
        }
        else
        {
            if (rand() % 2)
            {
                TurnLeftEnemy();
                botTurnDelay = 20;
            }
            else
            {
                TurnRightEnemy();
                botTurnDelay = 20;
            }
        }
    }
    else
    {
        if (rand() % 2)
        {
            TurnLeftEnemy();
            botTurnDelay = 20;
        }
        else
        {
            TurnRightEnemy();
            botTurnDelay = 20;
        }
    }
}

VOID DrawBmp(HBITMAP hBmp, int baseX, int baseY, int sizeX, int sizeY, int baseX2, int baseY2, int sizeX2, int sizeY2)
{
    hOldBitmap = SelectObject(hCompatibleDC, hBmp);
    TransparentBlt(hdc, baseX, baseY, sizeX, sizeY, hCompatibleDC, baseX2, baseY2, sizeX2, sizeY2, RGB(255, 255, 255));
    SelectObject(hCompatibleDC, hOldBitmap);
}