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
#include <collectc/hashtable.h>

typedef struct b3_wb_table_s
{
	/**
	 * HashTable of HWND and b3_bar_t *
	 */
	HashTable *wb_table;
} b3_wb_table_t;


static b3_wb_table_t *g_wb_table_instance = NULL;

static b3_wb_table_t *
b3_wb_table_new();

static int
b3_wb_table_free(b3_wb_table_t *wb_table);

static b3_wb_table_t *
b3_wb_table_get_instance();

static int
b3_wb_table_add_wb(b3_wb_table_t *wb_table, HWND window_handler, b3_bar_t *bar);

static HWND
b3_wb_table_get_w(b3_wb_table_t *wb_table, const b3_bar_t *bar);

static b3_bar_t *
b3_wb_table_get_bar(b3_wb_table_t *wb_table, HWND window_handler);

static int
b3_wb_table_remove_bar(b3_wb_table_t *wb_table, b3_bar_t *bar);

static int
b3_bar_draw(b3_bar_t *bar, HWND window_handler);

static LRESULT
CALLBACK b3_bar_WndProc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

b3_bar_t *
b3_bar_new(const char *monitor_name, RECT monitor_area)
{
	b3_bar_t *bar;

	bar = NULL;
	bar = malloc(sizeof(b3_bar_t));

	bar->area.top    = monitor_area.top;
	bar->area.bottom = DEFAULT_BAR_HEIGHT;
	bar->area.left   = monitor_area.left;
	bar->area.right  = monitor_area.right;

	return bar;
}

int
b3_bar_free(b3_bar_t *bar)
{
	b3_wb_table_t *wb_table;

	wb_table = b3_wb_table_get_instance();

	b3_wb_table_remove_bar(wb_table, bar);

	free(bar);
	return 0;
}

int
b3_bar_create_window(b3_bar_t *bar, const char *monitor_name)
{
	b3_wb_table_t *wb_table;
	int ret;
	WNDCLASSEX wc;
	HINSTANCE hInstance;
	HWND window_handler;

	wb_table = b3_wb_table_get_instance();

	ret = 0;

	if (b3_wb_table_get_w(wb_table, bar)) {
		ret = 1;
	}

	if (!ret) {
		hInstance = GetModuleHandle(NULL);

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
		wc.lpszClassName = monitor_name;
		wc.hIconSm	   = LoadIcon(NULL, IDI_APPLICATION);

		if(RegisterClassEx(&wc))
		{
			window_handler = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_TOPMOST,
												 monitor_name,
												 "b3 bar",
												 WS_DISABLED | WS_BORDER,
												 0, 0,
												 0, 0,
												 NULL, NULL, hInstance, bar);
			SetWindowPos(window_handler,
						 HWND_TOPMOST,
						 0, -20,
						 1920, 30 +DEFAULT_BAR_HEIGHT,
						 SWP_NOACTIVATE);

			UpdateWindow(window_handler);
			ShowWindow(window_handler, SW_SHOW);

			b3_wb_table_add_wb(wb_table, window_handler, bar);
		} else {
			ret = 2;
		}
	}

	return ret;
}

int
b3_bar_draw(b3_bar_t *bar, HWND window_handler)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	RECT text_rect;

	rect.top = bar->area.top;
	rect.bottom= 10;
	rect.left = bar->area.left;
	rect.right = rect.left + 100;

	text_rect.top = 0;
	text_rect.bottom = 10;
	text_rect.left = rect.right + 10;
	text_rect.right = text_rect.left + 100;

	hdc = GetDC(window_handler);
	BeginPaint(window_handler, &ps);

	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
   	DrawText(hdc, TEXT("Hello World!"), -1, &text_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

   	EndPaint(window_handler, &ps);
    ReleaseDC(window_handler, hdc);

	return 0;
}

#include <stdio.h>

LRESULT CALLBACK b3_bar_WndProc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT CALLBACK result;
	b3_wb_table_t *wb_table;
	b3_bar_t *bar;

	switch(msg)
	{
    	case WM_NCPAINT:
			fprintf(stdout, "a1\n");
			fflush(stdout);
			wb_table = b3_wb_table_get_instance();
    		bar = b3_wb_table_get_bar(wb_table, window_handler);
    		fprintf(stdout, "a2\n");
			fflush(stdout);
			if (bar != NULL) b3_bar_draw(bar, window_handler);
			break;

		case WM_CLOSE:
			//DestroyWindow(window_handler);
			break;

		default:
			result = DefWindowProc(window_handler, msg, wParam, lParam);
	}

	return result;
}

b3_wb_table_t *
b3_wb_table_new()
{
	b3_wb_table_t *wb_table;

	wb_table = malloc(sizeof(b3_wb_table_t));

	hashtable_new(&(wb_table->wb_table));

	return wb_table;
}

int
b3_wb_table_free(b3_wb_table_t *wb_table)
{
	hashtable_destroy(wb_table->wb_table);
	wb_table->wb_table = NULL;

	free(wb_table);

	return 0;
}

b3_wb_table_t *
b3_wb_table_get_instance()
{
	if (g_wb_table_instance == NULL) {
		g_wb_table_instance = b3_wb_table_new();
	}

	return g_wb_table_instance;
}

int
b3_wb_table_add_wb(b3_wb_table_t *wb_table, HWND window_handler, b3_bar_t *bar)
{
	hashtable_add(wb_table->wb_table, window_handler, bar);
}

HWND
b3_wb_table_get_w(b3_wb_table_t *wb_table, const b3_bar_t *bar)
{
	HashTableIter iter;
	TableEntry *entry;
	HWND window_handler;

	window_handler = NULL;
	hashtable_iter_init(&iter, wb_table->wb_table);
	while (!window_handler && hashtable_iter_next(&iter, &entry) != CC_ITER_END) {
		if (entry->value == bar) {
			window_handler = entry->key;
		}
	}

	return window_handler;
}

b3_bar_t *
b3_wb_table_get_bar(b3_wb_table_t *wb_table, HWND window_handler)
{
	b3_bar_t *bar;

	bar = NULL;
	hashtable_get(wb_table->wb_table, window_handler, (void *) &bar);

	return bar;
}

int
b3_wb_table_remove_bar(b3_wb_table_t *wb_table, b3_bar_t *bar)
{
	char found;
	HashTableIter iter;
	TableEntry *entry;

	found = 0;
	hashtable_iter_init(&iter, wb_table->wb_table);
	while (!found && hashtable_iter_next(&iter, &entry) != CC_ITER_END) {
		if (entry->value == bar) {
            hashtable_iter_remove(&iter, NULL);
            found = 1;
		}
	}

	if (found)
		return 0;
	else
		return 1;
}
