// Massive_GL.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Massive_GL.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#pragma comment (lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hWnd;
HGLRC hGLRC;
HDC hDC;

int mas_lenght = 3;
float g_scale = 0.0f;
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int SetWindowPixelFormat()
{
    int m_GLPixelIndex;
    PIXELFORMATDESCRIPTOR pfd;
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL |
        PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cRedShift = 16;
    pfd.cGreenBits = 8;
    pfd.cGreenShift = 8;
    pfd.cBlueBits = 8;
    pfd.cBlueShift = 0;
    pfd.cAlphaBits = 0;
    pfd.cAlphaShift = 0;
    pfd.cAccumBits = 64;
    pfd.cAccumRedBits = 16;
    pfd.cAccumGreenBits = 16;
    pfd.cAccumBlueBits = 16;
    pfd.cAccumAlphaBits = 0;
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    pfd.cAuxBuffers = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.bReserved = 0;
    pfd.dwLayerMask = 0;
    pfd.dwVisibleMask = 0;
    pfd.dwDamageMask = 0;
    m_GLPixelIndex = ChoosePixelFormat(hDC, &pfd);
    if (m_GLPixelIndex == 0) // Let's choose a default index.
    {
        m_GLPixelIndex = 1;
        if (DescribePixelFormat(hDC, m_GLPixelIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0)
            return 0;
    }
    if (SetPixelFormat(hDC, m_GLPixelIndex, &pfd) == FALSE)
        return 0;
    return 1;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MASSIVEGL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MASSIVEGL));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MASSIVEGL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MASSIVEGL);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
  
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void CALLBACK resize(int width, int height)
{
    // Здесь вы указываете ту часть окна,
    // куда осуществляется вывод OpenGL.
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Устанавливаем тип проекции.
    // glOrtho - параллельная
    // glFrustum - перспективная
    // Параметры у этих функций одинаковые.
    // Они определяют объем, который вы видите.
    // левая стенка - пять единиц влево
    // правая - пять единиц вправо
    // далее, нижняя стенка и верхняя
    // и наконец, передняя и задняя
    // см. ниже картинку
    glOrtho(-5, 5, -5, 5, 2, 12);
    // Устанавливаем точку, в которой
    // находится наш глаз ---(0,0,5)
    // направление, куда смотрим --- (0,0,0)
    // вектор, принимаемый за направление вверх --- (0,1,0)
    // этим вектором является ось Y
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
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
void Paint() {
    //int len = 7;
    float dx_w = 0.4f, dx_h = 0.1f, dx_s = 0.0f, color = 0.4;
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
    glPointSize(3);
    glScaled(0.5625 + g_scale, 1.0 + g_scale, 1.0);
    for (int i = 0; i < mas_lenght; i++) {
        for (int j = 0; j < 4; j++) {
            glRotatef(90, 0, 0, 1);
            glBegin(GL_TRIANGLES);
            glColor3f(color + 0.2, color + 0.1, color);  glVertex2f(dx_s, 0);
            glColor3f(color + 0.2, color + 0.1, color);  glVertex2f(dx_s + dx_w, dx_h);
            glColor3f(color + 0.2, color + 0.1, color);  glVertex2f(dx_s + dx_w, -dx_h);
            glEnd();

            glBegin(GL_LINES);
            glColor3f(0, 0, 0);
            glVertex2f(dx_s, 0);
            glVertex2f(dx_s + dx_w, 0);

            glVertex2f(dx_s, 0);
            glVertex2f(dx_s + dx_w, dx_h);

            glVertex2f(dx_s, 0);
            glVertex2f(dx_s + dx_w, -dx_h);

            if (i < mas_lenght - 1) {
                glVertex2f(dx_s + dx_w, dx_h);
                glVertex2f(dx_s + dx_w + dx_w, dx_h * 1.4);

                glVertex2f(dx_s + dx_w, -dx_h);
                glVertex2f(dx_s + dx_w + dx_w, -dx_h * 1.4);
            }

            glVertex2f(dx_s + dx_w, dx_h);
            glVertex2f(dx_s + dx_w, -dx_h);

            glEnd();

            glBegin(GL_POINTS);
            glColor3f(1, 0, 0);
            glVertex2f(dx_s, 0);
            glVertex2f(dx_s + dx_w, dx_h);
            glVertex2f(dx_s + dx_w, -dx_h);
            glVertex2f(dx_s + dx_w, 0);
            glEnd();
        }
 
        dx_s += dx_w;
        
        //dx_w *= 0.7;
        dx_h *= 1.4;
        color += 0.2;
        if (color >= 1.0)
            color = 0.4;
    }
    /*if (len > mas_lenght)
        g_scale -= 0.01;
    if (len < mas_lenght)
        g_scale += 0.01;
    len = mas_lenght;*/
    glPopMatrix();
    glFinish();
    SwapBuffers(hDC);
}
//
RECT rect;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GetClientRect(hWnd, &rect);
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
        hDC = GetDC(hWnd);
        SetWindowPixelFormat();
        hGLRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hGLRC);
        break;
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
            BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            Paint();

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        if (hGLRC)
        {
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(hGLRC);
        }
        // освобождаем контекст устройства нашего окна
        ReleaseDC(hWnd, hDC);
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        resize(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_CHAR:
        if (wParam == 49)
        {
            mas_lenght++;
            InvalidateRect(hWnd, &rect, TRUE);
        }
        if (wParam == 50)
        {
            if (mas_lenght != 0)
                mas_lenght--;
            InvalidateRect(hWnd, &rect, TRUE);
        }
        break;
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