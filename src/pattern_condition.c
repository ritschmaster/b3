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
 * @date 2020-01-07
 * @brief File contains the pattern condition class implementation and its private methods
 */

#include "pattern_condition.h"

#include <w32bindkeys/logger.h>

#include "utils.h"

static wbk_logger_t logger = { "pattern_condition" };

#define B3_PATTERN_CONDITION_TITLE_BUFFER_LENGTH 255

/**
 * Implementation of b3_condition_free().
 */
static int
b3_pattern_condition_free_impl(b3_condition_t *condition);

/**
 * Implementation of b3_condition_applies().
 */
static int
b3_pattern_condition_applies_impl(b3_condition_t *condition, b3_director_t *director, b3_win_t *win);

static pcre *
b3_pattern_condition_get_re_compiled_impl(b3_pattern_condition_t *pattern_condition);

static pcre_extra *
b3_pattern_condition_get_re_extra_impl(b3_pattern_condition_t *pattern_condition);

static char
b3_pattern_condition_get_use_focused_as_pattern_impl(b3_pattern_condition_t *pattern_condition);

b3_pattern_condition_t *
b3_pattern_condition_new(const char *pattern)
{
  int error;
  pcre *re_compiled;
  pcre_extra *re_extra;
  char use_focused_as_pattern;
  b3_pattern_condition_t *pattern_condition;
  b3_condition_t *condition;

  error = 0;
  pattern_condition = NULL;
  re_compiled = NULL;
  re_extra = NULL;
  use_focused_as_pattern = 0;

  if (strcmp(pattern, B3_PATTERN_CONDITION_PATTERN_FOCUSED)) {
    error = b3_compile_pattern(pattern, &re_compiled, &re_extra);
  } else {
    use_focused_as_pattern = 1;
  }

  if (!error) {
    pattern_condition = malloc(sizeof(b3_pattern_condition_t));

    if (pattern_condition == NULL) {
      wbk_logger_log(&logger, SEVERE, "Could not allocate memory.\n");
      error = 3;
    }
  }

  if (!error) {
    condition = b3_condition_new();
    memcpy(pattern_condition, condition, sizeof(b3_condition_t));
    free(condition); /* Just free the top level element */

    /** Retrieve super methods */
    pattern_condition->super_condition_free = pattern_condition->condition.condition_free;
    pattern_condition->super_condition_applies = pattern_condition->condition.condition_applies;

    /** Overwrite b3_condition_t methods */
    pattern_condition->condition.condition_free = b3_pattern_condition_free_impl;
    pattern_condition->condition.condition_applies = b3_pattern_condition_applies_impl;

    /** Place b3_pattern_condition_t methods */
    pattern_condition->pattern_condition_get_re_compiled = b3_pattern_condition_get_re_compiled_impl;
    pattern_condition->pattern_condition_get_re_extra = b3_pattern_condition_get_re_extra_impl;
    pattern_condition->pattern_condition_get_use_focused_as_pattern = b3_pattern_condition_get_use_focused_as_pattern_impl;

    pattern_condition->re_compiled = re_compiled;
    pattern_condition->re_extra = re_extra;
    pattern_condition->use_focused_as_pattern = use_focused_as_pattern;
  }

  return pattern_condition;
}

pcre *
b3_pattern_condition_get_re_compiled(b3_pattern_condition_t *pattern_condition)
{
  return pattern_condition->pattern_condition_get_re_compiled(pattern_condition);
}

pcre_extra *
b3_pattern_condition_get_re_extra(b3_pattern_condition_t *pattern_condition)
{
  return pattern_condition->pattern_condition_get_re_extra(pattern_condition);
}

char
b3_pattern_condition_get_use_focused_as_pattern(b3_pattern_condition_t *pattern_condition)
{
  return pattern_condition->pattern_condition_get_use_focused_as_pattern(pattern_condition);
}

int
b3_pattern_condition_free_impl(b3_condition_t *condition)
{
  b3_pattern_condition_t *pattern_condition;

  pattern_condition = (b3_pattern_condition_t *) condition;

  pcre_free(pattern_condition->re_compiled);
#ifdef PCRE_CONFIG_JIT
  pcre_free_study(pattern_condition->re_extra);
#else
  pcre_free(pattern_condition->re_extra);
#endif

  pattern_condition->super_condition_free(condition);

  return 0;
}

int
b3_pattern_condition_applies_impl(b3_condition_t *condition, b3_director_t *director, b3_win_t *win)
{
  return -1;
}

pcre *
b3_pattern_condition_get_re_compiled_impl(b3_pattern_condition_t *pattern_condition)
{
  return pattern_condition->re_compiled;
}

pcre_extra *
b3_pattern_condition_get_re_extra_impl(b3_pattern_condition_t *pattern_condition)
{
  return pattern_condition->re_extra;
}

char
b3_pattern_condition_get_use_focused_as_pattern_impl(b3_pattern_condition_t *pattern_condition)
{
  return pattern_condition->use_focused_as_pattern;
}
