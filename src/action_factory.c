/******************************************************************************
  This file is part of b3.

  Copyright 2020-2021 Richard Paul Baeck <richard.baeck@mailbox.org>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following floating_actions:

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
 * @brief File contains the action factory class implementation and its private
 * methods
 */

#include "action_factory.h"

#include <stdlib.h>

static int
b3_action_factory_free_impl(b3_action_factory_t *action_factory);

static b3_action_list_t *
b3_action_factory_create_list_impl(b3_action_factory_t *action_factory);

static b3_floating_action_t *
b3_action_factory_create_fa_impl(b3_action_factory_t *action_factory);

b3_action_factory_t *
b3_action_factory_new(void)
{
  b3_action_factory_t *action_factory;

  action_factory = malloc(sizeof(b3_action_factory_t));

  if (action_factory) {
    action_factory->action_factory_free = b3_action_factory_free_impl;
    action_factory->action_factory_create_list = b3_action_factory_create_list_impl;
    action_factory->action_factory_create_fa = b3_action_factory_create_fa_impl;
  }

  return action_factory;
}

int
b3_action_factory_free(b3_action_factory_t *action_factory)
{
  return action_factory->action_factory_free(action_factory);
}

b3_action_list_t *
b3_action_factory_create_list(b3_action_factory_t *action_factory)
{
  return action_factory->action_factory_create_list(action_factory);
}

b3_floating_action_t *
b3_action_factory_create_fa(b3_action_factory_t *action_factory)
{
  return action_factory->action_factory_create_fa(action_factory);
}

int
b3_action_factory_free_impl(b3_action_factory_t *action_factory)
{
  free(action_factory);

  return 0;
}

b3_action_list_t *
b3_action_factory_create_list_impl(b3_action_factory_t *action_factory)
{
  b3_action_list_t *action_list;

  action_list = b3_action_list_new();

  return action_list;
}

b3_floating_action_t *
b3_action_factory_create_fa_impl(b3_action_factory_t *action_factory)
{
  b3_floating_action_t *fa;

  fa = b3_floating_action_new();

  return fa;
}
