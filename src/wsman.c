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

#include "wsman.h"

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wbkbase/logger.h>

#include "monitor.h"
#include "ws.h"

static wbk_logger_t logger = { "wsman" };

static int
b3_wsman_free_monitor_arr(b3_wsman_t *wsman);

static const b3_monitor_t *
b3_wsman_get_monitor_by_monitor_name(b3_wsman_t *wsman, const char *monitor_name);

static BOOL CALLBACK
b3_wsman_enum_monitors(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM data);

b3_wsman_t *
b3_wsman_new(void)
{
	b3_wsman_t *wsman;

	wsman = NULL;
	wsman = malloc(sizeof(b3_wsman_t));

	array_new(&(wsman->monitor_arr));

	wsman->ws_factory = b3_ws_factory_new();

	return wsman;
}

int
b3_wsman_free(b3_wsman_t *wsman)
{
	b3_wsman_free_monitor_arr(wsman);

	b3_ws_factory_free(wsman->ws_factory);
	wsman->ws_factory = NULL;

	free(wsman);
	return 0;
}

int
b3_wsman_free_monitor_arr(b3_wsman_t *wsman)
{
	array_destroy_cb(wsman->monitor_arr, free); // TODO b3_monitor_free
	wsman->monitor_arr = NULL;

	return 0;
}

int
b3_wsman_refresh(b3_wsman_t *wsman)
{
	b3_monitor_t *monitor;

	char found;

	EnumDisplayMonitors(NULL, NULL, b3_wsman_enum_monitors, wsman);

	return 0;
}

BOOL CALLBACK
b3_wsman_enum_monitors(HMONITOR wmonitor, HDC hdc, LPRECT rect, LPARAM data)
{
    b3_wsman_t *wsman;
    MONITORINFOEX monitor_info;

    wsman = (b3_wsman_t *) data;

    monitor_info.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(wmonitor, &monitor_info);

    wbk_logger_log(&logger, INFO, "Found monitor: %s - %dx%d\n",
    			   monitor_info.szDevice,
    			   monitor_info.rcMonitor.right,
				   monitor_info.rcMonitor.bottom);

   array_add(wsman->monitor_arr,
		     b3_monitor_new(monitor_info.szDevice,
			   	            monitor_info.rcMonitor,
							wsman->ws_factory));

   return TRUE;
}

const Array *
b3_wsman_get_monitor_arr(b3_wsman_t *wsman)
{
	return wsman->monitor_arr;
}

const b3_monitor_t *
b3_wsman_get_monitor_by_monitor_name(b3_wsman_t *wsman, const char *monitor_name)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;
	char found;


	monitor = NULL;
	found = 0;
	array_iter_init(&monitor_iter, wsman->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END
		   && !found) {
		if (strcmp(b3_monitor_get_monitor_name(monitor), monitor_name) == 0) {
			found = 1;
		}
	}

	return monitor;
}

int
b3_wsman_show(b3_wsman_t *wsman)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;

	array_iter_init(&monitor_iter, wsman->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END) {
		b3_monitor_show(monitor);
	}

	return 0;
}

int
b3_wsman_draw(b3_wsman_t *wsman, HWND window_handler)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;

	array_iter_init(&monitor_iter, wsman->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END) {
		b3_monitor_draw(monitor, window_handler);
	}

	return 0;
}
