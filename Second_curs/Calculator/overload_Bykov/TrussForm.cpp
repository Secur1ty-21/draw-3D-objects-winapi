#pragma warning(disable : 4996)
//
// Приложение для Windows - демонстрация перегрузки
//

#include <windows.h>
#include <tchar.h>
#include "Form.h"
#include <stdlib.h>
#include <stdio.h>
#include "Resource.h"
 // имя класса окна
LPCTSTR g_szWindowClassName = _T("TrussForm_WindowClass");

// имя emf-файла
TCHAR g_acEmfFileName[MAX_PATH] = { 0 };
// for calculator
HDC clone;
double height = GetSystemMetrics(SM_CYSCREEN),
width = GetSystemMetrics(SM_CXSCREEN);
int first_num = 0,
second_num = 0;
int num_of_digital = 0;
char f1[10];
char f2[10];
char operation = ' ';
char* answer_text = (char*)malloc(sizeof(char*) * 20);
wchar_t text[42];
short pos_znak = 0;
int len = 0;
POINT click = { 0, 0 };
POINT move = { 0, 0 };
bool flag = true;
// for calculator
// for res
HINSTANCE hInst;
int rectangle = 0;
COLORREF g_color = RGB(0,0,0);
COLORREF g_color2 = RGB(255,255,255);
int g_width = 2;
HPEN g_hPen = CreatePen(PS_SOLID, g_width, g_color);
HBRUSH g_hBrush = CreateSolidBrush(g_color2);
HCURSOR hCurs1 = LoadCursor(NULL, IDC_SIZENS), hCurs2 = LoadCursor(NULL, IDC_IBEAM),
hCurs3 = LoadCursor(NULL, IDC_UPARROW), hCurs4 = LoadCursor(NULL, IDC_CROSS), hCurs5 = LoadCursor(NULL, IDC_ARROW);

// for res


//Обработчик сообщений для окна "О программе".
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
 // преобразование координат точки из модельной системы в оконную
 //
POINT coor_w(double x_, double y_, const RECT& rcWindow_, const ModelViewBox& modelViewBox_)
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

void SetModelViewBox(ModelViewBox& MinMax_, double x_min_, double y_min_, double x_max_, double y_max_)
{
	MinMax_.x_min = x_min_;
	MinMax_.y_min = y_min_;
	MinMax_.x_max = x_max_;
	MinMax_.y_max = y_max_;
}

