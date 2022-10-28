
 //
 // ���������� ��� Windows - ��������� ����� �� ����������
 //

#include <windows.h>
#include <tchar.h>
#include "Form.h"

 // ��� ������ ����
LPCTSTR g_szWindowClassName = _T("TrussForm_WindowClass");

 // ��� emf-�����
TCHAR g_acEmfFileName [MAX_PATH] = { 0 };

 //
 // �������������� ��������� ����� �� ��������� ������� � �������
 //
POINT coor_w (double x_, double y_, const RECT& rcWindow_, const ModelViewBox& modelViewBox_)
{
	int a, b, xc_w, yc_w;
	double k_scale, xc, yc, dx, dy, k1, k2;

	/*--------------------------------*/
	/*  ���������  ����  ������       */
	/*--------------------------------*/

	a = abs(rcWindow_.right - rcWindow_.left);
	b = abs(rcWindow_.top - rcWindow_.bottom);

	xc_w = (rcWindow_.right + rcWindow_.left) / 2;
	yc_w = (rcWindow_.top + rcWindow_.bottom) / 2;

	/*-----------------------------------------*/
	/*  �����������  ����������� ������������  */
	/*-----------------------------------------*/
	dx = fabs(modelViewBox_.x_max - modelViewBox_.x_min);
	dy = fabs(modelViewBox_.y_max - modelViewBox_.y_min);

	xc = (modelViewBox_.x_max + modelViewBox_.x_min) / 2;
	yc = (modelViewBox_.y_max + modelViewBox_.y_min) / 2;

	k1 = a / dx;
	k2 = b / dy;
	k_scale = (k1 < k2) ? k1 : k2;

	/*-----------------------------------------*/
	/*    ��������������� ���������            */
	/*-----------------------------------------*/
	//POINT ptw = { (int)((x_ - xc) * k_scale + xc_w), (int)(-(y_ - yc) * k_scale + yc_w) };
	POINT ptw = { (int)((x_ - xc) + xc_w), (int)(-(y_ - yc) + yc_w) };
	return ptw;
}

 //
 // ��������� �����
 //
void DrawLine (HDC hdc_, const POINT& pt0_, const POINT& pt1_)
{
	MoveToEx (hdc_, pt0_.x, pt0_.y, nullptr);
	LineTo (hdc_, pt1_.x, pt1_.y);
}

 //
 // ��������� �����
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
	double x0, y0, H, L;
	int n, lw;
	COLORREF color;

	if (   ! GetEditValue (0, x0)
		|| ! GetEditValue (1, y0)
		|| ! GetEditValue (2, L)
		//|| ! GetEditValue (3, H)
		|| ! GetEditValue (3, n)
		|| ! GetEditHexValue (4, (int&)color)
		|| ! GetEditValue (5, lw))
	{
		int nOldBkMode = SetBkMode (hdc_, TRANSPARENT);
		COLORREF nOldTextColor = SetTextColor (hdc_, RGB (192, 0, 0));
		UINT nOldAlign = SetTextAlign (hdc_, TA_CENTER | TA_TOP);
		HGDIOBJ hOldFont = SelectObject (hdc_, GetFormFont());

		LPCTSTR szText = _T("��������� ������ �������");
		TextOut (hdc_, (rect_.left + rect_.right) / 2, (rect_.top + rect_.bottom) / 2, szText, _tcslen (szText));

		SetBkMode (hdc_, nOldBkMode);
		SetTextColor (hdc_, nOldTextColor);
		SetTextAlign (hdc_, nOldAlign);
		SelectObject (hdc_, hOldFont);

		return;
	}

	HPEN hPen = CreatePen (PS_SOLID, lw, color);
	HGDIOBJ hOldPen = SelectObject (hdc_, hPen);

	 // �������� ���������
	H = L / 2;
	ModelViewBox MinMax;
	SetModelViewBox (MinMax, x0, y0, x0 + H * 2, y0 + H);

	double dx = (H * 2) / 1000;

	RECT rect = rect_;
	InflateRect (&rect, -lw/2, -lw/2);
	double dy = 0.0;
	double prev_dy = 0.0;
	for (int i = 0; i < 1000; i++)
	{
		dy = sqrt(1 - (abs((H - (i + 1) * dx)) / H) * (abs((H - (i + 1) * dx)) / H)) * H;
		prev_dy = sqrt(1 - (abs((H - i * dx)) / H) * (abs((H - i * dx)) / H)) * H;
		POINT pt0 = coor_w (x0 + i * dx, y0, rect, MinMax),
			  pt1 = coor_w (x0 + (i + 1) * dx, y0, rect, MinMax),
			  pt2 = coor_w (x0 + i * dx, y0 + prev_dy, rect, MinMax),
			pt3 = coor_w(x0 + (i + 1) * dx, y0 + dy,rect, MinMax);
		DrawLine (hdc_, pt0, pt1);
		DrawLine(hdc_, pt2, pt3);
		//DrawLine (hdc_, pt1, pt3);
		
	}
	dx = (H * 2) / n;
	for (int i = 0; i < n; i++)
	{
		dy = sqrt(1 - (abs((H - (i + 1) * dx)) / H) * (abs((H - (i + 1) * dx)) / H)) * H;
		prev_dy = sqrt(1 - (abs((H - i * dx)) / H) * (abs((H - i * dx)) / H)) * H;
		POINT pt0 = coor_w(x0 + i * dx, y0, rect, MinMax),
			pt1 = coor_w(x0 + (i + 1) * dx, y0, rect, MinMax),
			pt2 = coor_w(x0 + i * dx, y0 + prev_dy, rect, MinMax),
			pt3 = coor_w(x0 + (i + 1) * dx, y0 + dy, rect, MinMax);
		//DrawLine(hdc_, pt0, pt1);
		//DrawLine(hdc_, pt2, pt3);
		DrawLine (hdc_, pt1, pt3);

	}
	SelectObject (hdc_, hOldPen);
	DeleteObject (hPen);
}

 //
 // ����� ����� emf-����� ��� �������� �����������
 //
