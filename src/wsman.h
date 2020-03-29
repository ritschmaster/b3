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
 * @date 2020-02-26
 * @brief File contains the window class definition
 */

#ifndef B3_WSMAN_H
#define B3_WSMAN_H

#include "ws_factory.h"

typedef struct b3_wsman_s
{
	b3_ws_factory_t *ws_factory;

	b3_ws_t *focused_ws;

	/**
	 * Array of b3_ws_t *
	 */
	Array *ws_arr;
} b3_wsman_t;

/**
 * @brief Creates a new workspace manager 
 * @param ws_factory A workspace factory. It will not be freed by freeing the
 * workspace manager!
 * @return A new workspace manager or NULL if allocation failed
 */
extern b3_wsman_t *
b3_wsman_new(b3_ws_factory_t *ws_factory);

/**
 * @brief Deletes a workspace manager 
 * @return Non-0 if the deletion failed
 */
extern int
b3_wsman_free(b3_wsman_t *wsman);

/**
 * @return The workspace if the workspace was added, do not free it! Otherwise NULL.
 */
extern b3_ws_t *
b3_wsman_add(b3_wsman_t *wsman, const char *ws_id);

/**
 * @return 0 if the workspace was removed. Otherwise non-0.
 */
extern int
b3_wsman_remove(b3_wsman_t *wsman, const char *ws_id);

/**
 * @return Non-0 if it does contain the workspace. 0 otherwise.
 */
extern int
b3_wsman_contains_ws(b3_wsman_t *wsman, const char *ws_id);

extern b3_ws_t *
b3_wsman_get_focused_ws(b3_wsman_t *wsman);

extern int
b3_wsman_set_focused_ws(b3_wsman_t *wsman, const char *ws_id);

/**
 * @param win The object will not be freed.
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_wsman_remove_win(b3_wsman_t *wsman, b3_win_t *win);

/**
 * @return The workspace array of the workspace manager. Do not free it or
 * any element of it!
 */
extern Array *
b3_wsman_get_ws_arr(b3_wsman_t *wsman);

#endif // B3_WSMAN_H
