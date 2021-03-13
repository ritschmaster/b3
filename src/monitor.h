/******************************************************************************
  This file is part of b3.

  Copyright 2020-2021 Richard Paul Baeck <richard.baeck@mailbox.org>

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
 * @date 2020-02-16
 * @brief File contains the monitor definition
 */

#include <collectc/array.h>
#include <windows.h>

#include "bar.h"
#include "wsman_factory.h"
#include "wsman.h"
#include "ws_switcher.h"

#ifndef B3_MONITOR_H
#define B3_MONITOR_H

typedef struct b3_monitor_s
{
	b3_wsman_factory_t *wsman_factory;

	char *monitor_name;

	RECT monitor_area;

	b3_wsman_t *wsman;

	b3_bar_t *bar;
} b3_monitor_t;

/**
 * @brief Creates a new monitor object
 * @param monitor_name A string object. It will be copied.
 * @param monitor_area The rectangle of the work area
 * @param wsman_factory A workspace manager factory object. It will not be freed by the monitor.
 * @param ws_switcher
 * @return A new monitor object or NULL if allocation failed
 */
extern b3_monitor_t *
b3_monitor_new(const char *monitor_name,
			   RECT monitor_area,
			   b3_wsman_factory_t *wsman_factory,
			   b3_ws_switcher_t *ws_switcher);

/**
 * @brief Frees a monitor object
 * @return Non-0 if the freeing failed
 */
extern int
b3_monitor_free(b3_monitor_t *ws);

extern const char *
b3_monitor_get_monitor_name(b3_monitor_t *monitor);

extern RECT
b3_monitor_get_monitor_area(b3_monitor_t *monitor);

extern b3_bar_t *
b3_monitor_get_bar(b3_monitor_t *monitor);

/**
 * @return The workspace if found. NULL otherwise. Do not free the returned
 * workspace!
 */
extern b3_ws_t *
b3_monitor_contains_ws(b3_monitor_t *monitor, const char *ws_id);

extern int
b3_monitor_set_focused_ws(b3_monitor_t *monitor, const char *ws_id);

extern b3_ws_t *
b3_monitor_get_focused_ws(b3_monitor_t *monitor);

/**
 * @return The currently focused window of the monitor. If no window is focused,
 * then NULL is returned. Do not free it!
 */
extern b3_win_t *
b3_monitor_get_focused_win(b3_monitor_t *monitor);

/**
 * @param win The window to focus
 * @param ws_id The ID of the workspace to set the focused window for
 * @param 0 if the setting was successful. Non-0 otherwise (e.g. the passed window is not within the workspace).
 */
extern int
b3_monitor_set_focused_win(b3_monitor_t *monitor, const char *ws_id, const b3_win_t *win);


extern b3_wsman_t *
b3_monitor_get_wsman(b3_monitor_t *monitor);

/**
 * @param win The object will not be freed. Free it by yourself!
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_monitor_add_win(b3_monitor_t *monitor, b3_win_t *win);

/**
 * @param win The object will not be freed.
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_monitor_remove_win(b3_monitor_t *monitor, b3_win_t *win);

extern int
b3_monitor_arrange_wins(b3_monitor_t *monitor);

extern int
b3_monitor_remove_empty_ws(b3_monitor_t *monitor);

/**
 * @return The workspace the window is placed on or NULL if it cannot be found.
 * Do not free it!
 */
extern b3_ws_t *
b3_monitor_find_win(b3_monitor_t *monitor, const b3_win_t *win);

extern int
b3_monitor_show(b3_monitor_t *monitor);

extern int
b3_monitor_draw(b3_monitor_t *monitor, HWND window_handler);

#endif // B3_MONITOR_H
