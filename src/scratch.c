// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmonitorinfoa
// https://docs.microsoft.com/de-at/windows/win32/gdi/using-rectangles
// https://stackoverflow.com/questions/16159127/win32-how-to-draw-a-rectangle-around-a-text-string#16160648

// RegisterHotKey
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey

#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <wingdi.h>

const char g_szClassName[] = "myWindowClass";

void OnNcPaint(HWND hwnd)
{
	HDC hdc;
	PAINTSTRUCT ps;   // paint data for Begin/EndPaint
	RECT rect;
	RECT text_rect;

	rect.top = 0;
	rect.bottom= 10;
	rect.left = 0;
	rect.right = rect.left + 100;

	text_rect.top = 0;
	text_rect.bottom = 10;
	text_rect.left = rect.right + 10;
	text_rect.right = text_rect.left + 100;

	BeginPaint(hwnd, &ps);
    	//hdc = GetDCEx(hwnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
	hdc = GetDC(hwnd);
    	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
   	DrawText(hdc, TEXT("Hello World!"), -1, &text_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
       	EndPaint(hwnd, &ps);
    	ReleaseDC(hwnd, hdc);
}

// Step 4: the Window Procedure
LRESULT CALLBACK b3_bar_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT CALLBACK result;

	result = 0;

	switch(msg)
	{
		case WM_NCPAINT:
			OnNcPaint(hwnd);
			break;

		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;

        	case WM_LBUTTONDOWN:
		break;

		case WM_SIZING:
		break;

		case WM_HOTKEY:
			fprintf(stdout, "Hotkey pressed\n");
			fflush(stdout);

		default:
			result = DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return result;
}


BOOL CALLBACK MyInfoEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MONITORINFOEX iMonitor;
    iMonitor.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &iMonitor);


	fprintf(stdout, "Found monitor:\n");
	fprintf(stdout, "%d\n", iMonitor.rcMonitor.left);
	fprintf(stdout, "%d\n", iMonitor.rcMonitor.right);
	fprintf(stdout, "%d\n", iMonitor.rcMonitor.top);
	fprintf(stdout, "%d\n", iMonitor.rcMonitor.bottom);
	fprintf(stdout, "\n");
	fflush(stdout);

    if (iMonitor.dwFlags == DISPLAY_DEVICE_MIRRORING_DRIVER)
    {
        return TRUE;
    }
    else
    {
        return TRUE;
    };
}

BOOL CALLBACK MyWindowEnumProc(HWND hwnd, LPARAM lParam)
{
	char display_window_info;
	wchar_t class_name[255];
	wchar_t title[255];

	display_window_info = 0;
	// display_window_info = 1;

	GetClassName(hwnd, class_name, 255);
	GetWindowText(hwnd, title, 255);

	if (display_window_info) {
		fprintf(stdout, "Found window:\n");
		fprintf(stdout, "%s\n", class_name);
		fprintf(stdout, "%s\n", title);
		fprintf(stdout, "\n");
		fflush(stdout);
	}

	/** Class names ending with Class are "normal windows" */

	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;
	PTITLEBARINFO titlebar_info;
	int titlebar_height;

	titlebar_height = 20;

	//Step 1: Registering the Window Class
	wc.cbSize		= sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc   = b3_bar_WndProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor	   = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm	   = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Step 2: Creating the Window
	hwnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_TOPMOST,
		g_szClassName,
		"b3 statusbar",
		WS_DISABLED | WS_BORDER,
		0, 0, 1920, 10,
		NULL, NULL, hInstance, NULL);

	SetWindowPos(hwnd,
	             HWND_TOPMOST,
		     0, titlebar_height * -1,
		     1920, 10,
		     SWP_NOACTIVATE);

//	GetTitleBarInfo(hwnd, titlebar_info);
//	fprintf(stdout, "Titlebar:\n");
//	fprintf(stdout, "%d\n", titlebar_info->rcTitleBar.top);
//	fprintf(stdout, "%d\n", titlebar_info->rcTitleBar.bottom);
//	fprintf(stdout, "%d\n", titlebar_info->rcTitleBar.left);
//	fprintf(stdout, "%d\n", titlebar_info->rcTitleBar.right);
//	fflush(stdout);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	/** Find all monitors */
	EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, 0);

	/** Find all windows */
	EnumWindows(MyWindowEnumProc, 0);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	/* Register keys */
	RegisterHotKey(hwnd, 100, MOD_WIN | MOD_CONTROL, 'H');
	RegisterHotKey(hwnd, 100, MOD_WIN | MOD_CONTROL, 'J');
	RegisterHotKey(hwnd, 100, MOD_WIN | MOD_CONTROL, 'K');
	RegisterHotKey(hwnd, 100, MOD_WIN | MOD_CONTROL, 'L');


	// Step 3: The Message Loop
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
