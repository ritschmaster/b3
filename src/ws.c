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

#include <collectc/array.h>
#include <string.h>
#include <w32bindkeys/logger.h>
#include <windows.h>
#include <collectc/stack.h>

#include "win.h"
#include "winman.h"

static wbk_logger_t logger = { "ws" };

typedef struct b3_ws_toggle_floating_visitor_s
{
	int error;

	/**
	 * Found the window in the tree and toggled its state
	 */
	char toggled;

	b3_ws_t *ws;

	/**
	 * Window which should be floating.
	 */
	const b3_win_t *floating_win;
} b3_ws_toggle_floating_visitor_t;

typedef struct b3_ws_find_last_previous_s
{
	int found;

	/**
	 * The same as the member previously_focused_win_arr of the struct b3_ws_t.
	 */
	Array *previously_focused_win_arr;
} b3_ws_find_last_previous_t;

static int
b3_ws_free_impl(b3_ws_t *ws);

static int
b3_ws_set_name_impl(b3_ws_t *ws, const char *name);

static const char*
b3_ws_get_name_impl(b3_ws_t *ws);

static b3_win_t *
b3_ws_get_focused_win_impl(b3_ws_t *ws);

static int
b3_ws_set_focused_win_impl(b3_ws_t *ws, const b3_win_t *win);

static int
b3_ws_add_win_impl(b3_ws_t *ws, b3_win_t *win);

static int
b3_ws_remove_win_impl(b3_ws_t *ws, b3_win_t *win);

static int
b3_ws_split_impl(b3_ws_t *ws, b3_winman_mode_t mode);

static int
b3_ws_move_focused_win_impl(b3_ws_t *ws, b3_ws_move_direction_t direction);

static int
b3_ws_toggle_floating_win_impl(b3_ws_t *ws, b3_win_t *win);

static int
b3_ws_minimize_wins_impl(b3_ws_t *ws);

/**
 * @param data Must be NULL
 */
static void
b3_ws_minimize_wins_visitor(b3_winman_t *winman, void *data);

static b3_win_t *
b3_ws_contains_win_impl(b3_ws_t *ws, const b3_win_t *win);

static int
b3_ws_is_empty_impl(b3_ws_t *ws);

/**
 * @param data Must be actually of type int *.
 */
static void
b3_ws_is_empty_visitor(b3_winman_t *winman, void *data);

static b3_win_t *
b3_ws_get_win_rel_to_focused_win_impl_internal(b3_ws_t *ws,
											   b3_ws_move_direction_t direction,
											   char rolling);

static b3_win_t *
b3_ws_get_win_rel_to_focused_win_impl(b3_ws_t *ws,
									  b3_ws_move_direction_t direction,
									  char rolling);

/**
 * A traverser searching for the container containing the last (highest)
 * previously focused window.
 *
 * @param data Must be actually of type b3_ws_find_last_previous_t *.
 */
static void
b3_ws_find_last_previous_visitor(b3_winman_t *winman, void *data);

static int
b3_ws_arrange_wins_impl(b3_ws_t *ws, RECT monitor_area);

/**
 * @param data Must be actually of type Stack * (containing only RECT *).
 */
static void
b3_ws_arrange_wins_visitor(b3_winman_t *winman, void *data);

b3_ws_t *
b3_ws_new(const char *name)
{
	b3_ws_t *ws;

	ws = NULL;
	ws = malloc(sizeof(b3_ws_t));

	if (ws) {
		ws->b3_ws_free = b3_ws_free_impl;
		ws->b3_ws_set_name = b3_ws_set_name_impl;
		ws->b3_ws_get_name = b3_ws_get_name_impl;
		ws->b3_ws_set_focused_win = b3_ws_set_focused_win_impl;
		ws->b3_ws_get_focused_win = b3_ws_get_focused_win_impl;
		ws->b3_ws_add_win = b3_ws_add_win_impl;
		ws->b3_ws_remove_win = b3_ws_remove_win_impl;
		ws->b3_ws_split = b3_ws_split_impl;
		ws->b3_ws_move_focused_win = b3_ws_move_focused_win_impl;
		ws->b3_ws_toggle_floating_win = b3_ws_toggle_floating_win_impl;
		ws->b3_ws_minimize_wins = b3_ws_minimize_wins_impl;
		ws->b3_ws_contains_win = b3_ws_contains_win_impl;
		ws->b3_ws_is_empty = b3_ws_is_empty_impl;
		ws->b3_ws_get_win_rel_to_focused_win = b3_ws_get_win_rel_to_focused_win_impl;
		ws->b3_ws_arrange_wins = b3_ws_arrange_wins_impl;

		ws->winman = b3_winman_new(HORIZONTAL);
		ws->mode = DEFAULT;
		b3_ws_set_name(ws, name);
		ws->focused_win = NULL;
		array_new(&(ws->previously_focused_win_arr));
		array_new(&(ws->floating_win_arr));
	}

	return ws;
}

