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
 * @author Richard B�ck <richard.baeck@mailbox.org>
 * @date 2020-02-27
 * @brief File contains the key binding director command factory class
 * definition
 */

#include <w32bindkeys/kc.h>

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

/**
 * @return A new key binding director command of the type MOVE_ACTIVE_WINDOW_UP.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mawu(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_ACTIVE_WINDOW_DOWN.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mawd(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_ACTIVE_WINDOW_LEFT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mawl(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_ACTIVE_WINDOW_RIGHT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mawr(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type SET_ACTIVE_WINDOW_UP.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sawu(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type SET_ACTIVE_WINDOW_DOWN.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sawd(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type SET_ACTIVE_WINDOW_LEFT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sawl(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type SET_ACTIVE_WINDOW_RIGHT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sawr(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type TOGGLE_ACTIVE_WINDOW_FULLSCREEN.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_tawf(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type CLOSE_ACTIVE_WINDOW.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_caw(b3_kc_director_factory_t *kc_director_factory,
								  wbk_b_t *comb,
								  b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_FOCUSED_WORKSPACE_UP.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mfwu(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_FOCUSED_WORKSPACE_DOWN.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mfwd(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_FOCUSED_WORKSPACE_LEFT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mfwl(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_FOCUSED_WORKSPACE_RIGHT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mfwr(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type SET_FOCUSED_MONITOR_UP.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sfmu(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type SET_FOCUSED_MONITOR_DOWN.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sfmd(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type SET_FOCUSED_MONITOR_LEFT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sfml(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type SET_FOCUSED_MONITOR_RIGHT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sfmr(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_FOCUSED_WINDOW_TO_MONITOR_UP.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mfwtmu(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_FOCUSED_WINDOW_TO_MONITOR_DOWN.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mfwtmd(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_FOCUSED_WINDOW_TO_MONITOR_LEFT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mfwtml(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director);

/**
 * @return A new key binding director command of the type MOVE_FOCUSED_WINDOW_TO_MONITOR_RIGHT.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_mfwtmr(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
                                     b3_director_t *director);

/**
 * @return A new key binding director command of the type SPLIT_H.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sh(b3_kc_director_factory_t *kc_director_factory,
								 wbk_b_t *comb,
								 b3_director_t *director);

/**
 * @return A new key binding director command of the type SPLIT_V.
 * Free it by yourself!
 */
extern b3_kc_director_t *
b3_kc_director_factory_create_sv(b3_kc_director_factory_t *kc_director_factory,
								 wbk_b_t *comb,
								 b3_director_t *director);

#endif // B3_KC_DIRECTOR_FACTORY_H
