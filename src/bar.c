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
#include <windowsx.h>
#include <w32bindkeys/logger.h>

#include "ws.h"

static wbk_logger_t logger = { "bar" };

/**
 * Communication structure for b3_bar_draw() and b3_bar_draw_ws_visitor().
 */
typedef struct b3_bar_draw_comm_s
{
  b3_bar_t *bar;
  b3_ws_t *focused_ws;
  HDC hdc;
  PAINTSTRUCT ps;
  RECT rect;
  RECT text_rect;
  HBRUSH background_brush;
  HBRUSH focused_monitor_ws_brush;
  HBRUSH focused_ws_brush;
} b3_bar_draw_comm_t;

/**
 * Communication structure for b3_bar_handle_mouse_click() and
 * b3_bar_click_ws_visitor().
 */
typedef struct b3_bar_click_comm_s
{
    b3_bar_t *bar;
    b3_ws_t *focused_ws;
    HDC hdc;
    RECT rect;
    RECT text_rect;
    int x;
    int y;
} b3_bar_click_comm_t;

/**
 * Belongs to b3_bar_draw() and b3_bar_draw_ws_visitor(). Do not set it
 * somewhere else!
 */
static b3_bar_draw_comm_t g_draw_comm;

/**
 * Belongs to b3_bar_click() and b3_bar_click_ws_visitor_click(). Do not set it
 * somewhere else!
 */
static b3_bar_click_comm_t g_click_comm;

/**
 * Creates the window that will be used to paint the status bar on.
 */
static int
b3_bar_create_window(b3_bar_t *bar, const char *monitor_name);

/**
 * Draws the status bar.
 */
static int
b3_bar_draw(b3_bar_t *bar, HWND window_handler);

/**
 * Window procedure attached to the window created by b3_bar_create_window(). It
 * is used to trigger the drawing of the status bar.
 */
static LRESULT
CALLBACK b3_bar_WndProc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * Visitor used in b3_bar_draw() to draw each available workspace.
 */
static void
b3_bar_draw_ws_visitor(b3_ws_t *ws);

/**
 * Visitor used in b3_bar_handle_mouse_click() to check if the mouse click was
 * performed in a rectangle of/for a workspace.
 */
static void
b3_bar_click_ws_visitor(b3_ws_t *ws);

/**
 * Returns the rectangle of the area accommondated by the b3 bar on the Windows'
 * screen.
 */
static RECT
b3_bar_get_canvas_area(b3_bar_t *bar);

/**
 * Returns the rectangle of the area in which the workspace info are drawn. It
 * starts from the left.
 *
 * This rectangle is relative to b3_bar_get_canvas_area().
 */
static RECT
b3_bar_get_workspace_area(b3_bar_t *bar);

/**
 * Returns the rectangle of the area in which one workspace name is drawn. It
 * starts from the left.
 *
 * This rectangle is relative to b3_bar_get_workspace_area().
 */
static RECT
b3_bar_get_workspace_text_area(b3_bar_t *bar);

b3_bar_t *
b3_bar_new(const char *monitor_name,
		   RECT monitor_area,
		   b3_wsman_t *wsman,
		   b3_ws_switcher_t *ws_switcher)
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

    bar->ws_switcher = ws_switcher;

	b3_bar_create_window(bar, monitor_name);

	return bar;
}

