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
#include <w32bindkeys/logger.h>
#include <windows.h>
#include <collectc/stack.h>

#include "win.h"

static wbk_logger_t logger = { "ws" };

/**
 * Communication structure for b3_ws_get_win_amount() and
 * b3_ws_winman_amount_visitor().
 */
typedef struct b3_ws_get_win_amount_comm_s
{
  HANDLE mutex;
  int amount;
} b3_ws_get_win_amount_comm_t;

/**
 * Communication structure for b3_ws_add_win() and
 * b3_ws_winman_first_leaf_visitor().
 */
typedef struct b3_ws_winman_first_leaf_visitor_comm_s
{
  HANDLE mutex;
  b3_winman_t *first_leaf;
} b3_ws_winman_first_leaf_visitor_comm_t;

/**
 * Communication structure for b3_active_win_toggle_floating() and
 * b3_ws_winman_active_win_toggle_floating_visitor().
 */
typedef struct b3_active_win_toggle_floating_comm_s
{
  HANDLE mutex;
  const b3_win_t *win;
  int toggle_failed;
} b3_active_win_toggle_floating_comm_t;

/**
 * Communication structure for b3_ws_set_focused_win() and
 * b3_ws_winman_set_focused_win_visitor().
 */
typedef struct b3_ws_set_focused_win_comm_s
{
  HANDLE mutex;
  const b3_win_t *win;
  b3_win_t *found;
} b3_ws_set_focused_win_comm_t;

/**
 * Communication structure for b3_ws_arrange_wins() and
 * b3_ws_winman_amount_visitor().
 */
typedef struct b3_ws_arrange_wins_comm_s
{
  HANDLE mutex;
  int amount;
} b3_ws_arrange_wins_comm_t;

/**
 * Belongs to b3_ws_get_win_amount() and
 * b3_ws_get_win_amount_ws_visitor(). Do not set it somewhere else!
 */
static b3_ws_get_win_amount_comm_t g_get_win_amount_comm;

/**
 * Belongs to b3_ws_winman_add_win() and
 * b3_ws_winman_first_leaf_visitor(). Do not set it somewhere else!
 */
static b3_ws_winman_first_leaf_visitor_comm_t g_first_leaf;

/**
 * Belongs to b3_active_win_toggle_floating() and
 * b3_ws_winman_active_win_toggle_floating_visitor(). Do not set it somewhere
 * else!
 */
static b3_active_win_toggle_floating_comm_t g_active_win_toggle_floating_comm;

/**
 * Belongs to b3_ws_set_focused_win() and
 * b3_ws_winmanset_focused_win_ws_visitor(). Do not set it somewhere else!
 */
static b3_ws_set_focused_win_comm_t g_ws_set_focused_win_comm;

/**
 * Belongs to b3_ws_arrange_wins() and
 * b3_ws_arrange_wins_ws_visitor(). Do not set it somewhere else!
 */
static b3_ws_arrange_wins_comm_t g_arrange_wins_comm;

static void
b3_ws_winman_amount_visitor(b3_winman_t *winman);

static void
b3_ws_winman_first_leaf_visitor(b3_winman_t *winman);

static void
b3_ws_winman_active_win_toggle_floating_visitor(b3_winman_t *winman);

/**
 * @param area_ stack Stack of RECT *
 */
static void
b3_ws_winman_arrange(b3_ws_t *ws, b3_winman_t *winman, Stack *area_stack);

static void
b3_ws_winman_minimize_visitor(b3_winman_t *winman);

static void
b3_ws_winman_maximize_if_maximized_visitor(b3_winman_t *winman);

static void
b3_ws_winman_set_focused_win_visitor(b3_winman_t *winman);

/**
 * Get the window by direction in the default mode.
 */
static b3_win_t *
b3_ws_get_win_default(b3_ws_t *ws, b3_ws_move_direction_t direction);

/**
 * Moving the window up and down in the default mode.
 */
static int
b3_ws_move_active_win_default_up_down(b3_winman_t *start,
									  b3_winman_t *winman_of_focused_win,
									  b3_win_t *focused_win,
									  b3_ws_move_direction_t direction);

/**
 * Moving the window left and right in the default mode.
 */
static int
b3_ws_move_active_win_default_left_right(b3_winman_t *start,
										 b3_winman_t *winman_of_focused_win,
										 b3_win_t *focused_win,
										 b3_ws_move_direction_t direction);

