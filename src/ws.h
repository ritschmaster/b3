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

#ifndef B3_WS_H
#define B3_WS_H

#include <collectc/array.h>

#include "til.h"
#include "counter.h"
#include "win.h"

typedef struct b3_ws_s
{
	/**
	 * Array of b3_win_t *
	 */
	Array *win_arr;

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
b3_ws_get_win_arr(b3_ws_t *ws);

/**
 * @param win A window object. The object will be freed by the workspace!
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_ws_add_win(b3_ws_t *ws, b3_win_t *win);

/**
 * @param win The object will not be freed.
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_ws_remove_win(b3_ws_t *ws, const b3_win_t *win);

extern int
b3_ws_arrange_wins(b3_ws_t *ws, RECT monitor_area);

int
b3_ws_minimize_wins(b3_ws_t *ws);

/**
 * @return The window if it is within the workspace. NULL otherwise.
 */
extern b3_win_t *
b3_ws_contains_win(b3_ws_t *ws, const b3_win_t *win);

/**
 * @return 0 if it was possible to toggle the window. Non-0 otherwise (e.g. when it is not managed).
 */
extern int
b3_ws_active_win_toggle_floating(b3_ws_t *ws, const b3_win_t *win);

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

#endif // B3_WS_H