int
b3_ws_free(b3_ws_t *ws)
{
	return ws->b3_ws_free(ws);
}

int
b3_ws_set_name(b3_ws_t *ws, const char *name)
{
	return ws->b3_ws_set_name(ws, name);
}

const char*
b3_ws_get_name(b3_ws_t *ws)
{
	return ws->b3_ws_get_name(ws);
}

b3_win_t *
b3_ws_get_focused_win(b3_ws_t *ws)
{
	return ws->b3_ws_get_focused_win(ws);
}

int
b3_ws_set_focused_win(b3_ws_t *ws, const b3_win_t *win)
{
	return ws->b3_ws_set_focused_win(ws, win);
}

int
b3_ws_add_win(b3_ws_t *ws, b3_win_t *win)
{
	return ws->b3_ws_add_win(ws, win);
}

int
b3_ws_remove_win(b3_ws_t *ws, b3_win_t *win)
{
	return ws->b3_ws_remove_win(ws, win);
}

int
b3_ws_is_empty(b3_ws_t *ws)
{
	return ws->b3_ws_is_empty(ws);
}

int
b3_ws_split(b3_ws_t *ws, b3_winman_mode_t mode)
{
	return ws->b3_ws_split(ws, mode);
}

int
b3_ws_move_focused_win(b3_ws_t *ws, b3_ws_move_direction_t direction)
{
	return ws->b3_ws_move_focused_win(ws, direction);
}

int
b3_ws_toggle_floating_win(b3_ws_t *ws, b3_win_t *win)
{
	return ws->b3_ws_toggle_floating_win(ws, win);
}

int
b3_ws_minimize_wins(b3_ws_t *ws)
{
	return ws->b3_ws_minimize_wins(ws);
}

b3_win_t *
b3_ws_contains_win(b3_ws_t *ws, const b3_win_t *win)
{
	return ws->b3_ws_contains_win(ws, win);
}

b3_win_t *
b3_ws_get_win_rel_to_focused_win(b3_ws_t *ws,
								 b3_ws_move_direction_t direction,
								 char rolling)
{
	return ws->b3_ws_get_win_rel_to_focused_win(ws, direction, rolling);
}

int
b3_ws_arrange_wins(b3_ws_t *ws, RECT monitor_area)
{
	return ws->b3_ws_arrange_wins(ws, monitor_area);
}

int
b3_ws_free_impl(b3_ws_t *ws)
{
	b3_winman_free(ws->winman);
	ws->winman = NULL;

	free(ws->name);
	ws->name = NULL;

	ws->focused_win = NULL;

	array_destroy(ws->previously_focused_win_arr);
	ws->previously_focused_win_arr = NULL;

	array_destroy(ws->floating_win_arr);
	ws->floating_win_arr = NULL;

	free(ws);
	return 0;
}

int
b3_ws_set_name_impl(b3_ws_t *ws, const char *name)
{
	int length;

	length = strlen(name) + 1;
	ws->name = malloc(sizeof(char) * length);
	strcpy(ws->name, name);

	return 0;
}

const char*
b3_ws_get_name_impl(b3_ws_t *ws)
{
	return ws->name;
}

b3_win_t *
b3_ws_get_focused_win_impl(b3_ws_t *ws)
{
	return ws->focused_win;
}

