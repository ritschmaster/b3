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
 * @date 2020-01-05
 * @brief File contains the floating action class implementation and its private methods
 */

#include "floating_action.h"

#include <w32bindkeys/logger.h>

static wbk_logger_t logger = { "floating_action" };

/**
 * Implementation of b3_action_free().
 */
static int
b3_floating_action_free_impl(b3_action_t *action);

/**
 * Implementation of b3_action_exec().
 */
static int
b3_floating_action_exec_impl(b3_action_t *action, b3_director_t *director, b3_win_t *win);

b3_floating_action_t *
b3_floating_action_new(void)
{
  b3_action_t *action;
  b3_floating_action_t *floating_action;

  floating_action = malloc(sizeof(b3_floating_action_t));

  if (floating_action) {
    action = b3_action_new();
    memcpy(floating_action, action, sizeof(b3_action_t));
    free(action); /* Just free the top level element */

    floating_action->super_action_free = floating_action->action.action_free;
    floating_action->super_action_exec = floating_action->action.action_exec;

    floating_action->action.action_free = b3_floating_action_free_impl;
    floating_action->action.action_exec = b3_floating_action_exec_impl;
  }

  return floating_action;
}

int
b3_floating_action_free_impl(b3_action_t *action)
{
  b3_floating_action_t *floating_action;

  floating_action = (b3_floating_action_t *) action;

	floating_action->super_action_free(action);

  return 0;
}

int
b3_floating_action_exec_impl(b3_action_t *action, b3_director_t *director, b3_win_t *win)
{
  b3_floating_action_t *floating_action;
  ArrayIter iter;
  b3_monitor_t *monitor_iter;
  b3_ws_t *found_ws;
  int error;

  floating_action = (b3_floating_action_t *) action;

  error = 1;

  found_ws = NULL;
  array_iter_init(&iter, b3_director_get_monitor_arr(director));
  while (found_ws == NULL && array_iter_next(&iter, (void*) &monitor_iter) != CC_ITER_END) {
    found_ws = b3_monitor_find_win(monitor_iter, win);
  }

  if (found_ws) {
    error = b3_ws_toggle_floating_win(found_ws, win);
  }

  return error;
}
