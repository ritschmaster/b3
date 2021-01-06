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

#include <collectc/array.h>

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
 * Either creates a new workspace or re-uses a workspace currently not in use by
 * any other object if parameter id is NULL. If id is not NULL then it either
 * creates the workspace or returns the workspace (which might already be in use
 * in some other object).
 *
 * If the workspace is no longer used by any object, then call
 * b3_ws_factory_remove() for it.
 *
 * @param name The name of the workspace to be used. If NULL then the next free
 * number will be used.
 * @return Either an existing or a new workspace. Do not free it by yourself! If
 * no longer used, then call b3_ws_factory_remove() for that workspace id.
 */
extern b3_ws_t *
b3_ws_factory_create(b3_ws_factory_t *ws_factory, const char *id);

/**
 * Removes a workspace name from the used workspaces. This releases an
 * automatically generated id to be re-used by a later caller of
 * b3_ws_factory_create().
 *
 * @return 0 if the id was found and was released. Non-0 otherwise.
 *
 */
extern int
b3_ws_factory_remove(b3_ws_factory_t *ws_factory, const char *id);

#endif // B3_WS_FACTORY_H
