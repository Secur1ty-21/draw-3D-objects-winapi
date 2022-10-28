
 //
 // Приложение для Windows - демонстрация перегрузки
 //

#include <windows.h>
#include <tchar.h>
#include "Form.h"

 // имя класса окна
LPCTSTR g_szWindowClassName = _T("TrussForm_WindowClass");

 // имя emf-файла
TCHAR g_acEmfFileName [MAX_PATH] = { 0 };
// for overload
HDC clone;
void DrawLine(POINT, POINT, COLORREF);
void DrawLine(POINT, POINT, int);
void DrawLine(POINT, POINT, char*);
//

 //
 // преобразование координат точки из модельной системы в оконную
 //
POINT coor_w (double x_, double y_, const RECT& rcWindow_, const ModelViewBox& modelViewBox_)
{
	int a, b, xc_w, yc_w;
	double k_scale, xc, yc, dx, dy, k1, k2;

	/*--------------------------------*/
	/*  Параметры  окна  вывода       */
	/*--------------------------------*/

	a = abs(rcWindow_.right - rcWindow_.left);
	b = abs(rcWindow_.top - rcWindow_.bottom);

	xc_w = (rcWindow_.right + rcWindow_.left) / 2;
	yc_w = (rcWindow_.top + rcWindow_.bottom) / 2;

	/*-----------------------------------------*/
	/*  Определение  масштабного коэффициента  */
	/*-----------------------------------------*/
	dx = fabs(modelViewBox_.x_max - modelViewBox_.x_min);
	dy = fabs(modelViewBox_.y_max - modelViewBox_.y_min);

	xc = (modelViewBox_.x_max + modelViewBox_.x_min) / 2;
	yc = (modelViewBox_.y_max + modelViewBox_.y_min) / 2;

	k1 = a / dx;
	k2 = b / dy;
	k_scale = (k1 < k2) ? k1 : k2;

	/*-----------------------------------------*/
	/*    Масштабирование координат            */
	/*-----------------------------------------*/
	POINT ptw = { (int)((x_ - xc) * k_scale + xc_w), (int)(-(y_ - yc) * k_scale + yc_w) };
	return ptw;
}

 //
 // рисование линии
 //
void DrawLine (POINT pt1, POINT pt2, COLORREF color)
{
	HPEN hpen = CreatePen(BS_SOLID, 1, color);
	SelectObject(clone, hpen);
	MoveToEx (clone, pt1.x, pt1.y, nullptr);
	LineTo (clone, pt2.x, pt2.y);
	DeleteObject(hpen);
}
void DrawLine(POINT pt1, POINT pt2, int width)
{
	HPEN hpen = CreatePen(BS_SOLID, width, RGB(255, 0, 0));
	SelectObject(clone, hpen);
	MoveToEx(clone, pt1.x, pt1.y, nullptr);
	LineTo(clone, pt2.x, pt2.y);
	DeleteObject(hpen);
}
void DrawLine(POINT pt1, POINT pt2, char *s)
{
	HPEN hpen = CreatePen(BS_SOLID, 1, RGB(0, 255, 0));
	int i = 0;
	while (s[i] != '\0')
		i++;
	wchar_t* wString = new wchar_t[i];
	MultiByteToWideChar(CP_ACP, 0, s, -1, wString, i);
	SetBkMode(clone, TRANSPARENT);
	//
	SelectObject(clone, hpen);
	MoveToEx(clone, pt1.x, pt1.y, nullptr);
	LineTo(clone, pt2.x, pt2.y);
	TextOut(clone, pt2.x, pt2.y, wString, i);
	DeleteObject(hpen);
}
 //
 // Рисование 3-х линий
 //
void SetModelViewBox (ModelViewBox& MinMax_, double x_min_, double y_min_, double x_max_, double y_max_)
{
	MinMax_.x_min = x_min_;
	MinMax_.y_min = y_min_;
	MinMax_.x_max = x_max_;
	MinMax_.y_max = y_max_;
}

