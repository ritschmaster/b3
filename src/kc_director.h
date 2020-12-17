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
 * @brief File contains the key binding director command class definition
 *
 * b3_kc_director_t inherits all methods of wkb_kc_t (see w32bindkeys/kc.h).
 */

#include <w32bindkeys/kc.h>

#include "director.h"

#ifndef B3_KC_DIRECTOR_H
#define B3_KC_DIRECTOR_H

typedef enum b3_kc_director_kind_e
{
	CHANGE_WORKSPACE = 0,
	CHANGE_MONITOR,
	MOVE_ACTIVE_WINDOW_TO_WORKSPACE,
	ACTIVE_WINDOW_TOGGLE_FLOATING,
	MOVE_ACTIVE_WINDOW_UP,
	MOVE_ACTIVE_WINDOW_DOWN,
	MOVE_ACTIVE_WINDOW_LEFT,
	MOVE_ACTIVE_WINDOW_RIGHT,
	SET_ACTIVE_WINDOW_UP,
	SET_ACTIVE_WINDOW_DOWN,
	SET_ACTIVE_WINDOW_LEFT,
	SET_ACTIVE_WINDOW_RIGHT,
	TOGGLE_ACTIVE_WINDOW_FULLSCREEN,
	CLOSE_ACTIVE_WINDOW,
	MOVE_FOCUSED_WORKSPACE_UP,
	MOVE_FOCUSED_WORKSPACE_DOWN,
	MOVE_FOCUSED_WORKSPACE_LEFT,
	MOVE_FOCUSED_WORKSPACE_RIGHT,
	SET_FOCUSED_MONITOR_UP,
	SET_FOCUSED_MONITOR_DOWN,
	SET_FOCUSED_MONITOR_LEFT,
	SET_FOCUSED_MONITOR_RIGHT,
	MOVE_FOCUSED_WINDOW_TO_MONITOR_UP,
	MOVE_FOCUSED_WINDOW_TO_MONITOR_DOWN,
	MOVE_FOCUSED_WINDOW_TO_MONITOR_LEFT,
	MOVE_FOCUSED_WINDOW_TO_MONITOR_RIGHT
} b3_kc_director_kind_t;

typedef struct b3_kc_director_s
{
	wbk_kc_t kc;
  wbk_kc_t *(*super_kc_clone)(const wbk_kc_t *other);
  int (*super_kc_free)(wbk_kc_t *kc);
  int (*super_kc_exec)(const wbk_kc_t *kc);

	HANDLE global_mutex;

	b3_kc_director_kind_t kind;

	b3_director_t *director;

	/**
	 * depends on kind:
	 * - CHANGE_WORKSPACE: will need a char * as the name of the target
	 *   workspace
	 * - CHANGE_MONITOR: will need a char * as the name of the target monitor
	 * - MOVE_ACTIVE_WINDOW_TO_WORKSPACE: will need a char * as the name of the
	 * target workspace
	 * - All others will need NULL
	 */
	void *data;
} b3_kc_director_t;

/**
 * @brief Creates a new key binding command
 * @param comb The binding of the key command. The object will be freed by the key binding.
 * param kind The kind of the key command.
 * @param data The data. It will be freed by the key binding director command!
 * @return A new key binding command or NULL if allocation failed
 */
extern b3_kc_director_t *
b3_kc_director_new(wbk_b_t *comb, b3_director_t *director, b3_kc_director_kind_t kind, void *data);

#endif // B3_KC_DIRECTOR_H
