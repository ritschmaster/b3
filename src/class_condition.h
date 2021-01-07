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
 * @brief File contains the class condition and class definition
 *
 * b3_class_condition_t inherits all methods of b3_pattern_condition_t (see condition.h).
 */

#ifndef B3_CLASS_CONDITION_H
#define B3_CLASS_CONDITION_H

#include "pattern_condition.h"

#include <pcre.h>

#include "director.h"
#include "win.h"

typedef struct b3_class_condition_s b3_class_condition_t;

struct b3_class_condition_s
{
  b3_pattern_condition_t pattern_condition;
  int (*super_condition_free)(b3_condition_t *condition);
  int (*super_condition_applies)(b3_condition_t *condition, b3_director_t *director, b3_win_t *win);
};

extern b3_class_condition_t *
b3_class_condition_new(const char *pattern);

#endif // B3_CLASS_CONDITION_H
