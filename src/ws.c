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

#include <stdio.h>
#include <string.h>

#include "win.h"

b3_ws_t *
b3_ws_new(const char *name)
{
	b3_ws_t *ws;

	ws = NULL;
	ws = malloc(sizeof(b3_ws_t));

	array_new(&(ws->win_arr));

	b3_ws_set_name(ws, name);

	return ws;
}

int
b3_ws_free(b3_ws_t *ws)
{
	ArrayIter iter;
	b3_win_t *win_iter;

	array_iter_init(&iter, ws->win_arr);
    while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
    	array_iter_remove(&iter, NULL);
    	b3_win_free(win_iter);
    }
	array_destroy(ws->win_arr);
	ws->win_arr = NULL;

	free(ws->name);
	ws->name = NULL;

	free(ws);
	return 0;
}

Array *
b3_ws_get_win_arr(b3_ws_t *ws)
{
	return ws->win_arr;
}

int
b3_ws_add_win(b3_ws_t *ws, b3_win_t *win)
{
	ArrayIter iter;
	b3_win_t *win_iter;
	char found;

	found = 0;
	array_iter_init(&iter, ws->win_arr);
    while (!found && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
    	if (b3_win_compare(win_iter, win) == 0) {
    		found = 1;
    	}
    }

    if (!found) {
    	array_add(ws->win_arr, win);
    	return 0;
    }
	return 1;
}

int
b3_ws_remove_win(b3_ws_t *ws, b3_win_t *win)
{
	ArrayIter iter;
	b3_win_t *win_iter;
	int ret;

	ret = 1;
	array_iter_init(&iter, ws->win_arr);
    while (ret && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
    	if (b3_win_compare(win_iter, win) == 0) {
    		array_iter_remove(&iter, NULL);
    		ret = 0;
    	}
    }

    if (ret == 0) {
    	b3_win_free(win_iter);
    }

    return ret;
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