void DrawTruss(HDC hdc_, const RECT& rect_)
{
	COLORREF color = RGB(0, 0, 0);
	char* s;
	s = (char*)"it is my text";

	HPEN hPen = CreatePen(PS_SOLID, 2, color);
	HBRUSH hBrush = CreateSolidBrush(RGB(255,255,255));
	HGDIOBJ hOldPen = SelectObject(hdc_, hPen);
	HFONT hFont = CreateFont(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	SelectObject(hdc_, hFont);
	// габариты отсечения
	ModelViewBox MinMax;
	SetModelViewBox(MinMax, 0, 0, 0, 0);
	//push
	POINT push[4] = {
			{10, height / 8},
			{10, height / 8 + height / 4 / 8},
			{10 + width / 2 / 8, height / 8 + height / 4 / 8},
			{10 + width / 2 / 8, height / 8}
	};
	if (rectangle == 1)
	{
		SetBkMode(hdc_, TRANSPARENT);
		SetBkMode(hdc_, OPAQUE);
		g_hPen = CreatePen(PS_SOLID, g_width, g_color);
		g_hBrush = CreateSolidBrush(g_color2);
		SetBkColor(hdc_, g_color2);
		SelectObject(hdc_, g_hPen);
		SelectObject(hdc_, g_hBrush);
	}
	Polygon(hdc_, push, 4);
	SelectObject(hdc_, hPen);
	SelectObject(hdc_, hBrush);
	TextOut(hdc_, 10 + width / 8 / 8, height / 8 + 2, L"push", 4);
	SetBkColor(hdc_, RGB(255, 255, 255));
	if (click.x > 10 && click.x < 10 + width / 2 / 8 && click.y > height / 8 && click.y < height / 8 + height / 4 / 8 && flag && operation != '=')
	{
		SetTextFormEditBox();
		flag = false;
	}
	//push
	//pop
	POINT pop[4] = {
			{20 + width / 16, height / 8},
			{20 + width / 16, height / 8 + height / 4 / 8},
			{20 + width / 8, height / 8 + height / 4 / 8},
			{20 + width / 8, height / 8}
	};
	if (rectangle == 2)
	{
		g_hPen = CreatePen(PS_SOLID, g_width, g_color);
		g_hBrush = CreateSolidBrush(g_color2);
		SetBkMode(hdc_, TRANSPARENT);
		SetBkMode(hdc_, OPAQUE);
		SetBkColor(hdc_, g_color2);
		SelectObject(hdc_, g_hPen);
		SelectObject(hdc_, g_hBrush);
	}
	Polygon(hdc_, pop, 4);
	SelectObject(hdc_, hPen);
	SelectObject(hdc_, hBrush);
	TextOut(hdc_, 20 + width / 8 / 1.5, height / 8 + 2, L"pop", 3);
	SetBkColor(hdc_, RGB(255, 255, 255));
	if (click.x > 20 + width / 16 && click.x < 20 + width / 8 && click.y > height / 8 && click.y < height / 8 + height / 4 / 8 && flag)
	{
		RemoveTextFormEditBox();
		flag = false;
	}
	//pop
	//exit
	POINT exit[4] = {
			{30 + width / 8, height / 8},
			{30 + width / 8, height / 8 + height / 4 / 8},
			{30 + (width * 1.5) / 8, height / 8 + height / 4 / 8},
			{30 + (width * 1.5) / 8, height / 8}
	};
	Polygon(hdc_, exit, 4);
	TextOut(hdc_, 30 + width / 8 + width / 48, height / 8 + 2, L"exit", 4);

	if (click.x > 30 + width / 8 && click.x < 30 + (width * 1.5) / 8 && click.y > height / 8 && click.y < height / 8 + height / 4 / 8
		&& flag)
	{
		PostQuitMessage(0);
		flag = false;
	}
	//exit
	//1
	POINT one[4] = {
			{10, height / 4},
			{10, height / 4 + height / 4 / 8},
			{10 + width / 2 / 8, height / 4 + height / 4 / 8},
			{10 + width / 2 / 8, height / 4}
	};
	if (rectangle == 3)
	{
		g_hPen = CreatePen(PS_SOLID, g_width, g_color);
		g_hBrush = CreateSolidBrush(g_color2);
		SetBkMode(hdc_, TRANSPARENT);
		SetBkMode(hdc_, OPAQUE);
		SetBkColor(hdc_, g_color2);
		SelectObject(hdc_, g_hPen);
		SelectObject(hdc_, g_hBrush);
	}
	Polygon(hdc_, one, 4);
	SelectObject(hdc_, hPen);
	SelectObject(hdc_, hBrush);
	TextOut(hdc_, 10 + width / 4 / 8, height / 4 + 2, L"1", 1);
	SetBkColor(hdc_, RGB(255, 255, 255));
	if (click.x > 10 && click.x < 10 + width / 2 / 8 && click.y > height / 4 && click.y < height / 4 + height / 4 / 8 && flag && operation != '='
		&& len < 10 && num_of_digital < 10)
	{
		text[len] = '1';
		if (operation == ' ')
			f1[len] = '1';
		else
			f2[num_of_digital++] = '1';
		len++;
		flag = false;
	}
	//2
	POINT two[4] = {
			{20 + width / 16, height / 4},
			{20 + width / 16, height / 4 + height / 32},
			{20 + width / 8, height / 4 + height / 32},
			{20 + width / 8, height / 4}
	};
	if (rectangle == 4)
	{
		g_hPen = CreatePen(PS_SOLID, g_width, g_color);
		g_hBrush = CreateSolidBrush(g_color2);
		SetBkMode(hdc_, TRANSPARENT);
		SetBkMode(hdc_, OPAQUE);
		SetBkColor(hdc_, g_color2);
		SelectObject(hdc_, g_hPen);
		SelectObject(hdc_, g_hBrush);
	}
	Polygon(hdc_, two, 4);
	SelectObject(hdc_, hPen);
	SelectObject(hdc_, hBrush);
	TextOut(hdc_, 20 + width / 16 + width / 32, height / 4 + 2, L"2", 1);
	SetBkColor(hdc_, RGB(255, 255, 255));
	if (click.x > 20 + width / 16 && click.x < 20 + width / 8 && click.y > height / 4 && click.y < height / 4 + height / 4 / 8 && flag && operation != '='
		&& len < 10 && num_of_digital < 10)
	{
		text[len] = '2';
		if (operation == ' ')
			f1[len] = '2';
		else
			f2[num_of_digital++] = '2';
		len++;
		flag = false;
	}
	//2
	//3
	POINT three[4] = {
			{30 + width / 8, height / 4},
			{30 + width / 8, (height * 1.125) / 4},
			{30 + (width * 1.5) / 8, (height * 1.125) / 4},
			{30 + (width * 1.5) / 8, height / 4}
	};
	Polygon(hdc_, three, 4);
	TextOut(hdc_, 30 + width / 8 + width / 32, height / 4 + 2, L"3", 1);

	if (click.x > 30 + width / 8 && click.x < 30 + (width * 1.5) / 8 && click.y > height / 4 && click.y < (height * 1.125) / 4 && flag && operation != '='
		&& len < 10 && num_of_digital < 10)
	{
		text[len] = '3';
		if (operation == ' ')
			f1[len] = '3';
		else
			f2[num_of_digital++] = '3';
		len++;
		flag = false;
	}
	//3
	//+
	POINT plus[4] = {
			{40 + (width * 1.5) / 8, height / 4},
			{40 + (width * 1.5) / 8, (height * 1.125) / 4},
			{40 + (width * 2) / 8, (height * 1.125) / 4},
			{40 + (width * 2) / 8, height / 4}
	};
	Polygon(hdc_, plus, 4);
	TextOut(hdc_, 40 + (width * 1.5) / 8 + width / 32, height / 4 + 2, L"+", 1);

	if (click.x > 40 + (width * 1.5) / 8 && click.x < 40 + (width * 2) / 8 && click.y > height / 4 && click.y < (height * 1.125) / 4 && flag && operation != '+' &&
		operation != '=')
	{
		if (operation != ' ')
			text[pos_znak] = '+';
		else
		{
			pos_znak = (short)len;
			text[len] = '+';
			len++;
		}
		operation = '+';
		flag = false;
	}
	// +
	// =
	POINT answer[4] = {
			{50 + (width * 2) / 8, height / 4},
			{50 + (width * 2) / 8, (height * 1.125) / 4},
			{50 + (width * 2.5) / 8, (height * 1.125) / 4},
			{50 + (width * 2.5) / 8, height / 4}
	};
	Polygon(hdc_, answer, 4);
	TextOut(hdc_, 50 + (width * 2) / 8 + width / 32, height / 4 + 2, L"=", 1);

	if (click.x > 50 + (width * 2) / 8 && click.x < 50 + (width * 2.5) / 8 && click.y > height / 4 && click.y < (height * 1.125) / 4 && flag && operation != '='
		&& operation != ' ')
	{
		first_num = atoi(f1);
		second_num = atoi(f2);
		text[len] = '=';
		len++;
		if (operation == '+')
		{
			_itoa(first_num + second_num, answer_text, 10);
			mbstowcs(&text[len], answer_text, strlen(answer_text));
			len += strlen(answer_text);
			operation = '=';
		}
		if (operation == '-')
		{
			_itoa(first_num - second_num, answer_text, 10);
			mbstowcs(&text[len], answer_text, strlen(answer_text));
			len += strlen(answer_text);
			operation = '=';
		}
		if (operation == '*')
		{
			_itoa(first_num * second_num, answer_text, 10);
			mbstowcs(&text[len], answer_text, strlen(answer_text));
			len += strlen(answer_text);
			operation = '=';
		}
		if (operation == ':')
		{
			if (second_num != 0)
			{
				_itoa(first_num / second_num, answer_text, 10);
				mbstowcs(&text[len], answer_text, strlen(answer_text));
				len += strlen(answer_text);
			}

			else
			{
				mbstowcs(&text[len], "impossible", strlen(answer_text));
				len += 10;
			}
			operation = '=';
		}
		flag = false;
	}
	// =
	// 4
	POINT four[4] = {
			{10, 10 + (height * 1.125) / 4},
			{10, 10 + (height * 1.25) / 4},
			{10 + width / 16, 10 + (height * 1.25) / 4},
			{10 + width / 16, 10 + (height * 1.125) / 4}
	};
	Polygon(hdc_, four, 4);
	TextOut(hdc_, 10 + width / 32, 10 + (height * 1.125) / 4 + 2, L"4", 1);

	if (click.x > 10 && click.x < 10 + width / 16 && click.y > 10 + (height * 1.125) / 4 && click.y < 10 + (height * 1.25) / 4 && flag && operation != '='
		&& len < 10 && num_of_digital < 10)
	{
		text[len] = '4';
		if (operation == ' ')
			f1[len] = '4';
		else
			f2[num_of_digital++] = '4';
		len++;
		flag = false;
	}
	// 4
	// 5
	POINT five[4] = {
			{20 + width / 16, 10 + (height * 1.125) / 4},
			{20 + width / 16, 10 + (height * 1.25) / 4},
			{20 + width / 8, 10 + (height * 1.25) / 4},
			{20 + width / 8, 10 + (height * 1.125) / 4}
	};
	Polygon(hdc_, five, 4);
	TextOut(hdc_, 20 + width / 16 + width / 32, 10 + (height * 1.125) / 4 + 2, L"5", 1);

	if (click.x > 20 + width / 16 && click.x < 20 + width / 8 && click.y > 10 + (height * 1.125) / 4 && click.y < 10 + (height * 1.25) / 4
		&& flag && operation != '=' && len < 10 && num_of_digital < 10)
	{
		text[len] = '5';
		if (operation == ' ')
			f1[len] = '5';
		else
			f2[num_of_digital++] = '5';
		len++;
		flag = false;
	}
	// 5
	// 6
	POINT six[4] = {
			{30 + width / 8, 10 + (height * 1.125) / 4},
			{30 + width / 8, 10 + (height * 1.25) / 4},
			{30 + (width * 1.5) / 8, 10 + (height * 1.25) / 4},
			{30 + (width * 1.5) / 8, 10 + (height * 1.125) / 4}
	};
	Polygon(hdc_, six, 4);
	TextOut(hdc_, 30 + width / 8 + width / 32, 10 + (height * 1.125) / 4 + 2, L"6", 1);

	if (click.x > 30 + width / 16 && click.x < 30 + (width * 1.5) / 8 && click.y > 10 + (height * 1.125) / 4 && click.y < 10 + (height * 1.25) / 4
		&& flag && operation != '=' && len < 10 && num_of_digital < 10)
	{
		text[len] = '6';
		if (operation == ' ')
			f1[len] = '6';
		else
			f2[num_of_digital++] = '6';
		len++;
		flag = false;
	}
	// 6
	// -
	POINT minus[4] = {
			{40 + (width * 1.5) / 8, 10 + (height * 1.125) / 4},
			{40 + (width * 1.5) / 8, 10 + (height * 1.25) / 4},
			{40 + (width * 2) / 8, 10 + (height * 1.25) / 4},
			{40 + (width * 2) / 8, 10 + (height * 1.125) / 4}
	};
	Polygon(hdc_, minus, 4);
	TextOut(hdc_, 40 + (width * 1.5) / 8 + width / 32, 10 + (height * 1.125) / 4 + 2, L"-", 1);

	if (click.x > 40 + (width * 1.5) / 8 && click.x < 40 + (width * 2) / 8 && click.y > 10 + (height * 1.125) / 4 && click.y < 10 + (height * 1.25) / 4
		&& flag && operation != '-' && operation != '=')
	{
		if (operation != ' ')
			text[pos_znak] = '-';
		else
		{
			pos_znak = (short)len;
			text[len] = '-';
			len++;
		}
		operation = '-';
		flag = false;
	}
	// -
	// 7
	POINT seven[4] = {
			{10, 20 + (height * 1.25) / 4},
			{10, 20 + (height * 1.375) / 4},
			{10 + width / 16, 20 + (height * 1.375) / 4},
			{10 + width / 16, 20 + (height * 1.25) / 4}
	};
	Polygon(hdc_, seven, 4);
	TextOut(hdc_, 10 + width / 32, 20 + (height * 1.25) / 4 + 2, L"7", 1);

	if (click.x > 10 && click.x < 10 + width / 16 && click.y > 20 + (height * 1.25) / 4
		&& click.y < 20 + (height * 1.375) / 4 && flag && operation != '=' && len < 10 && num_of_digital < 10)
	{
		text[len] = '7';
		if (operation == ' ')
			f1[len] = '7';
		else
			f2[num_of_digital++] = '7';
		len++;
		flag = false;
	}
	// 7
	// 8
	POINT eight[4] = {
			{20 + width / 16, 20 + (height * 1.25) / 4},
			{20 + width / 16, 20 + (height * 1.375) / 4},
			{20 + width / 8, 20 + (height * 1.375) / 4},
			{20 + width / 8, 20 + (height * 1.25) / 4}
	};
	Polygon(hdc_, eight, 4);
	TextOut(hdc_, 20 + width / 16 + width / 32, 20 + (height * 1.25) / 4 + 2, L"8", 1);

	if (click.x > 20 + width / 16 && click.x < 20 + width / 8 && click.y > 20 + (height * 1.25) / 4
		&& click.y < 20 + (height * 1.375) / 4 && flag && operation != '=' && len < 10 && num_of_digital < 10)
	{
		text[len] = '8';
		if (operation == ' ')
			f1[len] = '8';
		else
			f2[num_of_digital++] = '8';
		len++;
		flag = false;
	}
	// 8
	// 9
	POINT nine[4] = {
			{30 + width / 8, 20 + (height * 1.25) / 4},
			{30 + width / 8, 20 + (height * 1.375) / 4},
			{30 + (width * 1.5) / 8, 20 + (height * 1.375) / 4},
			{30 + (width * 1.5) / 8, 20 + (height * 1.25) / 4}
	};
	Polygon(hdc_, nine, 4);
	TextOut(hdc_, 30 + width / 8 + width / 32, 20 + (height * 1.25) / 4 + 2, L"9", 1);

	if (click.x > 30 + width / 8 && click.x < 30 + (width * 1.5) / 8 && click.y > 20 + (height * 1.25) / 4
		&& click.y < 20 + (height * 1.375) / 4 && flag && operation != '=' && len < 10 && num_of_digital < 10)
	{
		text[len] = '9';
		if (operation == ' ')
			f1[len] = '9';
		else
			f2[num_of_digital++] = '9';
		len++;
		flag = false;
	}
	// 9
	// *
	POINT mult[4] = {
			{40 + (width * 1.5) / 8, 20 + (height * 1.25) / 4},
			{40 + (width * 1.5) / 8, 20 + (height * 1.375) / 4},
			{40 + (width * 2) / 8, 20 + (height * 1.375) / 4},
			{40 + (width * 2) / 8, 20 + (height * 1.25) / 4}
	};
	Polygon(hdc_, mult, 4);
	TextOut(hdc_, 40 + (width * 1.5) / 8 + width / 32, 20 + (height * 1.25) / 4 + 2, L"*", 1);

	if (click.x > 40 + (width * 1.5) / 8 && click.x < 40 + (width * 2) / 8 && click.y > 20 + (height * 1.25) / 4 && click.y < 20 + (height * 1.375) / 4
		&& flag && operation != '*' && operation != '=')
	{


		if (operation != ' ')
			text[pos_znak] = '*';
		else
		{
			pos_znak = (short)len;
			text[len] = '*';
			len++;
		}
		operation = '*';
		flag = false;
	}
	// *
	// 0
	POINT zero[4] = {
			{20 + width / 16, 30 + (height * 1.375) / 4},
			{20 + width / 16, 30 + (height * 1.5) / 4},
			{20 + width / 8, 30 + (height * 1.5) / 4},
			{20 + width / 8, 30 + (height * 1.375) / 4}
	};
	Polygon(hdc_, zero, 4);
	TextOut(hdc_, 20 + width / 16 + width / 32, 30 + (height * 1.375) / 4 + 2, L"0", 1);

	if (click.x > 20 + width / 16 && click.x < 20 + width / 8 && click.y > 30 + (height * 1.375) / 4
		&& click.y < 30 + (height * 1.5) / 4 && flag && operation != '=' && len < 10 && num_of_digital < 10)
	{
		text[len] = '0';
		if (operation == ' ')
			f1[len] = '0';
		else
			f2[num_of_digital++] = '0';
		len++;
		flag = false;
	}
	// 0
	// C
	POINT C[4] = {
			{30 + width / 8, 30 + (height * 1.375) / 4},
			{30 + width / 8, 30 + (height * 1.5) / 4},
			{30 + (width * 1.5) / 8, 30 + (height * 1.5) / 4},
			{30 + (width * 1.5) / 8, 30 + (height * 1.375) / 4}
	};
	Polygon(hdc_, C, 4);
	TextOut(hdc_, 30 + width / 8 + width / 32, 30 + (height * 1.375) / 4 + 2, L"C", 1);

	if (click.x > 30 + width / 8 && click.x < 30 + (width * 1.5) / 8 && click.y > 30 + (height * 1.375) / 4
		&& click.y < 30 + (height * 1.5) / 4 && flag)
	{
		text[len] = 'C';
		len = 0;
		while (text[len])
			text[len++] = '\0';
		len = 0;
		while (f1[len])
			f1[len++] = '\0';
		len = 0;
		while (f2[len])
			f2[len++] = '\0';
		len = 0;
		while (answer_text[len])
			answer_text[len++] = '\0';
		operation = ' ';
		len = 0;
		num_of_digital = 0;
		flag = false;
	}
	// C
	// /
	POINT mod[4] = {
			{40 + (width * 1.5) / 8, 30 + (height * 1.375) / 4},
			{40 + (width * 1.5) / 8, 30 + (height * 1.5) / 4},
			{40 + (width * 2) / 8, 30 + (height * 1.5) / 4},
			{40 + (width * 2) / 8, 30 + (height * 1.375) / 4}
	};
	Polygon(hdc_, mod, 4);
	TextOut(hdc_, 40 + (width * 1.5) / 8 + width / 32, 30 + (height * 1.375) / 4 + 2, L":", 1);

	if (click.x > 40 + (width * 1.5) / 8 && click.x < 40 + (width * 2) / 8 && click.y > 20 + (height * 1.375) / 4 && click.y < 20 + (height * 1.5) / 4
		&& flag && operation != ':' && operation != '=')
	{


		if (operation != ' ')
			text[pos_znak] = ':';
		else
		{
			pos_znak = (short)len;
			text[len] = ':';
			len++;
		}
		operation = ':';
		flag = false;
	}
	// /
	RECT rect = rect_;
	InflateRect(&rect, -1 / 2, -1 / 2);
	TextOut(hdc_, 10, height / 6, text, len);
	hdc_ = clone;
	SelectObject(hdc_, hOldPen);
	DeleteObject(hPen);
}

//
// выбор имени emf-файла для экспорта изображения
//
void SelectEmfFileName(HINSTANCE hInst_, HWND hWnd_, TCHAR acFileName_[/*MAX_PATH*/])
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
	HDC hdcEmf = CreateEnhMetaFile(nullptr, g_acEmfFileName, nullptr, nullptr);
	RECT rect = { 0, 0, 1000, 1000 };
	DrawTruss(hdcEmf, rect);

	HENHMETAFILE hEmf = CloseEnhMetaFile(hdcEmf);
	DeleteEnhMetaFile(hEmf);
}

