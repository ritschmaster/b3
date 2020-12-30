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
 * @brief File contains the lexical analyzator definition
 *
 * The lexical analyzator is not thread safe!
 */

#include <string.h>
#include <w32bindkeys/be.h>
#include <w32bindkeys/kbman.h>

#include "kc_director_factory.h"
#include "director.h"
#include "parser_gen.h"

static wbk_mk_t
get_modifier(char *str);

static wbk_mk_t
get_modifier(char *str)
{
	char *str_copy;
	int length;
	int i;
	wbk_mk_t modifier;

	length = strlen(str) + 1;
	str_copy = malloc(sizeof(char) * length);
	for (i = 0; i < length; i++) {
		str_copy[i] = tolower(str[i]);
	}

	if (strcmp(str_copy, "mod4") == 0) {
		modifier = WIN;
	} else if (strcmp(str_copy, "mod1") == 0) {
		modifier = ALT;
	} else if (strcmp(str_copy, "shift") == 0) {
		modifier = SHIFT;
	} else if (strcmp(str_copy, "space") == 0) {
		modifier = SPACE;
	} else if (strcmp(str_copy, "ctrl") == 0) {
		modifier = CTRL;
	} else if (strcmp(str_copy, "return") == 0) {
	  modifier = ENTER;
	} else if (strcmp(str_copy, "f1") == 0) {
		modifier = F1;
	} else if (strcmp(str_copy, "f2") == 0) {
		modifier = F2;
	} else if (strcmp(str_copy, "f3") == 0) {
		modifier = F3;
	} else if (strcmp(str_copy, "f4") == 0) {
		modifier = F4;
	} else if (strcmp(str_copy, "f5") == 0) {
		modifier = F5;
	} else if (strcmp(str_copy, "f6") == 0) {
		modifier = F6;
	} else if (strcmp(str_copy, "f7") == 0) {
		modifier = F7;
	} else if (strcmp(str_copy, "f8") == 0) {
		modifier = F8;
	} else if (strcmp(str_copy, "f9") == 0) {
		modifier = F9;
	} else if (strcmp(str_copy, "f10") == 0) {
		modifier = F10;
	} else if (strcmp(str_copy, "f11") == 0) {
		modifier = F11;
	} else if (strcmp(str_copy, "f12") == 0) {
		modifier = F12;
	} else {
		modifier = NOT_A_MODIFIER;
	}

	return modifier;
}


%}

%option outfile="lexer_gen.c" header-file="lexer_gen.h"
%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

MODIFIER        ([mM][oO][dD]1|[mM][oO][dD]4|[sS][hH][iI][fF][tT]|[sS][pP][aA][cC][eE]|[cC][tT][rR][lL]|[rR][eE][tT][uU][rR][nN]|[fF]([1-9]|10|11|12))
KEY             [a-zA-Z0-9]
PLUS            [+]
BINDSYM         bindsym
MOVE            move
FOCUS           focus
CONTAINER       container
WORKSPACE       workspace
UP              up
DOWN            down
LEFT            left
RIGHT           right
KILL            kill
FLOATING        floating
FULLSCREEN      fullscreen
EXEC            exec
NO_STARTUP_ID   --no-startup-id
TOGGLE          toggle
TO              to
OUTPUT          output
COMMENT         #.*
SPACE           [ \t]+
SPECIAL         [!"§\$%&/{\(\[\]\)=}\?\\`´\*\+\~'#,;\.:\-_\^°\<\>\|]
EOL             [\n]+
EOF             \Z

%%

{MODIFIER}               { yylval->modifier = get_modifier(yytext); return TOKEN_MODIFIER; }
{KEY}                    { yylval->key = tolower(yytext[0]); return TOKEN_KEY; }
{PLUS}                   { return TOKEN_PLUS; }
{BINDSYM}                { return TOKEN_BINDSYM; }
{MOVE}                   { return TOKEN_MOVE; }
{FOCUS}                  { return TOKEN_FOCUS; }
{CONTAINER}              { return TOKEN_CONTAINER; }
{WORKSPACE}              { return TOKEN_WORKSPACE; }
{UP}                     { return TOKEN_UP; }
{DOWN}                   { return TOKEN_DOWN; }
{LEFT}                   { return TOKEN_LEFT; }
{RIGHT}                  { return TOKEN_RIGHT; }
{KILL}                   { return TOKEN_KILL; }
{FLOATING}               { return TOKEN_FLOATING; }
{FULLSCREEN}             { return TOKEN_FULLSCREEN; }
{EXEC}                   { return TOKEN_EXEC; }
{NO_STARTUP_ID}          { return TOKEN_NO_STARTUP_ID; }
{TOGGLE}                 { return TOKEN_TOGGLE; }
{TO}                     { return TOKEN_TO; }
{OUTPUT}                 { return TOKEN_OUTPUT; }
{COMMENT}                { return TOKEN_COMMENT; }
{SPACE}                  { return TOKEN_SPACE; }
{SPECIAL}                { yylval->special = yytext[0]; return TOKEN_SPECIAL; }
{EOL}                    { return TOKEN_EOL; }
{EOF}                    { return 0; }

%%



