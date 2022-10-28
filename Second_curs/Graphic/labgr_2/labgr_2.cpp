
/************************************************************************/
/*                          lab2_gr_new                                 */
/*                          ===========                                 */
/*             Основы программирования для ОС Windows.                  */
/*                   Лабораторная работа №2.                            */
/*  Вывод графической информации в окно (разложение ф-ции в  ряд).      */
/************************************************************************/
#pragma warning(disable : 4996)
#include <windows.h>
#include <stdio.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS

HINSTANCE hi_app;
HWND hwnd_main;


LPCWSTR pcz_class1 = L"WindowClassName1",
pcz_wndTitle = L"Пример Windows-приложения 2";

BOOL RegisterWndClasses(void);
void UnregisterWndClasses(void);
BOOL CreateWindows(void);
LRESULT CALLBACK MainWindowProc(HWND, UINT, WPARAM, LPARAM);
BOOL MainOnPaint(HWND);
BOOL MainOnDestroy(HWND);
BOOL InitializeApp(void);
void UninitializeApp(void);
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int);



BOOL RegisterWndClasses()
{
	WNDCLASSEX wce_main;
	memset(&wce_main, 0, sizeof(WNDCLASSEX));
	wce_main.cbSize = sizeof(WNDCLASSEX);
	wce_main.hInstance = hi_app;
	wce_main.style = CS_VREDRAW | CS_HREDRAW;
	wce_main.lpfnWndProc = (WNDPROC)MainWindowProc;
	wce_main.hCursor = LoadCursor(NULL, IDC_ARROW);
	wce_main.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wce_main.lpszClassName = pcz_class1;
	return RegisterClassEx(&wce_main);
}

BOOL CreateWindows()
{
	int scrw2 = GetSystemMetrics(SM_CXSCREEN) / 2,
		scrh2 = GetSystemMetrics(SM_CYSCREEN) / 2;

	hwnd_main = CreateWindow(pcz_class1, pcz_wndTitle,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		scrw2 - 400, scrh2 - 300, 800, 600, NULL, NULL, hi_app, 0);
	return TRUE;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_PAINT:   return MainOnPaint(hwnd);
	case WM_DESTROY: return MainOnDestroy(hwnd);
	default:         return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0L;
}

BOOL MainOnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
	return TRUE;
}


/*****************************************************************/
/*                     MainOnPaint                               */
/*                     ===========                               */
/*   Рисование сложного набора  графиков  аналитических функций  */
/*****************************************************************/

#define ALL_PT  16      // число точек на графике
#define ALL_FUN 3       // число функций
#define K_GAB   0.05    // отступ от края
#define XN      0.3   // интервал 
#define XK       1.1   // 

#define MIN_MSERIES  3     //  минимальное  число членов ряда
#define MAX_MSERIES  15     //  максимальное число членов ряда


/*-----------------------------------------*/
/*    Прототипы  вспомогательных функций   */
/*-----------------------------------------*/
double fun_user(double);
double fun_expansion(double, int);

POINT   coor_w(RECT, double, double, double[2][2]);

void draw_point(HDC, int, int, int);
/*----------------------------------------*/



