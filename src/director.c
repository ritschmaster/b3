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
#include <w32bindkeys/logger.h>

#include "monitor.h"
#include "ws.h"

static wbk_logger_t logger = { "director" };

static int
b3_director_free_monitor_arr(b3_director_t *director);

/**
 * It is only possible to set a monitor as focused, that is already available in the director.
 *
 * @return 0 if the focusing was successful. Non-0 otherwise.
 */
static int
b3_director_set_focused_monitor(b3_director_t *director, b3_monitor_t *monitor);

static const b3_monitor_t *
b3_director_get_monitor_by_monitor_name(b3_director_t *director, const char *monitor_name);

static BOOL CALLBACK
b3_director_enum_monitors(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM data);

static DWORD WINAPI
b3_director_repaint_all_threaded(_In_ LPVOID param);

static int
b3_director_repaint_all(void);

b3_director_t *
b3_director_new(b3_monitor_factory_t *monitor_factory)
{
	b3_director_t *director;

	director = NULL;
	director = malloc(sizeof(b3_director_t));

	memset(director, 0, sizeof(b3_director_t));

	director->global_mutex = CreateMutex(NULL, FALSE, NULL);

	array_new(&(director->monitor_arr));

	director->ignore_set_foucsed_win = 0;

	director->monitor_factory = monitor_factory;

	return director;
}

int
b3_director_free(b3_director_t *director)
{
	ReleaseMutex(director->global_mutex);
	CloseHandle(director->global_mutex);

	director->focused_monitor = NULL;

	b3_director_free_monitor_arr(director);

	director->monitor_factory = NULL;

	free(director);

	return 0;
}

int
b3_director_free_monitor_arr(b3_director_t *director)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;

	array_iter_init(&monitor_iter, director->monitor_arr);
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

	WaitForSingleObject(director->global_mutex, INFINITE);

	b3_director_free_monitor_arr(director);
	array_new(&(director->monitor_arr));

	EnumDisplayMonitors(NULL, NULL, b3_director_enum_monitors, (LPARAM) director);

   	b3_director_repaint_all();

	ReleaseMutex(director->global_mutex);

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
    GetMonitorInfo(wmonitor, (LPMONITORINFO) &monitor_info);

    wbk_logger_log(&logger, INFO, "Found monitor: %s - X: %d - %d, Y: %d - %d (%dx%d)\n",
    			   monitor_info.szDevice,
    			   monitor_info.rcMonitor.left, monitor_info.rcMonitor.right,
				   monitor_info.rcMonitor.top, monitor_info.rcMonitor.bottom,
    			   monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
				   monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top);

    monitor = b3_monitor_factory_create(director->monitor_factory,
		    		 	 	 	 	   	monitor_info.szDevice,
									    monitor_info.rcWork);

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

	WaitForSingleObject(director->global_mutex, INFINITE);

	monitor = NULL;
	found = 0;
	array_iter_init(&monitor_iter, director->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END
		   && !found) {
		if (strcmp(b3_monitor_get_monitor_name(monitor), monitor_name) == 0) {
			found = 1;
		}
	}

	ReleaseMutex(director->global_mutex);

	return monitor;
}

b3_monitor_t *
b3_director_get_focused_monitor(b3_director_t *director)
{
	return director->focused_monitor;
}

int
b3_director_set_focused_monitor(b3_director_t *director, b3_monitor_t *monitor)
{
	int error;
	char managed;
	ArrayIter iter;
	b3_monitor_t *monitor_iter;

	WaitForSingleObject(director->global_mutex, INFINITE);

	error = 1;

	managed = 0;
	array_iter_init(&iter, director->monitor_arr);
    while (!managed && array_iter_next(&iter, (void*) &monitor_iter) != CC_ITER_END) {
    	if (monitor_iter == monitor) {
    		managed = 1;
    	}
    }

    if (managed) {
    	wbk_logger_log(&logger, INFO, "Switching to monitor %s.\n", b3_monitor_get_monitor_name(monitor));

		if (director->focused_monitor) {
			b3_bar_set_focused(b3_monitor_get_bar(director->focused_monitor), 0);
		}
		director->focused_monitor = monitor;

		b3_bar_set_focused(b3_monitor_get_bar(director->focused_monitor), 1);

		error = 0;
    }

	ReleaseMutex(director->global_mutex);

	return error;
}

