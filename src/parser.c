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
 * @brief File contains the parser class implementation and its private methods
 */

#include "parser.h"

#include <stdlib.h>

#include "parser_gen.h"
#include "lexer_gen.h"

static wbk_kbman_t *
b3_parser_parse(b3_parser_t *parser, b3_director_t *director, yyscan_t scanner);

b3_parser_t *
b3_parser_new(b3_kc_director_factory_t *kc_director_factory,
              b3_condition_factory_t *condition_factory,
              b3_action_factory_t *action_factory)
{
	b3_parser_t *parser;

	parser = malloc(sizeof(b3_parser_t));

	parser->kc_director_factory = kc_director_factory;
	parser->condition_factory = condition_factory;
	parser->action_factory = action_factory;

	return parser;
}

extern int
b3_parser_free(b3_parser_t *parser)
{
	parser->kc_director_factory = NULL;
	parser->condition_factory = NULL;
	parser->action_factory = NULL;

	free(parser);
	return 0;
}

wbk_kbman_t *
b3_parser_parse_str(b3_parser_t *parser, b3_director_t *director, const char *str)
{
	yyscan_t scanner;
	YY_BUFFER_STATE state;
	wbk_kbman_t *kbman;

	if (yylex_init(&scanner)) {
		// couldn't initialize return NULL;
		// TODO
	}

	state = yy_scan_string(str, scanner);

	kbman = b3_parser_parse(parser, director, scanner);

	yy_delete_buffer(state, scanner);

	yylex_destroy(scanner);

	return kbman;
}

wbk_kbman_t *
b3_parser_parse_file(b3_parser_t *parser, b3_director_t *director, FILE *file)
{
	yyscan_t scanner;
	YY_BUFFER_STATE state;
	wbk_kbman_t *kbman;

	if (yylex_init(&scanner)) {
		// couldn't initialize return NULL;
		// TODO
	}

	yyset_in(file, scanner);
	kbman = b3_parser_parse(parser, director, scanner);

	yy_delete_buffer(state, scanner);

	yylex_destroy(scanner);

	return kbman;
}

wbk_kbman_t *
b3_parser_parse(b3_parser_t *parser, b3_director_t *director, yyscan_t scanner)
{
	wbk_kbman_t *kbman;

	kbman = wbk_kbman_new();

	if (yyparse(&(parser->kc_director_factory),
              &(parser->condition_factory),
              &(parser->action_factory),
              &director,
              &kbman,
              scanner)) {
		wbk_kbman_free(kbman);
		kbman = NULL;
	}

	return kbman;
}
