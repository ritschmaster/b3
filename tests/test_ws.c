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
	if (error) {
		fprintf(stdout, "WINMAN_ARR:\n");
		fprintf(stdout, "exp: %s\n", exp);
		fprintf(stdout, "act: %s\n", act);
		fprintf(stdout, "\n");
	}
}

static void
dump_win_arr(int error, b3_win_t **act, b3_win_t **exp, int len)
{
	int i;

	if (error) {
		fprintf(stdout, "WIN_ARR:\n");
		for (i = 0; i < len; i++) {
			fprintf(stdout, "exp: %p\n", (void *) exp[i]);
			fprintf(stdout, "act: %p\n", (void *) act[i]);
			fprintf(stdout, "\n");
		}
	}
}

static void
winman_visitor(b3_winman_t *winman, void *data) {
	if (b3_winman_get_win(winman) == NULL) {
		if (b3_winman_get_mode(winman) == HORIZONTAL) {
			g_winman_arr[g_winman_arr_i] = 'H';
		} else if (b3_winman_get_mode(winman) == VERTICAL) {
			g_winman_arr[g_winman_arr_i] = 'V';
		}
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

	memset(g_win_arr, 0, ARR_LEN * sizeof(b3_win_t *));
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

	error = 0;

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

	error = check_winman_arr(ws->winman, "HLLLL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win2;
		win_arr_exp[2] = win3;
		win_arr_exp[3] = win4;

		error = check_win_arr(ws->winman, win_arr_exp);
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
		error = check_winman_arr(ws->winman, "HLLL");
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

	error = check_winman_arr(ws->winman, "HVLLLL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win2;
		win_arr_exp[2] = win3;
		win_arr_exp[3] = win4;

		error = check_win_arr(ws->winman, win_arr_exp);
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

	error = check_winman_arr(ws->winman, "HHLLLL");

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_complex_split_1(void)
{
int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	//b3_win_t *win_arr_exp[ARR_LEN]; //TODO

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** H */
	b3_ws_add_win(ws, win1);          /** HL */
	b3_ws_split(ws, VERTICAL);        /** HVL */
	b3_ws_add_win(ws, win2);          /** HVLL*/
	b3_ws_split(ws, HORIZONTAL);      /** HVLHL */
	b3_ws_add_win(ws, win3);          /** HVLHLL */

	error = check_winman_arr(ws->winman, "HVLHLL");

	if (!error) {
		b3_ws_set_focused_win(ws, win1);
		b3_ws_split(ws, HORIZONTAL);      /** HVHLHLL*/
		b3_ws_add_win(ws, win4);          /** HVHLLHLL*/

		error = check_winman_arr(ws->winman, "HVHLLHLL");
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_complex_split_2(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);

	ws = b3_ws_new("test");           /** H */
	b3_ws_add_win(ws, win1);          /** HL */
	b3_ws_add_win(ws, win2);          /** HLL*/
	b3_ws_set_focused_win(ws, win1);
	b3_ws_split(ws, HORIZONTAL);      /** HHLL */

	error = 0;

	if (!error) {
		error = check_winman_arr(ws->winman, "HLHL");

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win2;
			win_arr_exp[1] = win1;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "Failed splitting horizontally.\n");
		}
	}

	if (!error) {
		b3_ws_add_win(ws, win3);          /** HLHLL */

		error = check_winman_arr(ws->winman, "HLHLL");

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win2;
			win_arr_exp[1] = win1;
			win_arr_exp[2] = win3;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "Failed adding 'win3'.\n");
		}
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win2);
		if (b3_ws_get_win_rel_to_focused_win(ws, RIGHT, 0) != win3) {
			error = 1;
			wbk_logger_log(&logger, SEVERE, "Failed getting RIGHT of win2.\n");
		}
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win1);
		if (b3_ws_get_win_rel_to_focused_win(ws, LEFT, 0) != win2) {
			error = 1;
			wbk_logger_log(&logger, SEVERE, "Failed getting LEFT of win1.\n");
		}
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);

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

	error = 0;

	if (!error) {
		error = check_winman_arr(ws->winman, "HLLL");
	}

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win2;
		win_arr_exp[2] = win3;

		error = check_win_arr(ws->winman, win_arr_exp);
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

	error = check_winman_arr(ws->winman, "HLLL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win4;

		error = check_win_arr(ws->winman, win_arr_exp);
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
	//b3_win_t *win_arr_exp[ARR_LEN]; // TODO

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** H */
	b3_ws_add_win(ws, win1);          /** HL */
	b3_ws_set_focused_win(ws, win1);
	b3_ws_split(ws, VERTICAL);        /** HVL */
	b3_ws_add_win(ws, win2);          /** HVLL*/
	b3_ws_set_focused_win(ws, win2);
	b3_ws_split(ws, HORIZONTAL);      /** HVLHL */
	b3_ws_add_win(ws, win3);          /** HVLHLL */
	b3_ws_set_focused_win(ws, win3);

	b3_ws_set_focused_win(ws, win1);
	b3_ws_remove_win(ws, win1);

	error = check_winman_arr(ws->winman, "HVHLL");

	if (!error) {
		error = b3_ws_get_focused_win(ws) - win3;
	}

	if (!error) {
		/** win3 is focused */
		b3_ws_split(ws, HORIZONTAL);      /** HVHLHL */
		b3_ws_add_win(ws, win4);          /** HVHLHLL */

		error = check_winman_arr(ws->winman, "HVHLHLL");
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
	//char winman_arr_exp[ARR_LEN]; // TODO
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** H */
	b3_ws_add_win(ws, win1);          /** HL */
	b3_ws_add_win(ws, win2);          /** HLL*/
	b3_ws_add_win(ws, win3);          /** HLLL */
	b3_ws_add_win(ws, win4);          /** HLLLL */
	b3_ws_remove_win(ws, win1);       /** HLLL */
	b3_ws_remove_win(ws, win2);       /** HLL */
	b3_ws_remove_win(ws, win3);       /** HL */
	b3_ws_remove_win(ws, win4);       /** H */

	error = check_winman_arr(ws->winman, "H");

	if (!error) {
		error = b3_ws_get_focused_win(ws) == NULL ? 0 : 1;
		if (error) wbk_logger_log(&logger, SEVERE, "Expected focused window to be null\n");
	}

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));

		error = check_win_arr(ws->winman, win_arr_exp);
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
	//char winman_arr_exp[ARR_LEN]; // TODO
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");              /** H */
	b3_ws_add_win(ws, win1);             /** HL */
	b3_ws_add_win(ws, win2);             /** HLL*/
	b3_ws_add_win(ws, win3);             /** HLLL */
	b3_ws_add_win(ws, win4);             /** HLLLL */
	b3_ws_toggle_floating_win(ws, win2); /** HLLL */

	error = check_winman_arr(ws->winman, "HLLL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win4;

		error = check_win_arr(ws->winman, win_arr_exp);
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
	//char winman_arr_exp[ARR_LEN]; // TODO
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");              /** H */
	b3_ws_add_win(ws, win1);             /** HL */
	b3_ws_add_win(ws, win2);             /** HLL*/
	b3_ws_add_win(ws, win3);             /** HLLL */
	b3_ws_add_win(ws, win4);             /** HLLLL */
	b3_ws_toggle_floating_win(ws, win2); /** HLLL */
	b3_ws_toggle_floating_win(ws, win4); /** HLL */
	b3_ws_toggle_floating_win(ws, win2); /** HLLL */

	error = check_winman_arr(ws->winman, "HLLL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win2;

		error = check_win_arr(ws->winman, win_arr_exp);
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

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** H */
	b3_ws_add_win(ws, win1);          /** HL */
	b3_ws_add_win(ws, win2);          /** HLL*/
	b3_ws_add_win(ws, win3);          /** HLLL */
	b3_ws_add_win(ws, win4);          /** HLLLL */
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
	b3_win_t *win_arr_exp[ARR_LEN];

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** H */
	b3_ws_add_win(ws, win1);          /** HL */
	b3_ws_add_win(ws, win2);          /** HLL*/
	b3_ws_add_win(ws, win3);          /** HLLL */
	b3_ws_add_win(ws, win4);          /** HLLLL */
	b3_ws_set_focused_win(ws, win2);
	b3_ws_move_focused_win(ws, DOWN); /** VHLLLL */

	error = 0;

	if (!error) {
		error = check_winman_arr(ws->winman, "VHLLLL");

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win1;
			win_arr_exp[1] = win3;
			win_arr_exp[2] = win4;
			win_arr_exp[3] = win2;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "UP failed\n");
		}
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win3);
		b3_ws_move_focused_win(ws, LEFT); /** VHLLLL */

		error = check_winman_arr(ws->winman, "VHLLLL");

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win3;
			win_arr_exp[1] = win1;
			win_arr_exp[2] = win4;
			win_arr_exp[3] = win2;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "LEFT failed\n");
		}
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_complex_win_rel(void)
{
	int error;
	b3_ws_t *ws;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_win_t *win4;
	b3_win_t *found;

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** H */
	b3_ws_add_win(ws, win1);          /** HL */
	b3_ws_add_win(ws, win2);          /** HLL*/
	b3_ws_add_win(ws, win3);          /** HLLL */
	b3_ws_add_win(ws, win4);          /** HLLLL */
	b3_ws_set_focused_win(ws, win2);
	b3_ws_move_focused_win(ws, DOWN); /** VHLLLL */

	b3_ws_set_focused_win(ws, win3);

	/**
	 * This test shares the same base as test_simple_move()!
	 */
	error = test_simple_move();

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, LEFT, 0);
		error = b3_test_check_void(found, win1, "Failed LEFT");
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win1);
		found = b3_ws_get_win_rel_to_focused_win(ws, LEFT, 0);
		error = b3_test_check_void(found, NULL, "Failed LEFT");
		b3_ws_set_focused_win(ws, win3);
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win1);
		found = b3_ws_get_win_rel_to_focused_win(ws, LEFT, 1);
		error = b3_test_check_void(found, win4, "Failed rolling LEFT");
		b3_ws_set_focused_win(ws, win3);
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, RIGHT, 0);
		error = b3_test_check_void(found, win4, "Failed RIGHT");
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win4);
		found = b3_ws_get_win_rel_to_focused_win(ws, RIGHT, 0);
		error = b3_test_check_void(found, NULL, "Failed RIGHT");
		b3_ws_set_focused_win(ws, win3);
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win4);
		found = b3_ws_get_win_rel_to_focused_win(ws, RIGHT, 1);
		error = b3_test_check_void(found, win1, "Failed rolling RIGHT");
		b3_ws_set_focused_win(ws, win3);
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, DOWN, 0);
		error = b3_test_check_void(found, win2, "Failed DOWN");
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win2);
		found = b3_ws_get_win_rel_to_focused_win(ws, DOWN, 0);
		error = b3_test_check_void(found, NULL, "Failed DOWN");
		b3_ws_set_focused_win(ws, win3);
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, DOWN, 1);
		error = b3_test_check_void(found, win2, "Failed rolling DOWN");
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win2);
		found = b3_ws_get_win_rel_to_focused_win(ws, UP, 0);
		error = b3_test_check_void(found, win3, "Failed UP");
		b3_ws_set_focused_win(ws, win3);
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, UP, 0);
		error = b3_test_check_void(found, NULL, "Failed UP");
	}

	if (!error) {
		found = b3_ws_get_win_rel_to_focused_win(ws, UP, 1);
		error = b3_test_check_void(found, win2, "Failed rolling UP");
	}

	b3_ws_free(ws);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);
	b3_win_free(win4);

	return error;
}

