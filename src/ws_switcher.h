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
 * @brief File contains the workspace switcher interface definition
 *
 * The purpose of this class is to expose a function to switch workspaces to any
 * object. The actual object supplying the functionality is not managed by this
 * class/objects.
 */

#ifndef B3_WS_SWITCHER_H
#define B3_WS_SWITCHER_H

typedef struct b3_ws_switcher_s b3_ws_switcher_t;

struct b3_ws_switcher_s
{
    int (*ws_switcher_free)(b3_ws_switcher_t *ws_switcher);
    int (*ws_switcher_switch_to_ws)(b3_ws_switcher_t *ws_switcher, const char *ws_id);
};

extern b3_ws_switcher_t *
b3_ws_switcher_new(void);

extern int
b3_ws_switcher_free(b3_ws_switcher_t *ws_switcher);

/**
 * @return Non-0 if switching to the workspace failed (e.g. the monitor was not found).
 */
extern int
b3_ws_switcher_switch_to_ws(b3_ws_switcher_t *ws_switcher, const char *ws_id);

#endif // B3_WS_SWITCHER_H
