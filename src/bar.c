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
#include <w32bindkeys/logger.h>

#include "ws.h"

static wbk_logger_t logger = { "bar" };

static int
b3_bar_create_window(b3_bar_t *bar, const char *monitor_name);

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

	bar->position = B3_BAR_DEFAULT_POS;

	bar->area.top    = monitor_area.top;
	bar->area.bottom = monitor_area.top + B3_BAR_DEFAULT_BAR_HEIGHT;
	bar->area.left   = monitor_area.left;
	bar->area.right  = monitor_area.right;

	bar->wsman = wsman;

	b3_bar_create_window(bar, monitor_name);

	return bar;
}

int
b3_bar_free(b3_bar_t *bar)
{
	bar->wsman = NULL;

	DestroyWindow(bar->window_handler);
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

char
b3_bar_is_focused(b3_bar_t *bar)
{
	return bar->focused;
}

int
b3_bar_set_focused(b3_bar_t *bar, char focused)
{
	bar->focused = focused;
	return 0;
}

int
b3_bar_create_window(b3_bar_t *bar, const char *monitor_name)
{
	int error;
	WNDCLASSEX wc;
	HINSTANCE hInstance;
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
											 0, 0,
											 100, 100,
											 NULL, NULL, hInstance, bar);
	}

	if (!error) {
		SetWindowLong(bar->window_handler, GWL_STYLE, 0);
		SetWindowLongPtr(bar->window_handler, GWLP_USERDATA, (LONG_PTR) bar);

		b3_bar_hide(bar);
	}

	if (win_class) {
		free(win_class);
	}

	return error;
}

int
b3_bar_show(b3_bar_t *bar)
{
	RECT area;

	area = bar->area;

	wbk_logger_log(&logger, DEBUG,
				   "bar showing at - X: %d -> %d, Y: %d -> %d\n",
				   area.left, area.right - area.left,
				   area.top, area.bottom - area.top);

	SetWindowPos(bar->window_handler,
				 HWND_TOPMOST,
				 area.left, area.top,
				 area.right - area.left, area.bottom - area.top,
				 SWP_NOACTIVATE | SWP_NOSENDCHANGING);

	UpdateWindow(bar->window_handler);
	ShowWindow(bar->window_handler, SW_SHOW);
	return 0;
}

int
b3_bar_hide(b3_bar_t *bar)
{
	ShowWindow(bar->window_handler, SW_HIDE);
	return 0;
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
	HBRUSH background_brush;
	HBRUSH focused_monitor_ws_brush;
	HBRUSH focused_ws_brush;
	b3_ws_t *focused_ws;
	int i;
	int arr_length;
	int str_length;

	focused_ws = b3_wsman_get_focused_ws(bar->wsman);

	hdc = GetDC(window_handler);
	BeginPaint(window_handler, &ps);

	/** Init everything */
	background_brush = CreateSolidBrush(RGB(255, 255, 255));
	focused_monitor_ws_brush = CreateSolidBrush(RGB(255, 0, 0));
	focused_ws_brush = CreateSolidBrush(RGB(100, 100, 100));

	rect.top = 0;
	rect.bottom = bar->area.bottom - bar->area.top;
	rect.left = 0;
	rect.right = bar->area.right - bar->area.left;
	FillRect(hdc, &rect, background_brush);

	rect.top = B3_BAR_DEFAULT_PADDING_TO_WINDOW;
	rect.bottom = bar->area.bottom - bar->area.top - B3_BAR_DEFAULT_PADDING_TO_WINDOW;
	rect.left = 0; /** We are drawing relative to the window! */
	rect.right = 0; /** Will be overwritten */

	text_rect.top = rect.top + B3_BAR_DEFAULT_PADDING_TO_FRAME;
	text_rect.bottom = rect.bottom - B3_BAR_DEFAULT_PADDING_TO_FRAME;

	arr_length = array_size(b3_wsman_get_ws_arr(bar->wsman));
	for (i = 0; i < arr_length; i++) {
		array_get_at(b3_wsman_get_ws_arr(bar->wsman), i, (void *) &ws);
		GetTextExtentPoint32A(hdc, b3_ws_get_name(ws), strlen(b3_ws_get_name(ws)), &text_size);
		str_length = text_size.cx + B3_BAR_DEFAULT_PADDING_TO_FRAME;
		rect.right = rect.left + str_length + B3_BAR_DEFAULT_PADDING_TO_FRAME;

		text_rect.left = rect.left + B3_BAR_DEFAULT_PADDING_TO_FRAME;
		text_rect.right = rect.right - B3_BAR_DEFAULT_PADDING_TO_FRAME;

		if (strcmp(b3_ws_get_name(ws), b3_ws_get_name(focused_ws)) == 0) {
			if (b3_bar_is_focused(bar)) {
				FillRect(hdc, &rect, focused_monitor_ws_brush);
			} else {
				FillRect(hdc, &rect, focused_ws_brush);
			}
		} else {
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		}

		DrawText(hdc, b3_ws_get_name(ws), -1, &text_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		rect.left = rect.right + B3_BAR_DEFAULT_PADDING_TO_NEXT_FRAME;
	}

	DeleteObject(background_brush);
	DeleteObject(focused_monitor_ws_brush);
	DeleteObject(focused_ws_brush);

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
			if (bar != NULL) {
				if (b3_wsman_any_win_has_state(bar->wsman, MAXIMIZED)) {
					b3_bar_hide(bar);
				} else {
					b3_bar_show(bar);
					b3_bar_draw(bar, window_handler);
				}
			}
			break;

		case WM_CLOSE:
			DestroyWindow(window_handler);
			break;

		default:
			result = DefWindowProc(window_handler, msg, wParam, lParam);
	}

	return result;
}
