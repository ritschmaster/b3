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

#define B3_KBDAEMON_ARR_LEN 3

static wbk_logger_t logger = { "main" };

static const char g_szClassName[] = "myWindowClass";

static b3_director_t *g_director;

static wbk_kbdaemon_t **g_kbdaemon_arr = NULL;
static b3_kbman_t **g_kbman_arr = NULL;

static int
main_loop(HINSTANCE hInstance, int nCmdShow);

static LRESULT CALLBACK
window_callback(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

static int
kbdaemon_exec_fn(wbk_kbdaemon_t *kbdaemon, wbk_b_t *b);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int error;
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
	b3_kbman_t *g_kbman;
	int i;

	error = 0;

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

	parser = b3_parser_new(kc_director_factory);

	g_director = b3_director_new(monitor_factory);
	/**
	 * Setup key bindings
	 */
	if (g_director) {
		g_kbman = b3_kbman_new(); // TODO move me into the parser

		// TODO remove begin (Start of hardcoded keybindings)
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
		be = wbk_be_new(F1, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "11");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F2, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "12");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F3, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "13");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F4, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "14");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F5, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "15");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F6, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "16");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F7, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "17");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F8, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "18");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F9, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_cw(kc_director_factory, b, g_director, "19");
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
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F1, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "11");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F2, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "12");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F3, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "13");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F4, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "14");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F5, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "15");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F6, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "16");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F7, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "17");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F8, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "18");
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(F9, 0); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mawtw(kc_director_factory, b, g_director, "19");
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
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(ALT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'h'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwl(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(ALT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'j'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwd(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(ALT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'k'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwu(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(ALT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'l'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(ALT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'o'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'h'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sfml(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'j'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sfmd(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'k'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sfmu(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'l'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sfmr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'o'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_sfmr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'h'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwtml(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'j'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwtmd(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'k'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwtmu(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'l'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwtmr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		b = wbk_b_new();
		be = wbk_be_new(WIN, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(CTRL, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(SHIFT, 0); wbk_b_add(b, be); wbk_be_free(be);
		be = wbk_be_new(NOT_A_MODIFIER, 'o'); wbk_b_add(b, be); wbk_be_free(be);
		kc_director = b3_kc_director_factory_create_mfwtmr(kc_director_factory, b, g_director);
		b3_kbman_add_kc_director(g_kbman, kc_director);

		// TODO Remove end (End of hardcoded keybindings)

		if (g_kbman) {
			g_kbman_arr = b3_kbman_split(g_kbman, B3_KBDAEMON_ARR_LEN);

			b3_kbman_free(g_kbman);
			g_kbman = NULL;
		} else {
			error = 1;
		}
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
		main_loop(hInstance, nCmdShow);

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
				b3_kbman_free(g_kbman_arr[i]);
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

	b3_win_factory_free(win_factory);
	b3_ws_factory_free(ws_factory);
	b3_wsman_factory_free(wsman_factory);
	b3_monitor_factory_free(monitor_factory);
	b3_kc_director_factory_free(kc_director_factory);

	return error;
}

int
main_loop(HINSTANCE hInstance, int nCmdShow)
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
	if (kbdaemon == g_kbdaemon_arr[0])
		return b3_kbman_exec(g_kbman_arr[0], b);
	else if (kbdaemon == g_kbdaemon_arr[1])
		return b3_kbman_exec(g_kbman_arr[1], b);
	else if (kbdaemon == g_kbdaemon_arr[2])
		return b3_kbman_exec(g_kbman_arr[2], b);
	else
		return 1;
}