b3_ws_t *
b3_ws_new(const char *name)
{
	b3_ws_t *ws;

	if (g_get_win_amount_comm.mutex == NULL) g_get_win_amount_comm.mutex = CreateMutex(NULL, FALSE, NULL);
	if (g_first_leaf.mutex == NULL) g_first_leaf.mutex = CreateMutex(NULL, FALSE, NULL);
	if (g_active_win_toggle_floating_comm.mutex == NULL) g_active_win_toggle_floating_comm.mutex = CreateMutex(NULL, FALSE, NULL);
	if (g_ws_set_focused_win_comm.mutex == NULL) g_ws_set_focused_win_comm.mutex  = CreateMutex(NULL, FALSE, NULL);
	if (g_arrange_wins_comm.mutex == NULL) g_arrange_wins_comm.mutex = CreateMutex(NULL, FALSE, NULL);

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

	WaitForSingleObject(g_get_win_amount_comm.mutex, INFINITE);

	g_get_win_amount_comm.amount = 0;
	b3_winman_traverse(ws->winman, b3_ws_winman_amount_visitor);
	amount = g_get_win_amount_comm.amount;

	ReleaseMutex(g_get_win_amount_comm.mutex);

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

			WaitForSingleObject(g_first_leaf.mutex, INFINITE);

			g_first_leaf.first_leaf = NULL;
			b3_winman_traverse(ws->winman, b3_ws_winman_first_leaf_visitor);
			winman = g_first_leaf.first_leaf;

			ws->focused_win = win;

			ReleaseMutex(g_first_leaf.mutex);
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

		b3_winman_reorg(ws->winman);
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
		found_win = b3_winman_contains_win_leaf(winman, win);
	}

	return found_win;
}

int
b3_ws_active_win_toggle_floating(b3_ws_t *ws, const b3_win_t *win)
{
	int toggle_failed;

	WaitForSingleObject(g_active_win_toggle_floating_comm.mutex, INFINITE);

	g_active_win_toggle_floating_comm.win = win;
	g_active_win_toggle_floating_comm.toggle_failed = 1;
	b3_winman_traverse(ws->winman, b3_ws_winman_active_win_toggle_floating_visitor);
	toggle_failed = g_active_win_toggle_floating_comm.toggle_failed;

	ReleaseMutex(g_active_win_toggle_floating_comm.mutex);

	return toggle_failed;
}

int
b3_ws_arrange_wins(b3_ws_t *ws, RECT monitor_area)
{
	/**
	 * Stack of RECT *
	 */
	Stack *area_stack;

	WaitForSingleObject(g_arrange_wins_comm.mutex, INFINITE);


	stack_new(&area_stack);
	stack_push(area_stack, (void *) &monitor_area);

	if (b3_ws_any_win_has_state(ws, MAXIMIZED)) {
		b3_ws_minimize_wins(ws);
		b3_winman_traverse(ws->winman, b3_ws_winman_maximize_if_maximized_visitor);
	} else {
		b3_ws_winman_arrange(ws, ws->winman, area_stack);
		b3_winman_traverse(ws->winman, b3_ws_winman_maximize_if_maximized_visitor);
	}

	stack_pop(area_stack, (void *) NULL);
	stack_destroy(area_stack);

	ReleaseMutex(g_arrange_wins_comm.mutex);

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

	WaitForSingleObject(g_ws_set_focused_win_comm.mutex, INFINITE);

	g_ws_set_focused_win_comm.win = win;
	g_ws_set_focused_win_comm.found = NULL;
	b3_winman_traverse(ws->winman, b3_ws_winman_set_focused_win_visitor);
	set_focused_win_found = g_ws_set_focused_win_comm.found;

	ReleaseMutex(g_ws_set_focused_win_comm.mutex);

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
	b3_winman_t *winman_of_focused_win;

	error = 1;
	winman_of_focused_win = b3_winman_contains_win(ws->winman, b3_ws_get_focused_win(ws));
	if (winman_of_focused_win) {
		error = 2;
		switch (b3_ws_get_mode(ws)) {
		case DEFAULT:
			error = b3_ws_move_active_win_default_up_down(ws->winman,
												  winman_of_focused_win,
												  b3_ws_get_focused_win(ws),
												  direction);
			if (error) {
				error = b3_ws_move_active_win_default_left_right(ws->winman,
														 winman_of_focused_win,
														 b3_ws_get_focused_win(ws),
														 direction);
			}
			break;

		case STACKING:
		case TABBED:
		default:
			wbk_logger_log(&logger, WARNING, "Workspace %s - Mode %d does not support moving the focused window\n",
					       b3_ws_get_name(ws), b3_ws_get_mode(ws));
		}
	} else {
		wbk_logger_log(&logger, SEVERE, "Could not find focused window\n");
	}

	if (!error) {
		if (direction == LEFT) {
			wbk_logger_log(&logger, INFO, "Moved window left\n");
		} else if (direction == UP) {
			wbk_logger_log(&logger, INFO, "Moved window up\n");
		} else if (direction == RIGHT) {
			wbk_logger_log(&logger, INFO, "Moved window right\n");
		} else if (direction == DOWN) {
			wbk_logger_log(&logger, INFO, "Moved window down\n");
		}
	} else {
		wbk_logger_log(&logger, INFO, "Moving window failed\n");
	}

	return error;
}

