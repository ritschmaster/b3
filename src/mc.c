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
 * @brief File contains the mouse command class implementation and its private methods.
 */

#include "mc.h"

#include <stdlib.h>

static int
b3_mc_free_impl(b3_mc_t *mc);

static int
b3_mc_exec_impl(b3_mc_t *mc);

b3_mc_t *
b3_mc_new(void)
{
    b3_mc_t *mc;

    mc = malloc(sizeof(b3_mc_t));
    if (mc) {
        mc->b3_mc_free = b3_mc_free_impl;
    }

    return mc;
}

extern int
b3_mc_free(b3_mc_t *mc)
{
    return mc->b3_mc_free(mc);
}

int
b3_mc_exec(b3_mc_t *mc)
{
    return mc->b3_mc_exec(mc);
}

int
b3_mc_free_impl(b3_mc_t *mc)
{
    free(mc);

    return 0;

}

int
b3_mc_exec_impl(b3_mc_t *mc)
{
    // TODO
}
