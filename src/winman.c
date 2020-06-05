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

#include "winman.h"

/**
 * @author Richard Bäck
 * @date 26 January 2020
 * @brief File contains the window manager class implementation and private methods
 */

#include <stdlib.h>
#include <string.h>

static int
b3_winman_remove_win_leaf(b3_winman_t *winman, const b3_win_t *win);

b3_winman_t *
b3_winman_new(b3_winman_mode_t mode)
{
	b3_winman_t *winman;

	winman = NULL;
	winman = malloc(sizeof(b3_winman_t));

	array_new(&(winman->winman_arr));

	array_new(&(winman->win_arr));

	winman->type = LEAF;

	winman->mode = mode;

	return winman;
}

int
b3_winman_free(b3_winman_t *winman)
{
	ArrayIter iter;
	b3_winman_t *winman_iter;
	b3_win_t *win_iter;

	array_iter_init(&iter, winman->winman_arr);
    while (array_iter_next(&iter, (void*) &winman_iter) != CC_ITER_END) {
    	array_iter_remove(&iter, NULL);
    	b3_winman_free(winman_iter);
    }
	array_destroy(winman->winman_arr);
	winman->winman_arr = NULL;

	array_iter_init(&iter, winman->win_arr);
    while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
    	array_iter_remove(&iter, NULL);
    }
	array_destroy(winman->win_arr);
	winman->win_arr = NULL;

	free(winman);

	return 0;
}

Array *
b3_winman_get_winman_arr(b3_winman_t *winman)
{
	return winman->winman_arr;
}

int
b3_winman_add_winman(b3_winman_t *winman, b3_winman_t *other)
{
	return array_add(winman->winman_arr, other);
}

b3_win_t *
b3_winman_get_next_window(b3_winman_t *winman) {
	b3_win_t *next;
	Array *win_arr;

	next = NULL;
	if (winman->type == LEAF) {
		win_arr = b3_winman_get_win_arr(winman);

		if (array_size(win_arr) >= 1) {
			array_get_at(win_arr, 0, (void *) &next);
		}
	} else {
		next = b3_winman_get_next_window(winman);
	}

	return next;
}

int
b3_winman_remove_empty_nodes(b3_winman_t *winman)
{
	int removed;
	ArrayIter iter;
	b3_winman_t *winman_iter;

	removed = 0;
	if (array_size(winman->winman_arr)) {
		array_iter_init(&iter, winman->win_arr);
		while (array_iter_next(&iter, (void*) &winman_iter) != CC_ITER_END) {
			removed += b3_winman_remove_empty_nodes(winman_iter);
		}
	}

	return removed;
}

Array *
b3_winman_get_win_arr(b3_winman_t *winman)
{
	return winman->win_arr;
}

int
b3_winman_add_win(b3_winman_t *winman, b3_win_t *win)
{
	ArrayIter iter;
	b3_win_t *win_iter;
	char found;

	if (winman->type == LEAF) {
		found = 0;
		array_iter_init(&iter, winman->win_arr);
		while (!found && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, win) == 0) {
				found = 1;
			}
		}

		if (!found) {
			array_add(winman->win_arr, win);

			return 0;
		}
	}

	return 1;
}

int
b3_winman_remove_win(b3_winman_t *winman, const b3_win_t *win)
{
	b3_winman_t *winman_found;
	int ret;

	ret = 1;
	winman = b3_winman_contains_win(winman, win);
	if (winman) {
		ret = b3_winman_remove_win_leaf(winman, win);
	}

	return ret;
}

b3_winman_mode_t
b3_winman_get_mode(b3_winman_t *winman)
{
	return winman->mode;
}

b3_winman_t *
b3_winman_contains_win(b3_winman_t *winman, const b3_win_t *win)
{
	b3_winman_t *container;
	ArrayIter iter;
	b3_winman_t *winman_iter;
	b3_win_t *found_win;

	container = NULL;

	if (winman->type == INNER_NODE) {
		array_iter_init(&iter, winman->winman_arr);
		while (container == NULL && array_iter_next(&iter, (void*) &winman_iter) != CC_ITER_END) {
			container = b3_winman_contains_win(winman_iter, win);
		}
	} else {
		found_win = b3_winman_contains_win_leaf(winman, win);
		if (found_win) {
			container = winman;
		}
	}

	return container;
}

b3_win_t *
b3_winman_contains_win_leaf(b3_winman_t *winman, const b3_win_t *win)
{
	ArrayIter iter;
	b3_win_t *win_iter;
	b3_win_t *found;

	found = NULL;
	if (winman->type == LEAF) {
		array_iter_init(&iter, winman->win_arr);
		while (found == NULL && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, win) == 0) {
				found = win_iter;
			}
		}
	}

	return found;
}

void
b3_winman_traverse(b3_winman_t *winman, void visitor(b3_winman_t *winman))
{
	ArrayIter iter;
	b3_winman_t *winman_iter;

	visitor(winman);

	if (winman->type == INNER_NODE) {
		array_iter_init(&iter, winman->winman_arr);
		while (array_iter_next(&iter, (void*) &winman_iter) != CC_ITER_END) {
			b3_winman_traverse(winman_iter, visitor);
		}
	}
}

int
b3_winman_remove_win_leaf(b3_winman_t *winman, const b3_win_t *win)
{
	ArrayIter iter;
	b3_win_t *win_iter;
	int ret;

	ret = 1;
	if (winman->type == LEAF) {
		array_iter_init(&iter, winman->win_arr);
		while (ret && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, win) == 0) {
				array_iter_remove(&iter, NULL);
				ret = 0;
			}
		}
	}

    return ret;
}

