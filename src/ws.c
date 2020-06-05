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

#include "ws.h"

#include <string.h>
#include <wbkbase/logger.h>
#include <windows.h>

#include "win.h"

static wbk_logger_t logger = { "ws" };

static HANDLE g_get_win_amount_mutex = NULL;
static int g_get_win_amount;

static HANDLE g_first_leaf_mutex = NULL;
static b3_winman_t *g_first_leaf;

static HANDLE g_active_win_toggle_floating_mutex = NULL;
static const b3_win_t *g_active_win_toggle_floating_win;
static int g_active_win_toggle_floating_toggle_failed;

static HANDLE g_set_focused_win_found_mutex = NULL;
static const b3_win_t *g_set_focused_win_win;
static b3_win_t *g_set_focused_win_found;

static HANDLE g_arrange_wins_mutex = NULL;
static RECT g_arrange_wins_monitor_area;

static void
b3_ws_winman_amount_visitor(b3_winman_t *winman);

static void
b3_ws_winman_first_leaf(b3_winman_t *winman);

static void
b3_ws_winman_active_win_toggle_floating_visitor(b3_winman_t *winman);

static void
b3_ws_winman_arrange_visitor(b3_winman_t *winman);

static void
b3_ws_winman_minimize_visitor(b3_winman_t *winman);

static void
b3_ws_winman_set_focused_win_visitor(b3_winman_t *winman);

b3_ws_t *
b3_ws_new(const char *name)
{
	b3_ws_t *ws;

	if (g_get_win_amount_mutex == NULL) g_get_win_amount_mutex = CreateMutex(NULL, FALSE, NULL);
	if (g_first_leaf_mutex == NULL) g_first_leaf_mutex = CreateMutex(NULL, FALSE, NULL);
	if (g_active_win_toggle_floating_mutex == NULL) g_active_win_toggle_floating_mutex = CreateMutex(NULL, FALSE, NULL);
	if (g_set_focused_win_found_mutex == NULL) g_set_focused_win_found_mutex  = CreateMutex(NULL, FALSE, NULL);
	if (g_arrange_wins_mutex == NULL) g_arrange_wins_mutex = CreateMutex(NULL, FALSE, NULL);

	ws = NULL;
	ws = malloc(sizeof(b3_ws_t));

	ws->winman = b3_winman_new(HORIZONTAL);

	ws->mode = DEFAULT;

	b3_ws_set_name(ws, name);

	ws->focused_win = NULL;

	return ws;
}

int
b3_ws_free(b3_ws_t *ws)
{
	b3_winman_free(ws->winman);
	ws->winman = NULL;

	free(ws->name);
	ws->name = NULL;

	ws->focused_win = NULL;

	free(ws);
	return 0;
}

int
b3_ws_get_win_amount(b3_ws_t *ws)
{
	int amount;

	WaitForSingleObject(g_get_win_amount_mutex, INFINITE);

	g_get_win_amount = 0;
	b3_winman_traverse(ws->winman, b3_ws_winman_amount_visitor);
	amount = g_get_win_amount;

	ReleaseMutex(g_get_win_amount_mutex);

	return amount;
}

int
b3_ws_add_win(b3_ws_t *ws, b3_win_t *win)
{
	b3_winman_t *winman;
	int ret;

	ret = 1;
	winman = b3_winman_contains_win(ws->winman, win);
	if (!winman) {
		if (ws->focused_win) {
			wbk_logger_log(&logger, DEBUG, "Adding window to workspace %s - unfocused\n", b3_ws_get_name(ws));

			winman = b3_winman_contains_win(ws->winman, ws->focused_win);
		} else {
			wbk_logger_log(&logger, DEBUG, "Adding window to workspace %s - focused\n", b3_ws_get_name(ws));

			WaitForSingleObject(g_first_leaf_mutex, INFINITE);

			g_first_leaf = NULL;
			b3_winman_traverse(ws->winman, b3_ws_winman_first_leaf);
			winman = g_first_leaf;

			ws->focused_win = win;

			ReleaseMutex(g_first_leaf_mutex);
		}

		if (winman) {
			ret = b3_winman_add_win(winman, win);
		}
	}

	return ret;
}

int
b3_ws_remove_win(b3_ws_t *ws, const b3_win_t *win)
{
	int remove_failed;

	remove_failed = b3_winman_remove_win(ws->winman, win);
	if (!remove_failed) {
		if (b3_win_compare(win, ws->focused_win) == 0) {
			wbk_logger_log(&logger, DEBUG, "Removing window from workspace %s - change focus\n", b3_ws_get_name(ws));
			ws->focused_win = b3_winman_get_next_window(ws->winman);
		} else {
			wbk_logger_log(&logger, DEBUG, "Removing window from workspace %s\n", b3_ws_get_name(ws));
		}
	}

	return remove_failed;
}

