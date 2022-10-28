#pragma warning(disable : 4996)
//WS_BORDER - тонкая рамка
#define SIX 6
#include <windows.h>
#include <tchar.h>
#include <cmath>
#define PI 3.14
#define _CRT_SECURE_NO_WARNINGS


HINSTANCE hInstance;
LPCTSTR Class0 = _T("Class0"), Class9 = _T("Class9"), Class3_8 = _T("Class3_8");
HWND mas_window[SIX][SIX];
wchar_t* buff;

static int mas[SIX][SIX];

void add_mas() //Заполнение матрицы
{
	int i, j;
	int three = 3;
	//0
	for (j = 1; j < SIX; j++)
		for (i = 0; i < j - 1; i++)
			mas[i][j] = 0;
	//3_8
	for (i = 0; i < SIX; i++)
		mas[i][i] = three++;
	//9
	for (j = 0; j < SIX; j++)
		for (i = 5; i > j; i--)
			mas[i][j] = 9;
}

LRESULT CALLBACK WindowProc_3_8(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_DESTROY:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		return (0);
	case WM_LBUTTONDOWN:
		MessageBox(hwnd, _T("Нажатие левой клавиши мыши"), _T("WM_LBUTTONDOWN"), MB_ICONINFORMATION);
		return (0);
	case WM_KEYUP:
		MessageBox(hwnd, _T("Нажатие на кнопку клавиатуры"), _T("WM_KEYUP"), MB_ICONINFORMATION);
		return (0);
	default:
		return DefWindowProc(hwnd, Msg, wParam, lParam); //Сообщения, не требующие обработки, передаются стандартной оконной функцией
	}
	return 0;
}


LRESULT CALLBACK WindowProc_9(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_KEYUP:
		MessageBox(hwnd, _T("Нажатие на кнопку клавиатуры"), _T("WM_KEYUP"), MB_ICONINFORMATION);
		return (0);
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, Msg, wParam, lParam); //Сообщения, не требующие обработки, передаются стандартной оконной функцией
	}
	return 0;
}

BOOL MainOnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;

	HPEN hpen_1 = CreatePen(PS_SOLID, 5, RGB(255, 0, 255)), //Сплошная, фиолетовая
		hpen_2 = CreatePen(PS_DASH, 1, RGB(0, 0, 0)), //Раздробная, чёрная
		hpen_3 = CreatePen(PS_SOLID, 5, RGB(0, 255, 0)); //Сплошная, зелёная


	HBRUSH hbrush_1 = CreateSolidBrush(RGB(255, 255, 0)), // Сплошной, желтый
		hbrush_2 = CreateSolidBrush(RGB(255, 0, 255)),   // Сплошной, фиолетовый
		hbrush_3 = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0));   // Диагонали, чёрный


	HDC hdc = BeginPaint(hwnd, &ps);
	SetBkMode(hdc, TRANSPARENT);
	//12-угольный круг
	int storona = 10, x0 = (GetSystemMetrics(SM_CXSCREEN) / SIX - 5) / 2, y0 = (GetSystemMetrics(SM_CYSCREEN) / SIX - 5) / 3;
	SetBkMode(hdc, OPAQUE);
	SetBkColor(hdc, RGB(0, 0, 255));
	POINT circle12_coords[] = {
		{x0, y0},
		{x0 + storona * sin(75 * PI / 180),                                                                   y0 + storona * cos(75 * PI / 180)},
		{x0 + storona * sin(75 * PI / 180) + storona * sin(45 * PI / 180),                                    y0 + storona * cos(75 * PI / 180) + storona * cos(45 * PI / 180)},
		{x0 + storona * sin(75 * PI / 180) + storona * sin(45 * PI / 180) + storona * sin(15 * PI / 180),     y0 + storona * cos(75 * PI / 180) + storona * cos(45 * PI / 180) + storona * cos(15 * PI / 180)},
		{x0 + storona * sin(75 * PI / 180) + storona * sin(45 * PI / 180),                                    y0 + storona * cos(75 * PI / 180) + storona * cos(45 * PI / 180) + storona * cos(15 * PI / 180) * 2},
		{x0 + storona * sin(75 * PI / 180),                                                                   y0 + storona * cos(75 * PI / 180) + storona * cos(45 * PI / 180) * 2 + storona * cos(15 * PI / 180) * 2},
		{x0,                                                                                                  y0 + storona * cos(75 * PI / 180) * 2 + storona * cos(45 * PI / 180) * 2 + storona * cos(15 * PI / 180) * 2},
		{x0 - storona * sin(75 * PI / 180),                                                                   y0 + storona * cos(75 * PI / 180) + storona * cos(45 * PI / 180) * 2 + storona * cos(15 * PI / 180) * 2},
		{x0 - storona * sin(75 * PI / 180) - storona * sin(45 * PI / 180),                                    y0 + storona * cos(75 * PI / 180) + storona * cos(45 * PI / 180) + storona * cos(15 * PI / 180) * 2},
		{x0 - storona * sin(75 * PI / 180) - storona * sin(45 * PI / 180) - storona * sin(15 * PI / 180),     y0 + storona * cos(75 * PI / 180) + storona * cos(45 * PI / 180) + storona * cos(15 * PI / 180)},
		{x0 - storona * sin(75 * PI / 180) - storona * sin(45 * PI / 180),                                    y0 + storona * cos(75 * PI / 180) + storona * cos(45 * PI / 180)},
		{x0 - storona * sin(75 * PI / 180),                                                                   y0 + storona * cos(75 * PI / 180)}
	};
	SelectObject(hdc, hbrush_3);
	SelectObject(hdc, hpen_3);
	Polygon(hdc, circle12_coords, 12);

	//Конец закраски
	EndPaint(hwnd, &ps);

	DeleteObject(hpen_1);
	DeleteObject(hpen_2);
	DeleteObject(hbrush_1);
	DeleteObject(hbrush_2);

	return TRUE;
}

