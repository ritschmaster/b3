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

#include "kbman.h"

#include <wbkbase/logger.h>
#include <collectc/array.h>
#include <windows.h>

static wbk_logger_t logger =  { "kbman" };

static int
b3_kbman_exec_kc_director(b3_kbman_t *kbman, wbk_b_t *b);

static void *
b3_kbman_main_thread_wrapper(void *arg);

b3_kbman_t *
b3_kbman_new()
{
	b3_kbman_t *kbman;

	kbman = NULL;
	kbman = malloc(sizeof(b3_kbman_t));
	memset(kbman, 0, sizeof(b3_kbman_t));

	kbman->kbman = wbk_kbman_new();

	array_new(&(kbman->kc_director_arr));

	return kbman;
}

b3_kbman_t *
b3_kbman_free(b3_kbman_t *kbman)
{
	ArrayIter kb_iter;
	b3_kc_director_t *kc_director;

	//b3_kbman_main_stop(kbman);

	wbk_kbman_free(kbman->kbman);
	kbman->kbman= NULL;

	array_iter_init(&kb_iter, wbk_kbman_get_kb(kbman->kbman));
	while (array_iter_next(&kb_iter, (void *) &kc_director) != CC_ITER_END) {
		array_iter_remove(&kb_iter, NULL);
		b3_kc_director_free(kc_director);
	}
	array_destroy_cb(kbman->kc_director_arr, free);
	kbman->kc_director_arr = NULL;


	free(kbman);
}

Array *
b3_kbman_get_kb(b3_kbman_t* kbman)
{
	return kbman->kc_director_arr;
}

int
b3_kbman_add_kc_sys(b3_kbman_t *kbman, wbk_kc_sys_t *kc_sys)
{
	return wbk_kbman_add(kbman->kbman, kc_sys);
}

int
b3_kbman_add_kc_director(b3_kbman_t *kbman, b3_kc_director_t *kc_director)
{
	array_add(kbman->kc_director_arr, kc_director);
	return 0;
}

int
b3_kbman_exec(b3_kbman_t *kbman, wbk_b_t *b)
{
	int ret;

	ret = -1;

	if (ret) {
		ret = wbk_kbman_exec(kbman->kbman, b);
	}

	if (ret) {
		ret = b3_kbman_exec_kc_director(kbman, b);
	}

	return ret;
}

int
b3_kbman_exec_kc_director(b3_kbman_t *kbman, wbk_b_t *b)
{
	int ret;
	char found;
	ArrayIter kb_iter;
	b3_kc_director_t *kc_director;

	ret = -1;

	found = 0;
	array_iter_init(&kb_iter, b3_kbman_get_kb(kbman));
	while (!found && array_iter_next(&kb_iter, (void *) &kc_director) != CC_ITER_END) {
		if (wbk_b_compare(b3_kc_director_get_binding(kc_director), b) == 0) {
			found = 1;
		}
	}

	if (found) {
		b3_kc_director_exec(kc_director);
		ret = 0;
	}

	return ret;
}