int
b3_director_set_focused_monitor_by_name(b3_director_t *director, const char *monitor_name)
{
	char found;
	ArrayIter iter;
	b3_monitor_t *monitor;
	int ret;

	WaitForSingleObject(director->global_mutex, INFINITE);

	found = 0;
	array_iter_init(&iter, director->monitor_arr);
    while (!found && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
    	if (strcmp(monitor_name, b3_monitor_get_monitor_name(monitor)) == 0)	{
    		found = 1;
    	}
    }

    if (found) {
    	b3_director_set_focused_monitor(director, monitor);

    	wbk_logger_log(&logger, INFO, "Switching to monitor %s.\n", monitor_name);
    	ret = 0;
    } else {
    	wbk_logger_log(&logger, SEVERE, "Cannot switch monitor %s. Monitor not available.\n", monitor_name);
    	ret = 1;
    }

    b3_director_repaint_all();

	ReleaseMutex(director->global_mutex);

   	return ret;
}

int
b3_director_switch_to_ws(b3_director_t *director, const char *ws_id)
{
	char found;
	ArrayIter iter;
	b3_monitor_t *monitor;
	b3_win_t *focused_win;

	wbk_logger_log(&logger, DEBUG, "\n>>>>>> STARTING TO WAIT\n\n");
	WaitForSingleObject(director->global_mutex, INFINITE);

	found = 0;
	array_iter_init(&iter, director->monitor_arr);
    while (!found && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
    	if (b3_monitor_contains_ws(monitor, ws_id))	{
    		found = 1;
    	}
    }

    if (found) {
    	b3_director_set_focused_monitor(director, monitor);
    }

	b3_monitor_set_focused_ws(director->focused_monitor, ws_id);
	focused_win = b3_ws_get_focused_win(b3_monitor_get_focused_ws(b3_director_get_focused_monitor(director)));

    b3_director_arrange_wins(director);

    wbk_logger_log(&logger, INFO, "Switching to workspace %s.\n", ws_id);
    if (focused_win) {
    	wbk_logger_log(&logger, DEBUG, "Restoring focused window\n");
    	director->ignore_set_foucsed_win = 1;
    	b3_director_w32_set_active_window(b3_win_get_window_handler(focused_win), 0);
    }

    b3_director_repaint_all();

	ReleaseMutex(director->global_mutex);

	return 0;
}

int
b3_director_add_win(b3_director_t *director, const char *monitor_name, b3_win_t *win)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	char found;
	int error;

	WaitForSingleObject(director->global_mutex, INFINITE);

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

	ReleaseMutex(director->global_mutex);

	return error;
}

int
b3_director_remove_win(b3_director_t *director, b3_win_t *win)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	int error;

	WaitForSingleObject(director->global_mutex, INFINITE);

	error = 1;
	array_iter_init(&iter, director->monitor_arr);
    while (error && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
    	error = b3_monitor_remove_win(monitor, win);
    }

    if (!error) {
    	b3_director_arrange_wins(director);
    }

	ReleaseMutex(director->global_mutex);

    return error;
}

int
b3_director_arrange_wins(b3_director_t *director)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	int error;

	WaitForSingleObject(director->global_mutex, INFINITE);

	error = 0;
	array_iter_init(&iter, director->monitor_arr);
    while (!error && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
		error = b3_monitor_arrange_wins(monitor);
    }

    ReleaseMutex(director->global_mutex);

	return error;
}

int
b3_director_set_active_win(b3_director_t *director, b3_win_t *win)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	char found;
	b3_ws_t *ws;
	b3_win_t *found_win;
	int ret;

	WaitForSingleObject(director->global_mutex, INFINITE);

	if (!director->ignore_set_foucsed_win) {
		found = 0;
		array_iter_init(&iter, director->monitor_arr);
		while (!found && array_iter_next(&iter, (void*) &monitor) != CC_ITER_END) {
			ws = b3_monitor_find_win(monitor, win);
			if (ws) {
				found_win = b3_ws_contains_win(ws, win);
				found = 1;
			}
		}

		if (found) {
			wbk_logger_log(&logger, DEBUG, "Updating active window\n");
			b3_ws_set_focused_win(ws, found_win);

			if (strcmp(b3_monitor_get_monitor_name(b3_director_get_focused_monitor(director)),
					   b3_monitor_get_monitor_name(monitor))
				|| strcmp(b3_ws_get_name(b3_monitor_get_focused_ws(monitor)),
						  b3_ws_get_name(ws))) {
				b3_director_switch_to_ws(director, b3_ws_get_name(ws));
			}
			ret = 0;
		} else {
			wbk_logger_log(&logger, SEVERE, "Failed updating active window: activated window is unknown\n");
			ret = 1;
		}
	} else {
		director->ignore_set_foucsed_win = 0;
	}

	ReleaseMutex(director->global_mutex);

	return ret;
}

