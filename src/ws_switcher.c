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
 * @brief File contains the workspace switcher interface implementation and private methods
 */

#include "ws_switcher.h"

#include <stdlib.h>
#include <w32bindkeys/logger.h>

static wbk_logger_t logger = { "ws_switcher" };

static int
b3_ws_switcher_free_impl(b3_ws_switcher_t *ws_switcher);

static int
b3_ws_switcher_switch_to_ws_impl(b3_ws_switcher_t *ws_switcher, const char *ws_id);

b3_ws_switcher_t *
b3_ws_switcher_new(void)
{
    b3_ws_switcher_t *ws_switcher;

    ws_switcher = malloc(sizeof(b3_ws_switcher_t));

    if (ws_switcher) {
        ws_switcher->ws_switcher_free = b3_ws_switcher_free_impl;
        ws_switcher->ws_switcher_switch_to_ws = b3_ws_switcher_switch_to_ws_impl;
    }

    return ws_switcher;
}

int
b3_ws_switcher_free(b3_ws_switcher_t *ws_switcher)
{
   return ws_switcher->ws_switcher_free(ws_switcher);
}

int
b3_ws_switcher_switch_to_ws(b3_ws_switcher_t *ws_switcher, const char *ws_id)
{
    return ws_switcher->ws_switcher_switch_to_ws(ws_switcher, ws_id);
}

int
b3_ws_switcher_free_impl(b3_ws_switcher_t *ws_switcher)
{
    free(ws_switcher);

    return 0;
}

int
b3_ws_switcher_switch_to_ws_impl(b3_ws_switcher_t *ws_switcher, const char *ws_id)
{
    wbk_logger_log(&logger, SEVERE, "Not implemented\n");

    return 1;
}
