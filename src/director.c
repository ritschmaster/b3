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

/**
 * @author Richard Bäck <richard.baeck@mailbox.org>
 * @date 2020-02-26
 * @brief File contains the director class implementation and private methods
 */

#include "director.h"

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wbkbase/logger.h>

#include "monitor.h"
#include "ws.h"

static wbk_logger_t logger = { "director" };

static int
b3_director_free_monitor_arr(b3_director_t *director);

static const b3_monitor_t *
b3_director_get_monitor_by_monitor_name(b3_director_t *director, const char *monitor_name);

static BOOL CALLBACK
b3_director_enum_monitors(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM data);

static int
b3_director_repaint_all(void);

b3_director_t *
b3_director_new(b3_monitor_factory_t *monitor_factory)
{
	b3_director_t *director;

	director = NULL;
	director = malloc(sizeof(b3_director_t));

	memset(director, 0, sizeof(b3_director_t));

	array_new(&(director->monitor_arr));

	director->monitor_factory = monitor_factory;

	return director;
}

int
b3_director_free(b3_director_t *director)
{
	director->focused_monitor = NULL;

	b3_director_free_monitor_arr(director);

	director->monitor_factory = NULL;

	director->active_win = NULL;

	free(director);

	return 0;
}

int
b3_director_free_monitor_arr(b3_director_t *director)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;

	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END) {
		b3_monitor_free(monitor);
	}

	array_destroy_cb(director->monitor_arr, NULL);

	director->monitor_arr = NULL;

	return 0;
}

int
b3_director_refresh(b3_director_t *director)
{
	b3_monitor_t *monitor;

	char found;

	EnumDisplayMonitors(NULL, NULL, b3_director_enum_monitors, (LPARAM) director);

   	b3_director_repaint_all();

	return 0;
}

BOOL CALLBACK
b3_director_enum_monitors(HMONITOR wmonitor, HDC hdc, LPRECT rect, LPARAM data)
{
    b3_director_t *director;
    MONITORINFOEX monitor_info;
    b3_monitor_t *monitor;

    director = (b3_director_t *) data;

    monitor_info.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(wmonitor, &monitor_info);

    wbk_logger_log(&logger, INFO, "Found monitor: %s - %dx%d\n",
    			   monitor_info.szDevice,
    			   monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
				   monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top);

    monitor = b3_monitor_factory_create(director->monitor_factory,
		    		 	 	 	 	   	monitor_info.szDevice,
									    monitor_info.rcMonitor);
    array_add(director->monitor_arr,
	 	      monitor);
    if (director->focused_monitor == NULL) {
    	director->focused_monitor = monitor;
    }

    return TRUE;
}

Array *
b3_director_get_monitor_arr(b3_director_t *director)
{
	return director->monitor_arr;
}

const b3_monitor_t *
b3_director_get_monitor_by_monitor_name(b3_director_t *director, const char *monitor_name)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;
	char found;

	monitor = NULL;
	found = 0;
	array_iter_init(&monitor_iter, director->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END
		   && !found) {
		if (strcmp(b3_monitor_get_monitor_name(monitor), monitor_name) == 0) {
			found = 1;
		}
	}

	return monitor;
}

int
b3_director_set_focused_monitor(b3_director_t *director, const char *monitor_name)
{
	char found;
	ArrayIter iter;
	b3_monitor_t *monitor;
	int ret;

	found = 0;
	array_iter_init(&iter, director->monitor_arr);
    while (!found && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
    	if (strcmp(monitor_name, b3_monitor_get_monitor_name(monitor)) == 0)	{
    		found = 1;
    	}
    }

    if (found) {
    	director->focused_monitor = monitor;
    	wbk_logger_log(&logger, INFO, "Switching to monitor %s.\n", monitor_name);
    	ret = 0;
    } else {
    	wbk_logger_log(&logger, SEVERE, "Cannot switch monitor %s. Monitor not available.\n", monitor_name);
    	ret = 1;
    }

    b3_director_repaint_all();

   	return ret;
}

int
b3_director_switch_to_ws(b3_director_t *director, const char *ws_id)
{
	char found;
	ArrayIter iter;
	b3_monitor_t *monitor;
	b3_win_t *focused_win;

	found = 0;
	array_iter_init(&iter, director->monitor_arr);
    while (!found && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
    	if (b3_monitor_contains_ws(monitor, ws_id))	{
    		found = 1;
    	}
    }

    if (found) {
    	director->focused_monitor = monitor;
    	wbk_logger_log(&logger, INFO, "Switching to monitor %s.\n", b3_monitor_get_monitor_name(monitor));
    }
    b3_monitor_set_focused_ws(director->focused_monitor, ws_id);
    b3_director_arrange_wins(director);

    focused_win = b3_monitor_get_focused_win(director->focused_monitor);
    if (focused_win) {
    	SetActiveWindow(b3_win_get_window_handler(focused_win));
    	director->active_win = focused_win;
    }

    wbk_logger_log(&logger, INFO, "Switching to workspace %s.\n", ws_id);

    b3_director_repaint_all();

	return 0;
}

