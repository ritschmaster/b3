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
 * @brief File contains the monitor factory
 */

#include "wsman_factory.h"
#include "monitor.h"
#include "ws_switcher.h"

#ifndef B3_MONITOR_FACTORY_H
#define B3_MONITOR_FACTORY_H

typedef struct b3_monitor_factory_s
{
	b3_wsman_factory_t *wsman_factory;
} b3_monitor_factory_t;

/**
 * @brief Creates a new monitor factory
 * @param wsman_factory A workspace manager factory. It will not be freed by freeing the
 * monitor factory!
 * @return A new monitor factory or NULL if allocation failed
 */
extern b3_monitor_factory_t *
b3_monitor_factory_new(b3_wsman_factory_t *wsman_factory);

/**
 * @brief Deletes a monitor factory
 * @return Non-0 if the deletion failed
 */
extern int
b3_monitor_factory_free(b3_monitor_factory_t *monitor_factory);

/**
 * @return A new monitor. Free it by yourself!
 */
extern b3_monitor_t *
b3_monitor_factory_create(b3_monitor_factory_t *monitor_factory,
						  const char *monitor_name,
						  RECT monitor_area,
						  b3_ws_switcher_t *ws_switcher);

#endif // B3_MONITOR_FACTORY_H
