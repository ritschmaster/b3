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
 * @brief File contains the action list class definition
 *
 * b3_action_list_t inherits all methods of b3_action_t (see action.h).
 */

#ifndef B3_ACTION_LIST_H
#define B3_ACTION_LIST_H

#include "action.h"

#include <collectc/array.h>

#include "director.h"
#include "win.h"

typedef struct b3_action_list_s b3_action_list_t;

struct b3_action_list_s
{
  b3_action_t action;
  int (*super_action_free)(b3_action_t *action);
  int (*super_action_exec)(b3_action_t *action, b3_director_t *director, b3_win_t *win);

  int (*action_list_add)(b3_action_list_t *action_list, b3_action_t *new_action);

	/**
	 * Array of b3_action_t *
	 */
	Array *action_arr;
};

extern b3_action_list_t *
b3_action_list_new(void);

/**
 * Adds a new action.
 *
 * @param new_action The action will be freed by the action list object! Do not
 * free it by yourself!
 */
extern int
b3_action_list_add(b3_action_list_t *action_list, b3_action_t *new_action);

#endif // B3_ACTION_LIST_H
