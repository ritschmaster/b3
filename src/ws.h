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
 * @date 2020-02-16
 * @brief File contains the workspace definition
 */

#ifndef B3_WS_H
#define B3_WS_H

#include <collectc/array.h>
#include <windows.h>

#include "til.h"
#include "counter.h"
#include "winman.h"
#include "win.h"

typedef enum b3_ws_move_direction_s
{
	UP,
	DOWN,
	LEFT,
	RIGHT
} b3_ws_move_direction_t;

typedef struct b3_ws_s b3_ws_t;

struct b3_ws_s {
	int (*b3_ws_free)(b3_ws_t *ws);
	int (*b3_ws_set_name)(b3_ws_t *ws, const char *name);
	const char* (*b3_ws_get_name)(b3_ws_t *ws);
	b3_win_t * (*b3_ws_get_focused_win)(b3_ws_t *ws);
	int (*b3_ws_set_focused_win)(b3_ws_t *ws, const b3_win_t *win);
	int (*b3_ws_add_win)(b3_ws_t *ws, b3_win_t *win);
	int (*b3_ws_remove_win)(b3_ws_t *ws, b3_win_t *win);
	int (*b3_ws_split)(b3_ws_t *ws, b3_winman_mode_t mode);
	int (*b3_ws_move_focused_win)(b3_ws_t *ws, b3_ws_move_direction_t direction);
	int (*b3_ws_toggle_floating_win)(b3_ws_t *ws, b3_win_t *win);
	int (*b3_ws_minimize_wins)(b3_ws_t *ws);
	b3_win_t *(*b3_ws_contains_win)(b3_ws_t *ws, const b3_win_t *win);
	int (*b3_ws_is_empty)(b3_ws_t *ws);
	b3_win_t * (*b3_ws_get_win_rel_to_focused_win)(b3_ws_t *ws,
												   b3_ws_move_direction_t direction,
												   char rolling);
	int (*b3_ws_arrange_wins)(b3_ws_t *ws, RECT monitor_area);

	b3_winman_t *winman;

	b3_til_mode_t mode;

	char *name;

	/**
	 * The currently focused window of the workspace.
	 */
	b3_win_t *focused_win;

	/**
	 * Array of b3_win_t *
	 *
	 * Stack containing the previously focused windows. If a window is removed,
	 * then it is also removed from the previously focused windows.
	 *
	 * It also only contains windows that are actually managed by one of the
	 * following members:
	 * - floating_win_arr
	 * - winman
	 */
	Array *previously_focused_win_arr;

	/**
	 * Array of b3_win_t *
	 *
	 * Array containing the floating windows.
	 */
	Array *floating_win_arr;
};

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

extern int
b3_ws_set_name(b3_ws_t *ws, const char *name);

extern const char*
b3_ws_get_name(b3_ws_t *ws);

/**
 * Returns the currently focused window. No window can only be focused if the
 * workspace generall contains no windows.
 *
 * @return The currently focused window of the monitor. If no window is focused,
 * then NULL is returned. Do not free it!
 */
extern b3_win_t *
b3_ws_get_focused_win(b3_ws_t *ws);

/**
 * @param win The window to focus
 * @param 0 if the setting was successful. Non-0 otherwise (e.g. the passed window is not within the workspace).
 */
extern int
b3_ws_set_focused_win(b3_ws_t *ws, const b3_win_t *win);

/**
 * Place a new window in the workspace.
 *
 * @param win Will be stored within the workspace. The object will not be freed
 * by the workspace.
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_ws_add_win(b3_ws_t *ws, b3_win_t *win);

/**
 * Removes a window from the workspace. If it was the currently focused window,
 * then the currently focused window will point to the next found one according
 * to the window manager.
 *
 * @param win The window must already be within the workspace. The object will
 * not be freed.
 *
 * @return 0 if removed. Non-0 otherwise.
 */
extern int
b3_ws_remove_win(b3_ws_t *ws, b3_win_t *win);

/**
 * Add a split (= new window manager of parameter mode) above the window manager
 * of the focused window.
 *
 * @param mode The mode of the split (new window manager).
 * @return 0 if the vertical split was successful. Non-0 otherwise.
 */
extern int
b3_ws_split(b3_ws_t *ws, b3_winman_mode_t mode);

/**
 * Moves the currently focused window of the workspace in the parameter
 * direction.
 *
 * @return 0 if the moving was successful. Non-0 otherwise:
 * - 1 - Critical error indicating that the workspace is corrupted
 * - 2 - End of workspace in the selected direction was reached
 */
extern int
b3_ws_move_focused_win(b3_ws_t *ws, b3_ws_move_direction_t direction);

/**
 * Toggles the state floating state of the currently focused window of the
 * workspace.
 *
 * @return 0 if it was possible to toggle the window. Non-0 otherwise (e.g. when
 * it is not managed by this workspace).
 */
extern int
b3_ws_toggle_floating_win(b3_ws_t *ws, b3_win_t *win);

/**
 * Minimizes all windows on the workspace
 */
extern int
b3_ws_minimize_wins(b3_ws_t *ws);

/**
 * Searches for a window stored within the workspace.
 *
 * @return The window instance stored in the workspace - if found. NULL
 * otherwise. Do not free the returned window!
 */
extern b3_win_t *
b3_ws_contains_win(b3_ws_t *ws, const b3_win_t *win);

/**
 * @return 1 if the workspace contains no window.
 */
extern int
b3_ws_is_empty(b3_ws_t *ws);

/**
 * Returns the window in direction next to the focused window.
 *
 * @param rolling If non-0, then the window starting from the other end of the
 * window manager is used
 * @return The window in the direction next to the focused window. If not found
 * then NULL. Do not free it!
 */
extern b3_win_t *
b3_ws_get_win_rel_to_focused_win(b3_ws_t *ws,
								 b3_ws_move_direction_t direction,
								 char rolling);

/**
 * Arrange the windows on the workspace to the given area.
 *
 * @param ws The workspace to arrange the windows on.
 * @param monitor_area The available space on which the windows should be
 * arranged on.
 */
extern int
b3_ws_arrange_wins(b3_ws_t *ws, RECT monitor_area);

#endif // B3_WS_H