//
// оконная функция
//
RECT rect;

LRESULT CALLBACK WindowProc(HWND hWnd_, UINT nMsg_, WPARAM wParam_, LPARAM lParam_)
{
	GetClientRect(hWnd_, &rect);
	switch (nMsg_)
	{
	case WM_CREATE:
		//
		// инициализировать форму - задать имя файла картинки и поля редактирования параметров
		//
		FormOn_WM_CREATE(hWnd_, wParam_, lParam_, _T("schema.bmp"));
		AddFormEditBox(0, _T("Значение 1: "), CEditBox::vtTextView, 10, _T(""));
		AddFormEditBox(1, _T("Значение 2: "), CEditBox::vtTextView, 10, _T(""));
		AddFormEditBox(2, _T("Значение 3: "), CEditBox::vtTextView, 10, _T(""));
		AddFormEditBox(3, _T("Значение 4: "), CEditBox::vtTextView, 10, _T(""));
		AddFormEditBox(4, _T("Значение 5: "), CEditBox::vtTextView, 10, _T(""));
		AddFormEditBox(5, _T("Новое значение: "), CEditBox::vtText, 10, _T(""));

		return 0L;
	case WM_MOUSEMOVE:
		move.x = LOWORD(lParam_);
		move.y = HIWORD(lParam_);
		if (move.x > 10 && move.x < 10 + width / 2 / 8 && move.y > height / 8 && move.y < height / 8 + height / 4 / 8)
			SetCursor(hCurs1);
		else if (move.x > 20 + width / 16 && move.x < 20 + width / 8 && move.y > height / 8 && move.y < height / 8 + height / 4 / 8)
			SetCursor(hCurs2);
		else if (move.x > 10 && move.x < 10 + width / 2 / 8 && move.y > height / 4 && move.y < height / 4 + height / 4 / 8)
			SetCursor(hCurs3);
		else if (move.x > 20 + width / 16 && move.x < 20 + width / 8 && move.y > height / 4 && move.y < height / 4 + height / 4 / 8)
			SetCursor(hCurs4);
		else
			SetCursor(hCurs5);
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
		click.x = LOWORD(lParam_);
		click.y = HIWORD(lParam_);
		InvalidateRect(hWnd_, &rect, TRUE);
		flag = true;
		FormOn_WM_LBUTTONDOWN(wParam_, lParam_);
		return 0L;
	case WM_PAINT:
		FormOn_WM_PAINT(wParam_, lParam_);
		return 0L;
	case WM_KEYDOWN:
		FormOn_WM_KEYDOWN(wParam_, lParam_);
		return 0L;

	case WM_CHAR:
		if (wParam_ == 49 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '1';
			if (operation == ' ')
				f1[len] = '1';
			else
				f2[num_of_digital++] = '1';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if (wParam_ == 50 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '2';
			if (operation == ' ')
				f1[len] = '2';
			else
				f2[num_of_digital++] = '2';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if (wParam_ == 51 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '3';
			if (operation == ' ')
				f1[len] = '3';
			else
				f2[num_of_digital++] = '3';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if (wParam_ == 52 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '4';
			if (operation == ' ')
				f1[len] = '4';
			else
				f2[num_of_digital++] = '4';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if (wParam_ == 53 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '5';
			if (operation == ' ')
				f1[len] = '5';
			else
				f2[num_of_digital++] = '5';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if (wParam_ == 54 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '6';
			if (operation == ' ')
				f1[len] = '6';
			else
				f2[num_of_digital++] = '6';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if (wParam_ == 55 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '7';
			if (operation == ' ')
				f1[len] = '7';
			else
				f2[num_of_digital++] = '7';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if (wParam_ == 56 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '8';
			if (operation == ' ')
				f1[len] = '8';
			else
				f2[num_of_digital++] = '8';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if (wParam_ == 57 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '9';
			if (operation == ' ')
				f1[len] = '9';
			else
				f2[num_of_digital++] = '9';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if (wParam_ == 58 && operation != '=' && len < 10 && num_of_digital < 10)
		{
			text[len] = '0';
			if (operation == ' ')
				f1[len] = '0';
			else
				f2[num_of_digital++] = '0';
			len++;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if ((wchar_t)wParam_ == 'c' || (wchar_t)wParam_ == 'C')
		{
			text[len] = 'C';
			len = 0;
			while (text[len])
				text[len++] = '\0';
			len = 0;
			while (f1[len])
				f1[len++] = '\0';
			len = 0;
			while (f2[len])
				f2[len++] = '\0';
			len = 0;
			while (answer_text[len])
				answer_text[len++] = '\0';
			operation = ' ';
			len = 0;
			num_of_digital = 0;
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if ((wchar_t)wParam_ == '-' && operation != '=' && operation != '-')
		{
			if (operation != ' ')
				text[pos_znak] = '-';
			else
			{
				pos_znak = (short)len;
				text[len] = '-';
				len++;
			}
			operation = '-';
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if ((wchar_t)wParam_ == '*' && operation != '=' && operation != '*')
		{
			if (operation != ' ')
				text[pos_znak] = '*';
			else
			{
				pos_znak = (short)len;
				text[len] = '*';
				len++;
			}
			operation = '*';
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if ((wchar_t)wParam_ == '+' && operation != '=' && operation != '+')
		{
			if (operation != ' ')
				text[pos_znak] = '+';
			else
			{
				pos_znak = (short)len;
				text[len] = '+';
				len++;
			}
			operation = '+';
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if ((wchar_t)wParam_ == '/' && operation != '=' && operation != ':')
		{
			if (operation != ' ')
				text[pos_znak] = ':';
			else
			{
				pos_znak = (short)len;
				text[len] = ':';
				len++;
			}
			operation = ':';
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		if ((wchar_t)wParam_ == '=' && operation != '=' && operation != ' ')
		{
			first_num = atoi(f1);
			second_num = atoi(f2);
			text[len] = '=';
			len++;
			if (operation == '+')
			{
				_itoa(first_num + second_num, answer_text, 10);
				mbstowcs(&text[len], answer_text, strlen(answer_text));
				len += strlen(answer_text);
				operation = '=';
			}
			if (operation == '-')
			{
				_itoa(first_num - second_num, answer_text, 10);
				mbstowcs(&text[len], answer_text, strlen(answer_text));
				len += strlen(answer_text);
				operation = '=';
			}
			if (operation == '*')
			{
				_itoa(first_num * second_num, answer_text, 10);
				mbstowcs(&text[len], answer_text, strlen(answer_text));
				len += strlen(answer_text);
				operation = '=';
			}
			if (operation == ':')
			{
				if (second_num != 0)
					_itoa(first_num / second_num, answer_text, 10);
				else
					answer_text = (char*)"impossible";
				mbstowcs(&text[len], answer_text, strlen(answer_text));
				len += strlen(answer_text);
				operation = '=';
			}
			InvalidateRect(hWnd_, &rect, TRUE);
		}
		FormOn_WM_CHAR(wParam_, lParam_);
		return 0L;

	case WM_SYSCHAR:
		if ((wchar_t)wParam_ == 'x')
			PostQuitMessage(0);

	case WM_TIMER:
		FormOn_WM_TIMER(wParam_, lParam_);
		return 0L;

	case WM_GETMINMAXINFO:
		FormOn_WM_GETMINMAXINFO(wParam_, lParam_);
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
int WINAPI _tWinMain(HINSTANCE hApp_, HINSTANCE hPrevApp_, LPTSTR szCmdLine_, int nCmdShow_)
{
	MSG msg;


	// инициализация структуры описания класса окон
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

	// регистрация класса окна
	if (!RegisterClassEx(&wc))
		return 0;

	// определение размеров экрана в пикселях
	int scrw = GetSystemMetrics(SM_CXSCREEN),
		scrh = GetSystemMetrics(SM_CYSCREEN);

	// создание окна
	HWND hWnd = CreateWindow(g_szWindowClassName, _T("Second curs"), WS_OVERLAPPEDWINDOW,
		scrw / 2 - 400, scrh / 2 - 300, 800, 600,
		nullptr, nullptr, hApp_, nullptr);

	if (nullptr == hWnd)
		return 0;
	HACCEL hAccelTable = LoadAccelerators(hApp_, MAKEINTRESOURCE(IDC_WINDOWSPROJECT5));
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	hInst = hApp_;
	


	// обработка сообщений
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
}  //WinMain