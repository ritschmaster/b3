/******************************************************************************
  This file is part of b3.

  Copyright 2021 Richard Paul Baeck <richard.baeck@mailbox.org>

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
 * @date 2021-09-14
 * @brief File contains the mouse manager class definition.
 */

#include "mc.h"

#ifndef B3_MOUSEMAN_H
#define B3_MOUSEMAN_H

typedef struct b3_mouseman_s b3_mouseman_t;

struct b3_mouseman_s
{
   int (*b3_mouseman_free)(b3_mouseman_t *mouseman);
   int (*b3_mouseman_add)(b3_mouseman_t *mouseman, b3_mc_t *mc);
   int (*b3_mouseman_exec)(b3_mouseman_t *mouseman);

   int mc_arr_len;
   b3_mc_t **mc_arr;
};

extern b3_mouseman_t *
b3_mouseman_new(void);

extern int
b3_mouseman_free(b3_mouseman_t *mouseman);

/**
 * @param mc The mouse command to add. The added mouse command will be freed by the mouse manager.
 */
extern int
b3_mouseman_add(b3_mouseman_t *mouseman, b3_mc_t *mc);

extern int
b3_mouseman_exec(b3_mouseman_t *mouseman);

#endif // B3_MOUSEMAN_H