int
b3_ws_any_win_has_state(b3_ws_t *ws, b3_win_state_t state)
{
	return b3_winman_any_win_has_state(ws->winman, state);
}

b3_win_t *
b3_ws_get_win(b3_ws_t *ws, b3_ws_move_direction_t direction)
{
	b3_winman_t *winman_of_focused_win;
	ArrayIter iter;
	b3_win_t *win;
	int length;
	int i;
	int pos;
	b3_winman_t *parent_winman_of_focused_win;
	b3_winman_t *neighbour_winman;

	win = NULL;
	winman_of_focused_win = b3_winman_contains_win(ws->winman, b3_ws_get_focused_win(ws));
	if (winman_of_focused_win) {
		switch (b3_ws_get_mode(ws)) {
		case DEFAULT:
			win = b3_ws_get_win_default(ws, direction);
			break;

		case STACKING:
		case TABBED:
		default:
			wbk_logger_log(&logger, WARNING, "Workspace %s - Mode %d does not support getting the window by directions\n",
					       b3_ws_get_name(ws), b3_ws_get_mode(ws));
		}
	}

	return win;
}

void
b3_ws_winman_amount_visitor(b3_winman_t *winman)
{
	 if (winman->type == LEAF) {
		 g_get_win_amount_comm.amount += array_size(b3_winman_get_win_arr(winman));
	 }
}

void
b3_ws_winman_first_leaf_visitor(b3_winman_t *winman)
{
	if (winman->type == LEAF && g_first_leaf.first_leaf == NULL) {
		g_first_leaf.first_leaf = winman;
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
		while (g_active_win_toggle_floating_comm.toggle_failed && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, g_active_win_toggle_floating_comm.win) == 0) {
				floating = b3_win_get_floating(win_iter);
				b3_win_set_floating(win_iter, !floating);
				g_active_win_toggle_floating_comm.toggle_failed = 0;
			}
		}
	}
}

