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

#include "win.h"

/**
 * @author Richard Bäck
 * @date 26 January 2020
 * @brief File contains the window class implementation and private methods
 */

#include <stdlib.h>
#include <string.h>
#include <wbkbase/logger.h>

static wbk_logger_t logger = { "win" };

b3_win_t *
b3_win_new(HWND window_handler, char floating)
{
	b3_win_t *win;

	win = NULL;
	win = malloc(sizeof(b3_win_t));

	win->window_handler = window_handler;

	win->floating = floating;

	GetWindowRect(window_handler, &(win->rect));

	return win;
}

b3_win_t *
b3_win_copy(const b3_win_t *win)
{
	b3_win_t * copy;

	copy = b3_win_new(win->window_handler, win->floating);

	return copy;
}

int
b3_win_free(b3_win_t *win)
{
	win->window_handler = NULL;

	free(win);

	return 0;
}

const char *
b3_win_get_title(b3_win_t *win)
{
	// TODO
	return NULL;
}

char
b3_win_get_floating(b3_win_t *win)
{
	return win->floating;
}

int
b3_win_set_floating(b3_win_t *win, char floating)
{
	win->floating = floating;
	return 0;
}

HWND
b3_win_get_window_handler(b3_win_t *win)
{
	return win->window_handler;
}

int
b3_win_show(b3_win_t *win, char topmost)
{
	int error;
	HWND insert_after;

	error = 0;

	if (b3_win_get_window_handler(win) == NULL) {
		error = 1;
	}

	insert_after = HWND_TOP;
	if (topmost) {
		insert_after = HWND_TOPMOST;
	}

	if (!error) {
		ShowWindow(b3_win_get_window_handler(win), SW_SHOWNOACTIVATE);
		SetWindowPos(b3_win_get_window_handler(win),
					 insert_after,
					 win->rect.left,
					 win->rect.top,
					 win->rect.right - win->rect.left,
					 win->rect.bottom - win->rect.top,
					 SWP_NOACTIVATE);
	}

	return error;
}

int
b3_win_minimize(b3_win_t *win)
{
   	ShowWindow(win->window_handler, SW_SHOWMINNOACTIVE);
	return 0;
}

RECT
b3_win_get_rect(b3_win_t *win)
{
	return win->rect;
}

int
b3_win_set_rect(b3_win_t *win, RECT rect)
{
	win->rect = rect;

	return 0;
}


int
b3_win_compare(const b3_win_t *win, const b3_win_t *other)
{
	if (win->window_handler == other->window_handler) {
		return 0;
	} else {
		return 1;
	}
}
