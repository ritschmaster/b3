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
 * @brief File contains the title condition class implementation and its private methods
 */

#include "title_condition.h"

#include <w32bindkeys/logger.h>

#include "utils.h"

static wbk_logger_t logger = { "title_condition" };

#define B3_TITLE_CONDITION_TITLE_BUFFER_LENGTH 255

/**
 * Implementation of b3_condition_free().
 */
static int
b3_title_condition_free_impl(b3_condition_t *condition);

/**
 * Implementation of b3_condition_applies().
 */
static int
b3_title_condition_applies_impl(b3_condition_t *condition, b3_director_t *director, b3_win_t *win);

b3_title_condition_t *
b3_title_condition_new(const char *pattern)
{
  int error;
    b3_title_condition_t *title_condition;
  b3_pattern_condition_t *pattern_condition;

  error = 0;
  title_condition = NULL;

  if (!error) {
    title_condition = malloc(sizeof(b3_title_condition_t));

    if (title_condition == NULL) {
      wbk_logger_log(&logger, SEVERE, "Could not allocate memory.\n");
      error = 3;
    }
  }

  if (!error) {
    pattern_condition = b3_pattern_condition_new(pattern);
    memcpy(title_condition, pattern_condition, sizeof(b3_pattern_condition_t));
    free(pattern_condition); /* Just free the top level element */

    title_condition->super_condition_free = title_condition->pattern_condition.condition.condition_free;
    title_condition->super_condition_applies = title_condition->pattern_condition.condition.condition_applies;

    title_condition->pattern_condition.condition.condition_free = b3_title_condition_free_impl;
    title_condition->pattern_condition.condition.condition_applies = b3_title_condition_applies_impl;
  }

  return title_condition;
}

int
b3_title_condition_free_impl(b3_condition_t *condition)
{
  b3_title_condition_t *title_condition;

  title_condition = (b3_title_condition_t *) condition;

  title_condition->super_condition_free(condition);

  return 0;
}

int
b3_title_condition_applies_impl(b3_condition_t *condition, b3_director_t *director, b3_win_t *win)
{
  b3_title_condition_t *title_condition;
  int applies;
	char title[B3_TITLE_CONDITION_TITLE_BUFFER_LENGTH];
  int pcre_rc;
  pcre *re_compiled;
  pcre_extra *re_extra;
  int error;

  title_condition = (b3_title_condition_t *) condition;
  applies = 0;
  error = 0;

  if (!error) {
    if (b3_pattern_condition_get_use_focused_as_pattern((b3_pattern_condition_t *) title_condition)) {
      GetWindowText(b3_win_get_window_handler(b3_monitor_get_focused_win(b3_director_get_focused_monitor(director))),
                    title,
                    B3_TITLE_CONDITION_TITLE_BUFFER_LENGTH);
      error = b3_compile_pattern(title, &re_compiled, &re_extra);
    } else {
      re_compiled = b3_pattern_condition_get_re_compiled((b3_pattern_condition_t *) title_condition);
      re_extra = b3_pattern_condition_get_re_extra((b3_pattern_condition_t *) title_condition);
    }
  }

  if (!error) {
    GetWindowText(b3_win_get_window_handler(win), title, B3_TITLE_CONDITION_TITLE_BUFFER_LENGTH);
    pcre_rc = pcre_exec(re_compiled,
                        re_extra,
                        title,
                        strlen(title),
                        0,
                        0,
                        NULL,
                        0);
    if (pcre_rc >= 0) {
      applies = 1;
    }
  }

  if (!error) {
    if (b3_pattern_condition_get_use_focused_as_pattern((b3_pattern_condition_t *) title_condition)) {
      pcre_free(re_compiled);
#ifdef PCRE_CONFIG_JIT
      pcre_free_study(re_extra);
#else
      pcre_free(re_extra);
#endif
    }
  }

  return applies;
}
