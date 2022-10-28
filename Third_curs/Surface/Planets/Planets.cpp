#define _USE_MATH_DEFINES
#define STRICT
#include "framework.h"
#include "resource.h"
#include <cmath>
#include <stdio.h>
#include <cstdio>
#include <gl\\gl.h>
#include <gl\\glu.h>
#include <vector>
#include <algorithm>
#pragma comment (linker, "/defaultlib:opengl32.lib")
#pragma comment (linker, "/defaultlib:glu32.lib")
#define MAX_LOADSTRING 100
#define PI  3.14159265
using namespace std;

struct C3dVector
{
    double x, y, z;
};

class Surface
{
private:
    float hLow = 12.0f;
    float hMiddle = 4.0f;
    float hEnd = 6.0f;
    float lengthFromLowToMiddle = 30.0f;
    float lengthFromMiddleToEnd = 12.0f;
public:
    vector<C3dVector> points;
    float pt_insX = 0.0f;
    float pt_insY = 0.0f;
    float pt_insZ = 0.0f;
    void setHLow(float value); // Больший радиус
    void setHMiddle(float value); //Средний радиус
    void setHEnd(float value); //
    void setLengthFromLowToMiddle(float value); //
    void setLengthFromMiddleToEnd(float value); //
    float getHLow();
    float getHMiddle();
    float getHEnd();
    float getLengthFromLowToMiddle();
    float getLengthFromMiddleToEnd();
    void drawObject();
    void drawCone(double r_low, double r_up, double h, int n, bool isSecond);
    void drawCircle(float r);
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING] = {'S', 'u', 'r', 'f', 'a', 'c', 'e'}; // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND g_hWindow;
HGLRC hGLRC;
HDC hDC;
Surface first = Surface();
Surface second = Surface();
double angle = -75 * (PI / 180);

float xAlfa = 100; // наклон мира
float zAlfa = 45; // вращение мира
double surfaceAngle = 75.0; // наклон поверхностей
int g_wndWidth = -1, g_wndHeight = -1;
// размер сцены
double g_sceneWidth = 50.0;
double g_angle = 0.0;
const double g_angIncr = 1;
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Features(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ChangeParameters(HWND, UINT, WPARAM, LPARAM);
void find(); // Функция нахождения точек пересечения поверхностей
void updatePoints(); // Обновление координат точек поверхностей после поворота

float Surface::getHLow() {
    return hLow;
}

float Surface::getHMiddle() {
    return hMiddle;
}

float Surface::getHEnd() {
    return hEnd;
}

float Surface::getLengthFromLowToMiddle() {
    return lengthFromLowToMiddle;
}

float Surface::getLengthFromMiddleToEnd() {
    return lengthFromMiddleToEnd;
}

void Surface::setHLow(float value) {
    hLow = value;
}

void Surface::setHMiddle(float value) {
    hMiddle = value;
}

void Surface::setHEnd(float value) {
    hEnd = value;
}

void Surface::setLengthFromLowToMiddle(float value) {
    lengthFromLowToMiddle = value;
}

void Surface::setLengthFromMiddleToEnd(float value) {
    lengthFromMiddleToEnd = value;
}

void Surface::drawCone(double r_low, double r_up, double h, int n, bool isSecond) {
        double   x1, x2, x3, x4,
            y1, y2, y3, y4,
            z1, z2, z3, z4,
            z, dz, fi, dfi, r1, r2, dr, sina, l, r;
        int      nh, i;
        C3dVector  v1, v2, v3, v4, vn, v21, v31;
        r1 = (r_low > r_up) ? r_low : r_up;
        dr = r_low - r_up;
        l = sqrt(pow(h, 2.0) + pow(dr, 2.0));
        sina = h / l;
        dfi = 2.0 * PI / n;
        nh = (int)(l / (r1 * dfi));
        dz = (l / nh) * sina;
        dr /= nh;
        h *= 0.5;
        glBegin(GL_QUADS);
        for (r = r_low, z = -h, i = 1; i <= nh; z += dz, r -= dr, i++)
        {
            r1 = r - dr;
            for (fi = -PI; fi < PI; fi += dfi)
            {
                x1 = r * cos(fi);      y1 = r * sin(fi);       z1 = z;
                x2 = r * cos(fi + dfi);  y2 = r * sin(fi + dfi);	  z2 = z;
                x3 = r1 * cos(fi + dfi); y3 = r1 * sin(fi + dfi);  z3 = z + dz;
                x4 = r1 * cos(fi);     y4 = r1 * sin(fi);      z4 = z + dz;
                if (isSecond)
                {
                    v1.x = x1; v1.y = y1; v1.z = z1 + lengthFromLowToMiddle * 0.5;
                    v2.x = x2; v2.y = y2; v2.z = z2 + lengthFromLowToMiddle * 0.5;
                    v3.x = x3; v3.y = y3; v3.z = z3 + lengthFromLowToMiddle * 0.5;
                    v4.x = x4; v4.y = y4; v4.z = z4 + lengthFromLowToMiddle * 0.5;
                }
                else
                {
                    v1.x = x1; v1.y = y1; v1.z = z1;
                    v2.x = x2; v2.y = y2; v2.z = z2;
                    v3.x = x3; v3.y = y3; v3.z = z3;
                    v4.x = x4; v4.y = y4; v4.z = z4;
                }
                points.push_back(v1);
                points.push_back(v2);
                points.push_back(v3);
                points.push_back(v4);
                glNormal3f(-x1, -y1, -dz);
                glVertex3f(x1, y1, z1);

                glNormal3f(-x2, -y2, -dz);
                glVertex3f(x2, y2, z2);

                glNormal3f(-x3, -y3, -dz);
                glVertex3f(x3, y3, z3);

                glNormal3f(-x4, -y4, -dz);
                glVertex3f(x4, y4, z4);
            }
        }
        glEnd();
}

void Surface::drawCircle(float r) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 100; i++) {
        float dx = 2.0f * PI * float(i) / 100.0f;
        float x = r * cosf(dx);
        float y = r * sinf(dx);
        glVertex3f(x, y, 0);
    }
    glEnd();
}

