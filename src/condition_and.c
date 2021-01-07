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
 * @brief File contains the condition and class implementation and its private methods
 */

#include "condition_and.h"

/**
 * Implementation of b3_condition_free().
 */
static int
b3_condition_and_free_impl(b3_condition_t *condition);

/**
 * Implementation of b3_condition_applies().
 */
static int
b3_condition_and_applies_impl(b3_condition_t *condition, b3_director_t *director, b3_win_t *win);

static int
b3_condition_and_add_impl(b3_condition_and_t *condition_and, b3_condition_t *new_condition);

b3_condition_and_t *
b3_condition_and_new(void)
{
  b3_condition_t *condition;
  b3_condition_and_t *condition_and;

  condition_and = malloc(sizeof(b3_condition_and_t));

  if (condition_and) {
    condition = b3_condition_new();
    memcpy(condition_and, condition, sizeof(b3_condition_t));
    free(condition); /* Just free the top level element */

    condition_and->super_condition_free = condition_and->condition.condition_free;
    condition_and->super_condition_applies = condition_and->condition.condition_applies;

    condition_and->condition.condition_free = b3_condition_and_free_impl;
    condition_and->condition.condition_applies = b3_condition_and_applies_impl;

    condition_and->condition_and_add = b3_condition_and_add_impl;

    array_new(&(condition_and->condition_arr));
  }

  return condition_and;
}

int
b3_condition_and_add(b3_condition_and_t *condition_and, b3_condition_t *new_condition)
{
  return condition_and->condition_and_add(condition_and, new_condition);
}

int
b3_condition_and_free_impl(b3_condition_t *condition)
{
  b3_condition_and_t *condition_and;
  ArrayIter iter;
	b3_condition_t *condition_iter;

  condition_and = (b3_condition_and_t *) condition;

	array_iter_init(&iter, condition_and->condition_arr);
	while (array_iter_next(&iter, (void*) &condition_iter) != CC_ITER_END) {
    b3_condition_free(condition_iter);
  }

  array_destroy_cb(condition_and->condition_arr, NULL);

  condition_and->super_condition_free(condition);

  return 0;
}

int
b3_condition_and_applies_impl(b3_condition_t *condition, b3_director_t *director, b3_win_t *win)
{
  b3_condition_and_t *condition_and;
  ArrayIter iter;
	b3_condition_t *condition_iter;
  int applies;

  condition_and = (b3_condition_and_t *) condition;

  applies = 1;
	array_iter_init(&iter, condition_and->condition_arr);
	while (applies && array_iter_next(&iter, (void*) &condition_iter) != CC_ITER_END) {
    applies = b3_condition_applies(condition_iter, director, win);
  }

  return applies;
}

int
b3_condition_and_add_impl(b3_condition_and_t *condition_and, b3_condition_t *new_condition)
{
  array_add(condition_and->condition_arr, new_condition);

  return 0;
}
