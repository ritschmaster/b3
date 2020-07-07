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

#include "counter.h"

#include <stdlib.h>
#include <wbkbase/logger.h>

wbk_logger_t logger = { "counter" };

static int
arithmentic_cmp(void const *e1, void const *e2);

b3_counter_t *
b3_counter_new(int start, char reenable)
{
	b3_counter_t *counter;

	counter = malloc(sizeof(b3_counter_t));

	counter->counter = start;
	counter->reenable = reenable;
	list_new(&(counter->reenabled_ones));

	return counter;
}

int
b3_counter_free(b3_counter_t *counter)
{
	list_destroy_cb(counter->reenabled_ones, free);
	counter->reenabled_ones = NULL;

	free(counter);
	return 0;
}

int
b3_counter_next(b3_counter_t *counter)
{
	char fetch_next;
	int next;
	int *first;

	fetch_next = 0;
	if (b3_counter_is_reenable(counter)) {
		first = NULL;
		list_get_first(counter->reenabled_ones, (void *) &first);
		if (first) {
			next = *first;
			list_remove_first(counter->reenabled_ones, (void *) &first);
			free(first);
			first = NULL;
		} else {
			fetch_next = 1;
		}
	} else {
		fetch_next = 1;
	}

	if (fetch_next) {
		next = counter->counter;
		counter->counter++;
	}

	return next;
}

int
b3_counter_add(b3_counter_t *counter, int number)
{
	int error;
	int *reenabled;

	error = 1;
	if (b3_counter_is_reenable(counter)) {
		wbk_logger_log(&logger, DEBUG, "Re-enabling %d\n", number);

		reenabled = malloc(sizeof(int));
		*reenabled = number;
		list_add(counter->reenabled_ones, reenabled);
		list_sort_in_place(counter->reenabled_ones, arithmentic_cmp);
		error = 0;
	}

	return error;
}

int
b3_counter_disable(b3_counter_t *counter, int disable)
{
	int error;
	int i;
	int *reenabled;
	ListIter iter;

	error = 1;
	if (b3_counter_is_reenable(counter)) {
		wbk_logger_log(&logger, DEBUG, "Disabling until %d\n", disable);

		if (counter->counter <= disable) {
			for (i = counter->counter; i < disable; i++) {
				b3_counter_add(counter, i);
			}
			counter->counter = disable + 1;
		} else {
			list_iter_init(&iter, counter->reenabled_ones);
			while (list_iter_next(&iter, &reenabled) != CC_ITER_END) {
				if (*reenabled == disable) {
					list_iter_remove(&iter, NULL);
					free(reenabled);
				}
			}
		}
	}

	return error;
}

char
b3_counter_is_reenable(b3_counter_t *counter)
{
	return counter->reenable;
}

int
arithmentic_cmp(void const *e1, void const *e2)
{
    int i = *(*((int**) e1));
    int j = *(*((int**) e2));

    if (i < j)
        return -1;
    if (i == j)
        return 0;
    return 1;
}
