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

#include <stdio.h>
#include <collectc/array.h>
#include <windows.h>
#include <w32bindkeys/logger.h>
#include <w32bindkeys/kbdaemon.h>
#include <w32bindkeys/util.h>
#include <w32bindkeys/datafinder.h>
#include <w32bindkeys/kbman.h>
#include <getopt.h>

#include "../config.h"
#include "win_factory.h"
#include "ws_factory.h"
#include "wsman_factory.h"
#include "monitor_factory.h"
#include "kc_director_factory.h"
#include "condition_factory.h"
#include "action_factory.h"
#include "parser.h"
#include "director.h"
#include "win_watcher.h"

#define B3_GETOPT_OPTIONS "dvV"

#define B3_KBDAEMON_ARR_LEN 30

static struct option B3_GETOPT_LONG_OPTIONS[] = {
    /*   NAME          ARGUMENT           FLAG  SHORTNAME */
        {"all",        no_argument,       NULL, 'd'},
        {NULL,         0,                 NULL, 0}
    };

static wbk_logger_t logger = { "main" };

static const char g_szClassName[] = "myWindowClass";

static b3_director_t *g_director;

static wbk_kbdaemon_t **g_kbdaemon_arr = NULL;
static wbk_kbman_t **g_kbman_arr = NULL;

static int
print_version(void);

static int
parameterized_main(void);

static int
main_loop(void);

static LRESULT CALLBACK
window_callback(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

static int
kbdaemon_exec_fn(wbk_kbdaemon_t *kbdaemon, wbk_b_t *b);

int
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int error;
	char exec;
	LPWSTR *wargv;
	char **argv;
	int argc;
	int length;
	int i;
	size_t size;
	int opt;
	int option_index;

	wbk_logger_set_level(SEVERE);

	exec = 1;

	wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (wargv) {
		argv = malloc(sizeof(char *) * argc);
		for (i = 0; i < argc; i++) {
			size = sizeof(char) * (sizeof(wchar_t) * wcslen(wargv[i]) + 1);
			argv[i] = malloc(size);
			memset(argv[i], 0, size);
			wcstombs(argv[i], wargv[i], size);
		}

		option_index = 0;
		while ((opt = getopt_long(argc, argv,
								  B3_GETOPT_OPTIONS,
								  B3_GETOPT_LONG_OPTIONS,
								  &option_index))
				!= -1) {
			switch(opt) {
			case 'V':
				wbk_logger_set_level(INFO);
				break;

			case 'd':
				wbk_logger_set_level(DEBUG);
				break;

			case 'v':
				error = print_version();
				exec = 0;
				break;
			}
		}

		free(argv);
		LocalFree(wargv);
	}

	if (exec) {
		error = parameterized_main();
	}

	return error;
}