static int
test_complex_move_1(void)
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

	ws = b3_ws_new("test");           /** H */
	b3_ws_add_win(ws, win1);          /** HL */
	b3_ws_add_win(ws, win2);          /** HLL*/
	b3_ws_add_win(ws, win3);          /** HLLL */
	b3_ws_add_win(ws, win4);          /** HLLLL */
	b3_ws_set_focused_win(ws, win2);
	b3_ws_move_focused_win(ws, DOWN); /** VLLLHL */

	error = 0;

	if (!error) {
		b3_ws_move_focused_win(ws, UP);   /** VHLLLL */
		error = check_winman_arr(ws->winman, "VHLLLL");

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win1;
			win_arr_exp[1] = win3;
			win_arr_exp[2] = win4;
			win_arr_exp[3] = win2;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "Failed UP\n");
		}
	}


	if (!error) {
		b3_ws_move_focused_win(ws, UP);   /** VHLLLL */
		error = check_winman_arr(ws->winman, "VLHLLL");

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win2;
			win_arr_exp[1] = win1;
			win_arr_exp[2] = win3;
			win_arr_exp[3] = win4;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "Failed UP*2\n");
		}
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win2);
		if (b3_ws_get_win_rel_to_focused_win(ws, DOWN, 0) != win4) {
			wbk_logger_log(&logger, SEVERE, "Failed retrieving down of 'win2'\n");
			error = 1;
		}
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win4);
		b3_ws_move_focused_win(ws, LEFT);
		b3_ws_move_focused_win(ws, LEFT);

		error = check_winman_arr(ws->winman, "VLHLLL");

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win2;
			win_arr_exp[1] = win4;
			win_arr_exp[2] = win1;
			win_arr_exp[3] = win3;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "Failed LEFT*2\n");
		}
	}


	if (!error) {
		b3_ws_move_focused_win(ws, LEFT);

		error = check_winman_arr(ws->winman, "VLHLLL");
		//error = check_winman_arr(ws->winman, "HLVLHLL"); // TODO

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win2;
			win_arr_exp[1] = win4;
			win_arr_exp[2] = win1;
			win_arr_exp[3] = win3;

			//TODO
			//win_arr_exp[0] = win4;
			//win_arr_exp[1] = win2;
			//win_arr_exp[2] = win1;
			//win_arr_exp[3] = win3;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "Failed LEFT*3\n");
		}
	}

	if (!error) {
		b3_ws_remove_win(ws, win4);

		error = check_winman_arr(ws->winman, "VLHLL");
		//error = check_winman_arr(ws->winman, "HLVLHLL"); // TODO

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win2;
			win_arr_exp[1] = win1;
			win_arr_exp[2] = win3;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "Failed removing focused window\n");
		}
	}

	if (!error) {
		b3_ws_set_focused_win(ws, win3);
		b3_ws_move_focused_win(ws, RIGHT);
		b3_ws_move_focused_win(ws, RIGHT);
		b3_ws_move_focused_win(ws, RIGHT);

		error = check_winman_arr(ws->winman, "VLHLL");
		//error = check_winman_arr(ws->winman, "HLVLHLL"); // TODO

		if (!error) {
			memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
			win_arr_exp[0] = win2;
			win_arr_exp[1] = win1;
			win_arr_exp[2] = win3;

			error = check_win_arr(ws->winman, win_arr_exp);
		}

		if (error) {
			wbk_logger_log(&logger, SEVERE, "Failed moving 'win3' to right*3\n");
		}
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
	b3_win_t *win_arr_exp[ARR_LEN];

	monitor_area.top = 0;
	monitor_area.bottom = 1080;
	monitor_area.left = 0;
	monitor_area.right = 1920;

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");           /** H */
	b3_ws_add_win(ws, win1);          /** HL */
	b3_ws_add_win(ws, win2);          /** HLL*/
	b3_ws_add_win(ws, win3);          /** HLLL */
	b3_ws_add_win(ws, win4);          /** HLLLL */
	b3_ws_set_focused_win(ws, win2);

	b3_ws_arrange_wins(ws, monitor_area);

	error = check_winman_arr(ws->winman, "HLLLL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win2;
		win_arr_exp[2] = win3;
		win_arr_exp[3] = win4;

		error = check_win_arr(ws->winman, win_arr_exp);
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
	b3_win_t *win_arr_exp[ARR_LEN];

	monitor_area.top = 0;
	monitor_area.bottom = 1080;
	monitor_area.left = 0;
	monitor_area.right = 1920;

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);
	win4 = b3_win_new((HWND) 4, 0);

	ws = b3_ws_new("test");              /** H */
	b3_ws_add_win(ws, win1);             /** HL */
	b3_ws_add_win(ws, win2);             /** HLL*/
	b3_ws_add_win(ws, win3);             /** HLLL */
	b3_ws_add_win(ws, win4);             /** HLLLL */
	b3_ws_set_focused_win(ws, win2);
	b3_ws_toggle_floating_win(ws, win4); /** HLLL */
	b3_ws_move_focused_win(ws, DOWN);    /** HLLVL */

	b3_ws_arrange_wins(ws, monitor_area);

	error = check_winman_arr(ws->winman, "HLLVL");

	if (!error) {
		memset(win_arr_exp, 0, ARR_LEN * sizeof(b3_win_t *));
		win_arr_exp[0] = win1;
		win_arr_exp[1] = win3;
		win_arr_exp[2] = win2;

		error = check_win_arr(ws->winman, win_arr_exp);
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
	b3_test(setup, teardown, test_complex_split_1, "test_complex_split_1");
	b3_test(setup, teardown, test_complex_split_2, "test_complex_split_2");
	b3_test(setup, teardown, test_simple_remove_win, "test_simple_remove_win");
	b3_test(setup, teardown, test_remove_win_after_changed_focus, "test_remove_win_after_changed_focus");
	b3_test(setup, teardown, test_complex_remove_win, "test_complex_remove_win");
	b3_test(setup, teardown, test_remove_win_all, "test_remove_win_all");
	b3_test(setup, teardown, test_floating_simple, "test_floating_simple");
	b3_test(setup, teardown, test_floating_complex, "test_floating_complex");
	b3_test(setup, teardown, test_simple_win_rel, "test_simple_win_rel");
	b3_test(setup, teardown, test_simple_move, "test_simple_move");
	b3_test(setup, teardown, test_complex_win_rel, "test_complex_win_rel");
	b3_test(setup, teardown, test_complex_move_1, "test_complex_move_1");
	//b3_test(setup, teardown, test_simple_arrange, "test_simple_arrange");
	//b3_test(setup, teardown, test_complex_arrange, "test_complex_arrange");

	return 0;
}
