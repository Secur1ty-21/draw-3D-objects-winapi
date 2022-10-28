
// +++ labDlg.cpp +++

#define STRICT

#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#pragma warning(disable : 4996)
#include <commctrl.h>


#include "resource.h"



HINSTANCE g_hiApp;                           
LPCWSTR g_aczAppName = L"Диалоговые элементы"; 


HWND   g_hwndMain = NULL;                                   
LPCWSTR g_pczMainWndClassName = L"MainWndClass";              
LPCWSTR g_pczMainWndTitle = L"Пример приложения с диалогом";  

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL MainOnCommand(int, HWND, UINT);
BOOL MainOnPaint();
BOOL MainOnDestroy();


BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL DlgOnInitDialog(HWND);
BOOL DlgOnCommand(HWND, int, HWND, UINT);


BOOL RegisterWndClasses(void);
BOOL CreateWindows(void);
void InitializeApp(void);
void UninitializeApp(void);


int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int);



int g_iFigure = 0,      
g_iColor = 0,      
g_showMode = 0,     
g_figureWidth = 128,    
g_figureHeight = 128,
g_fShowFigureSize = FALSE,
count = 0;
wchar_t* wStrings[10];

LPCWSTR apczFigureNames[] =
{
	L"Прямоугольник",
	L"Эллипс",
	L"Прямоугольник со скруглёнными углами",
	L"Многоугольник",
};


COLORREF g_acrColors[] =
{
	RGB(0xFF, 0xFF, 0xFF),
	RGB(0xFF, 0x00, 0x00),
	RGB(0x00, 0xFF, 0x00),
	RGB(0x00, 0x00, 0xFF),
	RGB(0x80, 0x00, 0x00),
	RGB(0x00, 0x80, 0x00),
	RGB(0x00, 0x00, 0x80),
	RGB(0xFF, 0xFF, 0x00),
	RGB(0x00, 0xFF, 0xFF),
	RGB(0xFF, 0x00, 0xFF),
	0,						
};


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!g_hwndMain)
		g_hwndMain = hwnd;

	switch (msg)
	{
	case WM_COMMAND:
		return MainOnCommand(LOWORD(wParam), HWND(lParam), HIWORD(wParam));
	case WM_PAINT:
		return MainOnPaint();
	case WM_DESTROY:
		return MainOnDestroy();
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0L;
}

BOOL MainOnCommand(int cmdId, HWND hwndCtrlItem, UINT ntfCode)
{
	switch (cmdId)
	{
	case IDC_FILE_DIALOG:
		DialogBox(g_hiApp, MAKEINTRESOURCE(IDD_DIALOG), g_hwndMain, DlgProc);
		break;
	case IDC_FILE_ABOUT:
		MessageBox(g_hwndMain, L"Пример приложения с диалогом", g_aczAppName, MB_OK | MB_ICONINFORMATION);
		break;
	case IDC_FILE_EXIT:
		SendMessage(g_hwndMain, WM_CLOSE, 0, 0L);
		break;
	}
	return FALSE;
}