int
b3_ws_set_focused_win_impl(b3_ws_t *ws, const b3_win_t *win)
{
	b3_winman_t *winman;
	int error;
	int i;
	int j;
	int len;
	ArrayIter iter;
	b3_win_t *win_iter;
	b3_win_t *new_focused_win;
	b3_win_t *a;
	b3_win_t *b;

	error = 1;
	if (win) {
		new_focused_win = NULL;
		if (win->floating) {
			array_iter_init(&iter, ws->floating_win_arr);
			while (error && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
				if (b3_win_compare(win_iter, win) == 0) {
					new_focused_win = win_iter;
				}
			}
		} else {
			winman = b3_winman_contains_win(ws->winman, win);
			if (winman) {
				new_focused_win = b3_winman_get_win(winman);
			}
		}

		if (new_focused_win) {
			error = 0;
			if (ws->focused_win) {
				array_add(ws->previously_focused_win_arr, ws->focused_win);
			}
			ws->focused_win = new_focused_win;

			/**
			 * Remove all duplicates
			 */
			len = array_size(ws->previously_focused_win_arr);
			for (i = 0; i < len; i++) {
				array_get_at(ws->previously_focused_win_arr, i, (void *) &a);
				for (j = i + 1; j < len; j++) {
					array_get_at(ws->previously_focused_win_arr, j, (void *) &b);
					if (a == b) {
						array_remove_at(ws->previously_focused_win_arr, j, NULL);
						j--;
						len--;
					} else {
						j = len;
					}
				}
			}
		}
	} else if (b3_ws_is_empty(ws)) {
		ws->focused_win = NULL;
		error = 0;
	}

	return error;
}

int
b3_ws_add_win_impl(b3_ws_t *ws, b3_win_t *win)
{
	b3_winman_t *winman;
	b3_winman_t *winman_for_win;
	b3_win_t *focused_win;
	int error;

	error = 1;

	if (b3_win_get_floating(win)) {
		array_add(ws->floating_win_arr, win);
		error = 0;
	} else {
		winman = b3_winman_contains_win(ws->winman, win);
		if (!winman) {
			/**
			 * Window was not yet added
			 */

			focused_win = b3_ws_get_focused_win(ws);
			if (focused_win) {
				/**
				 * There is at least one window already in the workspace
				 */
				winman = b3_winman_contains_win(ws->winman, focused_win);
				winman = b3_winman_get_parent(ws->winman, winman);
			} else {
				/**
				 * The new window is the first window in the workspace
				 */
				winman = ws->winman;
			}

			winman_for_win = b3_winman_new(UNSPECIFIED);
			b3_winman_set_win(winman_for_win, win);

			b3_winman_add_winman(winman, winman_for_win);

			b3_ws_set_focused_win(ws, win);

			error = 0;
		}
	}

	return error;
}

int
b3_ws_remove_win_impl(b3_ws_t *ws, b3_win_t *win)
{
	int error;
	b3_winman_t *winman;
	b3_winman_t *root;
	b3_win_t *new_focused_win;
	ArrayIter iter;
	b3_win_t *win_iter;

	error = 1;

	winman = NULL;
	root = NULL;
	new_focused_win = NULL;

	array_iter_init(&iter, ws->floating_win_arr);
	while (error && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
		if (b3_win_compare(win_iter, win) == 0) {
			error = 0;
		}
	}

	if (error) {
		winman = b3_winman_contains_win(ws->winman, win);
		if (winman) {
			root = b3_winman_get_parent(ws->winman, winman);
			if (root) {
				error = b3_winman_remove_winman(root, winman);
			}
		}
	}


	if (!error) {
		/**
		 * Remove all occurrences of win in ws->floating_win_arr
		 */
		array_iter_init(&iter, ws->floating_win_arr);
		while (error && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, win) == 0) {
				array_iter_remove(&iter, NULL);
			}
		}

		/**
		 * Remove all occurrences of win in ws->previously_focused_win_arr.
		 */
		array_iter_init(&iter, ws->previously_focused_win_arr);
		while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, win) == 0) {
				array_iter_remove(&iter, NULL);
			}
		}

		/**
		 * Set new focused window
		 */
		if (b3_ws_get_focused_win(ws)
			&& b3_win_compare(b3_ws_get_focused_win(ws), win) == 0) {
			new_focused_win = NULL;

			if (array_size(ws->previously_focused_win_arr)) {
				array_remove_last(ws->previously_focused_win_arr, (void *) &new_focused_win);
			}

			b3_ws_set_focused_win(ws, new_focused_win);

			/**
			 * Again remove all occurrences of win in in
			 * ws->previously_focused_win_arr since b3_ws_set_focused_win_impl() added new_focused_win again.
			 */
			array_iter_init(&iter, ws->previously_focused_win_arr);
			while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
				if (b3_win_compare(win_iter, win) == 0) {
					array_iter_remove(&iter, NULL);
				}
			}
		}

		if (winman) {
			b3_winman_free(winman);
		}
		error = 0;

		/**
		 * Remove all parent window managers that are now empty
		 */
		while (root && root != ws->winman) {
			if (array_size(b3_winman_get_winman_arr(root)) <= 0) {
				winman = root;
				root = b3_winman_get_parent(ws->winman, winman);
				if (b3_winman_remove_winman(root, winman)) {
					wbk_logger_log(&logger, SEVERE, "Unexpected error in workspace. Please contact the project maintainer(s)!\n");

					/**
					 * Exit the loop
					 */
					root = NULL;
				} else {
					b3_winman_free(winman);
				}
			} else {
				/**
				 * Exit the loop
				 */
				root = NULL;
			}
		}
	}

	return error;
}

