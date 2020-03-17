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
 * @brief File contains the parser class definition
 */

#include <wbkbase/parser.h>

#include "kc_director_factory.h"

#ifndef B3_PARSER_H
#define B3_PARSER_H

typedef struct b3_parser_s
{
	wbk_parser_t *parser;

	b3_kc_director_factory_t *kc_director_factory;
} b3_parser_t;

/**
 * @param ws_factory A workspace factory. It will not be freed by freeing the
 * workspace manager factory!
 */
extern b3_parser_t *
b3_parser_new(b3_kc_director_factory_t *kc_director_factory, const char *filename);

extern int
b3_parser_free(b3_parser_t *parser);

#endif // B3_PARSER_H
