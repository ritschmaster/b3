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
 * @date 2021-02-21
 * @brief File contains the director workspace switcher interface definition
 */

#include "ws_switcher.h"
#include "director.h"

#ifndef B3_DIRECTOR_WS_SWITCHER_H
#define B3_DIRECTOR_WS_SWITCHER_H

typedef struct b3_director_s b3_director_t;
typedef struct b3_director_ws_switcher_s b3_director_ws_switcher_t;

struct b3_director_ws_switcher_s
{
    b3_ws_switcher_t ws_switcher;

    b3_director_t *director;
};

/**
 * @param director The director supplying the switching functionality. It will
 * not be freed by freeing the director workspace switcher!
 */
extern b3_director_ws_switcher_t *
b3_director_ws_switcher_new(b3_director_t *director);

#endif // B3_DIRECTOR_WS_SWITCHER_H
