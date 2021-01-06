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
 * @brief File contains the rule class implementation and its private methods
 */

#include "rule.h"

static int
b3_rule_free_impl(b3_rule_t *rule);

static int
b3_rule_applies_impl(b3_rule_t *rule, b3_director_t *director, b3_win_t *win);

static int
b3_rule_exec_impl(b3_rule_t *rule, b3_director_t *director, b3_win_t *win);

b3_rule_t *
b3_rule_new(b3_condition_t *condition, b3_action_t *action)
{
  b3_rule_t *rule;

  rule = malloc(sizeof(b3_rule_t));

  if (rule) {
    rule->rule_free = b3_rule_free_impl;
    rule->rule_applies = b3_rule_applies_impl;
    rule->rule_exec = b3_rule_exec_impl;

    rule->condition = condition;
    rule->action = action;
  }

  return rule;
}

int
b3_rule_free(b3_rule_t *rule)
{
  return rule->rule_free(rule);
}

int
b3_rule_applies(b3_rule_t *rule, b3_director_t *director, b3_win_t *win)
{
  return rule->rule_applies(rule, director, win);
}

int
b3_rule_exec(b3_rule_t *rule, b3_director_t *director, b3_win_t *win)
{
  return rule->rule_exec(rule, director, win);
}

int
b3_rule_free_impl(b3_rule_t *rule)
{
  b3_condition_free(rule->condition);
  rule->condition = NULL;

  b3_action_free(rule->action);
  rule->action = NULL;

  free(rule);

  return 0;
}

int
b3_rule_applies_impl(b3_rule_t *rule, b3_director_t *director, b3_win_t *win)
{
  return b3_condition_applies(rule->condition, director, win);
}

int
b3_rule_exec_impl(b3_rule_t *rule, b3_director_t *director, b3_win_t *win)
{
  return b3_action_exec(rule->action, director, win);
}
