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
 * @author Richard Bäck <richard.baeck@mailbox.org>
 * @date 2020-08-04
 * @brief File contains the common test function implementation
 */

#include "test.h"

#include <stdlib.h>
#include <stdio.h>
#include <w32bindkeys/logger.h>

static wbk_logger_t logger = { "test" };

void
b3_test(void (*setup)(void), void (*teardown)(void),
		int (*test_fn)(void), char *test_name)
{
	int error;

	setup();

	error = test_fn();

	teardown();

	if (error) {
		fprintf(stdout, "Failed: %s\n", test_name);
		exit(error);
	}
}

void
b3_test_empty_setup(void)
{
}

void
b3_test_empty_teardown(void)
{
}

int
b3_test_check_void(void *act, void *exp, char *msg)
{
	int error;

	if (act == exp) {
		error = 0;
	} else {
		error = 1;
		wbk_logger_log(&logger, SEVERE, "exp(%p) != act(%p): %s\n", exp, act, msg);
	}

	return error;
}
