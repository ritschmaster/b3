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
 * @date 2020-03-20
 * @brief File contains the window watcher class definition
 */

#ifndef B3_WIN_WATCHER_H
#define B3_WIN_WATCHER_H

#include <windows.h>

#include "director.h"

#define B3_WIN_WATCHER_BUFFER_LENGTH 1024

typedef struct b3_win_watcher_s
{
	b3_director_t *director;

	HWND window_handler;

	UINT shellhookid;
} b3_win_watcher_t;

/**
 * @param director The object will not be freed. Free it by yourself!
 */
extern b3_win_watcher_t *
b3_win_watcher_new(b3_director_t *director);

extern b3_win_watcher_t *
b3_win_watcher_free(b3_win_watcher_t *win_watcher);

/**
 * @brief Main loop, runs forever, but in a thread.
 */
extern int
b3_win_watcher_start(b3_win_watcher_t *win_watcher);

/**
 * @brief Stops the main loop.
 */
extern int
b3_win_watcher_stop(b3_win_watcher_t *win_watcher);


/**
 * @return 0 if the window is not managable. Non-0 if it is managable.
 */
extern int
b3_win_watcher_managable_window_handler(HWND window_handler);

#endif // B3_WIN_WATCHER_H