BOOL MainOnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	// запросить данные о повреждённой области окна, требующей перерисовки,
	// получить дескриптор контекста поверхности вывода
	HDC hdc = BeginPaint(hwnd, &ps);

	// разрешить рисование подложки окна
	SetBkMode(hdc, TRANSPARENT);

	// --- создать перья ---
	HPEN mpen[] = { CreatePen(PS_SOLID, 3, RGB(255, 0, 0)),       // красный
				   CreatePen(PS_SOLID, 2, RGB(100 , 0, 255)),    // синий
				   CreatePen(PS_SOLID, 1, RGB(255 , 0, 255)),    // фиолетовый
				   CreatePen(PS_SOLID, 1, RGB(0 , 0, 0)),       // чёрный
				   CreatePen(PS_SOLID, 2, RGB(0   , 255, 0))     // зелёный
	},

		// выделить созданное перо 1 в контексте,
		// сохранить дескриптор предыдущего активного в данном контексте пера
		hpen_prv = (HPEN)SelectObject(hdc, mpen[0]);


	POINT           coor_int[ALL_FUN][ALL_PT], // оконные координаты узлов графика
		pt1, pt2;

	int   Y_axis_x,
		i, j, pr_first = 0;

	double   mas_y[ALL_FUN][ALL_PT],  //  массив координат Y набора графиков
		mas_gab[2][2],           //  массив габаритных точек набора графиков
		y_max, y_min,
		dx, x_cur, y_cur;

	/*---------------------------------------*/
	/*   Формирование  массивов  координат   */
	/*---------------------------------------*/

	dx = fabs(XK - XN) / ALL_PT;

	for (i = 0; i < ALL_PT; i++)
	{
		x_cur = XN + dx * i;

		mas_y[0][i] = fun_user(x_cur);
		mas_y[1][i] = fun_expansion(x_cur, MIN_MSERIES);
		mas_y[2][i] = fun_expansion(x_cur, MAX_MSERIES);
	}

	// определение габаритов графика
	y_max = y_min = mas_y[0][1];
	for (i = 0; i < ALL_FUN; i++)
		for (j = 0; j < ALL_PT; j++)
		{
			y_cur = mas_y[i][j];

			if (y_cur >= y_max) y_max = y_cur;
			if (y_cur <= y_min) y_min = y_cur;
		}

	mas_gab[0][0] = XN - 0.1; mas_gab[0][1] = y_min - 0.1;
	mas_gab[1][0] = XK + 0.1; mas_gab[1][1] = y_max + 0.1;

	// определить размеры клиентской области окна
	RECT rc;
	GetClientRect(hwnd, &rc);
	/*----------------------------------------------*/
	/* вычислить оконные координаты узлов графика   */
	/*----------------------------------------------*/

	for (i = 0; i < ALL_FUN; i++)
		for (j = 0; j < ALL_PT; j++)
		{
			x_cur = XN + dx * j;
			y_cur = mas_y[i][j];

			coor_int[i][j] = coor_w(rc, x_cur, y_cur, mas_gab);
		}

	/*----------------------------------*/
	/*     Нарисовать  график           */
	/*----------------------------------*/

	for (i = 0; i < ALL_FUN; i++)
	{
		SelectObject(hdc, mpen[i]);
		pr_first = 0;
		for (j = 0; j < ALL_PT; j++)
		{
			pt1 = coor_int[i][j];

			// если узел не первый - нарисовать линию
			if (pr_first != 0)
				LineTo(hdc, pt1.x, pt1.y);
			else
			{
				MoveToEx(hdc, pt1.x, pt1.y, NULL);
				// отметить, что первый узел пройден
				pr_first = 1;
			}
		}
	}

	/*-----------------------------------------------------------------------*/
	/*   нарисовать  оси                                                     */
	/*-----------------------------------------------------------------------*/

	 //  ось Y
	POINT rec[3];
	SelectObject(hdc, mpen[3]);

	pt1 = coor_w(rc, XN - 0.1, y_min, mas_gab);
	pt2 = coor_w(rc, XN - 0.1, y_max + 0.05, mas_gab);
	rec[0].x = pt2.x - 7; rec[0].y = pt2.y;
	rec[1].x = pt2.x; rec[1].y = pt2.y - 20;
	rec[2].x = pt2.x + 7; rec[2].y = pt2.y;
	Polygon(hdc, rec, 3);
	TextOut(hdc, rec[2].x + 10, rec[1].y + 10, L"Y", 1);
	Y_axis_x = pt1.y;
	MoveToEx(hdc, pt1.x, pt1.y, NULL);
	LineTo(hdc, pt2.x, pt2.y);

	//  ось X

	pt1 = coor_w(rc, XN - 0.1, y_min, mas_gab);
	pt2 = coor_w(rc, XK , y_min, mas_gab);
	rec[0].x = pt2.x; rec[0].y = pt2.y - 7;
	rec[1].x = pt2.x + 20; rec[1].y = pt2.y;
	rec[2].x = pt2.x; rec[2].y = pt2.y + 7;
	Polygon(hdc, rec, 3);
	TextOut(hdc, rec[2].x + 10, rec[1].y + 20, L"X", 1);
	MoveToEx(hdc, pt1.x, pt1.y, NULL);
	LineTo(hdc, pt2.x, pt2.y);
	 // создать  перо

	HBRUSH
		hbrush_1 = CreateSolidBrush(RGB(100, 100, 255)), // голубая сплошная
		hbrush_2 = CreateSolidBrush(RGB(0, 255, 0)),     // зелёное
		hbrush_3 = CreateSolidBrush(RGB(255, 0, 0)),   // red
		hbrush_4 = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 255)), //blue
		hbrush_prv = (HBRUSH)SelectObject(hdc, hbrush_1);

	SelectObject(hdc, (HPEN)GetStockObject(NULL_PEN));

	POINT cp[4];

	for (i = 0; i < ALL_PT - 1; i += 2)
	{
		cp[0] = coor_int[1][i];
		cp[1] = coor_int[1][i + 1];
		cp[2] = coor_int[2][i + 1];
		cp[3] = coor_int[2][i];

		Polygon(hdc, cp, 4);
	}
	/*-----------------------------------------------------------------------*/
	/*   Нарисовать  трапеции через одну                                     */
	/*-----------------------------------------------------------------------*/

	hpen_prv = (HPEN)SelectObject(hdc, mpen[3]);
	hbrush_prv = (HBRUSH)SelectObject(hdc, hbrush_4);
	SetBkMode(hdc, TRANSPARENT);
	SetBkMode(hdc, OPAQUE);
	SetBkColor(hdc, RGB(255, 255, 255));
	SelectObject(hdc, hbrush_4);
	SelectObject(hdc, mpen[3]);
	int  znak = 1;
	POINT p[4];
	for (i = 0; i < ALL_PT; i += 2)
	{	
		p[0] = coor_int[0][i];
		p[3] = coor_w(rc, p[0].x, y_min, mas_gab);
		p[3].x = p[0].x;
		p[1] = coor_int[0][i + 1];
		p[2] = coor_w(rc, p[1].x, y_min, mas_gab);
		p[2].x = p[1].x;
		Polygon(hdc, p, 4);
		znak = -znak;
	}
	
	pt1 = coor_w(rc, XN - 0.1, y_max, mas_gab);
	pt2 = coor_w(rc, XN, y_max, mas_gab);
	TextOut(hdc, pt1.x - 50 , pt1.y , L"1.47" , lstrlenA("1.47"));
	MoveToEx(hdc, pt1.x - 20, pt1.y, NULL);
	LineTo(hdc, pt2.x, pt2.y);
	SetBkMode(hdc, TRANSPARENT);
	SetBkMode(hdc, OPAQUE);
	SetBkColor(hdc, RGB(255, 0, 0));
	SelectObject(hdc, hbrush_3);
	p[0].x = pt2.x - 5; p[0].y = pt2.y;
	p[1].x = pt2.x; p[1].y = pt2.y + 5;
	p[2].x = pt2.x + 5; p[2].y = pt2.y;
	p[3].x = pt2.x; p[3].y = pt2.y - 5;
	Polygon(hdc, p, 4);
	SetBkColor(hdc, RGB(255, 255, 255));
	/*-----------------------------------------------------------------------*/
	/*	                           создать шрифт                             */
	/*-----------------------------------------------------------------------*/
	LOGFONT lf; // структура с логическим описанием шрифта (параметры шрифта)
	 // получить параметры шрифта, используемого для рисования
	 // текста диалоговых элементов по умолчанию, в структуру lf
	GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

	//lf.lfItalic = TRUE;      // курсив
	lf.lfWeight = FW_BOLD;   // жирный
	lf.lfOrientation = 900;       // поворот на 90 градусов против ЧС
	lf.lfEscapement = 900;
	lf.lfHeight = 14;        // высота поля 25 pix
	 // создать физический шрифт,
	 // наиболее близко соответствующий описанному в lf
	HFONT hfont_1 = CreateFontIndirect(&lf);
	// получить дескриптор текущего активного шрифта
	HFONT hfont_prv = (HFONT)SelectObject(hdc, hfont_1);
	/*-----------------------------------------------------------------------*/
	/*	                           нарисовать номера узлов                   */
	/*-----------------------------------------------------------------------*/
	// выбрать цвет текста (чёрный)
	pt1 = coor_w(rc, XN, y_min, mas_gab);
	TextOut(hdc, pt1.x - 20, pt1.y - 20, L"0.3", lstrlenA("0.3"));
	SetTextColor(hdc, RGB(0, 0, 0));
	pt1 = coor_w(rc, XK, y_min, mas_gab);
	TextOut(hdc, pt1.x - 30, pt1.y - 5, L"1.1", lstrlenA("1.1"));
	SetTextColor(hdc, RGB(0, 0, 0));
	// нарисовать номера узлов
	
	/*-----------------------------------------------------------------------*/

	// --- восстановить контекст и удалить GDI-объекты ---
	// выделить в контексте перо, активное изначально
	SelectObject(hdc, hpen_prv);
	SelectObject(hdc, hbrush_prv);
	// удалить наши перья
	for (i = 0; i < sizeof(mpen) / sizeof(mpen[0]); i++)
		DeleteObject(mpen[i]);

	// удалить кисти
	DeleteObject(hbrush_1);
	DeleteObject(hbrush_2);
	DeleteObject(hbrush_3);
	DeleteObject(hbrush_4);
	// (аналогично для шрифтов)
	SelectObject(hdc, hfont_prv);
	DeleteObject(hfont_1);
	// объявить, что сообщение WM_PAINT обработано,
	// освободить полученный ранее контекст окна
	EndPaint(hwnd, &ps);
	// успешное завершение
	return TRUE;
}

