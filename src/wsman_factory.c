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

#include "wsman_factory.h"

#include <stdlib.h>

b3_wsman_factory_t *
b3_wsman_factory_new(b3_ws_factory_t *ws_factory)
{
	b3_wsman_factory_t *wsman_factory;

	wsman_factory = malloc(sizeof(b3_wsman_factory_t));
	if (wsman_factory) {
		wsman_factory->ws_factory = ws_factory;
	}

	return wsman_factory;
}

int
b3_wsman_factory_free(b3_wsman_factory_t *wsman_factory)
{
	wsman_factory->ws_factory = NULL;

	free(wsman_factory);

	return 0;
}

b3_wsman_t *
b3_wsman_factory_create(b3_wsman_factory_t *wsman_factory)
{
	b3_wsman_t *wsman;

	wsman = b3_wsman_new(wsman_factory->ws_factory);

	return wsman;
}