BOOL MainOnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(g_hwndMain, &ps);

	RECT rClient;
	GetClientRect(g_hwndMain, &rClient);

	int xCent = rClient.right / 2,
		yCent = rClient.bottom / 2;

	RECT rFigure =
	{
		xCent - g_figureWidth / 2,
		yCent - g_figureHeight / 2,
		xCent + g_figureWidth / 2,
		yCent + g_figureHeight / 2,
	};

	HPEN hpen = CreatePen(PS_SOLID, 1, g_acrColors[g_iColor]),
		hpenOld = (HPEN)SelectObject(hdc, hpen);
	HBRUSH hbrush = g_showMode ? CreateSolidBrush(g_acrColors[g_iColor]) : NULL,
		hbrushOld = (HBRUSH)SelectObject(hdc, g_showMode ? hbrush : GetStockObject(NULL_BRUSH));

	switch (g_iFigure)
	{
	case 0:
		Rectangle(hdc, rFigure.left, rFigure.top, rFigure.right, rFigure.bottom);
		break;
	case 1:
		Ellipse(hdc, rFigure.left, rFigure.top, rFigure.right, rFigure.bottom);
		break;
	case 2:
		RoundRect(hdc, rFigure.left, rFigure.top, rFigure.right, rFigure.bottom,
			g_figureWidth / 10, g_figureHeight / 10);
		break;

	case 3:
	{
		const int nVerts = 16;
		POINT apntVerts[nVerts];
		int i, sgn = 1;
		double dAng = 0.0,
			dAngDiff = 3.141592653589793 * 2 / nVerts;
		for (i = 0; i < nVerts; i++, dAng += dAngDiff, sgn *= -1)
		{
			apntVerts[i].x = xCent + sgn * int(g_figureWidth * cos(dAng) / 2);
			apntVerts[i].y = yCent + sgn * int(g_figureHeight * sin(dAng) / 2);
		}
		Polygon(hdc, apntVerts, nVerts);
	}
	break;
	}

	if (g_fShowFigureSize)
	{
		char mczBuff[0xFF];
		wsprintfA(mczBuff, "Размеры фигуры: ширина = %i, высота =  %i",
			g_figureWidth, g_figureHeight);
		SetTextColor(hdc, 0x8080FF);
		SetBkMode(hdc, TRANSPARENT);
		SetTextAlign(hdc, TA_CENTER | TA_BASELINE);
		wchar_t* y = (wchar_t*)malloc((strlen(mczBuff) + 1) * sizeof(wchar_t));
		mbstowcs(y, mczBuff, strlen(mczBuff));
		TextOut(hdc, xCent, yCent, y, lstrlenA(mczBuff));
	}

	SelectObject(hdc, hpenOld);
	if (hpen)
		DeleteObject(hpen);

	SelectObject(hdc, hbrushOld);
	if (hbrush)
		DeleteObject(hbrush);

	EndPaint(g_hwndMain, &ps);

	return TRUE;
}


BOOL MainOnDestroy()
{
	PostQuitMessage(0);
	return TRUE;
}


BOOL CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return DlgOnInitDialog(hdlg);
	case WM_COMMAND:
		return DlgOnCommand(hdlg, LOWORD(wParam), HWND(lParam), HIWORD(wParam));
	default:
		return FALSE;
	}
	return FALSE;
}


BOOL DlgOnInitDialog(HWND hdlg)
{

	int i,
		nCount = sizeof(apczFigureNames) / sizeof(apczFigureNames[0]);

	//for (i = 0; i < nCount; i++)
	//	SendDlgItemMessage(hdlg, IDC_LIST_FIGURE, LB_ADDSTRING, 0L, (LPARAM)apczFigureNames[i]);

	SendDlgItemMessage(hdlg, IDC_LIST_FIGURE, LB_SETCURSEL, g_iFigure, 0L);


	char mczBuff[0xFF];
	nCount = sizeof(g_acrColors) / sizeof(g_acrColors[0]) - 1;
	for (i = 0; i < nCount; i++)
	{
		wsprintfA(mczBuff, "%06X", g_acrColors[i]);
		wchar_t* color = (wchar_t*)malloc(7 * sizeof(wchar_t));
		mbstowcs(color, mczBuff, strlen(mczBuff));
		color[6] = '\0';
		SendDlgItemMessage(hdlg, IDC_COMBO_COLOR, CB_ADDSTRING, 0L, (LPARAM)color);
		free(color);
	}
	SendDlgItemMessage(hdlg, IDC_COMBO_COLOR, CB_SETCURSEL, g_iColor, 0L);

	SendDlgItemMessage(hdlg, IDC_COMBO_COLOR, CB_LIMITTEXT, 6, 0L);

	CheckRadioButton(hdlg, IDC_RADIO_SHOWMODE_CONTOUR, IDC_RADIO_SHOWMODE_FILL,
		!g_showMode ? IDC_RADIO_SHOWMODE_CONTOUR : IDC_RADIO_SHOWMODE_FILL);


	SetDlgItemInt(hdlg, IDC_EDIT_WIDTH, g_figureWidth, FALSE);
	SetDlgItemInt(hdlg, IDC_EDIT_HEIGHT, g_figureHeight, FALSE);

	SendDlgItemMessage(hdlg, IDC_EDIT_WIDTH, EM_LIMITTEXT, 4, 0L);
	SendDlgItemMessage(hdlg, IDC_EDIT_HEIGHT, EM_LIMITTEXT, 4, 0L);


	CheckDlgButton(hdlg, IDC_CHECK_SHOWSIZE, g_fShowFigureSize ? BST_CHECKED : BST_UNCHECKED);

	return FALSE;
}