/******************************************************************/
/*                       fun_user                                 */
/*                       ========                                 */
/*           пользовательская функция log((x + 1) / x)            */
/******************************************************************/

double fun_user(double x)
{
	return (log((x + 1) / x));
}

/**********************************************************************/
/*                       fun_expansion                                */
/*                       =============                                */
/*    разложение в ряд пользовательской функции                       */
/*    пользовательской функции  log(1-x)   на участке  0.3 <= x <= 1.1*/
/**********************************************************************/
double fun_expansion(double x, int n)
{
	int i;
	double sum = 1 / (2 * x + 1), xp = 3.0, xm = xp;

	for (i = 1; i < n; i++)
	{
		sum += 1 / (xm * pow(2 * x + 1, xm));
		xp += 2;
		xm = xp;
	}
	sum *= 2;
	return (sum);
}

/******************************************************************/
/*                      coor_w                                    */
/*                      ======                                    */
/*    Возвращает  указатель экранные координаты точки             */
/*     a- ширина области вывода                                   */
/*     b- высота области вывода                                   */
/******************************************************************/

POINT   coor_w(RECT  cp,
	double x, double y,
	double mas_gab[2][2]
)
{
	int     a, b, gab_a, gab_b, xc_w, yc_w;

	double  k_scale, xc, yc, x_max, y_max, x_min, y_min, dx, dy, k1, k2;

	POINT   ptw;

	/*--------------------------------*/
	/*  Параметры  окна  вывода       */
	/*--------------------------------*/

	a = abs(cp.right - cp.left);
	b = abs(cp.top - cp.bottom);

	gab_a = (int)((double)a * K_GAB);  // отступ по ширине
	gab_b = (int)((double)b * K_GAB);  // отступ по высоте

	a -= gab_a;
	b -= gab_b;

	xc_w = (cp.right + cp.left) / 2;
	yc_w = (cp.top + cp.bottom) / 2;

	/*-----------------------------------------*/
	/*  Определение  масштабного коэффициента  */
	/*-----------------------------------------*/
	x_min = mas_gab[0][0];   y_min = mas_gab[0][1];
	x_max = mas_gab[1][0];   y_max = mas_gab[1][1];

	dx = fabs(x_max - x_min);
	dy = fabs(y_max - y_min);

	xc = (x_max + x_min) / 2;
	yc = (y_max + y_min) / 2;

	k1 = a / dx;
	k2 = b / dy;

	k_scale = (k1 < k2) ? k1 : k2;

	/*-----------------------------------------*/
	/*    Масштабирование координат            */
	/*-----------------------------------------*/

	ptw.x = (int)((x - xc) * k_scale + xc_w);
	ptw.y = (int)(-(y - yc) * k_scale + yc_w);


	return (ptw);
}



BOOL InitializeApp()
{
	if (!RegisterWndClasses()) {
		MessageBox(NULL, L"Ошибка регистрации классов окон.", pcz_wndTitle, MB_OK);
		return FALSE;
	}
	CreateWindows();
	return TRUE;
}

void UninitializeApp()
{
	UnregisterClass(pcz_class1, hi_app);
}

int APIENTRY WinMain(HINSTANCE hi_crr, HINSTANCE hi_prv, LPSTR pc_cmdLine, int cmdShow)
{
	MSG msg;
	hi_app = hi_crr;
	InitializeApp();
	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	UninitializeApp();
	return msg.wParam;
}
