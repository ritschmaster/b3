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
#include <w32bindkeys/logger.h>

static wbk_logger_t logger = { "monitor" };

/**
 * Belongs to b3_monitor_arrange_wins() and
 * b3_monitor_arrange_wins_ws_visitor(). Do not set it somewhere else!
 */
static b3_monitor_t *g_arrange_monitor;

static void
b3_monitor_arrange_wins_ws_visitor(b3_ws_t *ws);

void
b3_monitor_arrange_wins_ws_visitor(b3_ws_t *ws)
{
  if (strcmp(b3_ws_get_name(ws),
             b3_ws_get_name(b3_monitor_get_focused_ws(g_arrange_monitor)))) {
    b3_ws_minimize_wins(ws);
  }
}

b3_monitor_t *
b3_monitor_new(const char *monitor_name, RECT monitor_area, b3_wsman_factory_t *wsman_factory)
{
	b3_monitor_t *monitor;
	int length;

	monitor = NULL;
	monitor = malloc(sizeof(b3_monitor_t));

	monitor->wsman_factory = wsman_factory;

	length = strlen(monitor_name) + 1;
	monitor->monitor_name = malloc(length);
	strcpy(monitor->monitor_name, monitor_name);

	monitor->monitor_area = monitor_area;

	monitor->wsman = b3_wsman_factory_create(wsman_factory);

	monitor->bar = b3_bar_new(monitor->monitor_name, monitor->monitor_area, monitor->wsman);

	return monitor;
}

int
b3_monitor_free(b3_monitor_t *monitor)
{
	monitor->wsman_factory = NULL;

	free(monitor->monitor_name);
	monitor->monitor_name = NULL;

	b3_bar_free(monitor->bar);
	monitor->bar = NULL;

	b3_wsman_free(monitor->wsman);
	monitor->wsman = NULL;

	free(monitor);
	return 0;
}

const char *
b3_monitor_get_monitor_name(b3_monitor_t *monitor)
{
	return monitor->monitor_name;
}

RECT
b3_monitor_get_monitor_area(b3_monitor_t *monitor)
{
	return monitor->monitor_area;
}

b3_bar_t *
b3_monitor_get_bar(b3_monitor_t *monitor)
{
	return monitor->bar;
}

b3_ws_t *
b3_monitor_contains_ws(b3_monitor_t *monitor, const char *ws_id)
{
	return b3_wsman_contains_ws(monitor->wsman, ws_id);
}

int
b3_monitor_set_focused_ws(b3_monitor_t *monitor, const char *ws_id)
{
	return b3_wsman_set_focused_ws(monitor->wsman, ws_id);
}

b3_ws_t *
b3_monitor_get_focused_ws(b3_monitor_t *monitor)
{
	return b3_wsman_get_focused_ws(monitor->wsman);
}

b3_win_t *
b3_monitor_get_focused_win(b3_monitor_t *monitor)
{
	return b3_ws_get_focused_win(b3_wsman_get_focused_ws(monitor->wsman));
}

int
b3_monitor_set_focused_win(b3_monitor_t *monitor, const char *ws_id, const b3_win_t *win)
{
	int error;
	b3_ws_t *ws;

	if (!error) {
		ws = b3_wsman_contains_ws(monitor->wsman, ws_id);
		if (!ws) {
			error = 1;
		}
	}

	if (!error) {
		error = b3_ws_set_focused_win(ws, win);
	}

	return error;
}

b3_wsman_t *
b3_monitor_get_wsman(b3_monitor_t *monitor)
{
	return monitor->wsman;
}

int
b3_monitor_add_win(b3_monitor_t *monitor, b3_win_t *win)
{
	return b3_ws_add_win(b3_monitor_get_focused_ws(monitor),
				  	     win);
}

int
b3_monitor_remove_win(b3_monitor_t *monitor, b3_win_t *win)
{
	return b3_wsman_remove_win(monitor->wsman,
							   win);
}

int
b3_monitor_arrange_wins(b3_monitor_t *monitor)
{
	RECT monitor_area;
	b3_bar_t *bar;
	RECT bar_area;
	int bar_height;

	monitor_area = monitor->monitor_area;

	bar = b3_monitor_get_bar(monitor);
	bar_area = b3_bar_get_area(bar);
	bar_height = bar_area.bottom - bar_area.top;

	if (b3_bar_get_position(bar) == TOP) {
		monitor_area.top = monitor_area.top + bar_height;
	} else if (b3_bar_get_position(bar) == BOTTOM) {
		monitor_area.bottom = monitor_area.bottom - bar_height;
	} else {
		wbk_logger_log(&logger, SEVERE, "Arraning wins - bar position %d is not supported\n", b3_bar_get_position(bar));
	}

  g_arrange_monitor = monitor;
  b3_wsman_iterate_ws_arr(monitor->wsman, b3_monitor_arrange_wins_ws_visitor);

	b3_ws_arrange_wins(b3_monitor_get_focused_ws(monitor), monitor_area);

	return 0;
}

int
b3_monitor_remove_empty_ws(b3_monitor_t *monitor)
{
    return b3_wsman_remove_empty_ws(monitor->wsman);
}

b3_ws_t *
b3_monitor_find_win(b3_monitor_t *monitor, const b3_win_t *win)
{
	return b3_wsman_find_win(monitor->wsman,
						     win);
}

int
b3_monitor_show(b3_monitor_t *monitor)
{
	b3_bar_show(monitor->bar);
	return 0;
}

int
b3_monitor_draw(b3_monitor_t *monitor, HWND window_handler)
{
}