void Surface::drawObject() {
    for (int i = 0; i < 2; i++) {
        glPushMatrix();
        if (i == 1) {
            glTranslatef(0, 0, lengthFromLowToMiddle / 2);
            drawCone(hMiddle, hEnd, lengthFromMiddleToEnd, 20, true);
            glPushMatrix();
            glTranslatef(0, 0, lengthFromMiddleToEnd / 2);
            drawCircle(hEnd);
            glPopMatrix();
        }
        else {
            drawCone(hLow, hMiddle, lengthFromLowToMiddle, 20, false);
            glPushMatrix();
            glTranslatef(0, 0, -lengthFromLowToMiddle / 2);
            drawCircle(hLow);
            glPopMatrix();
        }
        glPopMatrix();
    }
}


BOOL SetPixelFormat(HDC dc)
{
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cAlphaBits = 24;
    pfd.cStencilBits = 0;
    pfd.cDepthBits = 24;
    pfd.cAccumBits = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(dc, &pfd);
    SetPixelFormat(dc, pf, &pfd);

    return !(pfd.dwFlags & PFD_NEED_PALETTE);
}  //SetPixelFormat

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLANETS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANETS));

    MSG msg;

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

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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

    g_hWindow = CreateWindowW(szWindowClass, szTitle, WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
        | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0, 1000, 1000, nullptr, nullptr, hInstance, nullptr);

    if (!g_hWindow)
    {
        return FALSE;
    }

    ShowWindow(g_hWindow, SW_SHOW);
    UpdateWindow(g_hWindow);

    return TRUE;
}