int
b3_director_add_win(b3_director_t *director, const char *monitor_name, b3_win_t *win)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	char found;
	int error;

	found = 0;
	array_iter_init(&iter, director->monitor_arr);
    while (!found && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
    	if (strcmp(b3_monitor_get_monitor_name(monitor), monitor_name) == 0) {
    		found = 1;
    	}
    }

    error = 1;
    if (found) {
    	error = b3_monitor_add_win(monitor, win);
    }

    if (!error) {
		b3_director_arrange_wins(director);
    }

	return error;
}

int
b3_director_remove_win(b3_director_t *director, b3_win_t *win)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	int error;

	error = 1;
	array_iter_init(&iter, director->monitor_arr);
    while (error && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
    	error = b3_monitor_remove_win(monitor, win);
    }

    if (!error) {
    	b3_director_arrange_wins(director);
    }

    return error;
}

int
b3_director_arrange_wins(b3_director_t *director)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	int ret;

	ret = 1;
	array_iter_init(&iter, director->monitor_arr);
    while (ret && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
		b3_monitor_arrange_wins(director->focused_monitor);
    }

	return 0;
}

int
b3_director_set_active_win(b3_director_t *director, b3_win_t *win)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	char found;
	const b3_ws_t *ws;
	int ret;

	found = 0;
	array_iter_init(&iter, director->monitor_arr);
    while (!found && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
    	ws = b3_monitor_find_win(monitor, win);
    	if (ws) {
    		found = 1;
    	}
    }

    if (found) {
    	b3_director_switch_to_ws(director, ws->name);
    	b3_ws_set_focused_win(ws, win);
    	director->active_win = win;
		b3_director_arrange_wins(director);
    	ret = 0;
    } else {
    	wbk_logger_log(&logger, SEVERE, "Activated window is unknown\n");
    	ret = 1;
    }

	return ret;
}

int
b3_director_active_win_toggle_floating(b3_director_t *director)
{
	int toggle_failed;
	char floating;

	toggle_failed = 1;
	if (director->active_win) {
		b3_win_set_floating(director->active_win,
							!b3_win_get_floating(director->active_win));
		toggle_failed = 0;
		if (!toggle_failed) {
			wbk_logger_log(&logger, SEVERE, "Activated window toggled floating\n");
			b3_monitor_arrange_wins(director->focused_monitor);
		}
	}

	return toggle_failed;
}

int
b3_director_move_active_win_to_ws(b3_director_t *director, const char *ws_id)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	const b3_ws_t *ws;
	const b3_ws_t *ws_old;
	char found;
	int ret;

	/** Find the correct monitor to add */
	found = 0;
	array_iter_init(&iter, director->monitor_arr);
    while (!found && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
    	ws = b3_monitor_contains_ws(monitor, ws_id);
		if (ws) {
			found = 1;
		}
    }

    if (!found) {
    	ws = b3_wsman_add(b3_monitor_get_wsman(director->focused_monitor), ws_id);
    }

    ret = 1;
    if (ws) {
    	if (b3_director_remove_win(director, director->active_win) == 0) {
			b3_ws_add_win(ws, director->active_win);

			// TODO focus next window on the current workspace
	//    	b3_director_set_active_win_internal(director, monitor, ws, director->focused_win.win);

			wbk_logger_log(&logger, INFO, "Moved window to %s\n", ws_id);

			b3_director_arrange_wins(director);

			ret = 0;
    	}
    }

    if (ret) {
    	wbk_logger_log(&logger, INFO, "Unable to move window to %s\n", ws_id);
    }

    b3_director_repaint_all();

   	return ret;
}

int
b3_director_move_active_win(b3_director_t *director, b3_ws_move_direction_t direction)
{
	int error;

	error = b3_ws_move_active_win(b3_monitor_get_focused_ws(director->focused_monitor),
			 					  direction);
    b3_director_arrange_wins(director);

	return error;
}

int
b3_director_show(b3_director_t *director)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;

	array_iter_init(&monitor_iter, director->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END) {
		b3_monitor_show(monitor);
	}

	return 0;
}

int
b3_director_draw(b3_director_t *director, HWND window_handler)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;

	array_iter_init(&monitor_iter, director->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END) {
		b3_monitor_draw(monitor, window_handler);
	}

	return 0;
}

int
b3_director_repaint_all(void)
{
	SendMessage(HWND_BROADCAST, WM_NCPAINT, (void *) NULL, (void *) NULL);
	return 0;
}
