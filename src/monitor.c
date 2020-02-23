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

#include "monitor.h"

#include <stdlib.h>
#include <string.h>

b3_monitor_t *
b3_monitor_new(const char *monitor_name, RECT monitor_area, b3_ws_factory_t *ws_factory)
{
	b3_monitor_t *monitor;
	int length;

	monitor = NULL;
	monitor = malloc(sizeof(b3_monitor_t));

	length = strlen(monitor_name) + 1;
	monitor->monitor_name = malloc(length);
	strcpy(monitor->monitor_name, monitor_name);

	monitor->monitor_area = monitor_area;

	monitor->bar = b3_bar_new(monitor->monitor_name, monitor->monitor_area);

	monitor->ws_factory = ws_factory;

	array_new(&(monitor->wsid_arr));

	return monitor;
}

int
b3_monitor_free(b3_monitor_t *monitor)
{
	free(monitor->monitor_name);
	monitor->monitor_name = NULL;

	monitor->ws_factory = NULL;

	array_destroy_cb(monitor->wsid_arr, free);
	monitor->wsid_arr = NULL;

	free(monitor);
	return 0;
}

const char *
b3_monitor_get_monitor_name(b3_monitor_t *monitor)
{
	return monitor->monitor_name;
}

RECT
b3_ws_get_monitor_area(b3_monitor_t *monitor)
{
	return monitor->monitor_area;
}

const b3_bar_t *
b3_monitor_get_bar(b3_monitor_t *monitor)
{
	return monitor->bar;
}

int
b3_monitor_show(b3_monitor_t *monitor)
{
	b3_bar_create_window(monitor->bar,
						 b3_monitor_get_monitor_name(monitor));
	return 0;
}

int
b3_monitor_draw(b3_monitor_t *monitor, HWND window_handler)
{
}