b3_win_t *
b3_ws_contains_win(b3_ws_t *ws, const b3_win_t *win)
{
	b3_winman_t *winman;
	b3_win_t *found_win;

	found_win = NULL;
	winman = b3_winman_contains_win(ws->winman, win);
	if (winman) {
		found_win = b3_winman_contains_win_leaf(ws->winman, win);
	}

	return found_win;
}

int
b3_ws_active_win_toggle_floating(b3_ws_t *ws, const b3_win_t *win)
{
	int toggle_failed;

	WaitForSingleObject(g_active_win_toggle_floating_mutex, INFINITE);

	g_active_win_toggle_floating_win = win;
	g_active_win_toggle_floating_toggle_failed = 1;
	b3_winman_traverse(ws->winman, b3_ws_winman_active_win_toggle_floating_visitor);
	toggle_failed = g_active_win_toggle_floating_toggle_failed;

	ReleaseMutex(g_active_win_toggle_floating_mutex);

	return toggle_failed;
}

int
b3_ws_arrange_wins(b3_ws_t *ws, RECT monitor_area)
{

	WaitForSingleObject(g_arrange_wins_mutex, INFINITE);

	g_arrange_wins_monitor_area = monitor_area;
	b3_winman_traverse(ws->winman, b3_ws_winman_arrange_visitor);

	ReleaseMutex(g_arrange_wins_mutex);

    return 0;
}

int
b3_ws_minimize_wins(b3_ws_t *ws)
{
	b3_winman_traverse(ws->winman, b3_ws_winman_minimize_visitor);

    return 0;
}

b3_til_mode_t
b3_ws_get_mode(b3_ws_t *ws)
{
	return ws->mode;
}

int
b3_ws_set_mode(b3_ws_t *ws, b3_til_mode_t mode)
{
	ws->mode = mode;
	return 0;
}

int
b3_ws_set_name(b3_ws_t *ws, const char *name)
{
	int length;

	length = strlen(name) + 1;
	ws->name = malloc(sizeof(char) * length);
	strcpy(ws->name, name);

	return 0;
}

const char*
b3_ws_get_name(b3_ws_t *ws)
{
	return ws->name;
}

b3_win_t *
b3_ws_get_focused_win(b3_ws_t *ws)
{
	return ws->focused_win;
}

int
b3_ws_set_focused_win(b3_ws_t *ws, const b3_win_t *win)
{
	b3_win_t *set_focused_win_found;
	int error;

	WaitForSingleObject(g_set_focused_win_found_mutex, INFINITE);

	g_set_focused_win_win = win;
	g_set_focused_win_found = NULL;
	b3_winman_traverse(ws->winman, b3_ws_winman_set_focused_win_visitor);
	set_focused_win_found = g_set_focused_win_found;

	ReleaseMutex(g_set_focused_win_found_mutex);

	error = 1;
	if (set_focused_win_found) {
		ws->focused_win = set_focused_win_found;
		wbk_logger_log(&logger, DEBUG, "Updating focused window on workspace %s\n", ws->name);
		error = 0;
	}

	return error;
}

int
b3_ws_move_active_win(b3_ws_t *ws, b3_ws_move_direction_t direction)
{
	int error;
	b3_winman_t *winman;
	b3_win_t *win;
	int i;
	int length;
	int pos;

	error = 1;
	winman = b3_winman_contains_win(ws->winman, ws->focused_win);
	if (winman) {
		if ((ws->mode == DEFAULT || ws->mode == TABBED)
			 && (direction == LEFT || direction == RIGHT)) {
			pos = -1;
			length = array_size(b3_winman_get_win_arr(winman));
			for (i = 0; pos < 0 && i < length; i++) {
				array_get_at(b3_winman_get_win_arr(winman), i, (void *) &win);
				if (b3_win_compare(win, ws->focused_win) == 0) {
					pos = i;
				}
			}

			if ((direction == LEFT && pos > 0)
				|| (direction == RIGHT && pos < length - 1)) {
				array_remove_at(b3_winman_get_win_arr(winman), pos, NULL);
				if (direction == LEFT) {
					pos--;
					wbk_logger_log(&logger, INFO, "Moved window left\n");
				} else {
					pos++;
					wbk_logger_log(&logger, INFO, "Moved window right\n");
				}
				array_add_at(b3_winman_get_win_arr(winman), win, pos);
			}
		}
	}

	return error;
}

