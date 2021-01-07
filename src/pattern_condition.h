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
 * @brief File contains the pattern condition and class definition
 *
 * b3_pattern_condition_t inherits all methods of b3_condition_t (see condition.h).
 */

#ifndef B3_PATTERN_CONDITION_H
#define B3_PATTERN_CONDITION_H

#include "condition.h"

#include <pcre.h>

#include "director.h"
#include "win.h"

/**
 * If this special pattern is used then the title of the currently focused
 * window is used as the pattern.
 */
#define B3_PATTERN_CONDITION_PATTERN_FOCUSED "__focused__"

typedef struct b3_pattern_condition_s b3_pattern_condition_t;

struct b3_pattern_condition_s
{
  b3_condition_t condition;
  int (*super_condition_free)(b3_condition_t *condition);
  int (*super_condition_applies)(b3_condition_t *condition, b3_director_t *director, b3_win_t *win);

  pcre *(*pattern_condition_get_re_compiled)(b3_pattern_condition_t *pattern_condition);
  pcre_extra *(*pattern_condition_get_re_extra)(b3_pattern_condition_t *pattern_condition);
  char (*pattern_condition_get_use_focused_as_pattern)(b3_pattern_condition_t *pattern_condition);

  pcre *re_compiled;
  pcre_extra *re_extra;

  char use_focused_as_pattern;
};

extern b3_pattern_condition_t *
b3_pattern_condition_new(const char *pattern);

extern pcre *
b3_pattern_condition_get_re_compiled(b3_pattern_condition_t *pattern_condition);

extern pcre_extra *
b3_pattern_condition_get_re_extra(b3_pattern_condition_t *pattern_condition);

extern char
b3_pattern_condition_get_use_focused_as_pattern(b3_pattern_condition_t *pattern_condition);

#endif // B3_PATTERN_CONDITION_H
