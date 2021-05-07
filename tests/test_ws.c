/******************************************************************************
  This file is part of b3.

  Copyright 2020-2021 Richard Paul Baeck <richard.baeck@mailbox.org>

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
 * @date 2021-03-31
 * @brief File contains the tests for the ws class
 */

#include "../src/ws.h"

#include "test.h"

#include <stdio.h>
#include <w32bindkeys/logger.h>

#define ARR_LEN 10

static wbk_logger_t logger = { "test_ws" };

static int g_winman_arr_i;
static char g_winman_arr[ARR_LEN];

static void
dump_winman_arr(int error, char *act, char *exp, int len)
{
	int i;

	if (error) {
		fprintf(stdout, "WINMAN_ARR:\n");
		for (i = 0; i < len; i++) {
			fprintf(stdout, "act: %c\n", act[i]);
			fprintf(stdout, "exp: %c\n", exp[i]);
			fprintf(stdout, "\n");
		}
	}
}

static void
dump_win_arr(int error, b3_win_t **act, b3_win_t **exp, int len)
{
	int i;

	if (error) {
		fprintf(stdout, "WIN_ARR:\n");
		for (i = 0; i < len; i++) {
			fprintf(stdout, "act: %p\n", (void *) act[i]);
			fprintf(stdout, "exp: %p\n", (void *) exp[i]);
			fprintf(stdout, "\n");
		}
	}
}

static void
winman_visitor(b3_winman_t *winman, void *data) {
	if (b3_winman_get_win(winman) == NULL) {
		g_winman_arr[g_winman_arr_i] = 'I';
	} else  {
		g_winman_arr[g_winman_arr_i] = 'L';
	}

	g_winman_arr_i++;
	if (g_winman_arr_i >= ARR_LEN) {
		g_winman_arr_i = 0;
	}
}

static int g_win_arr_i;
static b3_win_t *g_win_arr[ARR_LEN];

static void
win_visitor(b3_winman_t *winman, void *data) {
	if (b3_winman_get_win(winman)) {
		g_win_arr[g_win_arr_i] = b3_winman_get_win(winman);

		g_win_arr_i++;
		if (g_win_arr_i >= ARR_LEN) {
			g_win_arr_i = 0;
		}
	}
}

static int
check_winman_arr(b3_winman_t *winman, char *winman_arr_exp)
{
	int error;
	char winman_arr_exp_cpy[ARR_LEN];

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(winman, winman_visitor, NULL);
	memset(winman_arr_exp_cpy, 0, ARR_LEN);
	strncpy(winman_arr_exp_cpy, winman_arr_exp, ARR_LEN);
	error = strcmp(g_winman_arr, winman_arr_exp_cpy);
	dump_winman_arr(error, g_winman_arr, winman_arr_exp_cpy, ARR_LEN);

	return error;
}

static int
check_win_arr(b3_winman_t *winman, b3_win_t **win_arr_exp)
{
	int error;

	memset(g_win_arr, 0, ARR_LEN);
	g_win_arr_i = 0;
	b3_winman_traverse(winman, win_visitor, NULL);
	error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
	dump_win_arr(error, g_win_arr, win_arr_exp, ARR_LEN);

	return error;
}


static void
setup(void)
{
}

static void
teardown(void)
{
}

static int
test_set_name(void)
{
	int error;
	b3_ws_t *ws;

	ws = b3_ws_new("test");

	if (!error) {
		error = strcmp(b3_ws_get_name(ws), "test");
	}

	if (!error) {
		b3_ws_set_name(ws, "Hello world");
		error = strcmp(b3_ws_get_name(ws), "Hello world");
	}

	b3_ws_free(ws);

	return error;
}

