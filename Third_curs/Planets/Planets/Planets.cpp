// Massive_GL.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Planets.h"
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
float vert[] = { 2,2,0, 2,-2,0, -2,-2,0, -2,2,0 };
float xAlfa = 0;
float zAlfa = 0;
float orbitAlfa = 0.01;
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Features(HWND, UINT, WPARAM, LPARAM);
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
    LoadStringW(hInstance, IDC_PLANETS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    UINT uResult = SetTimer(hWnd,
        1,            // timer identifier 
        50,                    // 20-mili second interval 
        (TIMERPROC)NULL);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANETS));

    MSG msg;

    //Camera
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 20);
    glTranslatef(0, 0, -2);
    // Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLANETS));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLANETS);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
    glViewport(0, 0, height, height);
    glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
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
    //glOrtho(-5, 5, -5, 5, 2, 12);
    // Устанавливаем точку, в которой
    // находится наш глаз ---(0,0,5)
    // направление, куда смотрим --- (0,0,0)
    // вектор, принимаемый за направление вверх --- (0,1,0)
    // этим вектором является ось Y
    //gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void ShowWorld() {
	glPushMatrix();
	glTranslatef(0, 0, 0.3);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &vert);
    glColor3f(0, 0, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}
void Paint() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    //Move camera
	glPushMatrix();
    glRotatef(xAlfa, 1, 0, 0);
    glRotatef(zAlfa, 0, 0, 1);
    ShowWorld();
    glPushMatrix();
        glColor3f(0.0f, 0.0f, 1.0f);
        glRotatef(-orbitAlfa, 0, 1, 0);
        glTranslatef(0.0, 0.0, 0.5);
        GLUquadricObj* pObj = gluNewQuadric();
        gluQuadricDrawStyle(pObj, GLU_FILL);
        gluSphere(pObj, 0.1f, 25, 25);
    glPopMatrix();
    glPushMatrix();
        glColor4f(1.0f, 1.0f, 0.1f, 1.0);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
		glEnable(GL_LIGHT0);
        GLUquadricObj* pObj2 = gluNewQuadric();
        gluSphere(pObj2, 0.3f, 30, 30);
    glPopMatrix();
	glPushMatrix();
		glColor4f(1.0f, 0.0f, 0.1f, 1.0);
		glRotatef(orbitAlfa, 0, 0, 1);
		glTranslatef(-0.0, -0.5, 0.0);
		GLUquadricObj* pObj3 = gluNewQuadric();
		gluSphere(pObj3, 0.2f, 30, 30);
	glPopMatrix();
	glPushMatrix();
	glColor4f(1.0f, 0.0f, 1.0f, 1.0);
	glRotatef(orbitAlfa, 0, 1, 1);
	glTranslatef(-0.0, -0.9, 0.0);
	GLUquadricObj* pObj4 = gluNewQuadric();
	gluSphere(pObj4, 0.2f, 30, 30);
	glPopMatrix();
	glPushMatrix();
	glColor4f(0.0f, 1.0f, 1.0f, 1.0);
	glRotatef(orbitAlfa, 1, 0, 1);
	glTranslatef(-0.0, 0.9, 0.0);
	GLUquadricObj* pObj5 = gluNewQuadric();
	gluSphere(pObj5, 0.15f, 30, 30);
	glPopMatrix();
	glPopMatrix();
    //
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
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), hWnd, About);
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
    case WM_TIMER:
        orbitAlfa += 1;
        InvalidateRect(hWnd, &rect, false);
        break;
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
    case WM_KEYDOWN:
        if (wParam == VK_UP)
        {
            xAlfa = ++xAlfa > 180 ? 180 : xAlfa;
            InvalidateRect(hWnd, &rect, false);
        }
        if (wParam == VK_DOWN)
        {
            xAlfa = --xAlfa < 0 ? 0 : xAlfa;
            InvalidateRect(hWnd, &rect, false);
        }
        if (wParam == VK_LEFT)
        {
            zAlfa = ++zAlfa;
            InvalidateRect(hWnd, &rect, false);
        }
        if (wParam == VK_RIGHT)
        {
            zAlfa = --zAlfa;
            InvalidateRect(hWnd, &rect, false);
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
        if (LOWORD(wParam) == IDC_CLOSE || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
		if (LOWORD(wParam) == IDC_UPDATEPICTURE){

		}
        break;
    }
    return (INT_PTR)FALSE;
}