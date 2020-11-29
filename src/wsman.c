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
#include <w32bindkeys/logger.h>

#include "monitor.h"
#include "ws.h"

static wbk_logger_t logger = { "wsman" };

/**
 * Be careful when accessing the workspace array, as it might be modified by
 * other threads. 
 *
 * @return The workspace array of the workspace manager. Do not free it or
 * any element of it!
 */
static Array *
b3_wsman_get_ws_arr(b3_wsman_t *wsman);

static int
b3_wsman_ws_comparator(void const *e1, void const *e2);

b3_wsman_t *
b3_wsman_new(b3_ws_factory_t *ws_factory)
{
	b3_wsman_t *wsman;

	wsman = NULL;
	wsman = malloc(sizeof(b3_wsman_t));

	wsman->global_mutex = CreateMutex(NULL, FALSE, NULL);

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
	ReleaseMutex(wsman->global_mutex);
	CloseHandle(wsman->global_mutex);

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

	WaitForSingleObject(wsman->global_mutex, INFINITE);

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

    ReleaseMutex(wsman->global_mutex);

	return ws;
}

int
b3_wsman_remove(b3_wsman_t *wsman, const char *ws_id)
{
	char found;
	ArrayIter iter;
	b3_ws_t *ws;
	b3_ws_t *new_focused_ws;
	int ret;

	WaitForSingleObject(wsman->global_mutex, INFINITE);

	found = 0;
    ret = 1;
	array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
    while (!found && array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
    	if (strcmp(b3_ws_get_name(ws), ws_id) == 0)	{
            array_iter_remove(&iter, NULL);
    		found = 1;
    		ret = 0;
    	}
    }

    if (found) {
		if (b3_wsman_get_focused_ws(wsman) == ws) {
			if (array_size(b3_wsman_get_ws_arr(wsman))) {
				array_get_at(b3_wsman_get_ws_arr(wsman), 0, (void *) &new_focused_ws);
			} else {
				new_focused_ws = b3_wsman_add(wsman, NULL);
			}
      b3_wsman_set_focused_ws(wsman, b3_ws_get_name(new_focused_ws));
		}
    }

    ReleaseMutex(wsman->global_mutex);

	return ret;
}

b3_ws_t *
b3_wsman_contains_ws(b3_wsman_t *wsman, const char *ws_id)
{
	int found;
	ArrayIter iter;
	b3_ws_t *ws;

	WaitForSingleObject(wsman->global_mutex, INFINITE);

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

    ReleaseMutex(wsman->global_mutex);

    return ws;
}

b3_ws_t *
b3_wsman_get_focused_ws(b3_wsman_t *wsman)
{
  b3_ws_t *focused_ws;

  WaitForSingleObject(wsman->global_mutex, INFINITE);

	focused_ws = wsman->focused_ws;

	ReleaseMutex(wsman->global_mutex);

  return focused_ws;
}

int
b3_wsman_set_focused_ws(b3_wsman_t *wsman, const char *ws_id)
{
	int error;
	int found;
	ArrayIter iter;
	b3_ws_t *ws;
	b3_ws_t *old_focused_ws;

	WaitForSingleObject(wsman->global_mutex, INFINITE);

	error = -1;
	old_focused_ws = b3_wsman_get_focused_ws(wsman);
	if (strcmp(b3_ws_get_name(wsman->focused_ws), ws_id) != 0) {
		found = 0;
		array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
		while (!found && array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
			if (strcmp(b3_ws_get_name(ws), ws_id) == 0)	{
				found = 1;
			}
		}

		if (!found) {
			ws = b3_wsman_add(wsman, ws_id);
		}

		if (b3_ws_get_win_amount(old_focused_ws) <= 0) {
			b3_wsman_remove(wsman, b3_ws_get_name(old_focused_ws));
			b3_ws_factory_remove(wsman->ws_factory, b3_ws_get_name(old_focused_ws));
		}

		wsman->focused_ws = ws;
		error = 0;
	}

	ReleaseMutex(wsman->global_mutex);

	return error;
}

int
b3_wsman_remove_win(b3_wsman_t *wsman, b3_win_t *win)
{
	ArrayIter iter;
	b3_ws_t *ws;
	int ret;

  WaitForSingleObject(wsman->global_mutex, INFINITE);

  ret = 1;

	array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
	while (array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
		if (b3_ws_remove_win(ws, win) == 0) {
			ret = 0;
		}
	}

  ReleaseMutex(wsman->global_mutex);

	return ret;
}

b3_ws_t *
b3_wsman_find_win(b3_wsman_t *wsman, const b3_win_t *win)
{
	ArrayIter iter;
	b3_ws_t *ws;
	char found;

  WaitForSingleObject(wsman->global_mutex, INFINITE);

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

	ReleaseMutex(wsman->global_mutex);

	return ws;
}

int
b3_wsman_any_win_has_state(b3_wsman_t *wsman, b3_win_state_t state)
{
  int any_has_state;

  WaitForSingleObject(wsman->global_mutex, INFINITE);

  any_has_state = b3_ws_any_win_has_state(b3_wsman_get_focused_ws(wsman), state);

	ReleaseMutex(wsman->global_mutex);

	return any_has_state;
}

int
b3_wsman_remove_empty_ws(b3_wsman_t *wsman)
{
  int i;
  int len;
	b3_ws_t *ws;

  WaitForSingleObject(wsman->global_mutex, INFINITE);

  i = 0;
  len = array_size(b3_wsman_get_ws_arr(wsman));
  while (i < len) {
    array_get_at(b3_wsman_get_ws_arr(wsman), i, (void *) &ws);

    if (b3_wsman_get_focused_ws(wsman) != ws && b3_ws_get_win_amount(ws) <= 0) {
			b3_wsman_remove(wsman, b3_ws_get_name(ws));
			b3_ws_factory_remove(wsman->ws_factory, b3_ws_get_name(ws));
      ws = NULL;

      i = -1;
      len = array_size(b3_wsman_get_ws_arr(wsman));
		}

    i++;
  }

	ReleaseMutex(wsman->global_mutex);

  return 0;
}

Array *
b3_wsman_get_ws_arr(b3_wsman_t *wsman)
{
  Array *ws_arr;

	ws_arr = wsman->ws_arr;

  return ws_arr;
}

int
b3_wsman_iterate_ws_arr(b3_wsman_t *wsman, void (*visitor)(b3_ws_t *ws))
{
  ArrayIter iter;
	b3_ws_t *ws;

  WaitForSingleObject(wsman->global_mutex, INFINITE);

	array_iter_init(&iter, b3_wsman_get_ws_arr(wsman));
	while (array_iter_next(&iter, (void*) &ws) != CC_ITER_END) {
    visitor(ws);
  }

	ReleaseMutex(wsman->global_mutex);

  return 0;
}

int
b3_wsman_ws_comparator(void const *e1, void const *e2)
{
    b3_ws_t *a = *((b3_ws_t **) e1);
    b3_ws_t *b = *((b3_ws_t **) e2);

    return strcmp(b3_ws_get_name(a), b3_ws_get_name(b));
}