BOOL DlgOnCommand(HWND hdlg, int cmdId, HWND hwndCtrlItem, UINT ntfCode)
{
	int i, * pFigureSize, nChars;
	wchar_t* text;
	char aczStr[7], c;
	BOOL fGetDlgItemIntSuccess, fEnable;
	double dRatio;

	switch (cmdId)
	{

	case IDC_LIST_FIGURE:
	
		if (LBN_SELCHANGE != ntfCode)
			break;

		i = SendDlgItemMessage(hdlg, cmdId, LB_GETCURSEL, 0, 0L);
		if (LB_ERR != i)
			g_iFigure = i;
		else

			SendDlgItemMessage(hdlg, cmdId, LB_SETCURSEL, g_iFigure = 0, 0L);

		SendMessage(hdlg, WM_COMMAND, IDC_UPDATEPICTURE, 0);
		break;


	case IDC_COMBO_COLOR:

		if (CBN_SELCHANGE == ntfCode)
		{
			i = SendDlgItemMessage(hdlg, cmdId, CB_GETCURSEL, 0, 0L);
			if (CB_ERR != i)
				g_iColor = i;
			else
				SendDlgItemMessage(hdlg, cmdId, CB_SETCURSEL, g_iColor = 0, 0L);
			SendMessage(hdlg, WM_COMMAND, IDC_UPDATEPICTURE, 0);
		}

		else if (CBN_EDITCHANGE == ntfCode)
		{
			wchar_t* clone = (wchar_t*)malloc(8 * sizeof(wchar_t));
			
			GetDlgItemText(hdlg, cmdId, clone, 7);
			MultiByteToWideChar(CP_ACP, 0, aczStr, -1, clone, 7);

			CharUpperA(aczStr);

			nChars = lstrlenA(aczStr);
			if (!nChars)
				SendDlgItemMessage(hdlg, cmdId, CB_SETCURSEL, g_iColor = 0, 0L);
			else
			{

				g_iColor = sizeof(g_acrColors) / sizeof(g_acrColors[0]) - 1;
				COLORREF& crColor = g_acrColors[g_iColor];
				crColor = 0;
				for (i = 0; i < nChars; i++)
				{
					c = aczStr[nChars - i - 1];
					if (c >= '0' && c <= '9')
						c -= '0';
					else if (c >= 'A' && c <= 'F')
						c -= 'A' - 0x0A;
					else
						c = 0;
					crColor += c * (1 << i * 4);
				}
				SendMessage(hdlg, WM_COMMAND, IDC_UPDATEPICTURE, 0);
			}
		}
		break;

	case IDC_PUSH:
		if (count < 5)
		{
			text = (wchar_t*)malloc(10 * sizeof(wchar_t*));
			GetDlgItemTextW(hdlg, IDC_EDIT_WIDTH, text, 10);
			text[9] = '\0';
				int lenght = 10;
				if (!(text == L""))
				{
					wStrings[count] = (wchar_t*)malloc(sizeof(wchar_t*) * lenght);
					for (int i = 0; i < lenght; i++)
						wStrings[count][i] = text[i];
					wStrings[count][lenght] = L'\0';
					SendDlgItemMessage(hdlg, IDC_LIST_FIGURE, LB_ADDSTRING, 0L, (LPARAM)text);
					SetDlgItemInt(hdlg, IDC_EDIT_WIDTH, 0, FALSE);
					count++;
				}
		}
		
		else if (count < 10)
		{
			text = (wchar_t*)malloc(10 * sizeof(wchar_t*));
			GetDlgItemTextW(hdlg, IDC_EDIT_WIDTH, text, 10);
			text[9] = '\0';
			if (!(text == L""))
			{
				int lenght = 10;
				wStrings[count] = (wchar_t*)malloc(sizeof(wchar_t*) * lenght);
				for (int i = 0; i < lenght; i++)
					wStrings[count][i] = text[i];
				wStrings[count][lenght] = L'\0';
				SetDlgItemInt(hdlg, IDC_EDIT_WIDTH, 0, FALSE);
				count++;
			}
		}
		break;
	case IDC_POP:
		text = (wchar_t*)malloc(10 * sizeof(wchar_t*));
		GetDlgItemTextW(hdlg, IDC_EDIT_WIDTH, text, 10);
		text[9] = '\0';
		if (!(text == L"") && count >= 0 && count <= 10)
		{
			for (int i = 0; i < 10; i++)
			{
				if (wStrings[i + 1] != 0 && i + 1 != 10)
				{
					wStrings[i] = (wchar_t*)L"";
					wStrings[i] = wStrings[i + 1];
				}
				else
					wStrings[i] = (wchar_t*)L"";
			}
			for (int z = count; z > 0; z--)
				SendDlgItemMessage(hdlg, IDC_LIST_FIGURE, LB_DELETESTRING, 0L, 0L);
			if (count != 0)
				count--;
			for (int y = 0; y < count && y < 5; y++)
				SendDlgItemMessage(hdlg, IDC_LIST_FIGURE, LB_ADDSTRING, 0L, (LPARAM)wStrings[y]);
		}

	case IDC_EDIT_WIDTH:
	case IDC_EDIT_HEIGHT:

		if (EN_CHANGE != ntfCode)
			break;

		//pFigureSize = IDC_EDIT_WIDTH == cmdId ? &g_figureWidth : &g_figureHeight;
		
		//*pFigureSize = (int)GetDlgItemInt(hdlg, cmdId, &fGetDlgItemIntSuccess, FALSE);
		
			
			
		
		//if (!fGetDlgItemIntSuccess || !*pFigureSize)
		//	SetDlgItemInt(hdlg, cmdId, *pFigureSize = 128, FALSE);

		fEnable = g_figureWidth < 500 && g_figureHeight < 500;
		EnableWindow(GetDlgItem(hdlg, IDC_INC), fEnable);
		fEnable = g_figureWidth > 5 && g_figureHeight > 5;
		// EnableWindow (GetDlgItem(hdlg, IDC_DEC), fEnable);
		ShowWindow(GetDlgItem(hdlg, IDC_DEC), fEnable ? SW_SHOW : SW_HIDE);
		SendMessage(hdlg, WM_COMMAND, IDC_UPDATEPICTURE, 0);
		break;


	case IDC_RADIO_SHOWMODE_CONTOUR:
	case IDC_RADIO_SHOWMODE_FILL:

		g_showMode = IsDlgButtonChecked(hdlg, IDC_RADIO_SHOWMODE_FILL);
		SendMessage(hdlg, WM_COMMAND, IDC_UPDATEPICTURE, 0);
		break;


	case IDC_INC:
	case IDC_DEC:

		dRatio = IDC_INC == cmdId ? 1.2 : 1 / 1.2;
		g_figureWidth = int(g_figureWidth * dRatio);
		g_figureHeight = int(g_figureHeight * dRatio);

		SetDlgItemInt(hdlg, IDC_EDIT_WIDTH, g_figureWidth, FALSE);
		SetDlgItemInt(hdlg, IDC_EDIT_HEIGHT, g_figureHeight, FALSE);
		SendMessage(hdlg, WM_COMMAND, IDC_UPDATEPICTURE, 0);
		break;


	case IDC_CHECK_SHOWSIZE:
		g_fShowFigureSize = IsDlgButtonChecked(hdlg, IDC_CHECK_SHOWSIZE);
		SendMessage(hdlg, WM_COMMAND, IDC_UPDATEPICTURE, 0);
		break;


	case IDC_UPDATEPICTURE:
		InvalidateRect(g_hwndMain, NULL, TRUE);
		break;


	case IDC_CLOSE:
	case IDCANCEL:
		EndDialog(hdlg, IDC_CLOSE);
		break;

	default:
		return FALSE;
	}
	return FALSE;
}


