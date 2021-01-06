/******************************************************************************
  This file is part of b3.

  Copyright 2020-2021 Richard Paul Baeck <richard.baeck@mailbox.org>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following floating_conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN CONDITION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*******************************************************************************/

/**
 * @author Richard Bäck <richard.baeck@mailbox.org>
 * @date 2020-01-05
 * @brief File contains the condition factory class implementation and its private
 * methods
 */

#include "condition_factory.h"

#include <stdlib.h>

static int
b3_condition_factory_free_impl(b3_condition_factory_t *condition_factory);

static b3_condition_and_t *
b3_condition_factory_create_and_impl(b3_condition_factory_t *condition_factory);

static b3_title_condition_t *
b3_condition_factory_create_tc_impl(b3_condition_factory_t *condition_factory, const char *pattern);

b3_condition_factory_t *
b3_condition_factory_new(void)
{
  b3_condition_factory_t *condition_factory;

  condition_factory = malloc(sizeof(b3_condition_factory_t));

  if (condition_factory) {
    condition_factory->condition_factory_free = b3_condition_factory_free_impl;
    condition_factory->condition_factory_create_and = b3_condition_factory_create_and_impl;
    condition_factory->condition_factory_create_tc = b3_condition_factory_create_tc_impl;
  }

  return condition_factory;
}

int
b3_condition_factory_free(b3_condition_factory_t *condition_factory)
{
  return condition_factory->condition_factory_free(condition_factory);
}

b3_condition_and_t *
b3_condition_factory_create_and(b3_condition_factory_t *condition_factory)
{
  return condition_factory->condition_factory_create_and(condition_factory);
}

b3_title_condition_t *
b3_condition_factory_create_tc(b3_condition_factory_t *condition_factory, const char *pattern)
{
  return condition_factory->condition_factory_create_tc(condition_factory, pattern);
}

int
b3_condition_factory_free_impl(b3_condition_factory_t *condition_factory)
{
  free(condition_factory);

  return 0;
}

b3_condition_and_t *
b3_condition_factory_create_and_impl(b3_condition_factory_t *condition_factory)
{
  b3_condition_and_t *condition_and;

  condition_and = b3_condition_and_new();

  return condition_and;
}

b3_title_condition_t *
b3_condition_factory_create_tc_impl(b3_condition_factory_t *condition_factory, const char *pattern)
{
  b3_title_condition_t *fa;

  fa = b3_title_condition_new(pattern);

  return fa;
}