int
b3_director_active_win_toggle_floating(b3_director_t *director)
{
	b3_win_t *active_win;
	int toggle_failed;
	char floating;

	WaitForSingleObject(director->global_mutex, INFINITE);

	toggle_failed = 1;
    active_win = b3_monitor_get_focused_win(director->focused_monitor);
	if (active_win) {
		b3_win_set_floating(active_win,
							!b3_win_get_floating(active_win));
		toggle_failed = 0;
		if (!toggle_failed) {
			wbk_logger_log(&logger, INFO, "Activated window toggled floating\n");
			b3_monitor_arrange_wins(director->focused_monitor);
		}
	}

	ReleaseMutex(director->global_mutex);

	return toggle_failed;
}

int
b3_director_move_active_win_to_ws(b3_director_t *director, const char *ws_id)
{
	ArrayIter iter;
	b3_monitor_t *monitor;
	b3_ws_t *ws;
	const b3_ws_t *ws_old;
	char found;
	int ret;
	b3_win_t *active_win;

	WaitForSingleObject(director->global_mutex, INFINITE);

	active_win = b3_monitor_get_focused_win(director->focused_monitor);
    if (active_win) {
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
			if (b3_director_remove_win(director, active_win) == 0) {
				wbk_logger_log(&logger, INFO, "Moving window to workspace %s\n", ws_id);

				b3_win_set_state(active_win, NORMAL);
				b3_ws_add_win(ws, active_win);

				active_win = b3_monitor_get_focused_win(b3_director_get_focused_monitor(director));

				b3_director_arrange_wins(director);

				if (active_win) {
					/**
					 * active_win might be NULL if the last window was moved from
					 * the current workspace.
					 */
					director->ignore_set_foucsed_win = 1;
					b3_director_w32_set_active_window(b3_win_get_window_handler(active_win), 1);
				}

				ret = 0;
			}
    	}
    } else {
    	ret = 0;
    }

    if (ret) {
    	wbk_logger_log(&logger, WARNING, "Moving window to workspace %s - failed\n", ws_id);
    }

    b3_director_repaint_all();

	ReleaseMutex(director->global_mutex);

   	return ret;
}

int
b3_director_move_active_win(b3_director_t *director, b3_ws_move_direction_t direction)
{
	int error;
	b3_win_t *focused_win;

	WaitForSingleObject(director->global_mutex, INFINITE);

	error = 1;
	focused_win = b3_ws_get_focused_win(b3_monitor_get_focused_ws(director->focused_monitor));
	if (focused_win) {
		if (b3_win_get_state(focused_win) != MAXIMIZED) {
			error = b3_ws_move_active_win(b3_monitor_get_focused_ws(director->focused_monitor),
										   direction);
      if (!error) {
        b3_director_arrange_wins(director);
      } else {
          /**
           * Try changing to the other monitor then
           */
          error = b3_director_move_focused_win_to_monitor_by_dir(director, direction);

          if (!error) {
            b3_director_set_focused_monitor_by_direction(director, direction);
          }
      }
		}
	} else {
		wbk_logger_log(&logger, INFO, "No focused window available to move in a direction.\n");
	}

	ReleaseMutex(director->global_mutex);

	return error;
}

int
b3_director_set_active_win_by_direction(b3_director_t *director, b3_ws_move_direction_t direction)
{
	int error;
	b3_win_t *win;

	WaitForSingleObject(director->global_mutex, INFINITE);

	error = 1;
	win = b3_ws_get_win(b3_monitor_get_focused_ws(director->focused_monitor),
		  			    direction);
	if (win) {
    	b3_win_set_state(b3_ws_get_focused_win(b3_monitor_get_focused_ws(director->focused_monitor)), NORMAL);
    	b3_ws_set_focused_win(b3_monitor_get_focused_ws(director->focused_monitor),
    						  win);
		director->ignore_set_foucsed_win = 1;
		b3_director_w32_set_active_window(b3_win_get_window_handler(win), 1);
		b3_director_arrange_wins(director);
	} else {
    /**
     * Try changing to the other monitor then
     */
    error = b3_director_set_focused_monitor_by_direction(director, direction);
  }

	ReleaseMutex(director->global_mutex);

	return error;
}

