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
#include <wbkbase/logger.h>
#include <wbkbase/kbdaemon.h>
#include <getopt.h>

#include "win_factory.h"
#include "ws_factory.h"
#include "wsman_factory.h"
#include "monitor_factory.h"
#include "kc_director_factory.h"
#include "parser.h"
#include "kbman.h"
#include "director.h"
#include "win_watcher.h"

static wbk_logger_t logger = { "main" };

static const char g_szClassName[] = "myWindowClass";

static b3_director_t *g_director;

static b3_kbman_t *g_kbman;

static int
main_loop(HINSTANCE hInstance, int nCmdShow);

static LRESULT CALLBACK
window_callback(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

static int
kbdaemon_exec_fn(wbk_b_t *b);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	b3_win_factory_t *win_factory;
	b3_ws_factory_t *ws_factory;
	b3_wsman_factory_t *wsman_factory;
	b3_monitor_factory_t *monitor_factory;
	b3_kc_director_factory_t *kc_director_factory;
	b3_parser_t *parser;
	b3_win_watcher_t *win_watcher;
	wbk_b_t *b; // TODO remove me
	wbk_be_t *be; // TODO remove me
	b3_kc_director_t *kc_director; // TODO remove me
	wbk_kbdaemon_t *kbdaemon;

	wbk_logger_set_level(SEVERE);
#ifdef DEBUG_ENABLED
	wbk_logger_set_level(DEBUG);
#else
	wbk_logger_set_level(INFO);
#endif

	win_factory = b3_win_factory_new();
	ws_factory = b3_ws_factory_new();
	wsman_factory = b3_wsman_factory_new(ws_factory);
	monitor_factory = b3_monitor_factory_new(wsman_factory);
	kc_director_factory = b3_kc_director_factory_new();

	parser = b3_parser_new(kc_director_factory, "");

	g_director = b3_director_new(monitor_factory);
	/**
	 * Setup key bindings
	 */
	if (g_director) {
		g_kbman = b3_kbman_new(); // TODO move me into the parser

		// TODO remove begin
		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '1'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "1");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '2'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "2");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '3'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "3");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '4'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "4");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '5'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "5");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '6'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "6");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '7'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "7");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '8'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "8");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '9'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "9");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '1'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "1");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '2'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "2");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '3'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "3");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '4'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "4");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '5'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "5");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '6'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "6");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '7'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "7");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '8'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "8");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '9'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "9");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '1'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cm(kc_director_factory, b, g_director, "\\\\.\\DISPLAY1");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, '2'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cm(kc_director_factory, b, g_director, "\\\\.\\DISPLAY2");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SPACE, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_awtf(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'f'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_tawf(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'h'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawl(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'j'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawd(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'k'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawu(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'l'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'o'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'h'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sawl(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'j'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sawd(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'k'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sawu(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'l'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sawr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'o'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sawr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'q'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_caw(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'h'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwl(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'j'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwd(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'k'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwu(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'l'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'o'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);
		// TODO Remove end
	}

	/**
	 * Setup window watcher
	 */
	if (g_director) {
		win_watcher = b3_win_watcher_new(win_factory, g_director);
	}

	/**
	 * Setup keyboard daemon
	 */
	if (g_director) {
		kbdaemon = wbk_kbdaemon_new(kbdaemon_exec_fn);
	}

	/**
	 * Start main loops
	 */
	if (g_director && kbdaemon && g_kbman && win_watcher) {
		b3_director_refresh(g_director);

		b3_win_watcher_start(win_watcher);
		wbk_kbdaemon_start(kbdaemon);

		main_loop(hInstance, nCmdShow);

		wbk_kbdaemon_stop(kbdaemon);
		b3_win_watcher_stop(win_watcher);
	}

	if (kbdaemon) {
		wbk_kbdaemon_free(kbdaemon);
	}

	if (g_director) {
		b3_director_free(g_director);
	}

	if (g_kbman) {
		b3_kbman_free(g_kbman);
	}

	if (win_watcher) {
		b3_win_watcher_free(win_watcher);
	}

	b3_win_factory_free(win_factory);
	b3_ws_factory_free(ws_factory);
	b3_wsman_factory_free(wsman_factory);
	b3_monitor_factory_free(monitor_factory);
	b3_kc_director_factory_free(kc_director_factory);

	return 0;
}

int
main_loop(HINSTANCE hInstance, int nCmdShow)
{
	MSG Msg;

	b3_director_show(g_director);
	b3_director_switch_to_ws(g_director,
						     b3_ws_get_name(b3_monitor_get_focused_ws(b3_director_get_focused_monitor(g_director))));

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

int
kbdaemon_exec_fn(wbk_b_t *b)
{
	return b3_kbman_exec(g_kbman, b);
}
