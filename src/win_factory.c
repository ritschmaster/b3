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
 * @date 2020-04-13
 * @brief File contains the window factory class implementation and private methods
 */

#include "win_factory.h"

#include <stdlib.h>
#include <string.h>

static int
b3_win_factory_free_impl(b3_win_factory_t *win_factory);

static b3_win_t *
b3_win_factory_win_create_impl(b3_win_factory_t *win_factory, HWND window_handler);

static int
b3_win_factory_win_free_impl(b3_win_factory_t *win_factory, b3_win_t *win);

b3_win_factory_t *
b3_win_factory_new(void)
{
	b3_win_factory_t *win_factory;

	win_factory = malloc(sizeof(b3_win_factory_t));
	if (win_factory) {
		memset(win_factory, 0, sizeof(b3_win_factory_t));

		win_factory->b3_win_factory_free = b3_win_factory_free_impl;
		win_factory->b3_win_factory_win_create = b3_win_factory_win_create_impl;
		win_factory->b3_win_factory_win_free = b3_win_factory_win_free_impl;

        win_factory->global_mutex = CreateMutex(NULL, FALSE, NULL);
		array_new(&(win_factory->win_arr));
	}

	return win_factory;
}

int
b3_win_factory_free(b3_win_factory_t *win_factory)
{
	return win_factory->b3_win_factory_free(win_factory);
}

b3_win_t *
b3_win_factory_win_create(b3_win_factory_t *win_factory, HWND window_handler)
{
	return win_factory->b3_win_factory_win_create(win_factory, window_handler);
}

int
b3_win_factory_win_free(b3_win_factory_t *win_factory, b3_win_t *win)
{
	return win_factory->b3_win_factory_win_free(win_factory, win);
}

int
b3_win_factory_free_impl(b3_win_factory_t *win_factory)
{
	ArrayIter iter;
	b3_win_t *win_iter;

	ReleaseMutex(win_factory->global_mutex);
	CloseHandle(win_factory->global_mutex);
	win_factory->global_mutex = NULL;

	array_iter_init(&iter, win_factory->win_arr);
	while (array_iter_next(&iter, (void *) &win_iter) != CC_ITER_END) {
		array_iter_remove(&iter, NULL);
		b3_win_free(win_iter);
	}
	array_destroy(win_factory->win_arr);
	win_factory->win_arr = NULL;

	free(win_factory);

	return 0;
}

b3_win_t *
b3_win_factory_win_create_impl(b3_win_factory_t *win_factory, HWND window_handler)
{
	ArrayIter iter;
	b3_win_t *win_iter;
	b3_win_t *win_new;
	b3_win_t *win;

	WaitForSingleObject(win_factory->global_mutex, INFINITE);

	win_new = b3_win_new(window_handler, 0);
	win = NULL;
	array_iter_init(&iter, win_factory->win_arr);
	while (win == NULL && array_iter_next(&iter, (void *) &win_iter) != CC_ITER_END) {
		if (b3_win_compare(win_new, win_iter) == 0) {
			win = win_iter;
		}
	}

	if (win) {
		b3_win_free(win_new);
	} else {
		win = win_new;
		array_add(win_factory->win_arr, win);
	}

	ReleaseMutex(win_factory->global_mutex);

	return win;
}

int
b3_win_factory_win_free_impl(b3_win_factory_t *win_factory, b3_win_t *win)
{
	int error;
	ArrayIter iter;
	b3_win_t *win_iter;
	char found;

	error = 1;

	WaitForSingleObject(win_factory->global_mutex, INFINITE);

	found = 0;
	array_iter_init(&iter, win_factory->win_arr);
	while (!found && array_iter_next(&iter, (void *) &win_iter) != CC_ITER_END) {
		if (b3_win_compare(win, win_iter) == 0) {
			array_iter_remove(&iter, NULL);
			found = 1;
		}
	}

	if (found) {
		if (win_iter != win) {
			b3_win_free(win_iter);
		}
		error = b3_win_free(win);
	}

	ReleaseMutex(win_factory->global_mutex);

	return error;
}