int
b3_bar_free(b3_bar_t *bar)
{
	bar->wsman = NULL;

    b3_ws_switcher_free(bar->ws_switcher);
    bar->ws_switcher = NULL;

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

void
b3_bar_draw_ws_visitor(b3_ws_t *ws)
{
  SIZE text_size;
  int str_length;

  /**
   * Enlarge the rectangle to enclose the workspace's name
   */
  GetTextExtentPoint32A(g_draw_comm.hdc, b3_ws_get_name(ws), strlen(b3_ws_get_name(ws)), &text_size);
  str_length = text_size.cx + B3_BAR_DEFAULT_PADDING_TO_FRAME;
  g_draw_comm.rect.right = g_draw_comm.rect.left + str_length + B3_BAR_DEFAULT_PADDING_TO_FRAME;

  g_draw_comm.text_rect.left = g_draw_comm.rect.left + B3_BAR_DEFAULT_PADDING_TO_FRAME;
  g_draw_comm.text_rect.right = g_draw_comm.rect.right - B3_BAR_DEFAULT_PADDING_TO_FRAME;

  if (strcmp(b3_ws_get_name(ws),
             b3_ws_get_name(g_draw_comm.focused_ws)) == 0) {
    if (b3_bar_is_focused(g_draw_comm.bar)) {
      FillRect(g_draw_comm.hdc, &g_draw_comm.rect, g_draw_comm.focused_monitor_ws_brush);
    } else {
      FillRect(g_draw_comm.hdc, &g_draw_comm.rect, g_draw_comm.focused_ws_brush);
    }
  } else {
    Rectangle(g_draw_comm.hdc,
              g_draw_comm.rect.left, g_draw_comm.rect.top,
              g_draw_comm.rect.right, g_draw_comm.rect.bottom);
  }

  DrawText(g_draw_comm.hdc, b3_ws_get_name(ws),
           -1,
           &g_draw_comm.text_rect,
           DT_CENTER | DT_SINGLELINE | DT_VCENTER);

  /**
   * Move the rectangle for the next workspace
   */
  g_draw_comm.rect.left = g_draw_comm.rect.right + B3_BAR_DEFAULT_PADDING_TO_NEXT_FRAME;
}

int
b3_bar_draw(b3_bar_t *bar, HWND window_handler)
{
    // g_draw_comm = { 0 };

	g_draw_comm.bar = bar;

	g_draw_comm.focused_ws = b3_wsman_get_focused_ws(bar->wsman);

	g_draw_comm.hdc = GetDC(window_handler);
	BeginPaint(window_handler, &g_draw_comm.ps);

	/** Init everything */
	g_draw_comm.background_brush = CreateSolidBrush(RGB(255, 255, 255));
	g_draw_comm.focused_monitor_ws_brush = CreateSolidBrush(RGB(255, 0, 0));
	g_draw_comm.focused_ws_brush = CreateSolidBrush(RGB(100, 100, 100));

	g_draw_comm.rect = b3_bar_get_canvas_area(bar);
	FillRect(g_draw_comm.hdc, &g_draw_comm.rect, g_draw_comm.background_brush);

	g_draw_comm.rect = b3_bar_get_workspace_area(bar);

	g_draw_comm.text_rect = b3_bar_get_workspace_text_area(bar);

  b3_wsman_iterate_ws_arr(bar->wsman, b3_bar_draw_ws_visitor);

	DeleteObject(g_draw_comm.background_brush);
	DeleteObject(g_draw_comm.focused_monitor_ws_brush);
	DeleteObject(g_draw_comm.focused_ws_brush);

   	EndPaint(window_handler, &g_draw_comm.ps);
    ReleaseDC(window_handler, g_draw_comm.hdc);

	return 0;
}

LRESULT CALLBACK
b3_bar_WndProc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT CALLBACK result;
	b3_bar_t *bar;
    int x;
    int y;

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
        case WM_LBUTTONDOWN:
            x = GET_X_LPARAM(lParam);
            y = GET_Y_LPARAM(lParam);
            b3_bar_handle_mouse_click(bar, x, y);
            break;

		case WM_CLOSE:
			DestroyWindow(window_handler);
			break;

		default:
			result = DefWindowProc(window_handler, msg, wParam, lParam);
	}

	return result;
}

void
b3_bar_click_ws_visitor(b3_ws_t *ws)
{
    SIZE text_size;
    int str_length;
    POINT point;

    /**
     * Enlarge the rectangle to enclose the workspace's name
     */
    GetTextExtentPoint32A(g_click_comm.hdc, b3_ws_get_name(ws),
                          strlen(b3_ws_get_name(ws)), &text_size);
    str_length = text_size.cx + B3_BAR_DEFAULT_PADDING_TO_FRAME;
    g_click_comm.rect.right =
        g_click_comm.rect.left + str_length + B3_BAR_DEFAULT_PADDING_TO_FRAME;

    point.x = g_click_comm.x;
    point.y = g_click_comm.y;
    if (PtInRect(&(g_click_comm.rect), point)) {
        b3_ws_switcher_switch_to_ws(g_click_comm.bar->ws_switcher, b3_ws_get_name(ws));
    }

    /**
     * Move the rectangle for the next workspace
     */
    g_click_comm.rect.left =
        g_click_comm.rect.right + B3_BAR_DEFAULT_PADDING_TO_NEXT_FRAME;
}
int
b3_bar_handle_mouse_click(b3_bar_t *bar, int x, int y)
{
    // g_click_comm = { 0 };

    g_click_comm.bar = bar;

    g_click_comm.focused_ws = b3_wsman_get_focused_ws(bar->wsman);

    g_click_comm.hdc = GetDC(bar->window_handler);

    g_click_comm.rect = b3_bar_get_canvas_area(bar);

    g_click_comm.rect = b3_bar_get_workspace_area(bar);

    g_click_comm.text_rect = b3_bar_get_workspace_text_area(bar);

    g_click_comm.x = x;
    g_click_comm.y = y;

    b3_wsman_iterate_ws_arr(bar->wsman, b3_bar_click_ws_visitor);

    ReleaseDC(bar->window_handler, g_draw_comm.hdc);

    return 0;
}

RECT
b3_bar_get_canvas_area(b3_bar_t *bar)
{
    RECT rect;

    rect.top = 0;
	rect.bottom = bar->area.bottom - bar->area.top;
	rect.left = 0;
	rect.right = bar->area.right - bar->area.left;

    return rect;
}

RECT
b3_bar_get_workspace_area(b3_bar_t *bar)
{
    RECT rect;

    rect.top = B3_BAR_DEFAULT_PADDING_TO_WINDOW;
	rect.bottom = bar->area.bottom - bar->area.top - B3_BAR_DEFAULT_PADDING_TO_WINDOW;
	rect.left = 0; /** We are drawing relative to the window! */
	rect.right = 0; /** Will be overwritten */

    return rect;
}

RECT
b3_bar_get_workspace_text_area(b3_bar_t *bar)
{
    RECT rect;

    rect = b3_bar_get_workspace_area(bar);

    rect.top = g_draw_comm.rect.top + B3_BAR_DEFAULT_PADDING_TO_FRAME;
	rect.bottom = g_draw_comm.rect.bottom - B3_BAR_DEFAULT_PADDING_TO_FRAME;

    return rect;
}
