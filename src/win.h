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
 * @date 26 January 2020
 * @brief File contains the window class definition
 */

#ifndef B3_WIN_H
#define B3_WIN_H

#include <windows.h>

typedef enum b3_win_state_e
{
	NORMAL = 0,
	MAXIMIZED
} b3_win_state_t;

typedef struct b3_win_s
{
	b3_win_state_t state;
	HWND window_handler;
	char floating;
	RECT rect;
} b3_win_t;

/**
 * @brief Creates a new window object
 * @param window_handler Will not be freed by the window!
 * @param floating
 * @return A new window object or NULL if allocation failed
 */
extern b3_win_t *
b3_win_new(HWND window_handler, char floating);

extern b3_win_t *
b3_win_copy(const b3_win_t *win);

/**
 * @brief Deletes a window object
 * @return Non-0 if the deletion failed
 */
extern int
b3_win_free(b3_win_t *win);

extern const char *
b3_win_get_title(b3_win_t *win);

extern b3_win_state_t
b3_win_get_state(b3_win_t *win);

extern int
b3_win_set_state(b3_win_t *win, b3_win_state_t state);

/**
  * @brief Gets if the window is floating.
  * @return Non-0 if the window is floating. 0 if it is not floating.
  */
extern char
b3_win_get_floating(b3_win_t *win);

/**
  * @brief Sets if the window is floating.
  * @return Non-0 if the setting failed
  */
extern int
b3_win_set_floating(b3_win_t *win, char floating);

extern HWND
b3_win_get_window_handler(b3_win_t *win);

/**
 * @param topmost Either 1 or 0.
 */
extern int
b3_win_show(b3_win_t *win, char topmost);

extern int
b3_win_minimize(b3_win_t *win);

extern RECT
b3_win_get_rect(b3_win_t *win);

extern int
b3_win_set_rect(b3_win_t *win, RECT rect);

/**
 * @brief Compares two windows
 * @return 0 if the both are similar. Non-0 if they are not similar.
 */
extern int
b3_win_compare(const b3_win_t *win, const b3_win_t *other);

#endif // B3_WIN_H
