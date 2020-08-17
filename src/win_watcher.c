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

#include "win_watcher.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <w32bindkeys/logger.h>
#include <windows.h>
#include <collectc/hashtable.h>

static wbk_logger_t logger =  { "win_watcher" };

static LRESULT CALLBACK
b3_win_watcher_wnd_proc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

static BOOL CALLBACK
b3_win_watcher_enum_windows(HWND window_handler, LPARAM param);

b3_win_watcher_t *
b3_win_watcher_new(b3_win_factory_t *win_factory, b3_director_t *director)
{
	b3_win_watcher_t *win_watcher;

	win_watcher = NULL;
	win_watcher = malloc(sizeof(b3_win_watcher_t));
	memset(win_watcher, 0, sizeof(b3_win_watcher_t));

	win_watcher->win_factory = win_factory;

	win_watcher->director = director;

	return win_watcher;
}

b3_win_watcher_t *
b3_win_watcher_free(b3_win_watcher_t *win_watcher)
{
	b3_win_watcher_stop(win_watcher);

	win_watcher->win_factory = NULL;

	win_watcher->director = NULL;

	win_watcher->window_handler = NULL;

	free(win_watcher);
}

int
b3_win_watcher_start(b3_win_watcher_t *win_watcher)
{
	HINSTANCE hInstance;
	WNDCLASSEX wc;
	HWND window_handler;
	int error;
	char classname[] = "b3 win watcher";

	error = 0;

	if (!error) {
		hInstance = GetModuleHandle(NULL);

		wc.cbSize		= sizeof(WNDCLASSEX);
		wc.style		 = 0;
		wc.lpfnWndProc   = b3_win_watcher_wnd_proc;
		wc.cbClsExtra	= 0;
		wc.cbWndExtra	= 0;
		wc.hInstance	 = hInstance;
		wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor	   = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = classname;
		wc.hIconSm	   = LoadIcon(NULL, IDI_APPLICATION);

		if(!RegisterClassEx(&wc)) {
			error = 1;
		}
	}

	if (!error) {
		window_handler = CreateWindowEx(WS_EX_NOACTIVATE,
						classname,
						classname,
						WS_DISABLED,
						0, 0,
						0, 0,
						NULL, NULL, hInstance, win_watcher);

		ShowWindow(window_handler, SW_HIDE);

		RegisterShellHookWindow(window_handler);
		win_watcher->shellhookid = RegisterWindowMessageW(L"SHELLHOOK");

		SetWindowLongPtr(window_handler, GWLP_USERDATA, (LONG_PTR) win_watcher);

		win_watcher->window_handler = window_handler;
	}

	if (!error) {
		EnumWindows(b3_win_watcher_enum_windows, (LPARAM) win_watcher);
	}

	if (!error) {
		b3_director_arrange_wins(win_watcher->director);
	}

	if (!error) {
		/**
		 * Hack to automatically retrieve the currently active window
		 */
		HWND foreground_window = GetForegroundWindow();
		ShowWindow(foreground_window, SW_MINIMIZE);
		b3_director_w32_set_active_window(foreground_window, 1);
	}

	return 0;
}

int
b3_win_watcher_stop(b3_win_watcher_t *win_watcher)
{
	if (win_watcher->window_handler) {
		DestroyWindow(win_watcher->window_handler);
		win_watcher->window_handler = NULL;
	}

	return 0;
}

LRESULT CALLBACK
b3_win_watcher_wnd_proc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	b3_win_watcher_t *win_watcher;
	b3_win_t *win;
	HMONITOR monitor;
    MONITORINFOEX monitor_info;

    win_watcher = (b3_win_watcher_t *) GetWindowLongPtr(window_handler, GWLP_USERDATA);

	switch(msg) {
	case WM_CLOSE:
		DestroyWindow(window_handler);
		break;

	default:
		if (win_watcher
		    && msg == win_watcher->shellhookid) {
			switch (wParam & 0x7fff) {
				case HSHELL_WINDOWCREATED:
					if (b3_win_watcher_managable_window_handler(win_watcher, (HWND) lParam)) {
						monitor = MonitorFromWindow((HWND) lParam, MONITOR_DEFAULTTONEAREST);
						monitor_info.cbSize = sizeof(MONITORINFOEX);
						GetMonitorInfo(monitor, (LPMONITORINFO) &monitor_info);

						win = b3_win_factory_win_create(win_watcher->win_factory, (HWND) lParam);
						if (b3_director_add_win(win_watcher->director, monitor_info.szDevice, win)) {
						}

						DeleteObject(monitor);
					}
					break;

				case HSHELL_WINDOWDESTROYED:
					win = b3_win_factory_win_create(win_watcher->win_factory, (HWND) lParam);
					if (b3_director_remove_win(win_watcher->director, win) == 0) {
						b3_win_factory_win_free(win_watcher->win_factory, win);
					}
					break;

				case HSHELL_WINDOWACTIVATED:
					if (b3_win_watcher_managable_window_handler(win_watcher, (HWND) lParam)) {
						win = b3_win_factory_win_create(win_watcher->win_factory, (HWND) lParam);
						if (b3_director_set_active_win(win_watcher->director, win) == 0) {
						}
					}
					break;
			}
		} else {
			return DefWindowProc(window_handler, msg, wParam, lParam);
		}
	}

	return 0;
}

