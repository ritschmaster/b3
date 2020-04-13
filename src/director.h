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
 * @brief File contains the director class definition
 */

#ifndef B3_DIRECTOR_H
#define B3_DIRECTOR_H

#include <collectc/array.h>
#include <windows.h>

#include "monitor_factory.h"
#include "win.h"

typedef struct b3_director_s
{
	b3_monitor_t *focused_monitor;

	/**
	 * Array of b3_monitor_t *
	 */
	Array *monitor_arr;

	b3_monitor_factory_t *monitor_factory;

	b3_win_t *active_win;
} b3_director_t;

/**
 * @brief Creates a new director
 * @param monitor_factory A monitor factory. It will not be freed by freeing
 * the director!
 * @return A new director or NULL if allocation failed
 */
extern b3_director_t *
b3_director_new(b3_monitor_factory_t *monitor_factory);

/**
 * @brief Deletes a director
 * @return Non-0 if the deletion failed
 */
extern int
b3_director_free(b3_director_t *director);

/**
 * @brief Refresh the currently available monitors
 */
extern int
b3_director_refresh(b3_director_t *director);

/**
  * @brief Gets the monitors of the director
  * @return The monitors of the director, as array of b3_monitor_t *.
  */
extern Array *
b3_director_get_monitor_arr(b3_director_t *director);

extern int
b3_director_set_focused_monitor(b3_director_t *director, const char *monitor_name);

/**
 * @return Non-0 if the monitor was not found.
 */
extern int
b3_director_switch_to_ws(b3_director_t *director, const char *ws_id);

/**
 * @param win The object will not be freed. Free it by yourself!
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_director_add_win(b3_director_t *director, const char *monitor_name, b3_win_t *win);

/**
 * @param win The object will not be freed. Free it by yourself!
 * @return 0 if removed. Non-0 otherwise.
 */
extern int
b3_director_remove_win(b3_director_t *director, b3_win_t *win);

extern int
b3_director_arrange_wins(b3_director_t *director);

/**
 * @param win The object will not be freed. Free it by yourself!
 * @return 0 if it was set. Non-0 otherwise.
 */
extern int
b3_director_set_active_win(b3_director_t *director, b3_win_t *win);

/**
 * @return 0 if it was possible to toggle the window. Non-0 otherwise (e.g. when it is not managed).
 */
extern int
b3_director_active_win_toggle_floating(b3_director_t *director);

/**
 * @return 0 if it was moved. Non-0 otherwise.
 */
extern int
b3_director_move_active_win_to_ws(b3_director_t *director, const char *ws_id);

/**
 * @brief Show the director
 */
extern int
b3_director_show(b3_director_t *director);

/**
 * @brief Draw all components of the director
 */
extern int
b3_director_draw(b3_director_t *director, HWND window_handler);

#endif // B3_DIRECTOR_H
