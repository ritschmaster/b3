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
 * @date 2021-09-15
 * @brief File contains the mouse daemon class definition.
 */

#include <windows.h>

#ifndef B3_MOUSEDAEMON_H
#define B3_MOUSEDAEMON_H

typedef struct b3_mousedaemon_s b3_mousedaemon_t;

struct b3_mousedaemon_s
{
	int (*b3_mousedaemon_free)(b3_mousedaemon_t *mousedaemon);

    HANDLE global_mutex;
};

extern b3_mousedaemon_t *
b3_mousedaemon_new(void);

extern int
b3_mousedaemon_free(b3_mousedaemon_t *mousedaemon);

#endif // B3_MOUSEDAEMON_H