//
// установка материала и источника света
//
void SetMaterialAndLight0()
{
    float r = 1.0,
        g = 0.5,
        b = 0.5;

    float diffuse[] = { r, g, b, 1.f },
        specular[] = { 0.0f, 1.0f, 1.0f, 1.f };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

    GLfloat ambient[] = { 0.75, 0.75, 0.75, 1.0 };
    GLfloat lightPos[] = {1.0, -1.0, 0.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

void InitProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLsizei viewportSize = g_wndWidth;
    glViewport(0, 0, viewportSize, viewportSize);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glOrtho(-g_sceneWidth, g_sceneWidth, -g_sceneWidth, g_sceneWidth, -g_sceneWidth, g_sceneWidth);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void CALLBACK resize(int width, int height)
{
    g_wndWidth = width;
    g_wndHeight = height;
}

void onDraw() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    first.points.clear();
    second.points.clear();
    InitProjection();
    SetMaterialAndLight0();

    glPushMatrix();
    glRotatef(xAlfa, 1, 0, 0);
    glRotatef(zAlfa, 0, 0, 1);
        glPushMatrix();
            glTranslatef(first.pt_insX, first.pt_insY, first.pt_insZ);
            //glRotatef(45, 1, 0, 0);
            first.drawObject();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(second.pt_insX, second.pt_insY, second.pt_insZ);
            glRotatef(surfaceAngle, 1, 0, 0);
            second.drawObject();
            updatePoints();
        glPopMatrix();
        find();
    glPopMatrix();
    glFinish();
    SwapBuffers(hDC);
}

C3dVector MatrMultiply(double* matrix, C3dVector vektor) // если необходимы целочисленные значения, можно заменить на int
{
    C3dVector rez;
    double temp = 0.0;
    temp += matrix[0 * 3 + 0] * vektor.x;
    temp += matrix[0 * 3 + 1] * vektor.y;
    temp += matrix[0 * 3 + 2] * vektor.z;
    rez.x = temp;
    temp = 0.0;
    temp += matrix[1 * 3 + 0] * vektor.x;
    temp += matrix[1 * 3 + 1] * vektor.y;
    temp += matrix[1 * 3 + 2] * vektor.z;
    rez.y = temp;
    temp = 0.0;
    temp += matrix[2 * 3 + 0] * vektor.x;
    temp += matrix[2 * 3 + 1] * vektor.y;
    temp += matrix[2 * 3 + 2] * vektor.z;
    rez.z = temp;
    return rez;
}

void updatePoints() {
   
    int size = second.points.size();
    angle = -surfaceAngle * (PI / 180);
    double m[3][3] = {
        {1, 0, 0},
        {0, cos(angle), sin(angle)},
        {0, -sin(angle), cos(angle)}
    };
    for (int i = 0; i < size; i++) {
        second.points[i] = MatrMultiply(m[0], second.points[i]);
    }
   /* size = first.points.size();
    for (int i = 0; i < size; i++) {
        first.points[i].y = cos(45) * first.points[i].y + sin(45) * first.points[i].z;
        first.points[i].z = -sin(45) * first.points[i].y + cos(45) * first.points[i].z;
    }*/
}

RECT rect;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GetClientRect(hWnd, &rect);
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
        hDC = GetDC(hWnd);
        SetPixelFormat(hDC);
        hGLRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hGLRC);
        SetTimer(hWnd, 0, 40, 0);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), hWnd, ChangeParameters);
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
        BeginPaint(g_hWindow, &ps);
        onDraw();
        EndPaint(g_hWindow, &ps);
        return TRUE;
    }
    case WM_TIMER:
        if ((g_angle = g_angle + g_angIncr) >= 360)
            g_angle -= 360;
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
            g_sceneWidth++;
            InvalidateRect(hWnd, &rect, false);
        }
        if (wParam == VK_DOWN)
        {
            g_sceneWidth--;
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
        if (wParam == VK_BACK) {
            angle += 0.1;
            InvalidateRect(hWnd, &rect, false);
        }
        if (wParam == VK_ESCAPE) {
            angle -= 0.1;
            InvalidateRect(hWnd, &rect, false);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

float getInput(HWND hdlg, int  ctrlid)
{
    char buff[300];
    float rez = 0;
    GetDlgItemText(hdlg, ctrlid, buff, sizeof(buff) / sizeof(buff[0]));
    return atof(buff);
}

void drawSphere(C3dVector pt_ins) {
    glPushMatrix();
    glTranslatef(pt_ins.x, pt_ins.y, pt_ins.z);
    GLUquadricObj* glu = gluNewQuadric();
    gluSphere(glu, 0.5, 10, 10);
    glPopMatrix();
}

bool comp(C3dVector a, C3dVector b) {
    return a.z < b.z;
}

void find() {
    vector<C3dVector> points1 = first.points;
    vector<C3dVector> points2 = second.points;
    sort(points1.begin(), points1.end(), comp);
    sort(points2.begin(), points2.end(), comp);
    int length1 = points1.size();
    int length2 = points2.size();
    /*for (int i = 0; i < length2; i++) {
        drawSphere(points2[i]);
    }
    for (int i = 0; i < length1; i++) {
        drawSphere(points1[i]);
    }*/
    //worked
    double z = 0.0;
    bool flag = false;
    for (int i = 0; i < length1; i++) {
        z = points1[i].z;
        for (int j = 0; j < length2; j++) {
            if (flag && z - points2[j].z > 45)
                break;
            while (j < length2 && z - points2[j].z > 45)
                j++;
            flag = true;
            double rezx = fabs(points1[i].x - points2[j].x);
            double rezy = fabs(points1[i].y - points2[j].y);
            double rezz = fabs(points1[i].z - points2[j].z);
            if (rezx <= 0.45 && rezy <= 0.45 && rezz <= 0.45) {
               drawSphere(points2[j]);
               j = length2;
            }
        }
        flag = false;
    }
}

// Обработчик сообщений для окна "Изменить параметры".
INT_PTR CALLBACK ChangeParameters(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        char buff[20];
        sprintf_s(buff, "%f", first.getHLow());
        SetDlgItemText(hDlg, 6000, buff);
        sprintf_s(buff, "%f", first.getHMiddle());
        SetDlgItemText(hDlg, 6001, buff);
        sprintf_s(buff, "%f", first.getHEnd());
        SetDlgItemText(hDlg, 6002, buff);
        sprintf_s(buff, "%f", first.getLengthFromLowToMiddle());
        SetDlgItemText(hDlg, 6003, buff);
        sprintf_s(buff, "%f", first.getLengthFromMiddleToEnd());
        SetDlgItemText(hDlg, 6004, buff);
        sprintf_s(buff, "%f", second.getHLow());
        SetDlgItemText(hDlg, 6005, buff);
        sprintf_s(buff, "%f", second.getHMiddle());
        SetDlgItemText(hDlg, 6006, buff);
        sprintf_s(buff, "%f", second.getHEnd());
        SetDlgItemText(hDlg, 6007, buff);
        sprintf_s(buff, "%f", second.getLengthFromLowToMiddle());
        SetDlgItemText(hDlg, 6008, buff);
        sprintf_s(buff, "%f", second.getLengthFromMiddleToEnd());
        SetDlgItemText(hDlg, 6009, buff);
        sprintf_s(buff, "%f", surfaceAngle);
        SetDlgItemText(hDlg, 6010, buff);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_CLOSE || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
		if (LOWORD(wParam) == IDC_UPDATEPICTURE){
            first.setHLow(getInput(hDlg, 6000));
            first.setHMiddle(getInput(hDlg, 6001));
            first.setHEnd(getInput(hDlg, 6002));
            first.setLengthFromLowToMiddle(getInput(hDlg, 6003));
            first.setLengthFromMiddleToEnd(getInput(hDlg, 6004));
            second.setHLow(getInput(hDlg, 6005));
            second.setHMiddle(getInput(hDlg, 6006));
            second.setHEnd(getInput(hDlg, 6007));
            second.setLengthFromLowToMiddle(getInput(hDlg, 6008));
            second.setLengthFromMiddleToEnd(getInput(hDlg, 6009));
            surfaceAngle = getInput(hDlg, 6010);
            InvalidateRect(g_hWindow, &rect, false);
		}
        break;
    }
    return (INT_PTR)FALSE;
}
