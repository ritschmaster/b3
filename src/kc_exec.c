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
 * @author Richard Bäck
 * @date 2020-12-20
 * @brief File contains the key binding exec command class implementation and private methods
 */

#include "kc_exec.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <windows.h>
#include <w32bindkeys/logger.h>

#include "ws.h"

#define B3_KC_EXEC_MAX_ITERATIONS 60

typedef struct wkb_kc_exec_comm_s
{
  DWORD process_id;
  b3_director_t *director;
  char *focused_ws_name;
  int iterations;
} wkb_kc_exec_comm_t;

static wbk_logger_t logger =  { "kc_exec" };

/**
 * Implementation of wbk_kc_clone().
 *
 * Clones a key binding system command
 */
static wbk_kc_t *
b3_kc_exec_clone_impl(const wbk_kc_t *super_other);

/**
 * Implementaiton of wbk_kc_free().
 *
 * @brief Frees a key binding system command
 * @return Non-0 if the freeing failed
 */
static int
b3_kc_exec_free_impl(wbk_kc_t *kc);

/**
 * Implementation of b3_kc_exec_get_cmd().
 *
 */
static const char *
b3_kc_exec_get_cmd_impl(const b3_kc_exec_t *kc_exec);

/**
 * Implementation of wbk_kc_exec().
 *
 * @brief Execute the system command of a key binding system command
 * @return Non-0 if the execution failed
 */
static int
b3_kc_exec_exec_impl(const wbk_kc_t *kc);

static BOOL CALLBACK
b3_kc_exec_enum_wins(_In_ HWND   window_handler,
                     _In_ LPARAM param);

static DWORD WINAPI
b3_kbthread_exec(LPVOID param);

b3_kc_exec_t *
b3_kc_exec_new(wbk_b_t *comb, b3_director_t *director, b3_kc_exec_type_t type, char *cmd)
{
  wbk_kc_t *kc;
	b3_kc_exec_t *kc_exec;
	int length;

	kc_exec = NULL;
	kc_exec = malloc(sizeof(b3_kc_exec_t));
	memset(kc_exec, 0, sizeof(b3_kc_exec_t));

  if (kc_exec) {
    kc = wbk_kc_new(comb);
    memcpy(kc_exec, kc, sizeof(wbk_kc_t));
    free(kc); /* Just free the top level element */

    kc_exec->super_kc_clone = kc_exec->kc.kc_clone;
    kc_exec->super_kc_free = kc_exec->kc.kc_free;
    kc_exec->super_kc_exec = kc_exec->kc.kc_exec;

    kc_exec->kc.kc_clone = b3_kc_exec_clone_impl;
    kc_exec->kc.kc_free = b3_kc_exec_free_impl;
    kc_exec->kc.kc_exec = b3_kc_exec_exec_impl;
    kc_exec->kc_exec_get_cmd = b3_kc_exec_get_cmd_impl;

		kc_exec->director = director;
    kc_exec->type = type;
    kc_exec->cmd = cmd;
	}

	return kc_exec;
}

wbk_kc_t *
b3_kc_exec_clone_impl(const wbk_kc_t *super_other)
{
	const b3_kc_exec_t *other;
  wbk_b_t *comb;
	char *cmd;
	int cmd_len;
	b3_kc_exec_t *kc_sys;

  other = (const b3_kc_exec_t *) super_other;

	kc_sys = NULL;
	if (other) {
		comb = wbk_b_clone(wbk_kc_get_binding((wbk_kc_t *) other));

		cmd_len = strlen(b3_kc_exec_get_cmd(other)) + 1;
		cmd = malloc(sizeof(char) * cmd_len);
		memcpy(cmd, b3_kc_exec_get_cmd(other), sizeof(char) * cmd_len);

		kc_sys = b3_kc_exec_new(comb, other->director, other->type, cmd);
	}

	return (wbk_kc_t *) kc_sys;
}

const char *
b3_kc_exec_get_cmd(const b3_kc_exec_t *kc_sys)
{
	return kc_sys->kc_exec_get_cmd(kc_sys);
}


