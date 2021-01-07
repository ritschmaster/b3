/******************************************************************************
  This file is part of b3.

  Copyright 2020-2021 Richard Paul Baeck <richard.baeck@mailbox.org>

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
 * @date 2020-01-03
 * @brief File contains the action list class implementation and its private methods
 */

#include "action_list.h"

/**
 * Implementation of b3_action_free().
 */
static int
b3_action_list_free_impl(b3_action_t *action);

/**
 * Implementation of b3_action_exec().
 */
static int
b3_action_list_exec_impl(b3_action_t *action, b3_director_t *director, b3_win_t *win);

static int
b3_action_list_add_impl(b3_action_list_t *action_list, b3_action_t *new_action);

b3_action_list_t *
b3_action_list_new(void)
{
  b3_action_t *action;
  b3_action_list_t *action_list;

  action_list = malloc(sizeof(b3_action_list_t));

  if (action_list) {
    action = b3_action_new();
    memcpy(action_list, action, sizeof(b3_action_t));
    free(action); /* Just free the top level element */

    action_list->super_action_free = action_list->action.action_free;
    action_list->super_action_exec = action_list->action.action_exec;

    action_list->action.action_free = b3_action_list_free_impl;
    action_list->action.action_exec = b3_action_list_exec_impl;

    action_list->action_list_add = b3_action_list_add_impl;

    array_new(&(action_list->action_arr));
  }

  return action_list;
}

int
b3_action_list_add(b3_action_list_t *action_list, b3_action_t *new_action)
{
  return action_list->action_list_add(action_list, new_action);
}

int
b3_action_list_free_impl(b3_action_t *action)
{
  b3_action_list_t *action_list;
  ArrayIter iter;
	b3_action_t *action_iter;

  action_list = (b3_action_list_t *) action;

	array_iter_init(&iter, action_list->action_arr);
	while (array_iter_next(&iter, (void*) &action_iter) != CC_ITER_END) {
    b3_action_free(action_iter);
  }

  array_destroy_cb(action_list->action_arr, NULL);

  action_list->super_action_free(action);

  return 0;
}

int
b3_action_list_exec_impl(b3_action_t *action, b3_director_t *director, b3_win_t *win)
{
  b3_action_list_t *action_list;
  ArrayIter iter;
	b3_action_t *action_iter;
  int error;

  action_list = (b3_action_list_t *) action;

  error = 0;
	array_iter_init(&iter, action_list->action_arr);
	while (!error && array_iter_next(&iter, (void*) &action_iter) != CC_ITER_END) {
    error = b3_action_exec(action_iter, director, win);
  }

  return error;
}

int
b3_action_list_add_impl(b3_action_list_t *action_list, b3_action_t *new_action)
{
  array_add(action_list->action_arr, new_action);

  return 0;
}
