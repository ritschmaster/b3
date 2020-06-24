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
 * @date 27 January 2020
 * @brief File contains the status bar class definition
 */

#include <windows.h>

#include "wsman.h"

#ifndef B3_BAR_H
#define B3_BAR_H

#define B3_BAR_WIN_NAME "b3 bar"
#define B3_BAR_WIN_NAME_LEN strlen(B3_BAR_WIN_NAME)

#define B3_BAR_DEFAULT_POS TOP
#define B3_BAR_DEFAULT_BAR_HEIGHT 20
#define B3_BAR_DEFAULT_PADDING_TO_WINDOW 1
#define B3_BAR_DEFAULT_PADDING_TO_FRAME 4
#define B3_BAR_DEFAULT_PADDING_TO_NEXT_FRAME 4

typedef enum b3_bar_pos_e
{
	TOP = 0,
	BOTTOM
} b3_bar_pos_t;

typedef struct b3_bar_s
{
	b3_bar_pos_t position;

	RECT area;

	b3_wsman_t *wsman;

	HWND window_handler;

	char focused;
} b3_bar_t;

/**
 * @brief Creates a new status bar
 * @param monitor_area The area used by the monitor the bar is painted on.
 * @param wsman A workspace manager object. It will not be freed by the bar.
 * @return A new status bar or NULL if allocation failed
 */
extern b3_bar_t *
b3_bar_new(const char *monitor_name, RECT monitor_area, b3_wsman_t *wsman);

/**
 * @brief Frees a status bar
 * @return Non-0 if the freeing failed
 */
extern int
b3_bar_free(b3_bar_t *bar);

extern RECT
b3_bar_get_area(b3_bar_t *bar);

extern b3_bar_pos_t
b3_bar_get_position(b3_bar_t *bar);

extern char
b3_bar_is_focused(b3_bar_t *bar);

extern int
b3_bar_show(b3_bar_t *bar);

extern int
b3_bar_hide(b3_bar_t *bar);

extern int
b3_bar_set_focused(b3_bar_t *bar, char focused);

#endif // B3_BAR_H
