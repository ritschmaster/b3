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
 * @date 2020-04-13
 * @brief File contains the window factory class definition
 */

#include <collectc/array.h>
#include <windows.h>

#include "win.h"

#ifndef B3_WIN_FACTORY_H
#define B3_WIN_FACTORY_H

typedef struct b3_win_factory_s b3_win_factory_t;

struct b3_win_factory_s
{
	int (* b3_win_factory_free)(b3_win_factory_t *win_factory);
	b3_win_t *(* b3_win_factory_win_create)(b3_win_factory_t *win_factory, HWND window_handler);
	int (* b3_win_factory_win_free)(b3_win_factory_t *win_factory, b3_win_t *win);

	HANDLE global_mutex;

	/**
	 *  Array of b3_win_t *
	 */
	Array *win_arr;
};

/**
 * @brief Creates a new window factory
 * @return A new window factory or NULL if allocation failed
 */
extern b3_win_factory_t *
b3_win_factory_new(void);

/**
 * @brief Deletes a window factory
 * @return Non-0 if the deletion failed
 */
extern int
b3_win_factory_free(b3_win_factory_t *win_factory);

/**
 * @return A new window. Free it by yourself by using b3_win_factory_free()!
 */
extern b3_win_t *
b3_win_factory_win_create(b3_win_factory_t *win_factory, HWND window_handler);

/**
 * @return 0 if the window is managed and freed. Non-0 otherwise.
 */
extern int
b3_win_factory_win_free(b3_win_factory_t *win_factory, b3_win_t *win);

#endif // B3_WIN_FACTORY_H