static int
test_simple_tree(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");
	b3_ws_add_win(ws, win1);
	b3_ws_add_win(ws, win2);
	b3_ws_add_win(ws, win3);
	b3_ws_add_win(ws, win4);

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);

	error = strcmp(g_winman_arr, "ILLLL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win2;
		win_arr_exp[2] = win3;
		win_arr_exp[3] = win4;

		memset(g_win_arr, 0, ARR_LEN * sizeof(b3_win_t *));
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);

		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_set_focused_win(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);

	ws = b3_ws_new("test");
	b3_ws_add_win(ws, win1);
	b3_ws_add_win(ws, win2);
	error = b3_ws_set_focused_win(ws, win1);

	if (!error) {
		error = b3_ws_add_win(ws, win3);
	}

	if (!error) {
		memset(g_winman_arr, 0, ARR_LEN);
		g_winman_arr_i = 0;
		b3_winman_traverse(ws->winman, winman_visitor, NULL);

		error = strcmp(g_winman_arr, "ILLL");
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);

	return error;
}

static int
test_vsplit(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");
	b3_ws_add_win(ws, win1);
	b3_ws_split(ws, VERTICAL);
	b3_ws_add_win(ws, win2);
	b3_ws_add_win(ws, win3);
	b3_ws_set_focused_win(ws, win1);
	b3_ws_add_win(ws, win4);

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);

	error = strcmp(g_winman_arr, "IILLLL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN);
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win2;
		win_arr_exp[2] = win3;
		win_arr_exp[3] = win4;

		memset(g_win_arr, 0, ARR_LEN);
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);
		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_hsplit(void)
{
int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");
	b3_ws_add_win(ws, win1);
	b3_ws_split(ws, HORIZONTAL);
	b3_ws_add_win(ws, win2);
	b3_ws_add_win(ws, win3);
	b3_ws_set_focused_win(ws, win1);
	b3_ws_add_win(ws, win4);

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);

	error = strcmp(g_winman_arr, "IILLLL");

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_complex_split(void)
{
int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** I */
	b3_ws_add_win(ws, win1);          /** IL */
	b3_ws_split(ws, VERTICAL);        /** IIL */
	b3_ws_add_win(ws, win2);          /** IILL*/
	b3_ws_split(ws, HORIZONTAL);      /** IILIL */
	b3_ws_add_win(ws, win3);          /** IILILL */

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);
	error = strcmp(g_winman_arr, "IILILL");

	if (!error) {
		b3_ws_set_focused_win(ws, win1);
		b3_ws_split(ws, HORIZONTAL);      /** IIILILL*/
		b3_ws_add_win(ws, win4);          /** IIILLILL*/

		memset(g_winman_arr, 0, ARR_LEN);
		g_winman_arr_i = 0;
		b3_winman_traverse(ws->winman, winman_visitor, NULL);
		error = strcmp(g_winman_arr, "IIILLILL");
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_simple_remove_win(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");
	b3_ws_add_win(ws, win1);
	b3_ws_add_win(ws, win2);
	b3_ws_add_win(ws, win3);
	b3_ws_add_win(ws, win4);

	b3_ws_remove_win(ws, win4);

	if (!error) {
		memset(g_winman_arr, 0, ARR_LEN);
		g_winman_arr_i = 0;
		b3_winman_traverse(ws->winman, winman_visitor, NULL);

		error = strcmp(g_winman_arr, "ILLL");
	}

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win2;
		win_arr_exp[2] = win3;

		memset(g_win_arr, 0, ARR_LEN * sizeof(b3_win_t *));
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);

		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
	}

	if (!error) {
		error = b3_ws_get_focused_win(ws) - win3;
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);

	return error;
}

static int
test_remove_win_after_changed_focus(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");
	b3_ws_add_win(ws, win1);
	b3_ws_add_win(ws, win2);
	b3_ws_set_focused_win(ws, win2);
	b3_ws_add_win(ws, win3);
	b3_ws_set_focused_win(ws, win3);
	b3_ws_add_win(ws, win4);
	b3_ws_set_focused_win(ws, win4);

	b3_ws_set_focused_win(ws, win2);
	b3_ws_remove_win(ws, win2);

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);

	error = strcmp(g_winman_arr, "ILLL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win4;

		memset(g_win_arr, 0, ARR_LEN * sizeof(b3_win_t *));
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);

		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
	}

	if (!error) {
		error = b3_ws_get_focused_win(ws) - win4;
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);

	return error;
}

