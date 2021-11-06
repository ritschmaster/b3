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
 * @brief File contains the mouse manager class implementation and its private methods.
 */

#include "mouseman.h"

#include <stdlib.h>
#include <w32bindkeys/logger.h>

static wbk_logger_t logger = { "mouseman" };

static int
b3_mouseman_free_impl(b3_mouseman_t *mouseman);

static int
b3_mouseman_add_impl(b3_mouseman_t *mouseman, b3_mc_t *mc);

static int
b3_mouseman_exec_impl(b3_mouseman_t *mouseman);

b3_mouseman_t *
b3_mouseman_new(void)
{
    b3_mouseman_t *mouseman;

    mouseman = malloc(sizeof(b3_mouseman_t));
    if (mouseman) {
        mouseman->b3_mouseman_free = b3_mouseman_free_impl;
        mouseman->b3_mouseman_add = b3_mouseman_add_impl;

        mouseman->mc_arr_len = 0;
        mouseman->mc_arr = NULL;
    }

    return mouseman;
}

int
b3_mouseman_free(b3_mouseman_t *mouseman)
{
    return mouseman->b3_mouseman_free(mouseman);
}

int
b3_mouseman_add(b3_mouseman_t *mouseman, b3_mc_t *mc)
{
    return mouseman->b3_mouseman_add(mouseman, mc);
}

int
b3_mouseman_exec(b3_mouseman_t *mouseman)
{
    return mouseman->b3_mouseman_exec(mouseman);
}

int
b3_mouseman_free_impl(b3_mouseman_t *mouseman)
{
    free(mouseman);

    return 0;
}

int
b3_mouseman_add_impl(b3_mouseman_t *mouseman, b3_mc_t *mc)
{
    mouseman->mc_arr_len++;
	mouseman->mc_arr = realloc(mouseman->mc_arr,
                               sizeof(b3_mc_t **) * mouseman->mc_arr_len);
	mouseman->mc_arr[mouseman->mc_arr_len - 1] = mc;
    return 0;
}
/**
 * https://docs.microsoft.com/en-us/windows/win32/winmsg/using-hooks
 * https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644986(v=vs.85)
 * https://docs.microsoft.com/de-at/windows/win32/inputdev/wm-mousemove?redirectedfrom=MSDN
 */
int
b3_mouseman_exec_impl(b3_mouseman_t *mouseman)
{
    // TODO
}
