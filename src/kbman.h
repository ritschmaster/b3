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
 * @author Richard Bäck
 * @date 2020-03-15
 * @brief File contains keyboard manager class definition
 */

#ifndef B3_KBMAN_H
#define B3_KBMAN_H

#include <wbkbase/ikbman.h>
#include <wbkbase/kc_sys.h>
#include <collectc/array.h>
#include <windows.h>
#include <pthread.h>

#include "kc_director.h"

typedef struct b3_kbman_s
{
	wbki_kbman_t *kbman;

	/**
	 * Array of b3_kc_director_t *
	 */
	Array *kc_director_arr;

	pthread_t thread;

	char stop;
} b3_kbman_t;

/**
 */
extern b3_kbman_t *
b3_kbman_new();

extern b3_kbman_t *
b3_kbman_free(b3_kbman_t *kbman);

/**
 * @brief Gets the key bindings of a key binding manager
 * @return Array of wbk_kc_sys *
 */
extern Array *
b3_kbman_get_kb(b3_kbman_t* kbman);

/**
 * @param kb The key binding to add. The added key binding will be freed by the key binding manager
 */
extern int
b3_kbman_add_kc_sys(b3_kbman_t *kbman, wbk_kc_sys_t *kc_sys);

/**
 * @param kb The key binding to add. The added key binding will be freed by the key binding manager
 */
extern int
b3_kbman_add_kc_director(b3_kbman_t *kbman, b3_kc_director_t *kc_director);

#endif // B3_KBMAN_H