int
b3_ws_split_impl(b3_ws_t *ws, b3_winman_mode_t mode)
{
	int error;
	b3_winman_t *root;
	b3_winman_t *winman;
	b3_winman_t *split;

	error = 1;
	winman = b3_winman_contains_win(ws->winman, b3_ws_get_focused_win(ws));
	if (winman) {
		root = b3_winman_get_parent(ws->winman, winman);
		if (root) {
			b3_winman_remove_winman(root, winman);

			split = b3_winman_new(mode);
			b3_winman_add_winman(split, winman);

			b3_winman_add_winman(root, split);

			error = 0;
	 	}
	}

	return error;
}

int
b3_ws_move_focused_win_impl(b3_ws_t *ws, b3_ws_move_direction_t direction)
{
	int error;
	b3_winman_mode_t mode;
	b3_winman_get_rel_t get;
	b3_win_t *focused_win;
	b3_winman_t *focused_win_container;
	b3_win_t *win_in_direction;
	b3_winman_t *container;
	b3_winman_t *root;
	b3_winman_t *root_new;
	int i;
	int arr_len;
	b3_winman_t *winman_iter;

	error = 1;
	container = NULL;
	root = NULL;
	root_new = NULL;
	// TODO: return an error on edge of the screen to allow moving a winodw to
	// the next monitor
	if (direction == LEFT) {
		mode = HORIZONTAL;
		get = PREVIOUS;
	} else if (direction == RIGHT) {
		mode = HORIZONTAL;
		get = NEXT;
	} else if (direction == UP) {
		mode = VERTICAL;
		get = PREVIOUS;
	} else if (direction == DOWN) {
		mode = VERTICAL;
		get = NEXT;
	} else  {
		wbk_logger_log(&logger, SEVERE, "Unknown value for 'direction': %d\n", direction);
	}

	focused_win = b3_ws_get_focused_win(ws);
	if (focused_win) {
		focused_win_container = b3_winman_contains_win(ws->winman, focused_win);
		if (focused_win_container) {
			root = b3_winman_get_parent(ws->winman, focused_win_container);
			if (root == NULL) {
				wbk_logger_log(&logger, SEVERE, "'root' (window manager^2 of focused window) was unexpectelty NULL.\n");
			}
		} else {
			wbk_logger_log(&logger, SEVERE, "'focused_win_container' (window manager of focused window) was unexpectelty NULL.\n");
		}
	}

	win_in_direction = b3_ws_get_win_rel_to_focused_win(ws, direction, 0);
	if (win_in_direction) {
		/**
		 * Utilize the available window manager
		 */
		container = b3_winman_contains_win(ws->winman, win_in_direction);
		if (container) {
			root_new = b3_winman_get_parent(ws->winman, container);
			if (root_new == NULL) {
				wbk_logger_log(&logger, SEVERE, "'root_new' (window manager of window manager of target window) was unexpectelty NULL.\n");
			}
		} else {
			wbk_logger_log(&logger, SEVERE, "'container' (window manager of target window) was unexpectelty NULL.\n");
		}
	} else {
		/**
		 * Propably add a new window manager
		 */

		container = focused_win_container;
		for (;;) {
			root_new = b3_winman_get_parent(ws->winman, container);
			if ((root_new && b3_winman_get_mode(root_new) == mode)
			    || root_new == NULL) {
				break;
			} else {
				container = root_new;
			}
		}

		if (root_new == NULL) {
			/**
			 * There is no a parent containing the correct mode
			 */
			container = b3_winman_get_parent(ws->winman, root);
			if (container) {
				root_new = container;
				container = b3_winman_new(mode);
				b3_winman_add_winman(container, root);
				b3_winman_add_winman(root_new, container);
			} else {
				/**
				 * root == ws->winman!
				 */
				root_new = b3_winman_new(mode);
				b3_winman_add_winman(root_new, root);
				ws->winman = root_new;
			}
		}
	}

	if (root == root_new) {
		/**
		 * As root == root_new we can directly operate with focused_win_container.
		 */
		arr_len = array_size(b3_winman_get_winman_arr(root_new));
		for (i = 0; i < arr_len; i++) {
			array_get_at(b3_winman_get_winman_arr(root_new), i, (void *) &winman_iter);
			if (winman_iter == focused_win_container) {
				array_remove_at(b3_winman_get_winman_arr(root_new), i, NULL);
				if (get == PREVIOUS) {
					i--;
				} else if (get == NEXT) {
					i++;
				}

				if (i < 0) {
					i = 0;
				} else if (i >= arr_len) {
					i = arr_len - 1;
				}

				array_add_at(b3_winman_get_winman_arr(root_new),
							 focused_win_container,
							 i);

				error = 0;
				i = arr_len;
			}
		}
	} else if (b3_winman_get_parent(ws->winman, root) == root_new) {
		/**
		 * root_new is the parent of the parent of root.
		 *
		 * Therefore we first have to remove focused_win_container from root.
		 */
		arr_len = array_size(b3_winman_get_winman_arr(root));
		for (i = 0; i < arr_len; i++) {
			array_get_at(b3_winman_get_winman_arr(root), i, (void *) &winman_iter);
			if (winman_iter == focused_win_container) {
				array_remove_at(b3_winman_get_winman_arr(root), i, NULL);
				i = arr_len;
			}
		}

		/**
		 * Now we can place focused_win_container before/after the position
		 * of root in root_new.
		 */
		arr_len = array_size(b3_winman_get_winman_arr(root_new));
		for (i = 0; i < arr_len; i++) {
			array_get_at(b3_winman_get_winman_arr(root_new), i, (void *) &winman_iter);
			if (winman_iter == root) {
				if (get == PREVIOUS) {
					i--;
				} else if (get == NEXT) {
					i++;
				}

				if (i < 0) {
					i = 0;
				} else if (i >= arr_len) {
					i = arr_len;
				}

				array_add_at(b3_winman_get_winman_arr(root_new),
							 focused_win_container,
							 i);

				i = arr_len;
			}
		}

		error = 0;
	} else {
		/**
		 * root_new is NO ancestor of root.
		 *
		 * Therefore we first have to remove focused_win_container from root.
		 */
		arr_len = array_size(b3_winman_get_winman_arr(root));
		for (i = 0; i < arr_len; i++) {
			array_get_at(b3_winman_get_winman_arr(root), i, (void *) &winman_iter);
			if (winman_iter == focused_win_container) {
				array_remove_at(b3_winman_get_winman_arr(root), i, NULL);
				i = arr_len;
			}
		}

		b3_winman_add_winman(root_new, focused_win_container);

		error = 0;
	}


	return error;
}

