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

/**
 * @author Richard Bäck <richard.baeck@mailbox.org>
 * @date 2020-02-26
 * @brief File contains the workspace manager implementation and its private methods
 */

#include "wsman.h"

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wbkbase/logger.h>

#include "monitor.h"
#include "ws.h"

static wbk_logger_t logger = { "wsman" };

static int
b3_wsman_ws_comparator(void const *e1, void const *e2);

b3_wsman_t *
b3_wsman_new(b3_ws_factory_t *ws_factory)
{
	b3_wsman_t *wsman;

	wsman = NULL;
	wsman = malloc(sizeof(b3_wsman_t));

	wsman->ws_factory = ws_factory;

	array_new(&(wsman->ws_arr));

	wsman->focused_ws = b3_ws_factory_create(wsman->ws_factory, NULL);
	array_add(b3_wsman_get_ws_arr(wsman),
    		  wsman->focused_ws);
	return wsman;
}

int
b3_wsman_free(b3_wsman_t *wsman)
{
	array_destroy_cb(wsman->ws_arr, free);
	wsman->ws_arr = NULL;

	wsman->ws_factory = NULL;

	free(wsman);
	return 0;
}

b3_ws_t *
b3_wsman_add(b3_wsman_t *wsman, const char *ws_id)
{
	char found;
	ArrayIter iter;
	b3_ws_t *ws;

	found = 0;
	ws = NULL;
	array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
    while (!found && array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
    	if (strcmp(b3_ws_get_name(ws), ws_id) == 0)	{
    		found = 1;
    	}
    }

    if (!found) {
    	ws = b3_ws_factory_create(wsman->ws_factory, ws_id);
    	array_add(b3_wsman_get_ws_arr(wsman), ws);
    	array_sort(b3_wsman_get_ws_arr(wsman), b3_wsman_ws_comparator);
    }

	return ws;
}

int
b3_wsman_remove(b3_wsman_t *wsman, const char *ws_id)
{
	char found;
	ArrayIter iter;
	b3_ws_t *ws;
	int ret;

	found = 0;
    ret = 1;
	array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
    while (!found && array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
    	if (strcmp(b3_ws_get_name(ws), ws_id) == 0)	{
            array_iter_remove(&iter, NULL);
            b3_ws_factory_remove(wsman->ws_factory, b3_ws_get_name(ws));
    		found = 1;
    		ret = 0;
    	}
    }

	return ret;
}

b3_ws_t *
b3_wsman_contains_ws(b3_wsman_t *wsman, const char *ws_id)
{
	int found;
	ArrayIter iter;
	b3_ws_t *ws;

	found = 0;
	array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
    while (!found && array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
    	if (strcmp(b3_ws_get_name(ws), ws_id) == 0)	{
    		found = 1;
    	}
    }

    if (!found) {
    	ws = NULL;
    }

    return ws;
}

b3_ws_t *
b3_wsman_get_focused_ws(b3_wsman_t *wsman)
{
	return wsman->focused_ws;
}

int
b3_wsman_set_focused_ws(b3_wsman_t *wsman, const char *ws_id)
{
	int found;
	ArrayIter iter;
	b3_ws_t *ws;

	if (strcmp(b3_ws_get_name(wsman->focused_ws), ws_id) != 0) {
		found = 0;
		array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
		while (!found && array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
			if (strcmp(b3_ws_get_name(ws), ws_id) == 0)	{
				found = 1;
			}
		}

		if (b3_ws_get_win_amount(wsman->focused_ws) <= 0) {
			b3_wsman_remove(wsman, b3_ws_get_name(wsman->focused_ws));
		}

		if (!found) {
			ws = b3_wsman_add(wsman, ws_id);
		}

		wsman->focused_ws = ws;
	}

    return 0;
}

int
b3_wsman_remove_win(b3_wsman_t *wsman, b3_win_t *win)
{
	ArrayIter iter;
	b3_ws_t *ws;
	int ret;

	ret = 1;

	array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
	while (array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
		if (b3_ws_remove_win(ws, win) == 0) {
			ret = 0;
		}
	}

	return ret;
}

b3_ws_t *
b3_wsman_find_win(b3_wsman_t *wsman, const b3_win_t *win)
{
	ArrayIter iter;
	b3_ws_t *ws;
	char found;

	found = 0;
	array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
	while (!found && array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
		if (b3_ws_contains_win(ws, win)) {
			found = 1;
		}
	}

	if (!found) {
		ws = NULL;
	}

	return ws;
}

Array *
b3_wsman_get_ws_arr(b3_wsman_t *wsman)
{
	return wsman->ws_arr;
}

int
b3_wsman_ws_comparator(void const *e1, void const *e2)
{
    b3_ws_t *a = *((b3_ws_t **) e1);
    b3_ws_t *b = *((b3_ws_t **) e2);

    return strcmp(b3_ws_get_name(a), b3_ws_get_name(b));
}
