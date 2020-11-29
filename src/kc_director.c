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

#include <w32bindkeys/logger.h>
#include <windows.h>

#include "monitor.h"

static wbk_logger_t logger =  { "kc_director" };

static int
b3_kc_director_exec_cw(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_cm(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawtw(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_awtf(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawu(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawd(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawl(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mawr(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_sawu(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_sawd(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_sawl(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_sawr(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_tawf(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_caw(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mfwu(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mfwd(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mfwl(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mfwr(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_sfmu(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_sfmd(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_sfml(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_sfmr(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mfwtmu(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mfwtmd(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mfwtml(b3_kc_director_t *kc_director);

static int
b3_kc_director_exec_mfwtmr(b3_kc_director_t *kc_director);

/**
 * Position the cursor in the middle of the monitor.
 */
static int
b3_kc_director_position_cursor(b3_monitor_t *monitor);

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

		kc_director->global_mutex = CreateMutex(NULL, FALSE, NULL);

		kc_director->director = director;

		kc_director->kind = kind;

		kc_director->data = data;
	}

	return kc_director;
}

/**
 * Clones a key binding command
 */
extern b3_kc_director_t *
b3_kc_director_clone(const b3_kc_director_t *other)
{
	wbk_b_t *comb;
	char *data_str;
	void *data;
	int len;
	b3_kc_director_t *kc_director;

	kc_director = NULL;
	if (other) {
		comb = wbk_b_clone(b3_kc_director_get_binding(other));

		switch (other->kind) {
		case CHANGE_WORKSPACE:
		case CHANGE_MONITOR:
		case MOVE_ACTIVE_WINDOW_TO_WORKSPACE:
			len = strlen((char *) other->data) + 1;
			data_str = malloc(sizeof(char) * len);
			memcpy(data_str, (char *) other->data, sizeof(char) * len);
			data = data_str;
			break;

		default:
			if (other->data) {
				wbk_logger_log(&logger, SEVERE, "Object to clone: kind %d does not expect data\n", other->kind);
			}
			data = NULL;
		}

		kc_director = b3_kc_director_new(comb, other->director, other->kind, data);
	}

	return kc_director;
}

int
b3_kc_director_free(b3_kc_director_t *kc_director)
{
	wbk_kc_free(kc_director->kc);
	kc_director->kc = NULL;

	ReleaseMutex(kc_director->global_mutex);
	CloseHandle(kc_director->global_mutex);

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

inline const wbk_b_t *
b3_kc_director_get_binding(const b3_kc_director_t *kc)
{
	return wbk_kc_get_binding(kc->kc);
}

int
b3_kc_director_exec(b3_kc_director_t *kc_director)
{
	int ret;

	WaitForSingleObject(kc_director->global_mutex, INFINITE);

#ifdef DEBUG_ENABLED
	char *binding;

	binding = wbk_b_to_str(b3_kc_director_get_binding(kc_director));
	wbk_logger_log(&logger, DEBUG, "Exec binding: %s\n", binding);
	free(binding);
	binding = NULL;
#endif

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

	case SET_ACTIVE_WINDOW_UP:
		ret = b3_kc_director_exec_sawu(kc_director);
		break;

	case SET_ACTIVE_WINDOW_DOWN:
		ret = b3_kc_director_exec_sawd(kc_director);
		break;

	case SET_ACTIVE_WINDOW_LEFT:
		ret = b3_kc_director_exec_sawl(kc_director);
		break;

	case SET_ACTIVE_WINDOW_RIGHT:
		ret = b3_kc_director_exec_sawr(kc_director);
		break;

	case TOGGLE_ACTIVE_WINDOW_FULLSCREEN:
		ret = b3_kc_director_exec_tawf(kc_director);
		break;

	case CLOSE_ACTIVE_WINDOW:
		ret = b3_kc_director_exec_caw(kc_director);
		break;

	case MOVE_FOCUSED_WORKSPACE_UP:
		ret = b3_kc_director_exec_mfwu(kc_director);
		break;

	case MOVE_FOCUSED_WORKSPACE_DOWN:
		ret = b3_kc_director_exec_mfwd(kc_director);
		break;

	case MOVE_FOCUSED_WORKSPACE_LEFT:
		ret = b3_kc_director_exec_mfwl(kc_director);
		break;

	case MOVE_FOCUSED_WORKSPACE_RIGHT:
		ret = b3_kc_director_exec_mfwr(kc_director);
		break;

	case SET_FOCUSED_MONITOR_UP:
		ret = b3_kc_director_exec_sfmu(kc_director);
		break;

	case SET_FOCUSED_MONITOR_DOWN:
		ret = b3_kc_director_exec_sfmd(kc_director);
		break;

	case SET_FOCUSED_MONITOR_LEFT:
		ret = b3_kc_director_exec_sfml(kc_director);
		break;

	case SET_FOCUSED_MONITOR_RIGHT:
		ret = b3_kc_director_exec_sfmr(kc_director);
		break;

	case MOVE_FOCUSED_WINDOW_TO_MONITOR_UP:
		ret = b3_kc_director_exec_mfwtmu(kc_director);
		break;

	case MOVE_FOCUSED_WINDOW_TO_MONITOR_DOWN:
		ret = b3_kc_director_exec_mfwtmd(kc_director);
		break;

	case MOVE_FOCUSED_WINDOW_TO_MONITOR_LEFT:
		ret = b3_kc_director_exec_mfwtml(kc_director);
		break;

	case MOVE_FOCUSED_WINDOW_TO_MONITOR_RIGHT:
		ret = b3_kc_director_exec_mfwtmr(kc_director);
		break;

	default:
		ret = -1;
		// TODO
	}

	ReleaseMutex(kc_director->global_mutex);

	return ret;
}

int
b3_kc_director_exec_cw(b3_kc_director_t *kc_director)
{
	int ret;
	char *ws_id;
  b3_monitor_t *focused_monitor_old;

	ws_id = kc_director->data;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_switch_to_ws(kc_director->director, ws_id);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

	return ret;
}

int
b3_kc_director_exec_cm(b3_kc_director_t *kc_director)
{
	int ret;
	char *monitor_name;
  b3_monitor_t *focused_monitor_old;

	monitor_name = kc_director->data;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_set_focused_monitor_by_name(kc_director->director, monitor_name);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

	return ret;
}

int
b3_kc_director_exec_mawtw(b3_kc_director_t *kc_director)
{
	int ret;
	char *ws_id;

	ws_id = kc_director->data;

	ret = b3_director_move_active_win_to_ws(kc_director->director, ws_id);

	return ret;
}

int
b3_kc_director_exec_awtf(b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_active_win_toggle_floating(kc_director->director);

	return ret;
}

int
b3_kc_director_exec_mawu(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_move_active_win(kc_director->director, UP);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

	return ret;
}

int
b3_kc_director_exec_mawd(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_move_active_win(kc_director->director, DOWN);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

	return ret;
}

int
b3_kc_director_exec_mawl(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_move_active_win(kc_director->director, LEFT);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

	return ret;
}

int
b3_kc_director_exec_mawr(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_move_active_win(kc_director->director, RIGHT);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

	return ret;
}

int
b3_kc_director_exec_sawu(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_set_active_win_by_direction(kc_director->director, UP);

	if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

  return ret;
}

int
b3_kc_director_exec_sawd(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_set_active_win_by_direction(kc_director->director, DOWN);

	if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

  return ret;
}

int
b3_kc_director_exec_sawl(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_set_active_win_by_direction(kc_director->director, LEFT);

	if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

  return ret;
}

int
b3_kc_director_exec_sawr(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_set_active_win_by_direction(kc_director->director, RIGHT);

	if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

  return ret;
}

int
b3_kc_director_exec_tawf(b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_toggle_active_win_fullscreen(kc_director->director);

	return ret;
}

int
b3_kc_director_exec_caw(b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_close_active_win(kc_director->director);

	return ret;
}

int
b3_kc_director_exec_mfwu(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_move_focused_ws_to_monitor_by_dir(kc_director->director, UP);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

	return ret;
}

int
b3_kc_director_exec_mfwd(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_move_focused_ws_to_monitor_by_dir(kc_director->director, DOWN);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }
	return ret;
}

int
b3_kc_director_exec_mfwl(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_move_focused_ws_to_monitor_by_dir(kc_director->director, LEFT);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }
	return ret;
}

int
b3_kc_director_exec_mfwr(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_move_focused_ws_to_monitor_by_dir(kc_director->director, RIGHT);

  if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

	return ret;
}

int
b3_kc_director_exec_sfmu(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_set_focused_monitor_by_direction(kc_director->director, UP);

	if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

  return ret;
}

int
b3_kc_director_exec_sfmd(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_set_focused_monitor_by_direction(kc_director->director, DOWN);

	if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

  return ret;
}

int
b3_kc_director_exec_sfml(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_set_focused_monitor_by_direction(kc_director->director, LEFT);

	if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

  return ret;
}

int
b3_kc_director_exec_sfmr(b3_kc_director_t *kc_director)
{
	int ret;
  b3_monitor_t *focused_monitor_old;

  focused_monitor_old = b3_director_get_focused_monitor(kc_director->director);

	ret = b3_director_set_focused_monitor_by_direction(kc_director->director, RIGHT);

	if (focused_monitor_old != b3_director_get_focused_monitor(kc_director->director)) {
    b3_kc_director_position_cursor(b3_director_get_focused_monitor(kc_director->director));
  }

  return ret;
}

int
b3_kc_director_exec_mfwtmu(b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_move_focused_win_to_monitor_by_dir(kc_director->director, UP);

	return ret;
}

int
b3_kc_director_exec_mfwtmd(b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_move_focused_win_to_monitor_by_dir(kc_director->director, DOWN);

	return ret;
}

int
b3_kc_director_exec_mfwtml(b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_move_focused_win_to_monitor_by_dir(kc_director->director, LEFT);

	return ret;
}

int
b3_kc_director_exec_mfwtmr(b3_kc_director_t *kc_director)
{
	int ret;

	ret = b3_director_move_focused_win_to_monitor_by_dir(kc_director->director, RIGHT);

	return ret;
}

int
b3_kc_director_position_cursor(b3_monitor_t *monitor)
{
  RECT monitor_area;
  POINT cursor_pos;

  monitor_area = b3_monitor_get_monitor_area(monitor);
  cursor_pos.y = monitor_area.top + (monitor_area.bottom - monitor_area.top) / 2;
  cursor_pos.x = monitor_area.left + (monitor_area.right - monitor_area.left) / 2;
  SetCursorPos(cursor_pos.x, cursor_pos.y);

  return 0;
}