int
b3_ws_toggle_floating_win_impl(b3_ws_t *ws, b3_win_t *win)
{
	int error;
	b3_win_t *my_win;

	error = 1;

	my_win = b3_ws_contains_win(ws, win);
	if (my_win) {
		/**
		 * Remove the window from the tree/floating windows (and later add it to
		 * the floating windows/tree).
		 */
		error = b3_ws_remove_win(ws, my_win);

		if (!error) {
			if (b3_win_get_floating(my_win)) {
				b3_win_set_floating(my_win, 0);
			} else {
				b3_win_set_floating(my_win, 1);
			}

			error = b3_ws_add_win(ws, my_win);
		}
	}

	return error;
}

int
b3_ws_minimize_wins_impl(b3_ws_t *ws)
{
	b3_winman_traverse(ws->winman,
						b3_ws_minimize_wins_visitor,
						NULL);
	return 0;
}

void
b3_ws_minimize_wins_visitor(b3_winman_t *winman, void *data)
{
	b3_win_t *win;

	win = b3_winman_get_win(winman);
	if (win) {
		b3_win_minimize(win);
	}
}

b3_win_t *
b3_ws_contains_win_impl(b3_ws_t *ws, const b3_win_t *win)
{
	b3_winman_t *container;
	b3_win_t *found;
	ArrayIter iter;
	b3_win_t *win_iter;

	found = NULL;
	container = b3_winman_contains_win(ws->winman, win);
	if (container) {
		found = b3_winman_get_win(container);
	} else {
		array_iter_init(&iter, ws->floating_win_arr);
		while (found == NULL && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, win) == 0) {
				found = win_iter;
			}
		}
	}

	return found;
}

