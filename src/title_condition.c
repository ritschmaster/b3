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
 * @brief File contains the condition title class implementation and its private methods
 */

#include "title_condition.h"

#include <w32bindkeys/logger.h>

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
  const char *pcre_err_str;
  int pcre_err_offset;
  pcre *re_compiled;
  pcre_extra *re_extra;
  b3_title_condition_t *title_condition;
  b3_condition_t *condition;

  error = 0;
  title_condition = NULL;

  if (!error) {
    re_compiled = pcre_compile(pattern, 0, &pcre_err_str, &pcre_err_offset, NULL);

    if(re_compiled == NULL) {
      wbk_logger_log(&logger, SEVERE, "Could not compile '%s': %s\n", pattern, pcre_err_str);
      error = 1;
    }
  }

  if (!error) {
    re_extra = pcre_study(re_compiled, 0, &pcre_err_str);

    if (pcre_err_str) {
      wbk_logger_log(&logger, SEVERE, "Could not study '%s': %s\n", pattern, pcre_err_str);
      error = 2;
    }
  }

  if (!error) {
    title_condition = malloc(sizeof(b3_title_condition_t));

    if (title_condition == NULL) {
      wbk_logger_log(&logger, SEVERE, "Could not allocate memory.\n");
      error = 3;
    }
  }

  if (!error) {
    condition = b3_condition_new();
    memcpy(title_condition, condition, sizeof(b3_condition_t));
    free(condition); /* Just free the top level element */

    title_condition->super_condition_free = title_condition->condition.condition_free;
    title_condition->super_condition_applies = title_condition->condition.condition_applies;

    title_condition->condition.condition_free = b3_title_condition_free_impl;
    title_condition->condition.condition_applies = b3_title_condition_applies_impl;

    title_condition->re_compiled = re_compiled;
    title_condition->re_extra = re_extra;
  }

  return title_condition;
}

int
b3_title_condition_free_impl(b3_condition_t *condition)
{
  b3_title_condition_t *title_condition;

  title_condition = (b3_title_condition_t *) condition;

  pcre_free(title_condition->re_compiled);
#ifdef PCRE_CONFIG_JIT
  pcre_free_study(title_condition->re_extra);
#else
  pcre_free(title_condition->re_extra);
#endif

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

  title_condition = (b3_title_condition_t *) condition;
  applies = 0;

  GetWindowText(b3_win_get_window_handler(win), title, B3_TITLE_CONDITION_TITLE_BUFFER_LENGTH);
  pcre_rc = pcre_exec(title_condition->re_compiled,
                      title_condition->re_extra,
                      title,
                      strlen(title),
                      0,
                      0,
                      NULL,
                      0);
  if (pcre_rc >= 0) {
    applies = 1;
  }

  return applies;
}
