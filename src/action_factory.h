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
 * @brief File contains the action factory class definition
 */

#ifndef B3_ACTION_FACTORY_H
#define B3_ACTION_FACTORY_H

#include "action_list.h"
#include "floating_action.h"

typedef struct b3_action_factory_s b3_action_factory_t;

struct b3_action_factory_s
{
  int (*action_factory_free)(b3_action_factory_t *action_factory);
  b3_action_list_t *(*action_factory_create_list)(b3_action_factory_t *action_factory);
  b3_floating_action_t *(*action_factory_create_fa)(b3_action_factory_t *action_factory);
};

extern b3_action_factory_t *
b3_action_factory_new(void);

extern int
b3_action_factory_free(b3_action_factory_t *action_factory);

/**
 * @param A new object. Free it by yourself!
 */
extern b3_action_list_t *
b3_action_factory_create_list(b3_action_factory_t *action_factory);

/**
 * @param A new object. Free it by yourself!
 */
extern b3_floating_action_t *
b3_action_factory_create_fa(b3_action_factory_t *action_factory);

#endif // B3_ACTION_FACTORY_H
