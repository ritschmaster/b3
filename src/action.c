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
 * @brief File contains the action class implementation and its private methods
 */

#include "action.h"

static int
b3_action_free_impl(b3_action_t *action);

static int
b3_action_exec_impl(b3_action_t *action, b3_director_t *director, b3_win_t *win);

b3_action_t *
b3_action_new(void)
{
  b3_action_t *action;

  action = malloc(sizeof(b3_action_t));

  if (action) {
    action->action_free = b3_action_free_impl;
    action->action_exec = b3_action_exec_impl;
  }

  return action;
}

int
b3_action_free(b3_action_t *action)
{
  return action->action_free(action);
}

int
b3_action_exec(b3_action_t *action, b3_director_t *director, b3_win_t *win)
{
  return action->action_exec(action, director, win);
}

int
b3_action_free_impl(b3_action_t *action)
{
  free(action);

  return 0;
}

int
b3_action_exec_impl(b3_action_t *action, b3_director_t *director, b3_win_t *win)
{
  return -1;
}
