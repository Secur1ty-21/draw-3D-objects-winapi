#include <windows.h>
#include <tchar.h>


HINSTANCE hi_app;
HWND hwnd_main;
LPCTSTR pc_WindowClassName = _T("WindowClassName");
LPCTSTR pc_wndTitle = _T("labgr_1_var4_Bykov");

BOOL RegisterWndClasses(void);
void UnregisterWndClasses(void);
BOOL CreateWindows(void);
LRESULT CALLBACK MainWindowProc(HWND, UINT, WPARAM, LPARAM);
BOOL MainOnDestroy(HWND);
BOOL MainOnPaint(HWND);
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
	wce_main.lpszClassName = pc_WindowClassName;
	return RegisterClassEx(&wce_main);
}

BOOL CreateWindows()
{
	int scrw2 = GetSystemMetrics(SM_CXSCREEN) / 2,
		scrh2 = GetSystemMetrics(SM_CYSCREEN) / 2;
	hwnd_main = CreateWindow(pc_WindowClassName, pc_wndTitle,
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN
		| WS_MINIMIZE | WS_VISIBLE,
		scrw2 - 400, scrh2 - 300, 800, 600, NULL, NULL, hi_app, 0);
	if (!hwnd_main)
		return FALSE;
	return TRUE;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_PAINT: return MainOnPaint(hwnd);
	case WM_DESTROY: return MainOnDestroy(hwnd);
	default:         return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0L;
}

BOOL MainOnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;

	HPEN hpen_1 = CreatePen(PS_SOLID, 3, RGB(255, 0, 0)), //red
		hpen_2 = CreatePen(PS_SOLID, 5, RGB(0, 255, 0)), //green
		hpen_3 = CreatePen(PS_DASH, 3, RGB(0, 0, 255)), //blue
		hpen_prv;


	HBRUSH hbrush_1 = CreateSolidBrush(RGB(255, 0, 0)), // yellow
		hbrush_2 = CreateSolidBrush(RGB(255, 0, 255)),   // purple
		hbrush_3 = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0)),   // purple with lines
		hbrush_prv;


	HDC hdc = BeginPaint(hwnd, &ps);
	SetBkMode(hdc, TRANSPARENT);

	hpen_prv = (HPEN)SelectObject(hdc, hpen_2);
	hbrush_prv = (HBRUSH)SelectObject(hdc, hbrush_1);
	//
	int dCoor = 100;
	POINT apnt_coords[] = {
		{ 100,       100       },
		{ 100 + 2 * dCoor, 100       },
		{ 100 + 2 * dCoor, 100 + dCoor },
		{ 100,       100 + dCoor }
	};
	SetBkMode(hdc, OPAQUE);
	SetBkColor(hdc, RGB(255, 0, 255));
	SelectObject(hdc, hbrush_3);
	SelectObject(hdc, hpen_2);
	Polygon(hdc, apnt_coords, 4);
	SetBkMode(hdc, TRANSPARENT);
//
	MoveToEx(hdc, 100 + 2 * dCoor, 100 + dCoor / 2, NULL);
	LineTo(hdc, 100 + 3 * dCoor, 100 + dCoor/ 2);
//
	SetTextColor(hdc, RGB(0, 0, 0));
	TextOut(hdc, 350, 100, _T("Да"), lstrlenA("Да"));
//
	//int dCoor = 100;
	POINT apnt_coords2[] = {
		{ 100 + 3 * dCoor, 100 + dCoor / 2},
		{ 100 + 3 * dCoor + dCoor / 2,100 + dCoor},
		{ 100 + 4 * dCoor, 100 + dCoor / 2},
		{ 100 + 3 * dCoor + dCoor / 2, 100}
	};
	SetBkMode(hdc, OPAQUE);
	SetBkColor(hdc, RGB(255, 0, 0));
	SelectObject(hdc, hbrush_1);
	Polygon(hdc, apnt_coords2, 4);
	SetBkMode(hdc, TRANSPARENT);
//
	SelectObject(hdc, hpen_3);
	MoveToEx(hdc, 100 + 3 * dCoor + dCoor / 2, 100 + dCoor, NULL);
	LineTo(hdc, 100 + 3 * dCoor + dCoor / 2, 300);
//
	SetTextColor(hdc, RGB(0, 0, 0));
	TextOut(hdc, 500, 250, _T("Нет"), lstrlenA("Нет"));

//
	SetBkMode(hdc, OPAQUE);
	SetBkColor(hdc, RGB(255, 0, 255));
	SelectObject(hdc, hbrush_2);
	SelectObject(hdc, hpen_3);
	//Ellipse()
	Ellipse(hdc, 300, 300, 100 + 5 * dCoor, 400);
	SetBkMode(hdc, TRANSPARENT);


	SelectObject(hdc, hpen_prv);
	SelectObject(hdc, hbrush_prv);

	EndPaint(hwnd, &ps);

	DeleteObject(hpen_1);
	DeleteObject(hpen_2);
	DeleteObject(hpen_3);
	DeleteObject(hbrush_1);
	DeleteObject(hbrush_2);

	return TRUE;
}

BOOL MainOnDestroy(HWND hwnd)
{
	PostQuitMessage(0); return TRUE;
}

BOOL InitializeApp()
{
	if (!RegisterWndClasses()) {
		MessageBox(NULL, _T("Error"), pc_wndTitle, MB_OK);
		return FALSE;
	}
	CreateWindows();
	//	ShowWindow (hwnd_main, SW_MINIMIZE);
	//	UpdateWindow (hwnd_main);
	return TRUE;
}

void UninitializeApp()
{
	UnregisterClass(pc_WindowClassName, hi_app);
}

int APIENTRY _tWinMain(HINSTANCE hi_crr, HINSTANCE hi_prv, LPTSTR pc_cmdLine, int cmdShow)
{
	MSG msg;
	hi_app = hi_crr;
	InitializeApp();
	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	UninitializeApp();
	return msg.wParam;
}
