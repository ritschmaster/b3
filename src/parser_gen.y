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

%{

/**
 * @author Richard Bäck <richard.baeck@mailbox.org>
 * @date 2020-08-03
 * @brief File contains the parser definition
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kbman.h"
#include "parser_gen.h"
#include "lexer_gen.h"

int
yyerror(b3_kbman_t **kbman, yyscan_t scanner, const char *msg) {
	fprintf(stderr, "Error: %s\n", msg);

	return 0;
}

%}

%code requires {

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

}

%output  "parser_gen.c"
%defines "parser_gen.h"

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { b3_kbman_t **kbman }
%parse-param { yyscan_t scanner }

%union {
	char c;
}

%token               TOKEN_MODIFIER
%token               TOKEN_KEY
%token               TOKEN_PLUS
%token               TOKEN_BINDSYM
%token               TOKEN_SPACE
%token               TOKEN_EOL
%token               TOKEN_EOF

%%

statements: TOKEN_EOL statement TOKEN_EOL statements
	  | TOKEN_EOL statement TOKEN_EOL statements
	  | statement TOKEN_EOF
	  | TOKEN_EOL statement TOKEN_EOF
	  | TOKEN_EOL TOKEN_EOF

statement: bindsym
	 ;

bindsym: TOKEN_BINDSYM TOKEN_SPACE binding TOKEN_SPACE

binding: TOKEN_MODIFIER
       | TOKEN_MODIFIER TOKEN_SPACE TOKEN_PLUS TOKEN_SPACE binding
       | TOKEN_KEY
       | TOKEN_KEY TOKEN_SPACE TOKEN_PLUS TOKEN_SPACE binding
       ;

%%
