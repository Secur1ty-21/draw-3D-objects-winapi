#pragma warning(disable : 4996)
//
// Приложение для Windows - демонстрация анимации
//
#define _USE_MATH_DEFINES
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include "targetver.h"
#include "Resource.h"
#include <math.h>

LPCTSTR g_szWindowClassName = _T("TrussForm_WindowClass");
HDC g_clone;
HWND hwnd_;
double height = GetSystemMetrics(SM_CYSCREEN),
width = GetSystemMetrics(SM_CXSCREEN);
//
//Timer g_param
//
double g_alpha = 1 * M_PI / 180.0;
double g_move_x = 0.0;
double g_move_y = 0.0;
double g_zoom = 1.0;
double g_znak = 1;
double g_znak_x = 1;
double g_znak_y = 1;
//
// Res g_param
//
HINSTANCE hInst;
int rectangle = 0;
COLORREF g_color = RGB(0, 0, 0);
COLORREF g_color2 = RGB(255, 255, 255);
int g_width = 2;
HPEN g_hPen = CreatePen(PS_SOLID, g_width, g_color);
HBRUSH g_hBrush = CreateSolidBrush(g_color2);
//
// Anin g_param
//
struct CPoint
{
	double x, y;
};
struct CAffineMatrix
{
	double m[3][3];
};
//
// обнуление матрицы
//
void SetZero(CAffineMatrix& m)
{
	ZeroMemory(&m, sizeof(m));
}
//
// умножение матриц
//
CAffineMatrix operator * (const CAffineMatrix& m1, const CAffineMatrix& m2)
{
	CAffineMatrix mRes;
	SetZero(mRes);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				mRes.m[i][j] += m1.m[i][k] * m2.m[k][j];

	return mRes;
}
//
// умножение точки на матрицу (преобразование)
//
CPoint operator * (const CPoint& p, const CAffineMatrix& m)
{
	CPoint pRes;
	pRes.x = p.x * m.m[0][0] + p.y * m.m[1][0] + m.m[2][0];
	pRes.y = p.x * m.m[0][1] + p.y * m.m[1][1] + m.m[2][1];

	return pRes;
}
// 
//Обработчик сообщений для окна "О программе".
//
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
//
//	Paint
//
void DrawTruss(WPARAM wParam_, LPARAM lParam_){
	PAINTSTRUCT ps;
	HDC hdc_ = BeginPaint(hwnd_, &ps);
	RECT rect_;
	GetClientRect(hwnd_, &rect_);
	COLORREF color = RGB(0, 0, 0);
	HPEN hPen = CreatePen(PS_SOLID, 4, color);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	HGDIOBJ hOldPen = SelectObject(hdc_, hPen);
	HFONT hFont = CreateFont(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	CAffineMatrix rotation, move, zoom;
	SetZero(rotation);
	SetZero(move);
	SetZero(zoom);
	rotation.m[0][0] = rotation.m[1][1] = cos(g_alpha);
	rotation.m[2][2] = 1.0;
	rotation.m[0][1] = sin(g_alpha);
	rotation.m[1][0] = -sin(g_alpha);
	move.m[0][0] = move.m[1][1] = move.m[2][2] = 1.0;
	move.m[2][0] = g_move_x;
	move.m[2][1] = g_move_y;
	zoom.m[0][0] = zoom.m[1][1] = g_zoom;
	zoom.m[2][2] = 1.0;
	SelectObject(hdc_, hFont);
	CPoint ferm[17] = {
		{width / 2.5, height / 1.2},
		{width - width / 2.5, height / 1.2},
		{width * 0.4666667, height / 2},
		{width * 0.53, height / 2},
		//
		{width / 2.5 + (width - width / 2.5 - width / 2.5) / 2.5, height / 1.2},
		{width / 2.5 + (width - width / 2.5 - width / 2.5) / 2.5, height / 1.4},
		{width - width / 2.5 - (width - width / 2.5 - width / 2.5) / 2.5, height / 1.4},
		{width - width / 2.5 - (width - width / 2.5 - width / 2.5) / 2.5, height / 1.2},
		//
		{width * 0.4866667,height / 1.8},
		//
		{width * 0.4966667,height / 2.8},
		{width * 0.4766667, height / 1.3},
		{width * 0.35834, height / 1.85},
		{width * 0.61486, height / 1.75},
		//
		{width * 0.4766667, height / 2.8},
		{width * 0.4966667, height / 1.3},
		{width * 0.35834, height / 1.75},
		{width * 0.61486, height / 1.85}
	};
	CPoint center_lop = ferm[8];
	for (int i = 0; i < 17; i++) {
		if (i >= 9) {
			ferm[i].x = ferm[i].x - center_lop.x;
			ferm[i].y = ferm[i].y - center_lop.y;
			ferm[i] = ferm[i] * rotation;
			ferm[i].x = center_lop.x + ferm[i].x;
			ferm[i].y = center_lop.y + ferm[i].y;
		}
		ferm[i] = ferm[i] * move * zoom;
	}
	for (int i = 0; i < 13; i++)
	{
		if (ferm[i].x > width || ferm[i].x < 0)
		{
			if (ferm[i].x > width)
				g_znak_x = -1;
			if (ferm[i].x < 0)
				g_znak_x = 1;
			g_move_x += 4 * g_znak_x;
			break;
		}
		if (ferm[i].y > height || ferm[i].y < 0)
		{
			if (ferm[i].y > height)
				g_znak_y = -1;
			if (ferm[i].y < 0)
				g_znak_y = 1;
			g_move_y += 4 * g_znak_y;
			break;
		}
	}
	MoveToEx(hdc_, ferm[0].x, ferm[0].y, nullptr);
	LineTo(hdc_, ferm[1].x, ferm[1].y);
	LineTo(hdc_, ferm[3].x, ferm[3].y);
	LineTo(hdc_, ferm[2].x, ferm[2].y);
	LineTo(hdc_, ferm[0].x, ferm[0].y);
	MoveToEx(hdc_, ferm[8].x, ferm[8].y, nullptr);
	LineTo(hdc_, ferm[9].x, ferm[9].y);
	LineTo(hdc_, ferm[13].x, ferm[13].y);
	LineTo(hdc_, ferm[8].x, ferm[8].y);
	MoveToEx(hdc_, ferm[8].x, ferm[8].y, nullptr);
	LineTo(hdc_, ferm[10].x, ferm[10].y);
	LineTo(hdc_, ferm[14].x, ferm[14].y);
	LineTo(hdc_, ferm[8].x, ferm[8].y);
	MoveToEx(hdc_, ferm[8].x, ferm[8].y, nullptr);
	LineTo(hdc_, ferm[11].x, ferm[11].y);
	LineTo(hdc_, ferm[15].x, ferm[15].y);
	LineTo(hdc_, ferm[8].x, ferm[8].y);
	MoveToEx(hdc_, ferm[8].x, ferm[8].y, nullptr);
	LineTo(hdc_, ferm[12].x, ferm[12].y);
	LineTo(hdc_, ferm[16].x, ferm[16].y);
	LineTo(hdc_, ferm[8].x, ferm[8].y);
	POINT door[4] = {
			{ferm[4].x, ferm[4].y},
			{ferm[5].x, ferm[5].y},
			{ferm[6].x, ferm[6].y},
			{ferm[7].x, ferm[7].y}
	};
	SetBkMode(hdc_, TRANSPARENT);
	SetBkMode(hdc_, OPAQUE);
	g_hPen = CreatePen(PS_SOLID, g_width, g_color);
	g_hBrush = CreateSolidBrush(g_color2);
	SetBkColor(hdc_, g_color2);
	SelectObject(hdc_, g_hPen);
	SelectObject(hdc_, g_hBrush);
	Polygon(hdc_, door, 4);
	SelectObject(hdc_, hPen);
	SelectObject(hdc_, hBrush);
	SetBkColor(hdc_, RGB(255, 255, 255));
	hdc_ = g_clone;
	SelectObject(hdc_, hOldPen);
	DeleteObject(hPen);
	DeleteDC(hdc_);
	EndPaint(hwnd_, &ps);
}
//
// оконная функция
//
RECT rect;
LRESULT CALLBACK WindowProc(HWND hWnd_, UINT nMsg_, WPARAM wParam_, LPARAM lParam_){
	GetClientRect(hWnd_, &rect);
	switch (nMsg_)
	{
	case WM_CREATE:
		hwnd_ = hWnd_;
		return 0L;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam_);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd_, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd_);
			break;
			// Выбор прямоугольника
		case IDM_1:
			if (rectangle == 1)
			{
				g_width = 2;
				rectangle = 0;
			}
			else
			{
				rectangle = 1;
				g_width = 4;
			}
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
		case IDM_2:
			if (rectangle == 2)
			{
				g_width = 2;
				rectangle = 0;
			}
			else
			{
				rectangle = 2;
				g_width = 4;
			}
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
		case IDM_3:
			if (rectangle == 3)
			{
				g_width = 2;
				rectangle = 0;
			}
			else
			{
				rectangle = 3;
				g_width = 4;
			}
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
		case IDM_4:
			if (rectangle == 4)
			{
				g_width = 2;
				rectangle = 0;
			}
			else
			{
				rectangle = 4;
				g_width = 4;
			}
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
			//Выбор прямоугольника
			//Цвет контура
		case IDM_C_GREEN:
			if (g_color == RGB(0, 255, 0))
				g_color = RGB(0, 0, 0);
			else
				g_color = RGB(0, 255, 0);
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
		case IDM_C_RED:
			if (g_color == RGB(255, 0, 0))
				g_color = RGB(0, 0, 0);
			else
				g_color = RGB(255, 0, 0);
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
		case IDM_C_BLUE:
			if (g_color == RGB(0, 0, 255))
				g_color = RGB(0, 0, 0);
			else
				g_color = RGB(0, 0, 255);
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
		case IDM_C_YELLOW:
			if (g_color == RGB(255, 255, 0))
				g_color = RGB(0, 0, 0);
			else
				g_color = RGB(255, 255, 0);
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
			//Цвет контура
			//Заливка
		case IDM_Z_GREEN:
			if (g_color2 == RGB(0, 255, 0))
				g_color2 = RGB(0, 0, 0);
			else
				g_color2 = RGB(0, 255, 0);
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
		case IDM_Z_RED:
			if (g_color2 == RGB(255, 0, 0))
				g_color2 = RGB(0, 0, 0);
			else
				g_color2 = RGB(255, 0, 0);
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
		case IDM_Z_BLUE:
			if (g_color2 == RGB(0, 0, 255))
				g_color2 = RGB(0, 0, 0);
			else
				g_color2 = RGB(0, 0, 255);
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
		case IDM_Z_YELLOW:
			if (g_color2 == RGB(255, 255, 0))
				g_color2 = RGB(0, 0, 0);
			else
				g_color2 = RGB(255, 255, 0);
			InvalidateRect(hWnd_, &rect, TRUE);
			break;
			//Заливка
		default:
			return DefWindowProc(hWnd_, nMsg_, wParam_, lParam_);
		}
	}
	break;
	case WM_LBUTTONDOWN:
		InvalidateRect(hWnd_, &rect, TRUE);
		return 0L;
	case WM_PAINT:
		DrawTruss(wParam_, lParam_);
		return 0L;
	case WM_SYSCHAR:
		if ((wchar_t)wParam_ == 'x')
			PostQuitMessage(0);
	case WM_SIZE:
		width = LOWORD(lParam_);
		height = HIWORD(lParam_);
		InvalidateRect(hWnd_, &rect, TRUE);
		return 0L;
	case WM_TIMER:
		g_alpha += 2 * M_PI / 180.0;
		g_move_x += 2 * g_znak_x;
		g_move_y += 2 * g_znak_y;
		g_zoom += 0.01 * g_znak;
		if (g_zoom >= 1.5)
			g_znak = -1;
		if (g_zoom <= 0.5)
			g_znak = 1;
		InvalidateRect(hWnd_, &rect, TRUE);
		return 0L;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0L;
	default:
		return DefWindowProc(hWnd_, nMsg_, wParam_, lParam_);
	}
	return 0L;
}  //WindowProc
 //
 // Функция-точка входа любого Windows-приложения
 //
