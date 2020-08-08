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

#include <wbkbase/kbman.h>
#include <wbkbase/kc_sys.h>
#include <windows.h>

#include "kc_director.h"

typedef struct b3_kbman_s
{
	wbk_kbman_t *kbman;

	int kc_director_arr_len;

	b3_kc_director_t **kc_director_arr;

	char stop;
} b3_kbman_t;

/**
 */
extern b3_kbman_t *
b3_kbman_new();

extern b3_kbman_t *
b3_kbman_free(b3_kbman_t *kbman);

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

/**
 * Create an array of nominator new key board managers and divide the internal
 * key commands by nominator over those new key board managers. The key commands
 * are copied during this processes. The returned array and the returned key
 * board managers need to be freed by yourself!
 */
extern b3_kbman_t **
b3_kbman_split(b3_kbman_t *kbman, int nominator);

extern int
b3_kbman_exec(b3_kbman_t *kbman, wbk_b_t *b);

#endif // B3_KBMAN_H