static int
test_complex_remove_win(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** I */
	b3_ws_add_win(ws, win1);          /** IL */
	b3_ws_set_focused_win(ws, win1);
	b3_ws_split(ws, VERTICAL);        /** IIL */
	b3_ws_add_win(ws, win2);          /** IILL*/
	b3_ws_set_focused_win(ws, win2);
	b3_ws_split(ws, HORIZONTAL);      /** IILIL */
	b3_ws_add_win(ws, win3);          /** IILILL */
	b3_ws_set_focused_win(ws, win3);

	b3_ws_set_focused_win(ws, win1);
	b3_ws_remove_win(ws, win1);

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);
	error = strcmp(g_winman_arr, "IIILL");

	if (!error) {
		error = b3_ws_get_focused_win(ws) - win3;
	}

	if (!error) {
		/** win3 is focused */
		b3_ws_split(ws, HORIZONTAL);      /** IIILIL */
		b3_ws_add_win(ws, win4);          /** IIILILL */

		memset(g_winman_arr, 0, ARR_LEN);
		g_winman_arr_i = 0;
		b3_winman_traverse(ws->winman, winman_visitor, NULL);
		error = strcmp(g_winman_arr, "IIILILL");
	}

	b3_ws_free(ws);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_remove_win_all(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	char winman_arr_exp[ARR_LEN];
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** I */
	b3_ws_add_win(ws, win1);          /** IL */
	b3_ws_add_win(ws, win2);          /** ILL*/
	b3_ws_add_win(ws, win3);          /** ILLL */
	b3_ws_add_win(ws, win4);          /** ILLLL */
	b3_ws_remove_win(ws, win1);       /** ILLL */
	b3_ws_remove_win(ws, win2);       /** ILL */
	b3_ws_remove_win(ws, win3);       /** IL */
	b3_ws_remove_win(ws, win4);       /** I */

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);
	strcpy(winman_arr_exp, "I");
	error = strcmp(g_winman_arr, winman_arr_exp);
	dump_winman_arr(error, g_winman_arr, winman_arr_exp, ARR_LEN);

	if (!error) {
		error = b3_ws_get_focused_win(ws) == NULL ? 0 : 1;
		if (error) wbk_logger_log(&logger, SEVERE, "Expected focused window to be null\n");
	}

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN);

		memset(g_win_arr, 0, ARR_LEN);
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);
		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
		dump_win_arr(error, g_win_arr, win_arr_exp, ARR_LEN);
	}

	b3_ws_free(ws);

	return error;
}