void
b3_ws_winman_arrange(b3_ws_t *ws, b3_winman_t *winman, Stack *area_stack)
{
	RECT *my_area;
	RECT *next_area;
	RECT set_rect;
	int length;
	int increment;
	ArrayIter iter;
	b3_winman_t *winman_iter;
	b3_win_t *win_iter;

	stack_peek(area_stack, (void *) &my_area);
	next_area = malloc(sizeof(RECT));
	next_area->top = my_area->top;
	next_area->left = my_area->left;
	next_area->bottom = my_area->bottom;
	next_area->right = my_area->right;
	stack_push(area_stack, (void *) next_area);

	if (b3_winman_get_type(winman) == INNER_NODE) {
		length = array_size(b3_winman_get_winman_arr(winman));
	} else if (b3_winman_get_type(winman) == LEAF) {
		length = 0;
		array_iter_init(&iter, b3_winman_get_win_arr(winman));
		while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (!b3_win_get_floating(win_iter)) {
				length++;
			}
		}
	}

	if (length > 0) {
		if (b3_winman_get_mode(winman) == HORIZONTAL) {
			increment = (my_area->right - my_area->left) / length;
		} else if (b3_winman_get_mode(winman) == VERTICAL) {
			increment = (my_area->bottom - my_area->top) / length;
		} else {
			wbk_logger_log(&logger, SEVERE, "Workspace %s - Unsupported window manager mode: %d",
						   b3_ws_get_name(ws),
						   b3_winman_get_mode(winman));
		}

		wbk_logger_log(&logger, DEBUG, "Workspace %s - Arranging %d wins/winmans as type %d and with mode %d\n",
					   b3_ws_get_name(ws),
					   length,
					   b3_winman_get_type(winman),
					   b3_winman_get_mode(winman));

		array_iter_init(&iter, b3_winman_get_winman_arr(winman));
		while (array_iter_next(&iter, (void*) &winman_iter) != CC_ITER_END) {
			if (b3_winman_get_mode(winman) == HORIZONTAL) {
				next_area->right = next_area->left + increment;
				b3_ws_winman_arrange(ws, winman_iter, area_stack);
				next_area->left += increment;
			} else if (b3_winman_get_mode(winman) == VERTICAL) {
				next_area->bottom = next_area->top + increment;
				b3_ws_winman_arrange(ws, winman_iter, area_stack);
				next_area->top += increment;
			}
		}

		array_iter_init(&iter, b3_winman_get_win_arr(winman));
		while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (!b3_win_get_floating(win_iter)) {
				if (b3_winman_get_mode(winman) == HORIZONTAL) {
					set_rect.left = next_area->left;
					set_rect.top = next_area->top;
					set_rect.right = set_rect.left + increment;
					set_rect.bottom = next_area->bottom;

					next_area->left += increment;
				} else if (b3_winman_get_mode(winman) == VERTICAL) {
					set_rect.left = next_area->left;
					set_rect.top = next_area->top;
					set_rect.right = next_area->right;
					set_rect.bottom = next_area->top + increment;

					next_area->top += increment;
				}

				wbk_logger_log(&logger, DEBUG,
							   "Workspace %s - Window placing - X: %d -> %d, Y: %d -> %d\n",
							   b3_ws_get_name(ws),
							   set_rect.left, set_rect.right - set_rect.left,
							   set_rect.top, set_rect.bottom - set_rect.top);
				b3_win_set_rect(win_iter, set_rect);
				b3_win_show(win_iter, 0);
			}
		}

		while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_get_floating(win_iter)) {
				GetWindowRect(b3_win_get_window_handler(win_iter), &set_rect);
				b3_win_set_rect(win_iter, set_rect);
				b3_win_show(win_iter, 1);
			}
		}
	}

	stack_pop(area_stack, (void *) NULL);
	free(next_area);
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
b3_ws_winman_maximize_if_maximized_visitor(b3_winman_t *winman)
{
	ArrayIter iter;
	b3_win_t *win_iter;

	if (winman->type == LEAF) {
		array_iter_init(&iter, b3_winman_get_win_arr(winman));
		while (array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_get_state(win_iter) == MAXIMIZED) {
				b3_win_show(win_iter, 1);
			}
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
		while (!g_ws_set_focused_win_comm.found && array_iter_next(&iter, (void*) &win_iter) != CC_ITER_END) {
			if (b3_win_compare(win_iter, g_ws_set_focused_win_comm.win) == 0) {
				g_ws_set_focused_win_comm.found = win_iter;
			}
		}
	}
}

int
b3_ws_move_active_win_default_up_down(b3_winman_t *start,
									  b3_winman_t *winman_of_focused_win,
									  b3_win_t *focused_win,
									  b3_ws_move_direction_t direction)
{
	int error;

	b3_winman_t *inner_winman;
	b3_winman_t *leaf_winman;
	b3_winman_t *new_winman;
	int i;
	int length;
	int delpos;
	int inspos;

	error = 1;
	inner_winman = NULL;
	leaf_winman = NULL;
	new_winman = NULL;

	if (direction == UP || direction == DOWN) {
		inner_winman = b3_winman_find_parent_of_winman(start, winman_of_focused_win);
		if (inner_winman) {
			inspos = b3_winman_find_winman_at(inner_winman, winman_of_focused_win);
			if (direction == UP) {
				inspos--;
			} else if (direction == DOWN) {
				if (array_size(b3_winman_get_winman_arr(inner_winman)) > 1) {
					inspos += 2;
				} else {
					inspos++;
				}
			}

			wbk_logger_log(&logger, DEBUG, ">>inspos %d\n", inspos);
			if (inspos == 0) {
				b3_winman_remove_win(start, focused_win);
				array_get_at(b3_winman_get_winman_arr(inner_winman), inspos, (void *) &leaf_winman);
				new_winman = leaf_winman;
			} else if ((inspos > 0 && inspos <= array_size(b3_winman_get_winman_arr(inner_winman)))
					   || (inspos < 0)) {
				if (inspos < 0) {
					inspos = 0;
				}

				b3_winman_remove_win(start, focused_win);
				leaf_winman = winman_of_focused_win;
				new_winman = b3_winman_new(b3_winman_get_mode(leaf_winman));
				array_add_at(b3_winman_get_winman_arr(inner_winman), new_winman, inspos);
			}
		} else {
			b3_winman_remove_win(start, focused_win);
			leaf_winman = b3_winman_to_inner_node(winman_of_focused_win);
			inner_winman = winman_of_focused_win;
			b3_winman_set_mode(inner_winman, VERTICAL);
			new_winman = b3_winman_new(HORIZONTAL);
			b3_winman_add_winman(inner_winman, new_winman);
		}
	}

	if (leaf_winman) {
		array_add(b3_winman_get_win_arr(new_winman), focused_win);
		b3_winman_reorg(start);
		error = 0;
	}

	return error;
}

