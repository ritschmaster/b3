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
 * @brief File contains the workspace definition
 */

#include <collectc/array.h>

#include "til.h"
#include "counter.h"

#ifndef WS_H
#define WS_H

typedef struct b3_ws_s
{
	/**
	 * Array of b3_win_t *
	 */
	Array *wins;

	b3_til_mode_t mode;

	char *name;
} b3_ws_t;

/**
 * @brief Creates a new workspace object
 * @param name
 * @return A new workspace object or NULL if allocation failed
 */
extern b3_ws_t *
b3_ws_new(const char *name);

/**
 * @brief Frees a workspace object
 * @return Non-0 if the freeing failed
 */
extern int
b3_ws_free(b3_ws_t *ws);

/**
  * @brief Get the windows of the workspace
  * @return A pointer to the windows. Do not free that memory.
  */
extern Array *
b3_ws_get_wins(b3_ws_t *ws);

/**
  * @brief Get the tiling mode of the workspace
  * @return The tiling mode of the workspace
  */
extern b3_til_mode_t
b3_ws_get_mode(b3_ws_t *ws);

/**
  * @brief Set the tiling mode
  * @return Non-0 if the setting failed
  */
extern int
b3_ws_set_mode(b3_ws_t *ws, b3_til_mode_t mode);

extern int
b3_ws_set_name(b3_ws_t *ws, const char *name);

extern const char*
b3_ws_get_name(b3_ws_t *ws);

#endif // WS_H