static int
test_floating_simple(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	char winman_arr_exp[ARR_LEN];
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");              /** I */
	b3_ws_add_win(ws, win1);             /** IL */
	b3_ws_add_win(ws, win2);             /** ILL*/
	b3_ws_add_win(ws, win3);             /** ILLL */
	b3_ws_add_win(ws, win4);             /** ILLLL */
	b3_ws_toggle_floating_win(ws, win2); /** ILLL */

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);
	memset(winman_arr_exp, 0, ARR_LEN);
	strcpy(winman_arr_exp, "ILLL");
	error = strcmp(g_winman_arr, winman_arr_exp);
	dump_winman_arr(error, g_winman_arr, winman_arr_exp, ARR_LEN);

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN);
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win4;

		memset(g_win_arr, 0, ARR_LEN);
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);
		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
		dump_win_arr(error, g_win_arr, win_arr_exp, ARR_LEN);
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_floating_complex(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	char winman_arr_exp[ARR_LEN];
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");              /** I */
	b3_ws_add_win(ws, win1);             /** IL */
	b3_ws_add_win(ws, win2);             /** ILL*/
	b3_ws_add_win(ws, win3);             /** ILLL */
	b3_ws_add_win(ws, win4);             /** ILLLL */
	b3_ws_toggle_floating_win(ws, win2); /** ILLL */
	b3_ws_toggle_floating_win(ws, win4); /** ILL */
	b3_ws_toggle_floating_win(ws, win2); /** ILLL */

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);
	memset(winman_arr_exp, 0, ARR_LEN);
	strcpy(winman_arr_exp, "ILLL");
	error = strcmp(g_winman_arr, winman_arr_exp);
	dump_winman_arr(error, g_winman_arr, winman_arr_exp, ARR_LEN);

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN);
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win2;

		memset(g_win_arr, 0, ARR_LEN);
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);
		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
		dump_win_arr(error, g_win_arr, win_arr_exp, ARR_LEN);
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_simple_win_rel(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	b3_win_t *found;
	char winman_arr_exp[ARR_LEN];
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** I */
	b3_ws_add_win(ws, win1);          /** IL */
	b3_ws_add_win(ws, win2);          /** ILL*/
	b3_ws_add_win(ws, win3);          /** ILLL */
	b3_ws_add_win(ws, win4);          /** ILLLL */
	b3_ws_set_focused_win(ws, win2);

	error = 0;

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, LEFT, 0);
		error = b3_test_check_void(found, win1, "Failed LEFT");
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win1);
		found = b3_ws_get_win_rel_to_focused_win(ws, LEFT, 1);
		error = b3_test_check_void(found, win4, "Failed rolling LEFT");
		b3_ws_set_focused_win(ws, win2);
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, RIGHT, 0);
		error = b3_test_check_void(found, win3, "Failed RIGHT");
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win4);
		found = b3_ws_get_win_rel_to_focused_win(ws, RIGHT, 1);
		error = b3_test_check_void(found, win1, "Failed rolling RIGHT");
		b3_ws_set_focused_win(ws, win2);
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, DOWN, 0);
		error = b3_test_check_void(found, NULL, "Failed DOWN");
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, DOWN, 1);
		error = b3_test_check_void(found, NULL, "Failed rolling DOWN");
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, UP, 0);
		error = b3_test_check_void(found, NULL, "Failed UP");
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, UP, 1);
		error = b3_test_check_void(found, NULL, "Failed rolling UP");
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_simple_move(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	char winman_arr_exp[ARR_LEN];
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** I */
	b3_ws_add_win(ws, win1);          /** IL */
	b3_ws_add_win(ws, win2);          /** ILL*/
	b3_ws_add_win(ws, win3);          /** ILLL */
	b3_ws_add_win(ws, win4);          /** ILLLL */
	b3_ws_set_focused_win(ws, win2);
	b3_ws_move_focused_win(ws, DOWN); /** ILLLIL */

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);
	memset(winman_arr_exp, 0, ARR_LEN);
	strcpy(winman_arr_exp, "ILLLIL");
	error = strcmp(g_winman_arr, winman_arr_exp);
	dump_winman_arr(error, g_winman_arr, winman_arr_exp, ARR_LEN);

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN);
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win4;
		win_arr_exp[3] = win2;

		memset(g_win_arr, 0, ARR_LEN);
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);
		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
		dump_win_arr(error, g_win_arr, win_arr_exp, ARR_LEN);
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_complex_move(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	char winman_arr_exp[ARR_LEN];
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** I */
	b3_ws_add_win(ws, win1);          /** IL */
	b3_ws_add_win(ws, win2);          /** ILL*/
	b3_ws_add_win(ws, win3);          /** ILLL */
	b3_ws_add_win(ws, win4);          /** ILLLL */
	b3_ws_set_focused_win(ws, win2);
	b3_ws_move_focused_win(ws, DOWN); /** ILLLIL */
	b3_ws_move_focused_win(ws, UP);   /** ILLLL */

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);
	memset(winman_arr_exp, 0, ARR_LEN);
	strcpy(winman_arr_exp, "ILLLL");
	error = strcmp(g_winman_arr, winman_arr_exp);
	dump_winman_arr(error, g_winman_arr, winman_arr_exp, ARR_LEN);

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN);
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win4;
		win_arr_exp[3] = win2;

		memset(g_win_arr, 0, ARR_LEN);
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);
		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
		dump_win_arr(error, g_win_arr, win_arr_exp, ARR_LEN);
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_simple_arrange(void)
{
	int error;
	b3_ws_t *ws;
	RECT monitor_area;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	char winman_arr_exp[ARR_LEN];
	b3_win_t *win_arr_exp[ARR_LEN];

	monitor_area.top = 0;
	monitor_area.bottom = 1080;
	monitor_area.left = 0;
	monitor_area.right = 1920;

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** I */
	b3_ws_add_win(ws, win1);          /** IL */
	b3_ws_add_win(ws, win2);          /** ILL*/
	b3_ws_add_win(ws, win3);          /** ILLL */
	b3_ws_add_win(ws, win4);          /** ILLLL */
	b3_ws_set_focused_win(ws, win2);

	b3_ws_arrange_wins(ws, monitor_area);

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);
	memset(winman_arr_exp, 0, ARR_LEN);
	strcpy(winman_arr_exp, "ILLLL");
	error = strcmp(g_winman_arr, winman_arr_exp);
	dump_winman_arr(error, g_winman_arr, winman_arr_exp, ARR_LEN);

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN);
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win2;
		win_arr_exp[2] = win3;
		win_arr_exp[3] = win4;

		memset(g_win_arr, 0, ARR_LEN);
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);
		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
		dump_win_arr(error, g_win_arr, win_arr_exp, ARR_LEN);
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_complex_arrange(void)
{
	int error;
	b3_ws_t *ws;
	RECT monitor_area;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	char winman_arr_exp[ARR_LEN];
	b3_win_t *win_arr_exp[ARR_LEN];

	monitor_area.top = 0;
	monitor_area.bottom = 1080;
	monitor_area.left = 0;
	monitor_area.right = 1920;

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");              /** I */
	b3_ws_add_win(ws, win1);             /** IL */
	b3_ws_add_win(ws, win2);             /** ILL*/
	b3_ws_add_win(ws, win3);             /** ILLL */
	b3_ws_add_win(ws, win4);             /** ILLLL */
	b3_ws_set_focused_win(ws, win2);
	b3_ws_toggle_floating_win(ws, win4); /** ILLL */
	b3_ws_move_focused_win(ws, DOWN);    /** ILLIL */

	b3_ws_arrange_wins(ws, monitor_area);

	memset(g_winman_arr, 0, ARR_LEN);
	g_winman_arr_i = 0;
	b3_winman_traverse(ws->winman, winman_visitor, NULL);
	memset(winman_arr_exp, 0, ARR_LEN);
	strcpy(winman_arr_exp, "ILLIL");
	error = strcmp(g_winman_arr, winman_arr_exp);
	dump_winman_arr(error, g_winman_arr, winman_arr_exp, ARR_LEN);

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN);
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win2;

		memset(g_win_arr, 0, ARR_LEN);
		g_win_arr_i = 0;
		b3_winman_traverse(ws->winman, win_visitor, NULL);
		error = memcmp(g_win_arr, win_arr_exp, ARR_LEN);
		dump_win_arr(error, g_win_arr, win_arr_exp, ARR_LEN);
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