int
b3_kc_exec_free_impl(wbk_kc_t *kc)
{
  b3_kc_exec_t *kc_sys;

  kc_sys = (b3_kc_exec_t *) kc;

  free(kc_sys->cmd);
	kc_sys->cmd = NULL;

  kc_sys->super_kc_free(kc);

	return 0;
}

const char *
b3_kc_exec_get_cmd_impl(const b3_kc_exec_t *kc_exec)
{
	return kc_exec->cmd;
}

BOOL CALLBACK
b3_kc_exec_enum_wins(_In_ HWND   window_handler,
                     _In_ LPARAM param)
{
  wkb_kc_exec_comm_t *kc_exec_comm;
  DWORD process_id;
  b3_win_t *win;

  kc_exec_comm = (wkb_kc_exec_comm_t *) param;

  GetWindowThreadProcessId(window_handler, &process_id);

  if (kc_exec_comm->process_id == process_id) {
    kc_exec_comm->iterations = B3_KC_EXEC_MAX_ITERATIONS;

    win = b3_win_new(window_handler, 0);
    b3_director_set_active_win(kc_exec_comm->director, win);
    b3_director_move_active_win_to_ws(kc_exec_comm->director,
                                      kc_exec_comm->focused_ws_name);
  }

  return TRUE;
}

DWORD WINAPI
b3_kbthread_exec(LPVOID param)
{
  const b3_kc_exec_t *kc_exec;
  b3_ws_t *focused_ws;
  wkb_kc_exec_comm_t kc_exec_comm;
	STARTUPINFOA startup_info;
  PROCESS_INFORMATION process_info;

	kc_exec = (const b3_kc_exec_t *) param;

  if (kc_exec->type == ON_START_WS) {
    /**
     * Get the ws on which the process is started as soon as possible
     */
    focused_ws = b3_monitor_get_focused_ws(b3_director_get_focused_monitor(kc_exec->director));

    kc_exec_comm.focused_ws_name = NULL;
    if (focused_ws) {
      kc_exec_comm.focused_ws_name = malloc(sizeof(char) * (strlen(b3_ws_get_name(focused_ws) + 1)));
      if (kc_exec_comm.focused_ws_name) {
        strcpy(kc_exec_comm.focused_ws_name, b3_ws_get_name(focused_ws));
      } else {
        kc_exec_comm.iterations = B3_KC_EXEC_MAX_ITERATIONS;
      }
    } else {
      kc_exec_comm.iterations = B3_KC_EXEC_MAX_ITERATIONS;
    }
  }

  CreateProcess(NULL,
                kc_exec->cmd,
                NULL,
                NULL,
                0,
                NORMAL_PRIORITY_CLASS,
                NULL,
                NULL,
                &startup_info,
                &process_info);

  if (kc_exec->type == ON_START_WS
      && kc_exec_comm.focused_ws_name) {
    kc_exec_comm.process_id = process_info.dwProcessId;
    kc_exec_comm.director = kc_exec->director;

    Sleep(250);

    while (kc_exec_comm.iterations < B3_KC_EXEC_MAX_ITERATIONS) {
      EnumWindows(b3_kc_exec_enum_wins, (LPARAM) &kc_exec_comm);
      kc_exec_comm.iterations++;
      Sleep(1000);
    }

    CloseHandle(process_info.hThread);
    CloseHandle(process_info.hProcess);

    free(kc_exec_comm.focused_ws_name);
  }

  return 0;
}

int
b3_kc_exec_exec_impl(const wbk_kc_t *kc)
{
  const b3_kc_exec_t *kc_exec;

  kc_exec = (const b3_kc_exec_t *) kc;

#ifdef DEBUG_ENABLED
	char *binding;

	binding = wbk_b_to_str(wbk_kc_get_binding(kc));
	wbk_logger_log(&logger, DEBUG, "Exec binding: %s\n", binding);
	free(binding);
	binding = NULL;
#endif

	HANDLE thread_handler;

	thread_handler = CreateThread(NULL,
                                0,
                                b3_kbthread_exec,
                                (LPVOID) kc_exec,
                                0,
                                NULL);
	if (thread_handler) {
		wbk_logger_log(&logger, INFO, "Exec: %s\n", kc_exec->cmd);
	} else {
		wbk_logger_log(&logger, SEVERE, "Exec failed: %s\n", kc_exec->cmd);
	}

	return 0;
}

