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
#include <sys/types.h>
#include <unistd.h>
#include <windows.h>

static wbk_logger_t logger =  { "kbman" };

static int
b3_kbman_exec_kc_director(b3_kbman_t *kbman, wbk_b_t *b);

DWORD WINAPI
b3_kbman_kc_director_exec_threaded(LPVOID param);

b3_kbman_t *
b3_kbman_new()
{
	b3_kbman_t *kbman;

	kbman = NULL;
	kbman = malloc(sizeof(b3_kbman_t));
	memset(kbman, 0, sizeof(b3_kbman_t));

	kbman->kbman = wbk_kbman_new();

	kbman->kc_director_arr_len = 0;
	kbman->kc_director_arr = NULL;

	return kbman;
}

b3_kbman_t *
b3_kbman_free(b3_kbman_t *kbman)
{
	int i;

	wbk_kbman_free(kbman->kbman);
	kbman->kbman= NULL;

	for (i = 0; i < kbman->kc_director_arr_len; i++) {
		b3_kc_director_free(kbman->kc_director_arr[i]);
		kbman->kc_director_arr[i] = NULL;
	}
	free(kbman->kc_director_arr);
	kbman->kc_director_arr = NULL;

	free(kbman);
}

int
b3_kbman_add_kc_sys(b3_kbman_t *kbman, wbk_kc_sys_t *kc_sys)
{
	return wbk_kbman_add(kbman->kbman, kc_sys);
}

int
b3_kbman_add_kc_director(b3_kbman_t *kbman, b3_kc_director_t *kc_director)
{
	kbman->kc_director_arr_len++;
	kbman->kc_director_arr = realloc(kbman->kc_director_arr,
									 sizeof(b3_kc_director_t **) * kbman->kc_director_arr_len);
	kbman->kc_director_arr[kbman->kc_director_arr_len - 1] = kc_director;
	return 0;
}

b3_kbman_t **
b3_kbman_split(b3_kbman_t *kbman, int nominator)
{
	b3_kbman_t **kbmans;
	int i;
	int j;

	kbmans = malloc(sizeof(b3_kbman_t **) * nominator);

	for (i = 0; i < nominator; i++) {
		kbmans[i] = b3_kbman_new();
		for (j = 0; j < kbman->kc_director_arr_len; j++) {
			if (j % nominator == i) {
				b3_kbman_add_kc_director(kbmans[i], b3_kc_director_clone(kbman->kc_director_arr[j]));
			}
		}

		for (j = 0; j < kbman->kbman->kc_sys_arr_len; j++) {
			if (j % nominator == i) {
				b3_kbman_add_kc_sys(kbmans[i], wbk_kc_sys_clone(kbman->kbman->kc_sys_arr[j]));
			}
		}
	}

	return kbmans;
}

int
b3_kbman_exec(b3_kbman_t *kbman, wbk_b_t *b)
{
	return b3_kbman_exec_kc_director(kbman, b);
}

int
b3_kbman_exec_kc_director(b3_kbman_t *kbman, wbk_b_t *b)
{
	int error;
	int i;
	int found_at;
	HANDLE thread_handler;

	error = -1;

	found_at = -1;
	for (i = 0; found_at < 0 && i < kbman->kc_director_arr_len; i++) {
		if (wbk_b_compare(b3_kc_director_get_binding(kbman->kc_director_arr[i]), b) == 0) {
			found_at = i;
		}
	}

	if (found_at >= 0) {
		thread_handler = CreateThread(NULL,
						 0,
						 b3_kbman_kc_director_exec_threaded,
						 kbman->kc_director_arr[found_at],
						 0,
						 NULL);
		error = 0;
	}

	return error;
}

DWORD WINAPI
b3_kbman_kc_director_exec_threaded(LPVOID param)
{
	b3_kc_director_t *kc_director;

	kc_director = (b3_kc_director_t *) param;

	wbk_logger_log(&logger, DEBUG, "Executing key binding\n");

	return b3_kc_director_exec(kc_director);
}