int
b3_director_toggle_active_win_fullscreen(b3_director_t *director)
{
	b3_win_t *active_win;
	WINDOWPLACEMENT windowplacement;

	WaitForSingleObject(director->global_mutex, INFINITE);

    active_win = b3_monitor_get_focused_win(director->focused_monitor);
    if (active_win) {
    	if (b3_win_get_state(active_win) != MAXIMIZED) {
    		b3_win_set_state(active_win, MAXIMIZED);
    	} else {
    		b3_win_set_state(active_win, NORMAL);
    	}
		b3_director_arrange_wins(director);
    } else {
		wbk_logger_log(&logger, INFO, "No focused window available to toggle fullscreen.\n");
    }

    b3_director_repaint_all();

	ReleaseMutex(director->global_mutex);

	return 0;
}

b3_monitor_t *
b3_director_get_monitor_by_direction(b3_director_t *director, b3_ws_move_direction_t direction)
{
	char found;
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;
	RECT focused_area;
	RECT other_area;

	WaitForSingleObject(director->global_mutex, INFINITE);

	found = 0;
	focused_area = b3_monitor_get_monitor_area(b3_director_get_focused_monitor(director));
	array_iter_init(&monitor_iter, b3_director_get_monitor_arr(director));
	while (!found && array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END) {
		if (monitor != b3_director_get_focused_monitor(director)) {
			other_area = b3_monitor_get_monitor_area(monitor);
			switch (direction) {
			case UP:
				if (focused_area.top >= other_area.bottom) {
					found = 1;
				}
				break;

			case DOWN:
				if (focused_area.bottom <= other_area.top) {
					found = 1;
				}
				break;

			case LEFT:
				if (focused_area.left >= other_area.right) {
					found = 1;
				}
				break;

			case RIGHT:
				if (focused_area.right <= other_area.left) {
					found = 1;
				}
				break;

			default:
				wbk_logger_log(&logger, SEVERE, "Retrieving monitor not possible - unknown direction %d\n", direction);
				break;
			}
		}
	}

	if (!found) {
		monitor = NULL;
	}

	ReleaseMutex(director->global_mutex);

	return monitor;

}

int
b3_director_move_focused_ws_to_monitor_by_dir(b3_director_t *director, b3_ws_move_direction_t direction)
{
	int error;
	b3_monitor_t *monitor;
	b3_wsman_t *old_focused_wsman;
	b3_wsman_t *new_focused_wsman;
	b3_ws_t *focused_ws;

	error = 1;

	monitor = b3_director_get_monitor_by_direction(director, direction);

	WaitForSingleObject(director->global_mutex, INFINITE);

	if (monitor) {
		old_focused_wsman = b3_monitor_get_wsman(b3_director_get_focused_monitor(director));
		new_focused_wsman = b3_monitor_get_wsman(monitor);

		focused_ws = b3_wsman_get_focused_ws(old_focused_wsman);
		b3_wsman_remove(old_focused_wsman, b3_ws_get_name(focused_ws));
		b3_wsman_add(new_focused_wsman, b3_ws_get_name(focused_ws));
		b3_director_switch_to_ws(director, b3_ws_get_name(focused_ws));

		wbk_logger_log(&logger, INFO, "Moving workspace to the monitor in direction %d\n", direction);

		error = 0;
	} else {
		wbk_logger_log(&logger, INFO, "Moving workspace not possible - no monitor in direction %d\n", direction);
	}

	ReleaseMutex(director->global_mutex);

	return error;
}

int
b3_director_set_focused_monitor_by_direction(b3_director_t *director, b3_ws_move_direction_t direction)
{
	int error;
	b3_monitor_t *monitor;

	error = 1;

	monitor = b3_director_get_monitor_by_direction(director, direction);

	WaitForSingleObject(director->global_mutex, INFINITE);

	if (monitor) {
		wbk_logger_log(&logger, INFO, "Changing focused monitor in direction %d\n", direction);
		error = b3_director_switch_to_ws(director,
									     b3_ws_get_name(b3_monitor_get_focused_ws(monitor)));
	} else {
		wbk_logger_log(&logger, INFO, "Changing focused monitor not possible - no monitor in direction %d\n", direction);
	}

	ReleaseMutex(director->global_mutex);

	return error;
}

