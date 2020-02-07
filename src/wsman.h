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
 * @date 26 January 2020
 * @brief File contains the window class definition
 */

#ifndef WSMAN_H
#define WSMAN_H

#include <collectc/array.h>

typedef struct b3_wsman_s
{
	/**
	 * Array of b3_ws_t *
	 */
	Array *ws;
} b3_wsman_t;

/**
 * @brief Creates a new workspace manager 
 * @return A new workspace manager or NULL if allocation failed
 */
extern b3_wsman_t *
b3_wsman_new(void);

/**
 * @brief Deletes a workspace manager 
 * @return Non-0 if the deletion failed
 */
extern int
b3_wsman_free(b3_wsman_t *wsman);

/**
  * @brief Gets the workspaces of workspace manager
  * @return The workspaces of the workspace manager . Do not free that memory.
  */
extern Array *
b3_wsman_get_ws(b3_wsman_t *wsman);

#endif // WSMAN_H
