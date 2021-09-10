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
#include <w32bindkeys/logger.h>

static wbk_logger_t logger = { "win" };

typedef struct b3_win_show_comm_s
{
  b3_win_t *win;
  char topmost;
} b3_win_show_comm_t;


/**
 * This function actually performs everything necessary to show a window.
 *
 * @param param Actually from type b3_win_show_comm_t *
 */
static DWORD WINAPI
b3_win_show_exec(LPVOID param);


b3_win_t *
b3_win_new(HWND window_handler, char floating)
{
	b3_win_t *win;

	win = NULL;
	win = malloc(sizeof(b3_win_t));

	win->state = NORMAL;

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

b3_win_state_t
b3_win_get_state(b3_win_t *win)
{
	return win->state;
}

int
b3_win_set_state(b3_win_t *win, b3_win_state_t state)
{
	win->state = state;

	if (win->state == MAXIMIZED) {
		SendMessage(b3_win_get_window_handler(win), WM_ENTERSIZEMOVE, (WPARAM) NULL, (LPARAM) NULL);
		ShowWindow(b3_win_get_window_handler(win), SW_MAXIMIZE);
		SendMessage(b3_win_get_window_handler(win), WM_EXITSIZEMOVE, (WPARAM) NULL, (LPARAM) NULL);
	}
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
  b3_win_show_comm_t *comm_data;

  comm_data = malloc(sizeof(b3_win_show_comm_t));
  comm_data->win = win;
  comm_data->topmost = topmost;

  CreateThread(NULL,
               0,
               b3_win_show_exec,
               (LPVOID) comm_data,
               0,
               NULL);
  return 0;
}

int
b3_win_minimize(b3_win_t *win)
{
	SendMessage(b3_win_get_window_handler(win), WM_ENTERSIZEMOVE, (WPARAM) NULL, (LPARAM) NULL);
   	ShowWindow(win->window_handler, SW_SHOWMINNOACTIVE);
	SendMessage(b3_win_get_window_handler(win), WM_EXITSIZEMOVE, (WPARAM) NULL, (LPARAM) NULL);
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

DWORD WINAPI
b3_win_show_exec(LPVOID param)
{
  /** Actual input parameters */
  b3_win_t *win;
  char topmost;

  b3_win_show_comm_t *comm_data;
  int error;
	HWND insert_after;
	RECT cur_rect;

  error = 0;

  if (!error) {
    comm_data = (b3_win_show_comm_t *) param;
    if (comm_data == NULL) {
      wbk_logger_log(&logger, SEVERE, "Communication data is empty.\n");
      error = 1;
    }
  }

  if (!error) {
    win = comm_data->win;
    topmost = comm_data->topmost;

    free(comm_data);
    comm_data = NULL;
  }

  if (!error) {
    if (win == NULL) {
      wbk_logger_log(&logger, SEVERE, "Window is empty.\n");
      error = 2;
    }
  }

  if (!error) {
    if (b3_win_get_window_handler(win) == NULL) {
      wbk_logger_log(&logger, SEVERE, "Window handler is empty.\n");
      error = 3;
    }

    insert_after = HWND_BOTTOM;
    if (topmost) {
      insert_after = HWND_TOPMOST;
    }
  }

  if (!error) {
    Sleep(100);

		SendMessage(b3_win_get_window_handler(win), WM_ENTERSIZEMOVE, (WPARAM) NULL, (LPARAM) NULL);

		ShowWindow(b3_win_get_window_handler(win), SW_SHOWNOACTIVATE);

    GetWindowRect(b3_win_get_window_handler(win), &cur_rect);
    if (b3_win_get_state(win) != MAXIMIZED
       && EqualRect(&cur_rect, &(win->rect)) == 0
          && !b3_win_get_floating(win)) {
        SetWindowPos(b3_win_get_window_handler(win),
                     insert_after,
                     win->rect.left,
                     win->rect.top,
                     win->rect.right - win->rect.left,
                     win->rect.bottom - win->rect.top,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        SetWindowPos(b3_win_get_window_handler(win),
                     insert_after,
                     win->rect.left,
                     win->rect.top,
                     win->rect.right - win->rect.left,
                     win->rect.bottom - win->rect.top,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        SetWindowPos(b3_win_get_window_handler(win),
                     insert_after,
                     win->rect.left,
                     win->rect.top,
                     win->rect.right - win->rect.left,
                     win->rect.bottom - win->rect.top,
                     SWP_NOACTIVATE | SWP_FRAMECHANGED);
        SetWindowPos(b3_win_get_window_handler(win),
                     insert_after,
                     win->rect.left,
                     win->rect.top,
                     win->rect.right - win->rect.left,
                     win->rect.bottom - win->rect.top,
                     SWP_NOACTIVATE | SWP_FRAMECHANGED);
    } else {
      SetWindowPos(b3_win_get_window_handler(win),
                   insert_after,
                   win->rect.left,
                   win->rect.top,
                   win->rect.right - win->rect.left,
                   win->rect.bottom - win->rect.top,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
      SetWindowPos(b3_win_get_window_handler(win),
                   insert_after,
                   win->rect.left,
                   win->rect.top,
                   win->rect.right - win->rect.left,
                   win->rect.bottom - win->rect.top,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
      SetWindowPos(b3_win_get_window_handler(win),
                   insert_after,
                   cur_rect.left,
                   cur_rect.top,
                   cur_rect.right - cur_rect.left,
                   cur_rect.bottom - cur_rect.top,
                   SWP_NOACTIVATE | SWP_FRAMECHANGED);
      SetWindowPos(b3_win_get_window_handler(win),
                   insert_after,
                   cur_rect.left,
                   cur_rect.top,
                   cur_rect.right - cur_rect.left,
                   cur_rect.bottom - cur_rect.top,
                   SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

		SendMessage(b3_win_get_window_handler(win), WM_EXITSIZEMOVE, (WPARAM) NULL, (LPARAM) NULL);
	}

	return 0;
}
