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
 * @date 2020-01-10
 * @brief File contains the move window to workspace action class implementation and its private methods
 */

#include "mwtw_action.h"

#include <w32bindkeys/logger.h>

static wbk_logger_t logger = { "mwtw_action" };

/**
 * Implementation of b3_action_free().
 */
static int
b3_mwtw_action_free_impl(b3_action_t *action);

/**
 * Implementation of b3_action_exec().
 */
static int
b3_mwtw_action_exec_impl(b3_action_t *action, b3_director_t *director, b3_win_t *win);

b3_mwtw_action_t *
b3_mwtw_action_new(char *ws_id)
{
  b3_action_t *action;
  b3_mwtw_action_t *mwtw_action;

  mwtw_action = malloc(sizeof(b3_mwtw_action_t));

  if (mwtw_action) {
    action = b3_action_new();
    memcpy(mwtw_action, action, sizeof(b3_action_t));
    free(action); /* Just free the top level element */

    mwtw_action->super_action_free = mwtw_action->action.action_free;
    mwtw_action->super_action_exec = mwtw_action->action.action_exec;

    mwtw_action->action.action_free = b3_mwtw_action_free_impl;
    mwtw_action->action.action_exec = b3_mwtw_action_exec_impl;

    mwtw_action->ws_id = ws_id;
  }

  return mwtw_action;
}

int
b3_mwtw_action_free_impl(b3_action_t *action)
{
  b3_mwtw_action_t *mwtw_action;

  mwtw_action = (b3_mwtw_action_t *) action;

  free(mwtw_action->ws_id);
  mwtw_action->ws_id = NULL;

	mwtw_action->super_action_free(action);

  return 0;
}

int
b3_mwtw_action_exec_impl(b3_action_t *action, b3_director_t *director, b3_win_t *win)
{
  b3_mwtw_action_t *mwtw_action;
  int error;

  mwtw_action = (b3_mwtw_action_t *) action;

	error = b3_director_move_win_to_ws(director, win, mwtw_action->ws_id);

  return error;
}