int
b3_director_move_focused_win_to_monitor_by_dir(b3_director_t *director, b3_ws_move_direction_t direction)
{
	int error;
	b3_monitor_t *monitor;

	error = 1;

	monitor = b3_director_get_monitor_by_direction(director, direction);

	WaitForSingleObject(director->global_mutex, INFINITE);

	if (monitor) {
		wbk_logger_log(&logger, INFO, "Moving the focused window to monitor in direction %d\n", direction);
		error = b3_director_move_active_win_to_ws(director,
									     	 	  b3_ws_get_name(b3_monitor_get_focused_ws(monitor)));
	} else {
		wbk_logger_log(&logger, INFO, "Moving the focused window to monitor not possible - no monitor in direction %d\n", direction);
	}

	ReleaseMutex(director->global_mutex);

	return error;
}

int
b3_director_show(b3_director_t *director)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;

	WaitForSingleObject(director->global_mutex, INFINITE);

	array_iter_init(&monitor_iter, director->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END) {
		b3_monitor_show(monitor);
	}

	ReleaseMutex(director->global_mutex);

	return 0;
}

int
b3_director_draw(b3_director_t *director, HWND window_handler)
{
	ArrayIter monitor_iter;
	b3_monitor_t *monitor;

	WaitForSingleObject(director->global_mutex, INFINITE);

	array_iter_init(&monitor_iter, director->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END) {
		b3_monitor_draw(monitor, window_handler);
	}

	ReleaseMutex(director->global_mutex);

	return 0;
}

int
b3_director_close_active_win(b3_director_t *director)
{
	int error;
	b3_win_t *focused_win;

	WaitForSingleObject(director->global_mutex, INFINITE);

	error = 1;
	focused_win = b3_ws_get_focused_win(b3_monitor_get_focused_ws(b3_director_get_focused_monitor(director)));
	if (focused_win) {
		SendMessage(b3_win_get_window_handler(focused_win),
					WM_CLOSE, (WPARAM) NULL, (LPARAM) NULL);
		error = 0;
	} else {
		wbk_logger_log(&logger, INFO, "No focused window available to close.\n");
	}

	ReleaseMutex(director->global_mutex);

	return error;
}

int
b3_director_remove_empty_ws(b3_director_t *director)
{
  ArrayIter monitor_iter;
	b3_monitor_t *monitor;

	WaitForSingleObject(director->global_mutex, INFINITE);

	array_iter_init(&monitor_iter, director->monitor_arr);
	while (array_iter_next(&monitor_iter, (void *) &monitor) != CC_ITER_END) {
		b3_monitor_remove_empty_ws(monitor);
	}

  b3_director_repaint_all();

	ReleaseMutex(director->global_mutex);

  return 0;
}

int
b3_director_w32_set_active_window(HWND window_handler, char generate_lag)
{
	int error;
	int i;
	DWORD this_tid;
	DWORD foreground_tid;

	error = 0;

	foreground_tid = GetCurrentThreadId();
	this_tid = GetWindowThreadProcessId(GetForegroundWindow(), 0);

	if(this_tid != foreground_tid) {
		AttachThreadInput(this_tid, foreground_tid, TRUE);
		AllowSetForegroundWindow(ASFW_ANY);
	}

	/**
	 * For some reason the WIN32 API needs to be spammed to actually update
	 * the focus.
	 */
	int upper;
	upper = 1;
	if (generate_lag) {
		upper = 10000;
	}
	for (i = 0; i < upper && GetActiveWindow() != window_handler; i++) {
		SetForegroundWindow(window_handler);
		SetActiveWindow(window_handler);
		SetFocus(window_handler);
		Sleep(0.5);
	}

	if(this_tid != foreground_tid) {
		AttachThreadInput(this_tid, foreground_tid, FALSE);
	}

	return error;
}

DWORD WINAPI
b3_director_repaint_all_threaded(_In_ LPVOID param)
{
	SendMessage(HWND_BROADCAST, WM_NCPAINT, (WPARAM) NULL, (LPARAM) NULL);
	return 0;
}

int
b3_director_repaint_all(void)
{
	HANDLE thread_handler;

	thread_handler = CreateThread(NULL,
					 0,
					 b3_director_repaint_all_threaded,
					 NULL,
					 0,
					 NULL);

	return 0;
}