int
b3_ws_is_empty_impl(b3_ws_t *ws)
{
	int number;

	number = 0;

	number = array_size(ws->floating_win_arr);
	if (number > 0) {
		number = 0;
	} else {
		b3_winman_traverse(ws->winman, b3_ws_is_empty_visitor, &number);
		if (number > 0) {
			number = 0;
		} else {
			number = 1;
		}
	}

	return number;
}

void
b3_ws_is_empty_visitor(b3_winman_t *winman, void *data)
{
	int *number;

	number = (int *) data;

	if (b3_winman_get_win(winman)) {
		(*number)++;
	}
}

b3_win_t *
b3_ws_get_win_rel_to_focused_win_impl_internal(b3_ws_t *ws,
											   b3_ws_move_direction_t direction,
											   char rolling)
{
	b3_win_t *found;
	b3_winman_mode_t mode;
	b3_winman_get_rel_t child_to_get;
	b3_winman_t *focused_win_container;
	b3_winman_t *container;
	b3_winman_t *parent;
	b3_ws_find_last_previous_t find_last_previous;

	found = NULL;

	if (direction == UP || direction == DOWN) {
		mode = VERTICAL;
	} else if (direction == LEFT || direction == RIGHT) {
		mode = HORIZONTAL;
	} else {
		wbk_logger_log(&logger, SEVERE, "Unknown value for 'direction': %d\n", direction);
	}

	if (direction == UP || direction == LEFT) {
		child_to_get = PREVIOUS;
	} else if (direction == DOWN || direction == RIGHT) {
		child_to_get = NEXT;
	}

	focused_win_container = b3_winman_contains_win(ws->winman, b3_ws_get_focused_win(ws));
	if (focused_win_container) {
		parent = focused_win_container;
		container = focused_win_container;
		while (container) {
		/**
		 * Go up from container to search for a node having the needed mode
		 */
		for (;;) {
			parent = b3_winman_get_parent(ws->winman, container);
			if ((parent && b3_winman_get_mode(parent) == mode)
			    || parent == NULL) {
				break;
			} else {
				container = parent;
			}
		}

		if (parent) {
			/**
			 * We now have the correct root node in variable parent. Lets switch to
			 * the one we look for.
			 */
			container = b3_winman_get_winman_rel_to_winman(parent,
														   container,
														   child_to_get,
														   rolling);

			if (container) {
				/**
				 * We are done.
				 */
				parent = container;
				container = NULL;
			} else {
				/**
				 * Maybe one of higher nodes does match our orientation too. Then we will utilize that node instead.
				 */
				container = parent;
			}
		} else {
			/**
			 * Exit the loop if we cannot find the correct parent
		     */
			container = NULL;
		}
		}

		/**
		 * We now have the root node of the window manager we are looking for. Let's go down.
		 */
		if (parent) {
			find_last_previous.found = -1;
			find_last_previous.previously_focused_win_arr = ws->previously_focused_win_arr;

			b3_winman_traverse(parent, b3_ws_find_last_previous_visitor,
							   (void *) &find_last_previous);

			if (find_last_previous.found >= 0) {
				array_get_at(ws->previously_focused_win_arr, find_last_previous.found,
							 (void *) &found);
			}
		}
	}

	return found;
}

