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
 * @brief File contains the condition and and class definition
 *
 * b3_condition_and_t inherits all methods of b3_condition_t (see condition.h).
 */

#ifndef B3_CONDITION_AND_H
#define B3_CONDITION_AND_H

#include "condition.h"

#include <collectc/array.h>

#include "director.h"
#include "win.h"

typedef struct b3_condition_and_s b3_condition_and_t;

struct b3_condition_and_s
{
  b3_condition_t condition;
  int (*super_condition_free)(b3_condition_t *condition);
  int (*super_condition_applies)(b3_condition_t *condition, b3_director_t *director, b3_win_t *win);

  int (*condition_and_add)(b3_condition_and_t *condition_and, b3_condition_t *new_condition);

	/**
	 * Array of b3_condition_t *
	 */
	Array *condition_arr;
};

extern b3_condition_and_t *
b3_condition_and_new(void);

/**
 * Adds a new condition.
 * @param new_condition The condition will be freed by the condition and object!
 */
extern int
b3_condition_and_add(b3_condition_and_t *condition_and, b3_condition_t *new_condition);

#endif // B3_CONDITION_AND_H