b3_win_t *
b3_ws_get_win(b3_ws_t *ws, b3_ws_move_direction_t direction)
{
	b3_winman_t *winman;
	ArrayIter iter;
	b3_win_t *win;
	int length;
	int i;
	int pos;

	win = NULL;
	winman = b3_winman_contains_win(ws->winman, ws->focused_win);
	if (winman) {
		if ((ws->mode == DEFAULT || ws->mode == TABBED)
			 && (direction == LEFT || direction == RIGHT)) {
			pos = -1;
			length = array_size(b3_winman_get_win_arr(winman));
			for (i = 0; pos < 0 && i < length; i++) {
				array_get_at(b3_winman_get_win_arr(winman), i, (void *) &win);
				if (b3_win_compare(win, ws->focused_win) == 0) {
					pos = i;
				}
			}

			win = NULL;

			if ((direction == LEFT && pos > 0)
				|| (direction == RIGHT && pos < length - 1)) {
				if (direction == LEFT) {
					pos--;
					wbk_logger_log(&logger, INFO, "Got window left\n");
				} else {
					pos++;
					wbk_logger_log(&logger, INFO, "Got window right\n");
				}
				array_get_at(b3_winman_get_win_arr(winman), pos, (void *) &win);
			}
		}
	}

	return win;
}

void
b3_ws_winman_amount_visitor(b3_winman_t *winman)
{
	 if (winman->type == LEAF) {
		 g_get_win_amount += array_size(b3_winman_get_win_arr(winman));
	 }
}

void
b3_ws_winman_first_leaf(b3_winman_t *winman)
{
	if (winman->type == LEAF && g_first_leaf == NULL) {
		g_first_leaf = winman;
	}
}

void
b3_ws_winman_active_win_toggle_floating_visitor(b3_winman_t *winman)
{
	char floating;
	ArrayIter iter;
	b3_win_t *win_iter;

	if (winman->type == LEAF) {
		array_iter_init(&iter, b3_winman_get_win_arr(winman));
		while (g_active_win_toggle_floating_toggle_failed && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, g_active_win_toggle_floating_win) == 0) {
				floating = b3_win_get_floating(win_iter);
				b3_win_set_floating(win_iter, !floating);
				g_active_win_toggle_floating_toggle_failed = 0;
			}
		}
	}
}

void
b3_ws_winman_arrange_visitor(b3_winman_t *winman)
{
	ArrayIter iter;
	b3_win_t *win_iter;
	int length;
	int split_size;
	int width;
	int height;

	if (winman->type == LEAF) {
		length = 0;
		array_iter_init(&iter, b3_winman_get_win_arr(winman));
		while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (!b3_win_get_floating(win_iter)) {
				length++;
			}
		}
		if (length) {
			split_size =  g_arrange_wins_monitor_area.right - g_arrange_wins_monitor_area.left;
			split_size /= length;

			width = g_arrange_wins_monitor_area.left;
			height = g_arrange_wins_monitor_area.bottom - g_arrange_wins_monitor_area.top;

			array_iter_init(&iter, b3_winman_get_win_arr(winman));
			while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
				if (!b3_win_get_floating(win_iter)) {
					ShowWindow(win_iter->window_handler, SW_SHOWNOACTIVATE);
					SetWindowPos(win_iter->window_handler, HWND_TOP,
								 width, g_arrange_wins_monitor_area.top,
								 split_size, height, SWP_NOACTIVATE);
					width += split_size;
				}
			}

			array_iter_init(&iter, b3_winman_get_win_arr(winman));
			while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
				if (b3_win_get_floating(win_iter)) {
					b3_win_show(win_iter);
				}
			}
		}
	}
}

void
b3_ws_winman_minimize_visitor(b3_winman_t *winman)
{
	ArrayIter iter;
	b3_win_t *win_iter;

	if (winman->type == LEAF) {
		array_iter_init(&iter, b3_winman_get_win_arr(winman));
		while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			b3_win_minimize(win_iter);
		}
	}
}

void
b3_ws_winman_set_focused_win_visitor(b3_winman_t *winman)
{
	ArrayIter iter;
	b3_win_t *win_iter;

	if (winman->type == LEAF) {
		array_iter_init(&iter, b3_winman_get_win_arr(winman));
		while (!g_set_focused_win_found && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, g_set_focused_win_win) == 0) {
				g_set_focused_win_found = win_iter;
			}
		}
	}
}