b3_win_t *
b3_ws_get_win_rel_to_focused_win_impl(b3_ws_t *ws,
									  b3_ws_move_direction_t direction,
									  char rolling)
{
	b3_win_t *found;

	found = NULL;

	if (rolling) {
		/**
		 * First try finding the window without using the rolling flag.
		 */
		found = b3_ws_get_win_rel_to_focused_win_impl_internal(ws, direction, 0);
	}

	if (found == NULL) {
		found = b3_ws_get_win_rel_to_focused_win_impl_internal(ws, direction, rolling);
	}

	return found;
}

void
b3_ws_find_last_previous_visitor(b3_winman_t *winman, void *data)
{
	b3_ws_find_last_previous_t *find_last_previous;
	b3_win_t *win;
	int i;
	int length;
	b3_win_t *win_iter;

	find_last_previous = (b3_ws_find_last_previous_t *) data;
	if (find_last_previous) {
		win = b3_winman_get_win(winman);
		if (win) {
			/**
			 * Is a leaf node
			 */

			length = array_size(find_last_previous->previously_focused_win_arr);
			for (i = find_last_previous->found + 1; i < length; i++) {
				array_get_at(find_last_previous->previously_focused_win_arr,
							 i,
							 (void *) &win_iter);
				if (win_iter
				    && b3_win_compare(win, win_iter) == 0) {
					find_last_previous->found = i;
				}
			}
		}
	}
}

int
b3_ws_arrange_wins_impl(b3_ws_t *ws, RECT monitor_area)
{
	ArrayIter iter;
	b3_win_t *win_iter;

	/**
	 * Stack of RECT *
	 */
	Stack *area_stack;

	RECT *my_area;

	stack_new(&area_stack);

	my_area = malloc(sizeof(RECT));
	my_area->top = monitor_area.top;
	my_area->bottom = monitor_area.bottom;
	my_area->left = monitor_area.left;
	my_area->right = monitor_area.right;
	stack_push(area_stack, my_area);

	b3_winman_traverse(ws->winman,
						b3_ws_arrange_wins_visitor,
						area_stack);

	stack_destroy(area_stack);

	/*
	 * Now show all floating windows.
	 */
	Sleep(200);
	array_iter_init(&iter, ws->floating_win_arr);
	while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
		b3_win_show(win_iter, 1);
	}

	return 0;
}

void
b3_ws_arrange_wins_visitor(b3_winman_t *winman, void *data)
{
	Stack *area_stack;
	b3_win_t *my_win;
	int current_pos;
	int increment;
	int size;
	int i;
	RECT *my_area;
	RECT *child_area;

	area_stack = (Stack *) data;
	my_area = NULL;
	stack_pop(area_stack, (void *) &my_area);

	my_win = b3_winman_get_win(winman);
	if (my_win == NULL) {
		/**
		 * Inner node containing only leaves
		 *
		 * my_area will be modified by the for loop.
		 */

		size = array_size(b3_winman_get_winman_arr(winman));
		if (size > 0) {
			if (b3_winman_get_mode(winman) == HORIZONTAL) {
				increment = (my_area->right - my_area->left) / size;
			} else if (b3_winman_get_mode(winman) == VERTICAL) {
				increment = (my_area->bottom - my_area->top) / size;
			}

			if (b3_winman_get_mode(winman) == HORIZONTAL) {
				current_pos = my_area->right;
			} else if (b3_winman_get_mode(winman) == VERTICAL) {
				current_pos = my_area->bottom;
			}

			/**
			 * The right/top most areas have to be added first, since the stack
			 * later will be popped the other way around. Thus start from
			 * left/bottom would place the windows in the reverse (wrong) order
			 * on the screen.
			 */

			for (i = 0; i < size; i++) {
				child_area = malloc(sizeof(RECT));
				child_area->top = my_area->top;
				child_area->bottom = my_area->bottom;
				child_area->left = my_area->left;
				child_area->right = my_area->right;

				current_pos -= increment;
				if (b3_winman_get_mode(winman) == HORIZONTAL) {
					child_area->left = current_pos;
					child_area->right = child_area->left + increment;
				} else if (b3_winman_get_mode(winman) == VERTICAL) {
					child_area->top = current_pos;
					child_area->bottom = child_area->top + increment;
				}

				stack_push(area_stack, (void *) child_area);
			}
		}
	} else {
		/**
		 * Leaf containing only a window
		 */
		b3_win_set_rect(my_win, *my_area);
		b3_win_show(my_win, 1);
	}

	if (my_area) {
		free(my_area);
		my_area = NULL;
	}
}