BOOL RegisterWndClasses()
{

	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.hInstance = g_hiApp;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.hIcon = LoadIcon(g_hiApp, MAKEINTRESOURCE(IDI_ICON));
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wc.lpszClassName = g_pczMainWndClassName;

	return RegisterClass(&wc);
}


BOOL CreateWindows()
{

	int scrw2 = GetSystemMetrics(SM_CXSCREEN) / 2,
		scrh2 = GetSystemMetrics(SM_CYSCREEN) / 2;

	CreateWindow(g_pczMainWndClassName, g_pczMainWndTitle,
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		scrw2 - 400, scrh2 - 300, 800, 600, NULL, NULL, g_hiApp, NULL);
	if (!g_hwndMain)
		return FALSE;

	return TRUE;
}

int APIENTRY WinMain(HINSTANCE hiApp_, HINSTANCE, LPSTR, int)
{
	MSG msg;
	g_hiApp = hiApp_;

	RegisterWndClasses();
	CreateWindows();
	ShowWindow(g_hwndMain, SW_SHOW);
	UpdateWindow(g_hwndMain);


	CreateDialog(g_hiApp, MAKEINTRESOURCE(IDD_DIALOG), g_hwndMain, DlgProc);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass(g_pczMainWndClassName, g_hiApp);

	return msg.wParam;
}

// --- labDlg.cpp ---
