//#pragma comment(linker,"\"/manifestdependency:type='win32' \
//name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
//processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>
int g_x = GetSystemMetrics(SM_CXSCREEN);
int g_y = GetSystemMetrics(SM_CYSCREEN);
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR szCmdLine, int nCmdShow)
{
	MSG msg{};
	HWND hwnd{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		switch (uMsg)
		{
		case WM_CREATE:
		{
			SetWindowPos(hWnd, HWND_TOPMOST, g_x / 6, g_y / 6, g_x / 6 * 2, g_y / 6 * 4, SWP_NOSENDCHANGING);
		}
		return (0);
		case WM_DESTROY:
		{
			PostQuitMessage(EXIT_SUCCESS);
		}
		return (0);
		case WM_LBUTTONDOWN:
		{
			MessageBox(hWnd, L"Левая кнопка мыши, Быков САП-211", L"WM_LBUTTONDOWN", MB_ICONINFORMATION);
		}
		return (0);
		case WM_MBUTTONDOWN:
		{
			MessageBox(hWnd, L"Средняя кнопка мыши, Быков САП-211", L"WM_MBUTTONDOWN", MB_ICONINFORMATION);
		}
		return 0;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};
	wc.lpszClassName = L"MyAppClass";
	wc.lpszMenuName = nullptr;
	wc.style = WS_VSCROLL & WS_SIZEBOX;
	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	if (hwnd = CreateWindow(wc.lpszClassName, L"Window", WS_OVERLAPPEDWINDOW, 0, 0, 600, 600, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);



	HWND hwnd2{};
	WNDCLASSEX wc2{ sizeof(WNDCLASSEX) };
	wc2.cbClsExtra = 0;
	wc2.cbWndExtra = 0;
	wc2.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(COLOR_BTNFACE));
	wc2.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc2.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc2.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc2.hInstance = hInstance;
	wc2.lpfnWndProc = [](HWND hWnd2, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		switch (uMsg)
		{
		case WM_CREATE:
		{
			SetWindowPos(hWnd2, HWND_TOP, g_x / 6 * 4, 0, g_x / 6 * 2, g_y / 6 * 3, SWP_NOSENDCHANGING);
		}
		return (0);
		case WM_DESTROY:
		{
			PostQuitMessage(EXIT_SUCCESS);
		}
		return (0);
		case WM_RBUTTONDOWN:
		{
			MessageBox(hWnd2, L"Правая кнопка мыши, Быков САП-211", L"WM_LBUTTONDOWN", MB_ICONINFORMATION);
		}
		return (0);
		case WM_KEYUP:
		{
			MessageBox(hWnd2, L"Любая кнопка на клавиатуре, Быков САП-211", L"WM_KEYUP", MB_ICONINFORMATION);
			PostQuitMessage(EXIT_SUCCESS);
		}
		return 0;
		}
		return DefWindowProc(hWnd2, uMsg, wParam, lParam);
	};
	wc2.lpszClassName = L"MyAppClass2";
	wc2.lpszMenuName = nullptr;
	wc2.style = WS_BORDER & WS_MAXIMIZEBOX;
	if (!RegisterClassEx(&wc2))
		return EXIT_FAILURE;

	if (hwnd2 = CreateWindow(wc2.lpszClassName, L"Window2", WS_OVERLAPPEDWINDOW, 0, 0, 600, 600, nullptr, nullptr, wc2.hInstance, nullptr); hwnd2 == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;
	ShowWindow(hwnd2, nCmdShow);
	UpdateWindow(hwnd2);




	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);

}