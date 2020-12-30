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
 *
 * The parser is not thread safe!
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <w32bindkeys/be.h>
#include <w32bindkeys/b.h>
#include <w32bindkeys/kc_sys.h>
#include <w32bindkeys/kbman.h>

#include "kc_director_factory.h"
#include "director.h"
#include "kc_director.h"
#include "kc_exec.h"
#include "parser_gen.h"
#include "lexer_gen.h"

static wbk_b_t *g_b = NULL;
static wbk_kc_t *g_kc = NULL;
static char *g_ws_text = NULL;
static char *g_exec_text = NULL;
static char g_word[1024];


static char *
add_c_to_s(char *modified_str, char new_c);

char *
add_s_to_s(char *modified_str, const char *new_s);

static int
add_to_g_b(wbk_mk_t modifier, char key);

static int
add_to_g_ws_text(char c);

static int
add_to_kbman(wbk_kbman_t *kbman);

static int
add_c_to_g_exec_text(char c);

static int
add_s_to_g_exec_text(const char *s);

char *
add_c_to_s(char *modified_str, char new_c)
{
   int length;

   if (modified_str == NULL) {
     length = 2;
     modified_str = malloc(sizeof(char) * length);
   } else {
     length = strlen(modified_str) + 1 + 1;
     modified_str = realloc(modified_str, sizeof(char) * length);
   }

   modified_str[length - 2] = new_c;
   modified_str[length - 1] = '\0';

   return modified_str;
}

char *
add_s_to_s(char *modified_str, const char *new_s)
{
   int offset;
   int length;

   offset = 0;
   length = 0;


   if (modified_str != NULL) {
     offset = strlen(modified_str);
     length += offset;
   }

   length += strlen(new_s);
   length += 1; /** \0 */

   if (modified_str) {
     modified_str = realloc(modified_str, sizeof(char) * length);
   } else {
     modified_str = malloc(sizeof(char) * length);
   }

   strcpy(modified_str + offset, new_s);

   return modified_str;
}

int
add_to_g_b(wbk_mk_t modifier, char key)
{
	wbk_be_t *be;

	if (g_b == NULL) {
		g_b = wbk_b_new();
	}

	be = wbk_be_new(modifier, key);
	wbk_b_add(g_b, be);
	wbk_be_free(be);

	return 0;
}

int
add_to_g_ws_text(char new_c)
{
  g_ws_text = add_c_to_s(g_ws_text, new_c);

  return 0;
}

int
add_to_kbman(wbk_kbman_t *kbman)
{
	wbk_kbman_add(kbman, g_kc);

	g_b = NULL;
	g_kc = NULL;
	if (g_ws_text) {
		free(g_ws_text);
		g_ws_text = NULL;
	}
}


int
add_c_to_g_exec_text(char new_c)
 {
   g_exec_text = add_c_to_s(g_exec_text, new_c);

   return 0;
}

int
add_s_to_g_exec_text(const char *new_s)
{
  g_exec_text = add_s_to_s(g_exec_text, new_s);

  return 0;
}

