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
 * @date 2020-02-27
 * @brief File contains the key binding director command factory class
 * definition
 */

#include <wbkbase/kc.h>

#include "kc_director.h"
#include "director.h"

#ifndef B3_KC_DIRECTOR_FACTORY_H
#define B3_KC_DIRECTOR_FACTORY_H

typedef struct b3_kc_director_factory_s
{

} b3_kc_director_factory_t;

/**
 * @brief Creates a new key binding director command factory
 * @return A new key binding director command factory or NULL if allocation failed
 */
extern b3_kc_director_factory_t *
b3_kc_director_factory_new();

/**
 * @brief Deletes a key binding director command factory
 * @return Non-0 if the deletion failed
 */
extern int
b3_kc_director_factory_free(b3_kc_director_factory_t *kc_director_factory);

/**
 * @return A new key binding director command of the type CHANGE_WORKSPACE.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_cw(b3_kc_director_factory_t *kc_director_factory,
							     wbk_b_t *comb,
								 b3_director_t *director,
								 const char *ws_id);

/**
 * @return A new key binding director command of the type CHANGE_MONITOR.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_cm(b3_kc_director_factory_t *kc_director_factory,
								 wbk_b_t *comb,
								 b3_director_t *director,
								 const char *monitor_name);

/**
 * @return A new key binding director command of the type MOVE_ACTIVE_WINDOW_TO_WORKSPACE.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mawtw(b3_kc_director_factory_t *kc_director_factory,
								    wbk_b_t *comb,
								    b3_director_t *director,
						            const char *ws_id);

/**
 * @return A new key binding director command of the type ACTIVE_WINDOW_TOGGLE_FLOATING.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_awtf(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);


#endif // B3_KC_DIRECTOR_FACTORY_H
