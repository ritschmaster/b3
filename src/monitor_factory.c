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

#include "monitor_factory.h"

#include <stdlib.h>

b3_monitor_factory_t *
b3_monitor_factory_new(b3_wsman_factory_t *wsman_factory)
{
	b3_monitor_factory_t *monitor_factory;

	monitor_factory = malloc(sizeof(b3_monitor_factory_t));

	monitor_factory->wsman_factory = wsman_factory;

	return monitor_factory;
}

int
b3_monitor_factory_free(b3_monitor_factory_t *monitor_factory)
{
	monitor_factory->wsman_factory = NULL;

	free(monitor_factory);

	return 0;
}

b3_monitor_t *
b3_monitor_factory_create(b3_monitor_factory_t *monitor_factory, const char *monitor_name, RECT monitor_area)
{
	b3_monitor_t *monitor;

	monitor = b3_monitor_new(monitor_name,
							 monitor_area,
							 monitor_factory->wsman_factory);

	return monitor;
}
