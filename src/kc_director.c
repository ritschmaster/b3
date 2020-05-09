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
 * @brief File contains the key binding director command class implementation and its private methods
 */

#include "kc_director.h"

#include <wbkbase/logger.h>
#include <pthread.h>

static wbk_logger_t logger =  { "kc_director" };

pthread_mutex_t g_exec_mutex = PTHREAD_MUTEX_INITIALIZER;

static int
b3_kc_director_exec_cw(const b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_cm(const b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawtw(const b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_awtf(const b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawu(const b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawd(const b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawl(const b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawr(const b3_kc_director_t *kc_director);

b3_kc_director_t *
b3_kc_director_new(wbk_b_t *comb, b3_director_t *director, b3_kc_director_kind_t kind, void *data)
{
	b3_kc_director_t *kc_director;
	int length;

	kc_director = NULL;
	kc_director = malloc(sizeof(b3_kc_director_t));
	memset(kc_director, 0, sizeof(b3_kc_director_t));

	if (kc_director != NULL) {
		kc_director->kc = wbk_kc_new(comb);

		kc_director->director = director;

		kc_director->kind = kind;

		kc_director->data = data;
	}

	return kc_director;
}

int
b3_kc_director_free(b3_kc_director_t *kc_director)
{
	wbk_kc_free(kc_director->kc);
	kc_director->kc = NULL;

	kc_director->director = NULL;

	if (kc_director->data) {
		switch (kc_director->kind) {
		case CHANGE_WORKSPACE:
			free(kc_director->data);
			kc_director->data = NULL;
			break;

		case CHANGE_MONITOR:
			free(kc_director->data);
			kc_director->data = NULL;
			break;

		case MOVE_ACTIVE_WINDOW_TO_WORKSPACE:
			free(kc_director->data);
			kc_director->data = NULL;
			break;

		case ACTIVE_WINDOW_TOGGLE_FLOATING:
			kc_director->data = NULL;
			break;
		}
	}

	kc_director->data = NULL;

	free(kc_director);

	return 0;
}

const wbk_b_t *
b3_kc_director_get_binding(const b3_kc_director_t *kc)
{
	return wbk_kc_get_binding(kc->kc);
}

int
b3_kc_director_exec(const b3_kc_director_t *kc_director)
{
	int ret;

	pthread_mutex_lock(&g_exec_mutex);

	switch(kc_director->kind) {
	case CHANGE_WORKSPACE:
		ret = b3_kc_director_exec_cw(kc_director);
		break;

	case CHANGE_MONITOR:
		ret = b3_kc_director_exec_cm(kc_director);
		break;

	case MOVE_ACTIVE_WINDOW_TO_WORKSPACE:
		ret = b3_kc_director_exec_mawtw(kc_director);
		break;

	case ACTIVE_WINDOW_TOGGLE_FLOATING:
		ret = b3_kc_director_exec_awtf(kc_director);
		break;

	case MOVE_ACTIVE_WINDOW_UP:
		ret = b3_kc_director_exec_mawu(kc_director);
		break;

	case MOVE_ACTIVE_WINDOW_DOWN:
		ret = b3_kc_director_exec_mawd(kc_director);
		break;

	case MOVE_ACTIVE_WINDOW_LEFT:
		ret = b3_kc_director_exec_mawl(kc_director);
		break;

	case MOVE_ACTIVE_WINDOW_RIGHT:
		ret = b3_kc_director_exec_mawr(kc_director);
		break;

	default:
		ret = -1;
		// TODO
	}

	pthread_mutex_unlock(&g_exec_mutex);

	return ret;
}

int
b3_kc_director_exec_cw(const b3_kc_director_t *kc_director)
{
	int ret;
	char *ws_id;

	ws_id = kc_director->data;

	ret = b3_director_switch_to_ws(kc_director->director, ws_id);

	return ret;
}

int
b3_kc_director_exec_cm(const b3_kc_director_t *kc_director)
{
	int ret;
	char *monitor_name;

	monitor_name = kc_director->data;

	ret = b3_director_set_focused_monitor(kc_director->director, monitor_name);

	return ret;
}

int
b3_kc_director_exec_mawtw(const b3_kc_director_t *kc_director)
{
	int ret;
	char *ws_id;

	ws_id = kc_director->data;

	ret = b3_director_move_active_win_to_ws(kc_director->director, ws_id);

	return ret;
}

int
b3_kc_director_exec_awtf(const b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_active_win_toggle_floating(kc_director->director);

	return ret;
}

int
b3_kc_director_exec_mawu(const b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_move_active_win(kc_director->director, UP);

	return ret;
}

int
b3_kc_director_exec_mawd(const b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_move_active_win(kc_director->director, DOWN);

	return ret;
}

int
b3_kc_director_exec_mawl(const b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_move_active_win(kc_director->director, LEFT);

	return ret;
}

int
b3_kc_director_exec_mawr(const b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_move_active_win(kc_director->director, RIGHT);

	return ret;
}
