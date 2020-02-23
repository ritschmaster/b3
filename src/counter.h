/******************************************************************************
  This file is part of b3.

  Copyright 2020 Richard Paul Baeck <richard.baeck@mailbox.org>

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
 * @author Richard B�ck
 * @date 2020-02-16
 * @brief File contains the counter definition
 */

#include <collectc/list.h>

#ifndef COUNTER_H
#define COUNTER_H

typedef struct b3_counter_s
{
	int counter;
	int reenable;

	/**
	 * List of int *
	 */
	List *reenabled_ones;
} b3_counter_t;

extern b3_counter_t *
b3_counter_new(int start, char reenable);

extern int
b3_counter_free(b3_counter_t *counter);

extern int
b3_counter_next(b3_counter_t *counter);

extern int
b3_counter_add(b3_counter_t *counter, int reenable);

extern char
b3_counter_is_reenable(b3_counter_t *counter);


#endif // COUNTER_H