void DrawTruss (HDC hdc_, const RECT& rect_)
{
	double l1x0, l1y0, l1x1, l1y1, l2x0, l2y0, l2x1, l2y1, l3x0, l3y0, l3x1, l3y1;
	int n, lw;
	COLORREF color;
	char* s;
	s = (char*)"it is my text";
	if (   ! GetEditValue (0, l1x0)
		|| ! GetEditValue (1, l1y0)
		|| ! GetEditValue (2, l1x1)
		|| ! GetEditValue (3, l1y1)
		|| ! GetEditValue (4, l2x0)
		|| ! GetEditValue (5, l2y0)
		|| ! GetEditValue (6, l2x1)
		|| ! GetEditValue (7, l2y1)
		|| ! GetEditValue (8, l3x0)
		|| ! GetEditValue (9, l3y0)
		|| ! GetEditValue (10, l3x1)
		|| ! GetEditValue (11, l3y1)
		//|| ! GetEditValue (12, s)
		|| ! GetEditHexValue (12, (int&)color)
		|| ! GetEditValue (13, lw))
	{
		int nOldBkMode = SetBkMode (hdc_, TRANSPARENT);
		COLORREF nOldTextColor = SetTextColor (hdc_, RGB (192, 0, 0));
		UINT nOldAlign = SetTextAlign (hdc_, TA_CENTER | TA_TOP);
		HGDIOBJ hOldFont = SelectObject (hdc_, GetFormFont());

		LPCTSTR szText = _T("Параметры заданы неверно");
		TextOut (hdc_, (rect_.left + rect_.right) / 2, (rect_.top + rect_.bottom) / 2, szText, _tcslen (szText));

		SetBkMode (hdc_, nOldBkMode);
		SetTextColor (hdc_, nOldTextColor);
		SetTextAlign (hdc_, nOldAlign);
		SelectObject (hdc_, hOldFont);

		return;
	}

	HPEN hPen = CreatePen (PS_SOLID, lw, color);
	HGDIOBJ hOldPen = SelectObject (hdc_, hPen);

	 // габариты отсечения
	ModelViewBox MinMax;
	SetModelViewBox (MinMax, 0, 0, 0, 0);


	RECT rect = rect_;
	InflateRect (&rect, -lw/2, -lw/2);
	POINT pt1, pt2;
	pt1.x = l1x0;
	pt1.y = l1y0;
	pt2.x = l1x1;
	pt2.y = l1y1;
	clone = hdc_;
	DrawLine(pt1, pt2, s);
	pt1.x = l2x0;
	pt1.y = l2y0;
	pt2.x = l2x1;
	pt2.y = l2y1;
	DrawLine(pt1, pt2, color);
	pt1.x = l3x0;
	pt1.y = l3y0;
	pt2.x = l3x1;
	pt2.y = l3y1;
	DrawLine(pt1, pt2, lw);
	hdc_ = clone;
	SelectObject (hdc_, hOldPen);
	DeleteObject (hPen);
}

 //
 // выбор имени emf-файла для экспорта изображения
 //
void SelectEmfFileName (HINSTANCE hInst_, HWND hWnd_, TCHAR acFileName_ [/*MAX_PATH*/])
{
	LPCTSTR szFilter = _T("Метафайлы Windows wmf (*.wmf)\0*.wmf\0\0");
	OPENFILENAME ofn =
	{
		sizeof(OPENFILENAME),
		hWnd_,
		hInst_,
		szFilter,
		nullptr, 0, 0,
		acFileName_, MAX_PATH,
		nullptr, 0, nullptr,
		_T("Создать метафайл Windows"),
		OFN_EXPLORER | OFN_PATHMUSTEXIST,
		0, 0, _T("*"), 0L, nullptr, nullptr
	};
	GetSaveFileName(&ofn);
}

 //
 // запись изображения фермы в метафайл
 //
void CreateEmfFile()
{
	HDC hdcEmf = CreateEnhMetaFile (nullptr, g_acEmfFileName, nullptr, nullptr);
	RECT rect = { 0, 0, 1000, 1000 };

	DrawTruss (hdcEmf, rect);

	HENHMETAFILE hEmf = CloseEnhMetaFile (hdcEmf);
	DeleteEnhMetaFile (hEmf);
}

 //
 // оконная функция
 //
