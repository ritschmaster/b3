/******************************************************************************
  This file is part of b3.

  Copyright 2020 Richard Paul Baeck <richard.baeck@mailbox.org>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*******************************************************************************/

#include <stdio.h>
#include <collectc/array.h>
#include <windows.h>
#include <wbkbase/logger.h>

#include "wsman.h"

static wbk_logger_t logger = { "main" };

static const char g_szClassName[] = "myWindowClass";

static b3_wsman_t *g_wsman;

static int
main_loop(HINSTANCE hInstance, int nCmdShow);

static LRESULT CALLBACK
window_callback(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	wbk_logger_set_level(SEVERE);
	wbk_logger_set_level(INFO);

	g_wsman = b3_wsman_new();
	if (g_wsman) {
		b3_wsman_refresh(g_wsman);
		main_loop(hInstance, nCmdShow);

		b3_wsman_free(g_wsman);
	}

	return 0;
}

int
main_loop(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;
	PTITLEBARINFO titlebar_info;

	wc.cbSize		= sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc   = window_callback;
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
		return 1;
	}

//	hwnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_TOPMOST,
//		                  g_szClassName,
//						  "b3",
//						  WS_DISABLED | WS_BORDER,
//						  0, 0,
//						  1920, 10,
//						  NULL, NULL, hInstance, NULL);
//	SetWindowPos(hwnd,
//	             HWND_TOPMOST,
//		     0, -20,
//		     1920, 10,
//		     SWP_NOACTIVATE);


//	ShowWindow(hwnd, nCmdShow);
//	UpdateWindow(hwnd);

	b3_wsman_show(g_wsman);

	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return 0;
}

LRESULT CALLBACK
window_callback(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT CALLBACK result;
	int id;

	result = 0;

	switch(msg)
	{
	case WM_NCPAINT:
		b3_wsman_draw(g_wsman, window_handler);
		break;

	case WM_CLOSE:
		DestroyWindow(window_handler);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_HOTKEY:
		break;

	default:
		result = DefWindowProc(window_handler, msg, wParam, lParam);
	}

	return result;
}
