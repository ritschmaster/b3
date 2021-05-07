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
 * @date 2021-03-29
 * @brief File contains the tests for the winman class
 */

#include "../src/winman.h"

#include "test.h"

#define ARR_LEN 255

static int g_arr_i;
static char g_arr[ARR_LEN];

static void
visitor(b3_winman_t *winman, void *data) {
	if (array_size(b3_winman_get_winman_arr(winman)) != 0) {
		g_arr[g_arr_i] = 'I';
	} else  {
		g_arr[g_arr_i] = 'L';
	}

	g_arr_i++;
	if (g_arr_i >= ARR_LEN) {
		g_arr_i = 0;
	}
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
test_simple_tree(void)
{
	b3_winman_t *root;
	b3_winman_t *inner;
	int error;

	root = b3_winman_new(VERTICAL);

	inner = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, inner);
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));

	inner = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, inner);
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));

	memset(g_arr, 0, ARR_LEN);
	g_arr_i = 0;
	b3_winman_traverse(root, visitor, NULL);

	error = strcmp(g_arr, "IILLILLLL");

	b3_winman_free(root);

	return error;
}

static int
test_remove_winman(void)
{
	b3_winman_t *root;
	b3_winman_t *inner;
	b3_winman_t *winman_to_remove;
	int error;

	root = b3_winman_new(VERTICAL);

	inner = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, inner);
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));

	inner = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, inner);
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));

	winman_to_remove = b3_winman_new(VERTICAL);
	b3_winman_add_winman(inner, winman_to_remove);
	b3_winman_add_winman(winman_to_remove, b3_winman_new(VERTICAL));
	b3_winman_add_winman(winman_to_remove, b3_winman_new(VERTICAL));

	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));

	if (!error) {
		memset(g_arr, 0, ARR_LEN);
		g_arr_i = 0;
		b3_winman_traverse(root, visitor, NULL);
		error = strcmp(g_arr, "IILLILILLL");
	}

	if (!error) {
		error = b3_winman_remove_winman(root, winman_to_remove);
	}

	if (!error) {
		memset(g_arr, 0, ARR_LEN);
		g_arr_i = 0;
		b3_winman_traverse(root, visitor, NULL);
		error = strcmp(g_arr, "IILLILL");
	}

	if (!error) {
		memset(g_arr, 0, ARR_LEN);
		g_arr_i = 0;
		b3_winman_traverse(winman_to_remove, visitor, NULL);
		error = strcmp(g_arr, "ILL");
	}

	b3_winman_free(root);
	b3_winman_free(winman_to_remove);

	return error;
}


static int
test_get_parent(void)
{
	b3_winman_t *root;
	b3_winman_t *inner;
	b3_winman_t *search;
	b3_winman_t *result_act;
	b3_winman_t *result_exp;
	int error;

	root = b3_winman_new(VERTICAL);

	inner = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, inner);
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));

	result_exp = b3_winman_new(VERTICAL);
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));
	search = b3_winman_new(VERTICAL);
	b3_winman_add_winman(inner, search);

	inner = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, inner);
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));
	b3_winman_add_winman(inner, search);
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));
	b3_winman_add_winman(inner, b3_winman_new(VERTICAL));

	result_act = b3_winman_get_parent(root, search);

	error = result_act == result_exp;

	b3_winman_free(root);

	return error;
}

static int
test_contains_win(void)
{
	b3_winman_t *root;
	b3_winman_t *inner;
	b3_winman_t *inner_2;
	b3_winman_t *leaf;
	b3_winman_t *winman_to_find;
	b3_winman_t *winman_found;
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	int error;

	error = 1;

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);

	root = b3_winman_new(VERTICAL);

	inner = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, inner);

	leaf = b3_winman_new(VERTICAL);
	b3_winman_add_winman(inner, leaf);
	b3_winman_set_win(leaf, win1);

	inner = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, inner);

	inner_2= b3_winman_new(VERTICAL);
	b3_winman_add_winman(inner, inner_2);

	leaf = b3_winman_new(VERTICAL);
	b3_winman_add_winman(inner_2, leaf);
	b3_winman_set_win(leaf, win2);
	winman_to_find = leaf;

	inner_2= b3_winman_new(VERTICAL);
	b3_winman_add_winman(inner, inner_2);

	leaf = b3_winman_new(VERTICAL);
	b3_winman_add_winman(inner_2, leaf);
	b3_winman_set_win(leaf, win3);

	winman_found = b3_winman_contains_win(root, win2);
	if (winman_to_find == winman_found) {
	 	error = 0;
	}

	b3_winman_free(root);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);

	return error;
}

static int
test_get_rel(void)
{
	b3_win_t *win1;
	b3_win_t *win2;
	b3_win_t *win3;
	b3_winman_t *root;
	b3_winman_t *leaf1;
	b3_winman_t *leaf2;
	b3_winman_t *leaf3;
	b3_winman_t *found;
	int error;

	error = 1;

	win1 = b3_win_new((HWND) 1, 0);
	win2 = b3_win_new((HWND) 2, 0);
	win3 = b3_win_new((HWND) 3, 0);

	root = b3_winman_new(VERTICAL);

	leaf1 = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, leaf1);
	b3_winman_set_win(leaf1, win1);

	leaf2 = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, leaf2);
	b3_winman_set_win(leaf2, win2);

	leaf3 = b3_winman_new(VERTICAL);
	b3_winman_add_winman(root, leaf3);
	b3_winman_set_win(leaf3, win3);

	error = 0;

	if (!error) {
		found = b3_winman_get_winman_rel_to_winman(root, leaf2, PREVIOUS, 0);
		error = b3_test_check_void(found, leaf1, "Failed PREVIOUS.");
	}

	if (!error) {
		found = b3_winman_get_winman_rel_to_winman(root, leaf1, PREVIOUS, 0);
		error = b3_test_check_void(found, NULL, "Failed PREVIOUS.");
	}

	if (!error) {
		found = b3_winman_get_winman_rel_to_winman(root, leaf1, PREVIOUS, 1);
		error = b3_test_check_void(found, leaf3, "Failed rolling PREVIOUS.");
	}

	if (!error) {
		found = b3_winman_get_winman_rel_to_winman(root, leaf2, NEXT, 0);
		error = b3_test_check_void(found, leaf3, "Failed NEXT.");
	}

	if (!error) {
		found = b3_winman_get_winman_rel_to_winman(root, leaf3, NEXT, 0);
		error = b3_test_check_void(found, NULL, "Failed NEXT.");
	}

	if (!error) {
		found = b3_winman_get_winman_rel_to_winman(root, leaf3, NEXT, 1);
		error = b3_test_check_void(found, leaf1, "Failed NEXT.");
	}

	b3_winman_free(root);
	b3_win_free(win1);
	b3_win_free(win2);
	b3_win_free(win3);

	return error;
}

int
main(void)
{
	b3_test(setup, teardown, test_simple_tree, "test_simple_tree");
	b3_test(setup, teardown, test_remove_winman, "test_remove_winman");
	b3_test(setup, teardown, test_get_parent, "test_get_parent");
	b3_test(setup, teardown, test_contains_win, "test_contains_win");
	b3_test(setup, teardown, test_get_rel, "test_get_rel");

	return 0;
}
