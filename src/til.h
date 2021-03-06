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
 * @brief File contains the window tiler definition
 */

#ifndef B3_TIL_H
#define B3_TIL_H

typedef enum b3_til_mode_e {
	DEFAULT = 1,
	STACKING,
	TABBED,
} b3_til_mode_t;

typedef struct b3_til_s
{
} b3_til_t;

/**
 * @brief Creates a new window tiler object object
 * @return A new window tiler object or NULL if allocation failed
 */
extern b3_til_t *
b3_til_new(void);

/**
 * @brief Deletes a window tiler object
 * @return Non-0 if the deletion failed
 */
extern int
b3_til_free(b3_til_t* b3_til);

#endif // B3_TIL_H
