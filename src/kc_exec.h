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
 * @brief File contains the key binding exec command class definition
 *
 * wbk_kc_exec_t inherits all methods of wkb_kc_t (see kc.h).
 */

#include <w32bindkeys/kc.h>

#include "director.h"

#ifndef B3_KC_EXEC_H
#define B3_KC_EXEC_H

typedef struct b3_kc_exec_s b3_kc_exec_t;

typedef enum b3_kc_exec_type_e {
  ON_START_WS = 0,
  ON_CURRENT_WS
} b3_kc_exec_type_t;

struct b3_kc_exec_s
{
	wbk_kc_t kc;
  wbk_kc_t *(*super_kc_clone)(const wbk_kc_t *other);
  int (*super_kc_free)(wbk_kc_t *kc);
  int (*super_kc_exec)(const wbk_kc_t *kc);

  const char *(*kc_exec_get_cmd)(const b3_kc_exec_t *kc_sys);

	b3_director_t *director;
  b3_kc_exec_type_t type;
	char *cmd;
};

/**
 * @brief Creates a new key binding exec command
 * @param comb The binding of the key command. The object will be freed by the key binding.
 * @param director The director which will be altered by this key command. It will not be freed!
 * @param type The exec type.
 * @param cmd The system command of the key command. The passed string will be freed by the key binding.
 * @return A new key binding command or NULL if allocation failed
 */
extern b3_kc_exec_t *
b3_kc_exec_new(wbk_b_t *comb, b3_director_t *director, b3_kc_exec_type_t type, char *cmd);

/**
 * @brief Gets the command of a key binding exec command.
 * @return The command of a key binding exec command.
 */
extern const char *
b3_kc_exec_get_cmd(const b3_kc_exec_t *kc_sys);

#endif // B3_KC_EXEC_H