int
parameterized_main(void)
{
	int error;
	char *dir;
	char *config_filename;
	FILE *config_file;
	wbk_datafinder_t *datafinder;
	b3_win_factory_t *win_factory;
	b3_ws_factory_t *ws_factory;
	b3_wsman_factory_t *wsman_factory;
	b3_monitor_factory_t *monitor_factory;
	b3_kc_director_factory_t *kc_director_factory;
	b3_condition_factory_t *condition_factory;
	b3_action_factory_t *action_factory;
	b3_parser_t *parser;
	b3_win_watcher_t *win_watcher;
	wbk_kbman_t *g_kbman;
	int i;

	error = 0;

	win_factory = b3_win_factory_new();
	ws_factory = b3_ws_factory_new();
	wsman_factory = b3_wsman_factory_new(ws_factory);
	monitor_factory = b3_monitor_factory_new(wsman_factory);
	kc_director_factory = b3_kc_director_factory_new();
	condition_factory = b3_condition_factory_new();
	action_factory = b3_action_factory_new();

	parser = b3_parser_new(kc_director_factory, condition_factory, action_factory);

	dir = wbk_path_from_home(".b3");
	datafinder = wbk_datafinder_new(dir);
	free(dir);

	dir = wbk_path_from_home(".config\\b3");
	wbk_datafinder_add_datadir(datafinder, dir);
	free(dir);

	config_filename = wbk_datafinder_gen_path(datafinder, "config");
	if (config_filename) {
		config_file = fopen(config_filename, "r");
		g_director = NULL;

		if (config_file) {
			g_director = b3_director_new(monitor_factory);
		} else {
			wbk_logger_log(&logger, SEVERE, "Could not open %s\n", config_filename);
			error = 1;
		}

		free(config_filename);
	} else {
		wbk_logger_log(&logger, SEVERE, "Could not find a config file\n");
		error = 2;
	}


	wbk_datafinder_free(datafinder);

	/**
	 * Setup key bindings
	 */
	if (g_director) {
		g_kbman = b3_parser_parse_file(parser, g_director, config_file);

		if (g_kbman) {
			g_kbman_arr = wbk_kbman_split(g_kbman, B3_KBDAEMON_ARR_LEN);

			wbk_kbman_free(g_kbman);
			g_kbman = NULL;
		} else {
			wbk_logger_log(&logger, SEVERE, "Could not parse config file.\n");
			error = 1;
		}
	}

	if (config_file) {
		fclose(config_file);
	}

	/**
	 * Setup window watcher
	 */
	if (!error) {
		win_watcher = b3_win_watcher_new(win_factory, g_director);
	}

	/**
	 * "Start" director
	 */
	if (!error) {
		b3_director_refresh(g_director);

		b3_director_show(g_director);
		b3_director_switch_to_ws(g_director,
								 b3_ws_get_name(b3_monitor_get_focused_ws(b3_director_get_focused_monitor(g_director))));

	}

	/**
	 * Start win watcher
	 */
	if (!error) {
		b3_win_watcher_start(win_watcher);
	}

	/**
	 * Setup keyboard daemon
	 */
	if (!error) {
		g_kbdaemon_arr = malloc(sizeof(wbk_kbdaemon_t **) * B3_KBDAEMON_ARR_LEN);
		for (i = 0; i < B3_KBDAEMON_ARR_LEN; i++) {
			g_kbdaemon_arr[i] = wbk_kbdaemon_new(kbdaemon_exec_fn);
			if (g_kbdaemon_arr[i]) {
				error = wbk_kbdaemon_start(g_kbdaemon_arr[i]);
			} else {
				error = 1;
			}
		}
	}

	/**
	 * Start main loops
	 */
	if (!error) {
		main_loop();

		b3_win_watcher_stop(win_watcher);
	}

	if (g_kbdaemon_arr) {
		for (i = 0; i < B3_KBDAEMON_ARR_LEN; i++) {
			if (g_kbdaemon_arr[i]) {
				wbk_kbdaemon_free(g_kbdaemon_arr[i]);
				g_kbdaemon_arr[i] = NULL;
			}
		}
		free(g_kbdaemon_arr);
		g_kbdaemon_arr = NULL;
	}

	if (g_kbman_arr) {
		for (i = 0; i < B3_KBDAEMON_ARR_LEN; i++) {
			if (g_kbman_arr[i]) {
				wbk_kbman_free(g_kbman_arr[i]);
				g_kbman_arr[i] = NULL;
			}
		}
		free(g_kbman_arr);
		g_kbman_arr = NULL;
	}

	if (win_watcher) {
		b3_win_watcher_free(win_watcher);
	}

	if (g_director) {
		b3_director_free(g_director);
	}

	b3_kc_director_factory_free(kc_director_factory);
	b3_monitor_factory_free(monitor_factory);
	b3_wsman_factory_free(wsman_factory);
	b3_ws_factory_free(ws_factory);
	b3_win_factory_free(win_factory);

	return error;
}

int
main_loop(void)
{
	MSG Msg;

	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return 0;
}

LRESULT CALLBACK
window_callback(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT CALLBACK result;
	int id;

	result = 0;

	switch(msg)
	{
	case WM_NCPAINT:
		b3_director_draw(g_director, window_handler);
		break;

	case WM_CLOSE:
		DestroyWindow(window_handler);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_HOTKEY:
		break;

	default:
		result = DefWindowProc(window_handler, msg, wParam, lParam);
	}

	return result;
}

inline int
kbdaemon_exec_fn(wbk_kbdaemon_t *kbdaemon, wbk_b_t *b)
{
	int i;

	for (i = 0; i < B3_KBDAEMON_ARR_LEN; i++) {
		if (kbdaemon == g_kbdaemon_arr[i]) {
			return wbk_kbman_exec(g_kbman_arr[i], b);
		}
	}
	return 1;
}

int
print_version(void)
{
	fprintf(stdout, "%s version %s\n", PACKAGE, PACKAGE_VERSION);
	return 0;
}