BOOL CALLBACK
b3_win_watcher_enum_windows(HWND window_handler, LPARAM param)
{
	HMONITOR monitor;
    MONITORINFOEX monitor_info;
	b3_win_t *win;
	b3_win_watcher_t *win_watcher;

	win_watcher = (b3_win_watcher_t *) param;
	if (b3_win_watcher_managable_window_handler(win_watcher, window_handler)) {
		monitor = MonitorFromWindow(window_handler, MONITOR_DEFAULTTONEAREST);
		monitor_info.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(monitor, (LPMONITORINFO) &monitor_info);

		win = b3_win_factory_win_create(win_watcher->win_factory, window_handler);

		if (b3_director_add_win(win_watcher->director, monitor_info.szDevice, win)) {
			b3_win_factory_win_free(win_watcher->win_factory, win);
		}

		DeleteObject(monitor);
	}

	return TRUE;
}

int
b3_win_watcher_managable_window_handler(b3_win_watcher_t *win_watcher, HWND window_handler)
{
	int exstyle;
	HWND window_owner;
	HWND parent;
	RECT rect;
	ArrayIter iter;
	b3_monitor_t *monitor_iter;
	int parent_managable;
	int ret;
	char classname[B3_WIN_WATCHER_BUFFER_LENGTH];
	char title[B3_WIN_WATCHER_BUFFER_LENGTH];
	char iter_title[B3_WIN_WATCHER_BUFFER_LENGTH];

	parent_managable = 0;
	if (window_handler != 0) {
		parent = GetParent(window_handler);
		parent_managable = b3_win_watcher_managable_window_handler(win_watcher, parent);

		GetWindowText(window_handler, title, B3_WIN_WATCHER_BUFFER_LENGTH);
		GetClassName(window_handler, classname, B3_WIN_WATCHER_BUFFER_LENGTH);

		ret = 1;
		array_iter_init(&iter, b3_director_get_monitor_arr(win_watcher->director));
		while (ret && array_iter_next(&iter, (void*) &monitor_iter) != CC_ITER_END) {
			GetWindowText(b3_monitor_get_bar(monitor_iter)->window_handler, iter_title, B3_WIN_WATCHER_BUFFER_LENGTH);
			if (strcmp(title, iter_title) == 0) {
				ret = 0;
			}
		}

		GetWindowText(win_watcher->window_handler, iter_title, B3_WIN_WATCHER_BUFFER_LENGTH);

	    if (ret
			/**
			 * Not the win_watcher himself!
			 */
	    	&& strcmp(title, iter_title)

	    	/**
	    	 * All Windows related
	    	 */
	    	&& strcmp(classname, "Windows.UI.Core.CoreWindow")
	        && strcmp(title, "Windows Shell Experience Host")
	        && strcmp(title, "Microsoft Text Input Application")
	        && strcmp(title, "Action center")
	        && strcmp(title, "New Notification")
	        && strcmp(title, "Date and Time Information")
	        && strcmp(title, "Volume Control")
	        && strcmp(title, "Network Connections")
	        && strcmp(title, "Cortana")
	        && strcmp(title, "Start")
	        && strcmp(title, "Windows Default Lock Screen")
	        && strcmp(title, "Search")
	        && strcmp(title, "Microsoft Store")
	        && strcmp(title, "TaskManagerWindow")
			&& strcmp(classname, "ForegroundStaging")
	        && strcmp(classname, "ApplicationManager_DesktopShellWindow")
	        && strcmp(classname, "Static")
	        && strcmp(classname, "Scrollbar")
	        && strcmp(classname, "Progman")
	        && strcmp(classname, "TaskManagerWindow")
	        && strcmp(classname, "ApplicationFrameWindow")

			/**
			 * General window stuff
			 */
		    && IsWindow(window_handler)
			&& IsWindowVisible(window_handler)
			&& (!parent || parent_managable)
			) {
			exstyle = GetWindowLong(window_handler, GWL_EXSTYLE);
			window_owner = GetWindow(window_handler, GW_OWNER);

			GetWindowRect(window_handler, &rect);


			if (((((exstyle & WS_EX_TOOLWINDOW) == 0) && window_owner == 0)
				  || ((exstyle & WS_EX_APPWINDOW) && window_owner != 0))
				&& (((exstyle & WS_EX_NOACTIVATE) == 0) && window_owner == 0)) {
				wbk_logger_log(&logger, DEBUG, "Managable - title: %s, classname: %s\n", title, classname);
				return 1;
			}
		}

		wbk_logger_log(&logger, DEBUG, "Not managable - title: %s, classname: %s\n", title, classname);
	}

	return 0;
}
