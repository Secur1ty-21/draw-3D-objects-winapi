#pragma warning(disable : 4996)
//WS_BORDER - тонкая рамка
#define MATR_DIM 6
#include <windows.h>
#include <tchar.h>
#include <cmath>
#define PI 3.14159265
#define _CRT_SECURE_NO_WARNINGS


HINSTANCE hi_App;
LPCTSTR Class0 = _T("Class0"), Class7 = _T("Class7");//Названия классов
HWND aa_hwnd[MATR_DIM][MATR_DIM];
wchar_t* mc_buff;

static int matr[MATR_DIM][MATR_DIM];

void add_matr() //Заполнение матрицы
{
	int i, j;
	for (j = 0; j < MATR_DIM; j++)
		for (i = 0; i <= j; i++)
			matr[i][j] = 7;
}

LRESULT CALLBACK WindowProc_0( //Оконная функция
	HWND hwnd1,
	UINT nMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_MBUTTONDOWN:
		MessageBox(NULL, _T("Нажатие колёсика мыши"), _T("Месседж бокс материализовался"), MB_OK);
		break;
	default:
		return DefWindowProc(hwnd1, nMsg, wParam, lParam); //Сообщения, не требующие обработки, передаются стандартной оконной функцией
	}
	return 0;
}

BOOL MainOnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;

	HPEN hpen_1 = CreatePen(PS_SOLID, 5, RGB(255, 0, 255)), //Сплошная, фиолетовая
		hpen_2 = CreatePen(PS_DASH, 1, RGB(0, 0, 0)), //Раздробная, чёрная
		hpen_3 = CreatePen(PS_SOLID, 5, RGB(0, 255, 0)), //Сплошная, зелёная
		hpen_prv;


	HBRUSH hbrush_1 = CreateSolidBrush(RGB(255, 255, 0)), // Сплошной, желтый
		hbrush_2 = CreateSolidBrush(RGB(255, 0, 255)),   // Сплошной, фиолетовый
		hbrush_3 = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0)),   // Диагонали, чёрный
		hbrush_prv;


	HDC hdc = BeginPaint(hwnd, &ps);
	SetBkMode(hdc, TRANSPARENT);
	//12-угольный круг
	int storona = 10, x0 = (GetSystemMetrics(SM_CXSCREEN) / MATR_DIM - 5) / 2, y0 = (GetSystemMetrics(SM_CYSCREEN) / MATR_DIM - 5) / 3;
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

LRESULT CALLBACK WindowProc_7( //Оконная функция
	HWND hwnd2,
	UINT nMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_RBUTTONDOWN: return MainOnPaint(hwnd2);
	case WM_PAINT: break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd2, nMsg, wParam, lParam); //Сообщения, не требующие обработки, передаются стандартной оконной функцией
	}
	return 0;
}

BOOL RegisterWndClasses()//Регистрация классов окон
{
	WNDCLASSEX wc_main;
	//Окно 0
	ZeroMemory(&wc_main, sizeof(WNDCLASSEX));
	wc_main.cbSize = sizeof(WNDCLASSEX); //Размер структуры
	wc_main.hInstance = hi_App; //Дескриптор экземпляра приложения
	wc_main.style = CS_VREDRAW | CS_HREDRAW | CS_NOCLOSE; //Стили класса окна
	wc_main.lpfnWndProc = &WindowProc_0; //адрес оконной функции
	wc_main.hCursor = LoadCursor(NULL, IDC_ARROW); //Загрузка типа курсора - стрелка
	wc_main.hbrBackground = static_cast <HBRUSH> (GetStockObject(WHITE_BRUSH)); //Фон окна
	wc_main.lpszClassName = Class0; //Имя класса окна
	if (RegisterClassEx(&wc_main) == NULL) return FALSE; //Проверка регистрации
	//Окно 7
	ZeroMemory(&wc_main, sizeof(WNDCLASSEX));
	wc_main.cbSize = sizeof(WNDCLASSEX); //Размер структуры
	wc_main.hInstance = hi_App; //Дескриптор экземпляра приложения
	wc_main.style = CS_VREDRAW | CS_HREDRAW; //Стили класса окна
	wc_main.lpfnWndProc = &WindowProc_7; //адрес оконной функции
	wc_main.hCursor = LoadCursor(NULL, IDC_ARROW); //Загрузка типа курсора - стрелка
	wc_main.hbrBackground = static_cast <HBRUSH> (GetStockObject(BLACK_BRUSH)); //Фон окна - чёрный
	wc_main.lpszClassName = Class7; //Имя класса окна
	return RegisterClassEx(&wc_main);
}

BOOL CreateWindows()//Создание окон
{
	TCHAR   mc_buff[255];
	LPCTSTR cl_cur;
	int w_Style = WS_SYSMENU | WS_BORDER;
	int scrW = GetSystemMetrics(SM_CXSCREEN),
		scrH = GetSystemMetrics(SM_CYSCREEN),
		gap = 5,
		wndW = scrW / MATR_DIM - gap,
		wndH = scrH / MATR_DIM - gap;
	add_matr();
	int i, j, x, y;
	for (j = 0; j < MATR_DIM; j++)
		for (i = 0; i <= MATR_DIM; i++)
		{
			x = i * (wndW + gap);
			y = j * (wndH + gap);
			_stprintf(mc_buff, _T("matr[%i][%i]=%i"), i, j, matr[i][j]);
			if (matr[i][j] == 7)
				cl_cur = Class7;
			else
				cl_cur = Class0;
			aa_hwnd[i][j] = CreateWindow(
				cl_cur,
				mc_buff,
				w_Style,
				x, y,
				wndW, wndH,
				NULL, NULL, hi_App, NULL);
			if (aa_hwnd[i][j] == NULL) return FALSE;
		}
	return TRUE;
}

int WINAPI _tWinMain(HINSTANCE hApp, HINSTANCE hPrevApp, LPTSTR szCmdLine, int nCmdShow)
{
	int i, j;
	hi_App = hApp;
	MSG msg; //Структура данных о текущем сообщении
	RegisterWndClasses();
	CreateWindows();
	for (j = 0; j < MATR_DIM; j++)
		for (i = 0; i <= MATR_DIM; i++)
			ShowWindow(aa_hwnd[i][j], SW_SHOW);
	while (GetMessage(&msg, NULL, 0, 0) != 0)//Цикл ожиданий и обработки сообщения
		DispatchMessage(&msg);
	UnregisterClass(Class0, hApp);
	UnregisterClass(Class7, hApp);
	return msg.wParam;
}