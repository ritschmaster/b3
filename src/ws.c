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

#include "ws.h"

#include <stdlib.h>

b3_ws_t *
b3_ws_new(void)
{
	b3_ws_t *b3_ws;

	b3_ws = NULL;
	b3_ws = malloc(sizeof(b3_ws_t));

	return b3_ws;
}

int
b3_ws_free(b3_ws_t *ws)
{
	free(ws);
	return 0;
}

Array *
b3_get_wins(b3_ws_t *ws)
{
	return ws->wins;
}

b3_til_mode_t
b3_get_mode(b3_ws_t *ws)
{
	return ws->mode;
}

int
b3_set_mode(b3_ws_t *ws, b3_til_mode_t mode)
{
	ws->mode = mode;
	return 0;
}