int
main(void)
{
	b3_test(setup, teardown, test_set_name, "test_set_name");
	b3_test(setup, teardown, test_simple_tree, "test_simple_tree");
	b3_test(setup, teardown, test_set_focused_win, "test_set_focused_win");
	b3_test(setup, teardown, test_vsplit, "test_vsplit");
	b3_test(setup, teardown, test_hsplit, "test_hsplit");
	b3_test(setup, teardown, test_complex_split, "test_complex_split");
	b3_test(setup, teardown, test_simple_remove_win, "test_simple_remove_win");
	b3_test(setup, teardown, test_remove_win_after_changed_focus, "test_remove_win_after_changed_focus");
	b3_test(setup, teardown, test_complex_remove_win, "test_complex_remove_win");
	b3_test(setup, teardown, test_remove_win_all, "test_remove_win_all");
	b3_test(setup, teardown, test_floating_simple, "test_floating_simple");
	b3_test(setup, teardown, test_floating_complex, "test_floating_complex");
	b3_test(setup, teardown, test_simple_win_rel, "test_simple_win_rel");
	b3_test(setup, teardown, test_simple_move, "test_simple_move");
	//b3_test(setup, teardown, test_complex_move, "test_complex_move");
	//b3_test(setup, teardown, test_simple_arrange, "test_simple_arrange");
	//b3_test(setup, teardown, test_complex_arrange, "test_complex_arrange");

	return 0;
}
