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

#include <stdlib.h>
#include <string.h>

b3_win_t *
b3_win_new(HWND window_handler, char floating)
{
	b3_win_t *win;

	win = NULL;
	win = malloc(sizeof(b3_win_t));

	win->window_handler = window_handler;

	win->floating = floating;

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
b3_win_show(b3_win_t *win)
{
	int error;
	RECT win_rect;

	if (!win->window_handler) {
		error = 1;
	}

	if (!error) {
		if (b3_win_get_floating(win)) {
			GetWindowRect(win->window_handler, &win_rect);
//			ShowWindow(win->window_handler, SW_RESTORE);
			ShowWindow(win->window_handler, SW_SHOWDEFAULT);
			SetWindowPos(win->window_handler,
						 HWND_TOPMOST,
						 win_rect.left, win_rect.top,
						 win_rect.right, win_rect.bottom,
						 0);
		} else {
			ShowWindow(win->window_handler, SW_RESTORE);
		}
	}

	return error;
}

int
b3_win_minimize(b3_win_t *win)
{
   	ShowWindow(win->window_handler, SW_MINIMIZE);
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