int
b3_ws_move_active_win_default_left_right(b3_winman_t *start,
										 b3_winman_t *winman_of_focused_win,
										 b3_win_t *focused_win,
										 b3_ws_move_direction_t direction)
{
	int error;
	int i;
	int length;
	int delpos;
	int inspos;
	b3_win_t *win;

	error = 1;

	if (direction == LEFT || direction == RIGHT) {
		delpos = -1;
		length = array_size(b3_winman_get_win_arr(winman_of_focused_win));
		for (i = 0; delpos < 0 && i < length; i++) {
			array_get_at(b3_winman_get_win_arr(winman_of_focused_win), i, (void *) &win);
			if (b3_win_compare(win, focused_win) == 0) {
				delpos = i;
			}
		}

		error = 2;
		if ((direction == LEFT && delpos > 0)
			|| (direction == RIGHT && delpos < length - 1)) {
			array_remove_at(b3_winman_get_win_arr(winman_of_focused_win), delpos, NULL);

			if (direction == LEFT) inspos = delpos - 1;
			if (direction == RIGHT) inspos = delpos + 1;

			array_add_at(b3_winman_get_win_arr(winman_of_focused_win), win, inspos);

			error = 0;
		}
	}

	if (!error) {
		b3_winman_reorg(start);
	}

	return error;
}

b3_win_t *
b3_ws_get_win_default(b3_ws_t *ws, b3_ws_move_direction_t direction)
{
	b3_winman_t *winman_of_focused_win;
	ArrayIter iter;
	b3_win_t *win;
	int length;
	int i;
	int pos;
	b3_winman_t *parent_winman_of_focused_win;
	b3_winman_t *neighbour_winman;

	win = NULL;
	winman_of_focused_win = b3_winman_contains_win(ws->winman, b3_ws_get_focused_win(ws));
	if (winman_of_focused_win) {
		if (direction == LEFT || direction == RIGHT) {
			pos = -1;
			length = array_size(b3_winman_get_win_arr(winman_of_focused_win));
			for (i = 0; pos < 0 && i < length; i++) {
				array_get_at(b3_winman_get_win_arr(winman_of_focused_win), i, (void *) &win);
				if (b3_win_compare(win, b3_ws_get_focused_win(ws)) == 0) {
					pos = i;
				}
			}

			win = NULL;

			if (direction == LEFT) {
				pos--;
			} else if (direction == RIGHT) {
				pos++;
			}

			if (array_get_at(b3_winman_get_win_arr(winman_of_focused_win), pos, (void *) &win) == CC_OK) {
				if (direction == LEFT) {
					wbk_logger_log(&logger, INFO, "Focusing window to the left\n");
				} else if (direction == RIGHT) {
					wbk_logger_log(&logger, INFO, "Focusing window to the right\n");
				}
			}
		} else if (direction == UP || direction == DOWN) {
			parent_winman_of_focused_win = b3_winman_find_parent_of_winman(ws->winman, winman_of_focused_win);
			if (parent_winman_of_focused_win) {
				pos = b3_winman_find_winman_at(parent_winman_of_focused_win, winman_of_focused_win);

				if (direction == UP) {
					pos--;
				} else if (direction == DOWN) {
					pos++;
				}
				if (array_get_at(b3_winman_get_winman_arr(parent_winman_of_focused_win),
								 pos,
								 (void *) &neighbour_winman) == CC_OK) {
					win = b3_winman_get_next_window(neighbour_winman);

					if (direction == UP) {
						wbk_logger_log(&logger, INFO, "Focusing window to the top\n");
					} else if (direction == DOWN) {
						wbk_logger_log(&logger, INFO, "Focusing window to the bottom\n");
					}
				}
			}
		} else {
			wbk_logger_log(&logger, SEVERE, "Focusing window failed - unsupported direction %d\n", direction);
		}
	}

	return win;

}
