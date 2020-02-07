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
 * @brief File contains the bar manager class definition
 */

#ifndef BARMAN_H
#define BARMAN_H

#include "bar.h"
#include "ws.h"

typedef struct b3_barman_s
{
	b3_bar_t *bar;
} b3_barman_t;

/**
 * @brief Creates a new status bar manager 
 * @return A new status bar manager or NULL if allocation failed
 */
extern b3_barman_t *
b3_barman_new(void);

/**
 * @brief Frees a status bar manager
 * @return Non-0 if the freeing failed
 */
extern int
b3_barman_free(b3_barman_t* barman);

/**
 * @brief Draws a status bar manager on a workspace
 * @return Non-0 if the freeing failed
 */
extern int
b3_barman_draw_on(b3_barman_t* barman, b3_ws_t *ws);

#endif // BARMAN_H