LRESULT CALLBACK WindowProc_0(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_LBUTTONDOWN: return MainOnPaint(hwnd);
		case WM_PAINT: break;
		default:
			return DefWindowProc(hwnd, Msg, wParam, lParam); //Сообщения, не требующие обработки, передаются стандартной оконной функцией
	}
	return 0;
}



BOOL RegisterWndClasses()
{
	HBRUSH hbrush;
	hbrush =CreateSolidBrush(RGB(0, 0, 255));
	WNDCLASSEX wc;
	//Окно 0
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_NOCLOSE;
	wc.lpfnWndProc = &WindowProc_0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast <HBRUSH> (GetStockObject(WHITE_BRUSH));
	wc.lpszClassName = Class0;
	if (RegisterClassEx(&wc) == NULL) return FALSE;
	//Окно 3-8
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = &WindowProc_3_8;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast <HBRUSH> (GetStockObject(BLACK_BRUSH));
	wc.lpszClassName = Class3_8;
	if (RegisterClassEx(&wc) == NULL) return FALSE;
	//Окно 9
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = &WindowProc_9;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = hbrush;
	wc.lpszClassName = Class9;
	return RegisterClassEx(&wc);
}

BOOL CreateWindows()
{
	TCHAR   buff[255];
	LPCTSTR cl_cur;
	int w_Style = WS_SYSMENU | WS_BORDER;
	int width = GetSystemMetrics(SM_CXSCREEN) / SIX,
		height = GetSystemMetrics(SM_CYSCREEN) / SIX;
	add_mas();
	int i, j, x, y;
	for (j = 0; j < SIX; j++)
		for (i = 0; i < SIX; i++)
		{
			x = i * width;
			y = j * height;
			_stprintf(buff, _T("matr[%d][%d]=%d"), i, j, mas[i][j]);
			if (mas[i][j] == 9)
			{
				cl_cur = Class9;
				mas_window[i][j] = CreateWindow(cl_cur, buff, WS_SYSMENU, x, y, width, height, nullptr, nullptr, hInstance, nullptr);
			}
			else if (mas[i][j] > 2 && mas[i][j] < 9)
			{
				cl_cur = Class3_8;
				mas_window[i][j] = CreateWindow(cl_cur, buff, w_Style, x, y, width, height, nullptr, nullptr, hInstance, nullptr);
			}
			else
			{
				cl_cur = Class0;
				mas_window[i][j] = CreateWindow(cl_cur, buff, w_Style, x, y, width, height, nullptr, nullptr, hInstance, nullptr);
			}
			
			if (mas_window[i][j] == NULL)
				return FALSE;
		}
	return TRUE;
}

int WINAPI _tWinMain(HINSTANCE h, HINSTANCE hPrevApp, LPTSTR szCmdLine, int nCmdShow)
{
	int i, j;
	hInstance = h;
	MSG msg;
	RegisterWndClasses();
	CreateWindows();
	for (j = 0; j < SIX; j++)
		for (i = 0; i < SIX; i++)
			ShowWindow(mas_window[i][j], SW_SHOW);
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnregisterClass(Class0, h);
	UnregisterClass(Class3_8, h);
	UnregisterClass(Class9, h);
	return msg.wParam;
}