void SelectEmfFileName (HINSTANCE hInst_, HWND hWnd_, TCHAR acFileName_ [/*MAX_PATH*/])
{
	LPCTSTR szFilter = _T("��������� Windows wmf (*.wmf)\0*.wmf\0\0");
	OPENFILENAME ofn =
	{
		sizeof(OPENFILENAME),
		hWnd_,
		hInst_,
		szFilter,
		nullptr, 0, 0,
		acFileName_, MAX_PATH,
		nullptr, 0, nullptr,
		_T("������� �������� Windows"),
		OFN_EXPLORER | OFN_PATHMUSTEXIST,
		0, 0, _T("*"), 0L, nullptr, nullptr
	};
	GetSaveFileName(&ofn);
}

 //
 // ������ ����������� ����� � ��������
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
 // ������� �������
 //
LRESULT CALLBACK WindowProc (HWND hWnd_, UINT nMsg_, WPARAM wParam_, LPARAM lParam_)
{
	switch (nMsg_)
	{
		case WM_CREATE:
			 //
			 // ���������������� ����� - ������ ��� ����� �������� � ���� �������������� ����������
			 //
			FormOn_WM_CREATE (hWnd_, wParam_, lParam_, _T("schema.bmp"));
			AddFormEditBox (0, _T("����� ������� x0: "), CEditBox::vtReal, 10, _T("0"));
			AddFormEditBox (1, _T("����� ������� y0: "), CEditBox::vtReal, 10, _T("0"));
			AddFormEditBox (2, _T("������� D: "), CEditBox::vtReal, 10, _T("200"));
			AddFormEditBox (3, _T("���������� ������� n: "), CEditBox::vtInt, 3, _T("8"));
			AddFormEditBox (4, _T("���� (hex): "), CEditBox::vtHexInt, 6, _T("007F00"));
			AddFormEditBox (5, _T("������� �����: "), CEditBox::vtInt, 2, _T("2"));
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
 // �������-����� ����� ������ Windows-����������
 //
int WINAPI _tWinMain (HINSTANCE hApp_, HINSTANCE hPrevApp_, LPTSTR szCmdLine_, int nCmdShow_)
{
	MSG msg;

	 // ������������� ��������� �������� ������ ����
	WNDCLASSEX wc;
	ZeroMemory (& wc, sizeof (WNDCLASSEX));
	wc.cbSize = sizeof (WNDCLASSEX);
	wc.hInstance = hApp_;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = & WindowProc;
	wc.hCursor = LoadCursor (nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast <HBRUSH> (GetStockObject (LTGRAY_BRUSH));
	wc.lpszClassName = g_szWindowClassName;

	 // ����������� ������ ����
	if (! RegisterClassEx (&wc))
		return 0;

	 // ����������� �������� ������ � ��������
	int scrw = GetSystemMetrics (SM_CXSCREEN),
		scrh = GetSystemMetrics (SM_CYSCREEN);

	 // �������� ����
	HWND hWnd = CreateWindow (g_szWindowClassName, _T("��������� �����"), WS_OVERLAPPEDWINDOW,
							  scrw / 2 - 400, scrh / 2 - 300, 800, 600,
							  nullptr, nullptr, hApp_, nullptr);

	if (nullptr == hWnd)
		return 0;

	ShowWindow (hWnd, SW_SHOW);
	UpdateWindow (hWnd);

	 // ����� ����� emf-����� ��� �������� �����������
	SelectEmfFileName (hApp_, hWnd, g_acEmfFileName);

	 // ��������� ���������
	while (GetMessage (&msg, nullptr, 0, 0) != 0)
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	UnregisterClass (g_szWindowClassName, hApp_);

	return msg.wParam;
}  //WinMain