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

#include "bar.h"

#include <stdlib.h>
#include <string.h>
#include <collectc/hashtable.h>
#include <collectc/array.h>
#include <windows.h>
#include <wbkbase/logger.h>

#include "ws.h"

static wbk_logger_t logger = { "bar" };

static int
b3_bar_draw(b3_bar_t *bar, HWND window_handler);

static LRESULT
CALLBACK b3_bar_WndProc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

b3_bar_t *
b3_bar_new(const char *monitor_name, RECT monitor_area, b3_wsman_t *wsman)
{
	b3_bar_t *bar;

	bar = NULL;
	bar = malloc(sizeof(b3_bar_t));

	bar->position = TOP; // TODO default

	bar->area.top    = monitor_area.top;
	bar->area.bottom = B3_BAR_DEFAULT_BAR_HEIGHT;
	bar->area.left   = monitor_area.left;
	bar->area.right  = monitor_area.right;

	bar->wsman = wsman;

	bar->height= B3_BAR_DEFAULT_BAR_HEIGHT;

	return bar;
}

int
b3_bar_free(b3_bar_t *bar)
{
	bar->wsman = NULL;

	bar->window_handler = NULL;

	free(bar);
	return 0;
}

RECT
b3_bar_get_area(b3_bar_t *bar)
{
	return bar->area;
}

b3_bar_pos_t
b3_bar_get_position(b3_bar_t *bar)
{
	return bar->position;
}
#include <stdio.h>
int
b3_bar_create_window(b3_bar_t *bar, const char *monitor_name)
{
	int error;
	WNDCLASSEX wc;
	HINSTANCE hInstance;
	TITLEBARINFO titlebar_info;
	int monitor_name_len;
	int titlebar_height;
	char *win_class;

	error = 0;
	win_class = NULL;

	if (!error) {
		hInstance = GetModuleHandle(NULL);

		monitor_name_len = strlen(monitor_name);
		win_class = malloc(sizeof(char) * (B3_BAR_WIN_NAME_LEN + monitor_name_len + 1));
		strcpy(win_class, B3_BAR_WIN_NAME);
		strcpy(win_class + B3_BAR_WIN_NAME_LEN, monitor_name);
		win_class[B3_BAR_WIN_NAME_LEN + monitor_name_len] = '\0';

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
		wc.lpszClassName = win_class;
		wc.hIconSm	   = LoadIcon(NULL, IDI_APPLICATION);

		if(!RegisterClassEx(&wc)) {
			error = 1;
		}
	}

	if (!error) {
		bar->window_handler = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_TOPMOST,
											 win_class,
											 B3_BAR_WIN_NAME,
											 WS_DISABLED | WS_BORDER,
											 bar->area.left, 20,
											 100, 100,
											 NULL, NULL, hInstance, bar);
	}

	if (!error) {
		titlebar_info.cbSize = sizeof(TITLEBARINFO);
		GetTitleBarInfo(bar->window_handler, &titlebar_info);
		titlebar_height = titlebar_info.rcTitleBar.bottom - titlebar_info.rcTitleBar.top;
		titlebar_height *= 1.3;
	}

	if (!error) {
		SetWindowLongPtr(bar->window_handler, GWLP_USERDATA, (LONG_PTR) bar);

		SetWindowPos(bar->window_handler,
					 HWND_TOPMOST,
					 bar->area.left, bar->area.top - 20,
					 bar->area.right, titlebar_height + bar->height,
					 SWP_NOACTIVATE | SWP_NOSENDCHANGING);

		UpdateWindow(bar->window_handler);
		ShowWindow(bar->window_handler, SW_SHOW);
	}

	if (win_class) {
		free(win_class);
	}

	return error;
}

int
b3_bar_draw(b3_bar_t *bar, HWND window_handler)
{
	HDC hdc;
	PAINTSTRUCT ps;
	ArrayIter iter;
	b3_ws_t *ws;
	RECT rect;
	RECT text_rect;
	SIZE text_size;
	HBRUSH brush;
	b3_ws_t *focused_ws;
	int i;
	int arr_length;
	int str_length;

	focused_ws = b3_wsman_get_focused_ws(bar->wsman);

	hdc = GetDC(window_handler);
	BeginPaint(window_handler, &ps);

	/** Init everything */
	brush = CreateSolidBrush(RGB(255, 255, 255));
	rect.top = bar->area.top;
	rect.bottom = bar->area.bottom;
	rect.left = 0; /** We are drawing relative to the window! */
	rect.right = bar->area.right - bar->area.left;
	FillRect(hdc, &rect, brush);
	DeleteObject(brush);

	rect.top = bar->area.top;
	rect.bottom = bar->area.bottom;
	rect.left = 0; /** We are drawing relative to the window! */
	rect.right = 0; /** Will be overwritten */

	text_rect.top = rect.top + B3_BAR_BORDER_TO_TEXT_DISTANCE;
	text_rect.bottom = rect.bottom - B3_BAR_BORDER_TO_TEXT_DISTANCE;

	brush = CreateSolidBrush(RGB(255, 0, 0));
	arr_length = array_size(b3_wsman_get_ws_arr(bar->wsman));
	for (i = 0; i < arr_length; i++) {
		array_get_at(b3_wsman_get_ws_arr(bar->wsman), i, (void *) &ws);
		GetTextExtentPoint32A(hdc, b3_ws_get_name(ws), strlen(b3_ws_get_name(ws)), &text_size);
		str_length = text_size.cx + B3_BAR_BORDER_TO_TEXT_DISTANCE;
		rect.right = rect.left + str_length + B3_BAR_BORDER_TO_TEXT_DISTANCE;

		text_rect.left = rect.left + B3_BAR_BORDER_TO_TEXT_DISTANCE;
		text_rect.right = rect.right - B3_BAR_BORDER_TO_TEXT_DISTANCE;

		if (strcmp(b3_ws_get_name(ws), b3_ws_get_name(focused_ws)) == 0) {
			FillRect(hdc, &rect, brush);
		} else {
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		}

		DrawText(hdc, b3_ws_get_name(ws), -1, &text_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		rect.left = rect.right + B3_BAR_WORKSPACE_INDICATOR_DISTANCE;
	}
	DeleteObject(brush);

   	EndPaint(window_handler, &ps);
    ReleaseDC(window_handler, hdc);

	return 0;
}

LRESULT CALLBACK
b3_bar_WndProc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT CALLBACK result;
	b3_bar_t *bar;

	result = 0;

    bar = (b3_bar_t *) GetWindowLongPtr(window_handler, GWLP_USERDATA);

	switch(msg)
	{
    	case WM_NCPAINT:
			if (bar != NULL) b3_bar_draw(bar, window_handler);
			break;

		case WM_CLOSE:
			DestroyWindow(window_handler);
			break;

		default:
			result = DefWindowProc(window_handler, msg, wParam, lParam);
	}

	return result;
}