LRESULT CALLBACK WindowProc (HWND hWnd_, UINT nMsg_, WPARAM wParam_, LPARAM lParam_)
{
	switch (nMsg_)
	{
		case WM_CREATE:
			 //
			 // инициализировать форму - задать имя файла картинки и поля редактирования параметров
			 //
			FormOn_WM_CREATE (hWnd_, wParam_, lParam_, _T("schema.bmp"));
			AddFormEditBox (0, _T("Линия1 x0: "), CEditBox::vtReal, 10, _T("100"));
			AddFormEditBox (1, _T("Линия1 y0: "), CEditBox::vtReal, 10, _T("300"));
			AddFormEditBox (2, _T("Линия1 x1: "), CEditBox::vtReal, 10, _T("200"));
			AddFormEditBox (3, _T("Линия1 y1: "), CEditBox::vtReal, 10, _T("300"));
			AddFormEditBox (4, _T("Линия2 x0: "), CEditBox::vtReal, 10, _T("100"));
			AddFormEditBox (5, _T("Линия2 y0: "), CEditBox::vtReal, 10, _T("400"));
			AddFormEditBox (6, _T("Линия2 x1: "), CEditBox::vtReal, 10, _T("200"));
			AddFormEditBox (7, _T("Линия2 y1: "), CEditBox::vtReal, 10, _T("400"));
			AddFormEditBox (8, _T("Линия3 x0: "), CEditBox::vtReal, 10, _T("100"));
			AddFormEditBox (9, _T("Линия3 y0: "), CEditBox::vtReal, 10, _T("500"));
			AddFormEditBox (10, _T("Линия3 x1: "), CEditBox::vtReal, 10, _T("200"));
			AddFormEditBox (11, _T("Линия3 y1: "), CEditBox::vtReal, 10, _T("500"));
			//AddFormEditBox (12, _T("Текст: "), CEditBox::vtText, 10, _T("Some_text"));
			AddFormEditBox (12, _T("Цвет (hex): "), CEditBox::vtHexInt, 6, _T("000000"));
			AddFormEditBox (13, _T("Толщина линии у красной линии: "), CEditBox::vtInt, 2, _T("2"));
		return 0L;

		case WM_LBUTTONDOWN:
			FormOn_WM_LBUTTONDOWN (wParam_, lParam_);
		return 0L;

		case WM_KEYDOWN:
			FormOn_WM_KEYDOWN (wParam_, lParam_);
		return 0L;

		case WM_CHAR:
			FormOn_WM_CHAR (wParam_, lParam_);
		return 0L;

		case WM_PAINT:
			FormOn_WM_PAINT (wParam_, lParam_);
		return 0L;

		case WM_TIMER:
			FormOn_WM_TIMER (wParam_, lParam_);
		return 0L;

		case WM_GETMINMAXINFO:
			FormOn_WM_GETMINMAXINFO (wParam_, lParam_);
		return 0L;

		case WM_DESTROY:
			PostQuitMessage (0);
		return 0L;

		default:
			return DefWindowProc (hWnd_, nMsg_, wParam_, lParam_);
	}

	return 0L;
}  //WindowProc

 //
 // Функция-точка входа любого Windows-приложения
 //
int WINAPI _tWinMain (HINSTANCE hApp_, HINSTANCE hPrevApp_, LPTSTR szCmdLine_, int nCmdShow_)
{
	MSG msg;

	 // инициализация структуры описания класса окон
	WNDCLASSEX wc;
	ZeroMemory (& wc, sizeof (WNDCLASSEX));
	wc.cbSize = sizeof (WNDCLASSEX);
	wc.hInstance = hApp_;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = & WindowProc;
	wc.hCursor = LoadCursor (nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast <HBRUSH> (GetStockObject (LTGRAY_BRUSH));
	wc.lpszClassName = g_szWindowClassName;

	 // регистрация класса окна
	if (! RegisterClassEx (&wc))
		return 0;

	 // определение размеров экрана в пикселях
	int scrw = GetSystemMetrics (SM_CXSCREEN),
		scrh = GetSystemMetrics (SM_CYSCREEN);

	 // создание окна
	HWND hWnd = CreateWindow (g_szWindowClassName, _T("Overload"), WS_OVERLAPPEDWINDOW,
							  scrw / 2 - 400, scrh / 2 - 300, 800, 600,
							  nullptr, nullptr, hApp_, nullptr);

	if (nullptr == hWnd)
		return 0;

	ShowWindow (hWnd, SW_SHOW);
	UpdateWindow (hWnd);

	 // выбор имени emf-файла для экспорта изображения
	//SelectEmfFileName (hApp_, hWnd, g_acEmfFileName);

	 // обработка сообщений
	while (GetMessage (&msg, nullptr, 0, 0) != 0)
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	UnregisterClass (g_szWindowClassName, hApp_);

	return msg.wParam;
}  //WinMain