int WINAPI _tWinMain(HINSTANCE hApp_, HINSTANCE hPrevApp_, LPTSTR szCmdLine_, int nCmdShow_){
	MSG msg;
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hApp_;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.hIcon = LoadIcon(hApp_, MAKEINTRESOURCE(IDI_WINDOWSPROJECT5));
	wc.lpfnWndProc = &WindowProc;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast <HBRUSH> (GetStockObject(LTGRAY_BRUSH));
	wc.lpszClassName = g_szWindowClassName;
	wc.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT5);
	wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!RegisterClassEx(&wc))
		return 0;
	int scrw = GetSystemMetrics(SM_CXSCREEN),
		scrh = GetSystemMetrics(SM_CYSCREEN);
	HWND hWnd = CreateWindow(g_szWindowClassName, _T("Animation of the mill"), WS_OVERLAPPEDWINDOW,
		0, 0, scrw, scrh,
		nullptr, nullptr, hApp_, nullptr);
	UINT uResult = SetTimer(hWnd,
		1,            // timer identifier 
		50,                    // 20-mili second interval 
		(TIMERPROC)NULL);
	if (nullptr == hWnd)
		return 0;
	HACCEL hAccelTable = LoadAccelerators(hApp_, MAKEINTRESOURCE(IDC_WINDOWSPROJECT5));
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	hInst = hApp_;
	while (GetMessage(&msg, nullptr, 0, 0) != 0)
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	UnregisterClass(g_szWindowClassName, hApp_);
	return msg.wParam;
} //WinMain