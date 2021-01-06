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
 * @brief File contains the rule class definition
 */

#ifndef B3_RULE_H
#define B3_RULE_H

#include "director.h"
#include "win.h"
#include "condition.h"
#include "action.h"

typedef struct b3_rule_s b3_rule_t;

struct b3_rule_s
{
  int (*rule_free)(b3_rule_t *rule);
  int (*rule_applies)(b3_rule_t *rule, b3_director_t *director, b3_win_t *win);
  int (*rule_exec)(b3_rule_t *rule, b3_director_t *director, b3_win_t *win);

  b3_condition_t *condition;
  b3_action_t *action;
};

/**
 * @param condition The condition object will be freed by the rule object. Do not free it by yourself!
 * @param action The action object will be freed by the rule object. Do not free it by yourself!
 */
extern b3_rule_t *
b3_rule_new(b3_condition_t *condition, b3_action_t *action);

extern int
b3_rule_free(b3_rule_t *rule);

/**
 * Checks if rule applies to director and win.
 */
extern int
b3_rule_applies(b3_rule_t *rule, b3_director_t *director, b3_win_t *win);

/**
 * Executes rule with director and win.
 */
extern int
b3_rule_exec(b3_rule_t *rule, b3_director_t *director, b3_win_t *win);

#endif // B3_RULE_H
