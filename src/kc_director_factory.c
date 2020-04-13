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
 * implementation and its private methods
 */

#include "kc_director_factory.h"

#include <stdlib.h>
#include <string.h>

b3_kc_director_factory_t *
b3_kc_director_factory_new()
{
	b3_kc_director_factory_t *kc_director_factory;

	kc_director_factory = malloc(sizeof(b3_kc_director_factory_t));

	return kc_director_factory;
}

int
b3_kc_director_factory_free(b3_kc_director_factory_t *kc_director_factory)
{
	free(kc_director_factory);

	return 0;
}

b3_kc_director_t *
b3_kc_director_factory_create_cw(b3_kc_director_factory_t *kc_director_factory,
							     wbk_b_t *comb,
								 b3_director_t *director,
								 const char *ws_id)
{
	b3_kc_director_t *cw;
	char *data;
	int length;

	length = strlen(ws_id) + 1;
	data = malloc(sizeof(char) * length);
	strcpy(data, ws_id);

	cw = b3_kc_director_new(comb, director, CHANGE_WORKSPACE, data);

	return cw;
}

b3_kc_director_t *
b3_kc_director_factory_create_cm(b3_kc_director_factory_t *kc_director_factory,
								 wbk_b_t *comb,
								 b3_director_t *director,
								 const char *monitor_name)
{
	b3_kc_director_t *cm;
	char *data;
	int length;

	length = strlen(monitor_name) + 1;
	data = malloc(sizeof(char) * length);
	strcpy(data, monitor_name);

	cm = b3_kc_director_new(comb, director, CHANGE_MONITOR, data);

	return cm;
}

b3_kc_director_t *
b3_kc_director_factory_create_mawtw(b3_kc_director_factory_t *kc_director_factory,
								    wbk_b_t *comb,
								    b3_director_t *director,
						            const char *ws_id)
{
	b3_kc_director_t *mawtw;
	char *data;
	int length;

	length = strlen(ws_id) + 1;
	data = malloc(sizeof(char) * length);
	strcpy(data, ws_id);

	mawtw = b3_kc_director_new(comb, director, MOVE_ACTIVE_WINDOW_TO_WORKSPACE, data);

	return mawtw;
}

b3_kc_director_t *
b3_kc_director_factory_create_awtf(b3_kc_director_factory_t *kc_director_factory,
								   wbk_b_t *comb,
								   b3_director_t *director)
{
	b3_kc_director_t *mawtw;

	mawtw = b3_kc_director_new(comb, director, ACTIVE_WINDOW_TOGGLE_FLOATING, NULL);

	return mawtw;
}