int
yyerror(b3_kc_director_factory_t **kc_director_factory, b3_director_t **director, wbk_kbman_t **kbman, yyscan_t scanner, const char *msg) {
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
%parse-param { b3_kc_director_factory_t **kc_director_factory }
%parse-param { b3_director_t **director }
%parse-param { wbk_kbman_t **kbman }
%parse-param { yyscan_t scanner }

%union {
	wbk_mk_t modifier;
	char key;
	char special;
}

%token <modifier>    TOKEN_MODIFIER
%token <key>         TOKEN_KEY
%token               TOKEN_PLUS
%token               TOKEN_BINDSYM
%token               TOKEN_MOVE
%token               TOKEN_FOCUS
%token               TOKEN_CONTAINER
%token               TOKEN_WORKSPACE
%token               TOKEN_UP
%token               TOKEN_DOWN
%token               TOKEN_LEFT
%token               TOKEN_RIGHT
%token               TOKEN_KILL
%token               TOKEN_FLOATING
%token               TOKEN_FULLSCREEN
%token               TOKEN_EXEC
%token               TOKEN_NO_STARTUP_ID
%token               TOKEN_TOGGLE
%token               TOKEN_TO
%token               TOKEN_OUTPUT
%token               TOKEN_COMMENT
%token               TOKEN_SPACE
%token <special>     TOKEN_SPECIAL
%token               TOKEN_EOL
%token               TOKEN_EOF

%%

statements: 
          // statement
            statement TOKEN_EOL statements
	  | TOKEN_EOL statement TOKEN_EOL statements
	  | statement
	  | statement TOKEN_EOL
	  | TOKEN_EOL statement
	  | TOKEN_EOL statement TOKEN_EOL
	  | TOKEN_EOL statements
          
          // comment
          | comment TOKEN_EOL statements
          | TOKEN_EOL comment TOKEN_EOL statements
	  | comment 
	  | comment TOKEN_EOL
          | TOKEN_EOL comment
          | TOKEN_EOL comment TOKEN_EOL

          // statements
	  | TOKEN_SPACE statements
          |
	  ;

comment: TOKEN_COMMENT ; 

statement: bindsym
	 ;

bindsym: TOKEN_BINDSYM TOKEN_SPACE binding TOKEN_SPACE command
         { add_to_kbman(*kbman); }
       ;

binding: TOKEN_MODIFIER
         { add_to_g_b($1, 0); }
       | TOKEN_MODIFIER TOKEN_PLUS binding
         { add_to_g_b($1, 0); }
       | TOKEN_KEY
         { add_to_g_b(NOT_A_MODIFIER, $1); }
       | TOKEN_KEY TOKEN_PLUS binding
         { add_to_g_b(NOT_A_MODIFIER, $1); }
       ;

command: cmd-focus
       | cmd-move
       | cmd-workspace
       | cmd-kill
       | cmd-floating
       | cmd-fullscreen
       | cmd-exec
       ;

cmd-focus: TOKEN_FOCUS TOKEN_SPACE cmd-focus-direction
         | TOKEN_FOCUS TOKEN_SPACE cmd-focus-output
         ;

cmd-focus-direction: TOKEN_UP
                     { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sawu(*kc_director_factory, g_b, *director); }
			       | TOKEN_DOWN
             { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sawd(*kc_director_factory, g_b, *director); }
		           | TOKEN_LEFT
			         { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sawl(*kc_director_factory, g_b, *director); }
                   | TOKEN_RIGHT
                   { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sawr(*kc_director_factory, g_b, *director); }
                   ;

cmd-focus-output: TOKEN_OUTPUT TOKEN_SPACE TOKEN_UP
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sfmu(*kc_director_factory, g_b, *director); }
                | TOKEN_OUTPUT TOKEN_SPACE TOKEN_DOWN
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sfmd(*kc_director_factory, g_b, *director); }
                | TOKEN_OUTPUT TOKEN_SPACE TOKEN_LEFT
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sfml(*kc_director_factory, g_b, *director); }
                | TOKEN_OUTPUT TOKEN_SPACE TOKEN_RIGHT
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sfmr(*kc_director_factory, g_b, *director); }
                ;

cmd-move: TOKEN_MOVE TOKEN_SPACE cmd-move-direction
        | TOKEN_MOVE TOKEN_SPACE cmd-move-container
        | TOKEN_MOVE TOKEN_SPACE cmd-move-workspace
        ;

cmd-move-direction: TOKEN_UP
                    { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawu(*kc_director_factory, g_b, *director); }
			      | TOKEN_DOWN
            { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawd(*kc_director_factory, g_b, *director); }
		          | TOKEN_LEFT
			        { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawl(*kc_director_factory, g_b, *director); }
                  | TOKEN_RIGHT
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawr(*kc_director_factory, g_b, *director); }
                  ;

cmd-move-container: TOKEN_CONTAINER TOKEN_SPACE TOKEN_TO TOKEN_SPACE cmd-move-container-output-direction
                  | TOKEN_CONTAINER TOKEN_SPACE TOKEN_TO TOKEN_SPACE cmd-move-container-workspace
                  ;

cmd-move-container-output-direction: TOKEN_OUTPUT TOKEN_SPACE TOKEN_UP
{ g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwtmu(*kc_director_factory, g_b, *director); }
			                       | TOKEN_OUTPUT TOKEN_SPACE TOKEN_DOWN
                             { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwtmd(*kc_director_factory, g_b, *director); }
			                       | TOKEN_OUTPUT TOKEN_SPACE TOKEN_LEFT
                             { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwtml(*kc_director_factory, g_b, *director); }
			                       | TOKEN_OUTPUT TOKEN_SPACE TOKEN_RIGHT
                             { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwtmr(*kc_director_factory, g_b, *director); }
			                       ;

cmd-move-container-workspace: TOKEN_WORKSPACE TOKEN_SPACE cmd-workspace-text
{ g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawtw(*kc_director_factory, g_b, *director, g_ws_text); }
			                ;

