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
 * @date 2020-08-04
 * @brief File contains the tests for the parser class
 */

#include "../src/parser.h"

#include "test.h"

static b3_parser_t *g_parser = NULL;
static b3_kc_director_factory_t *g_kc_director_factory;

static void
setup(void)
{
	g_kc_director_factory = b3_kc_director_factory_new();
	g_parser = b3_parser_new(g_kc_director_factory);
}

static void
teardown(void)
{
	b3_parser_free(g_parser);
	b3_kc_director_factory_free(g_kc_director_factory);
}

static int
test_parse_str(void)
{
	b3_kbman_t *kbman;

	kbman = b3_parser_parse_str(g_parser, "");

	b3_kbman_free(kbman);

	return 0;
}

static int
test_parse_file(void)
{
	return 0;
}

int
main(void)
{
	b3_test(b3_test_empty_setup, b3_test_empty_teardown, test_parse_str);
	b3_test(b3_test_empty_setup, b3_test_empty_teardown, test_parse_file);

	return 0;
}
