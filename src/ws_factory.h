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
 * @date 2020-02-17
 * @brief File contains the workspace factory class definition
 */

#include "counter.h"
#include "ws.h"

#ifndef B3_WS_FACTORY_H
#define B3_WS_FACTORY_H

typedef struct b3_ws_factory_s
{
	/**
	 * Array of b3_ws_t *
	 */
	Array *ws_arr;

	b3_counter_t *ws_counter;
} b3_ws_factory_t;

/**
 * @brief Creates a new workspace factory object
 * @return A new workspace factory object or NULL if allocation failed
 */
extern b3_ws_factory_t *
b3_ws_factory_new(void);

/**
 * @brief Frees a status bar manager
 * @return Non-0 if the freeing failed
 */
extern int
b3_ws_factory_free(b3_ws_factory_t *ws_factory);

/**
 * @param name If NULL then the next number by ws_counter will be used
 * @return Either an existing or a new workspace. Do not free it by yourself.
 *         Instead free an id by calling b3_ws_factory_remove.
 */
extern b3_ws_t *
b3_ws_factory_create(b3_ws_factory_t *ws_factory, const char *id);

#endif // B3_WS_FACTORY_H