cmd-move-workspace: TOKEN_WORKSPACE TOKEN_SPACE TOKEN_TO TOKEN_SPACE TOKEN_OUTPUT TOKEN_SPACE cmd-move-workspace-direction
                  ;

cmd-move-workspace-direction: TOKEN_UP
			                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwu(*kc_director_factory, g_b, *director); }
			                | TOKEN_DOWN
			                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwd(*kc_director_factory, g_b, *director); }
			                | TOKEN_LEFT
			                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwl(*kc_director_factory, g_b, *director); }
			                | TOKEN_RIGHT
			                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwr(*kc_director_factory, g_b, *director); }
			                ;

cmd-workspace: TOKEN_WORKSPACE TOKEN_SPACE cmd-workspace-text
               { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_cw(*kc_director_factory, g_b, *director, g_ws_text); }
             ;

cmd-workspace-text: TOKEN_KEY
                    { add_to_g_ws_text($1); }
                  | cmd-workspace-text TOKEN_KEY[K]
                    { add_to_g_ws_text($K); }
                  ;

cmd-kill: TOKEN_KILL
          { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_caw(*kc_director_factory, g_b, *director); }
        ;

cmd-floating: TOKEN_FLOATING TOKEN_SPACE cmd-floating-toggle
            ;

cmd-floating-toggle: TOKEN_TOGGLE
                     { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_awtf(*kc_director_factory, g_b, *director); }
                   ;

cmd-fullscreen: TOKEN_FULLSCREEN TOKEN_SPACE cmd-fullscreen-toggle
              ;

cmd-fullscreen-toggle: TOKEN_TOGGLE
                       { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_tawf(*kc_director_factory, g_b, *director); }
                     ;

cmd-exec: TOKEN_EXEC TOKEN_SPACE TOKEN_NO_STARTUP_ID TOKEN_SPACE cmd-exec-text
{ g_kc = (wbk_kc_t *) b3_kc_exec_new(g_b, *director, ON_CURRENT_WS, g_exec_text); g_exec_text = NULL; }
| TOKEN_EXEC TOKEN_SPACE cmd-exec-text
{ g_kc = (wbk_kc_t *) b3_kc_exec_new(g_b, *director, ON_START_WS, g_exec_text); g_exec_text = NULL; }
        ;

cmd-exec-text: TOKEN_KEY
             { add_c_to_g_exec_text($1); }
             | cmd-exec-text TOKEN_KEY[K]
             { add_c_to_g_exec_text($K); }
             | TOKEN_SPACE
             { add_c_to_g_exec_text(' '); }
             | cmd-exec-text TOKEN_SPACE
             { add_c_to_g_exec_text(' '); }
             | TOKEN_SPECIAL
             { add_c_to_g_exec_text($1); }
             | cmd-exec-text TOKEN_SPECIAL[S]
             { add_c_to_g_exec_text($S); }
             | word-in-text
             { add_s_to_g_exec_text(g_word); }
             | cmd-exec-text word-in-text
             { add_s_to_g_exec_text(g_word); }
             ;

word-in-text: TOKEN_TOGGLE
              { strcpy(g_word, "toggle"); }
            | TOKEN_BINDSYM
              { strcpy(g_word, "bindsym"); }
            | TOKEN_MOVE
              { strcpy(g_word, "move"); }
            | TOKEN_FOCUS
              { strcpy(g_word, "focus"); }
            | TOKEN_CONTAINER
              { strcpy(g_word, "container"); }
            | TOKEN_WORKSPACE
              { strcpy(g_word, "workspace"); }
            | TOKEN_UP
              { strcpy(g_word, "up"); }
            | TOKEN_DOWN
              { strcpy(g_word, "down"); }
            | TOKEN_LEFT
              { strcpy(g_word, "left"); }
            | TOKEN_RIGHT
              { strcpy(g_word, "right"); }
            | TOKEN_KILL
              { strcpy(g_word, "kill"); }
            | TOKEN_FLOATING
              { strcpy(g_word, "floating"); }
            | TOKEN_FULLSCREEN
              { strcpy(g_word, "fullscreen"); }
            | TOKEN_EXEC
              { strcpy(g_word, "exec"); }
            | TOKEN_NO_STARTUP_ID
              { strcpy("--no-startup-id", g_word); }
            | TOKEN_TOGGLE
              { strcpy(g_word, "toggle"); }
            | TOKEN_TO
              { strcpy(g_word, "to"); }
            | TOKEN_OUTPUT
              { strcpy(g_word, "output"); }
            ;

%%
