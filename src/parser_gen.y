/******************************************************************************
  This file is part of b3.

  Copyright 2020-2021 Richard Paul Baeck <richard.baeck@mailbox.org>

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
#include <w32bindkeys/logger.h>
#include <w32bindkeys/be.h>
#include <w32bindkeys/b.h>
#include <w32bindkeys/kc_sys.h>
#include <w32bindkeys/kbman.h>

#include "utils.h"
#include "kc_director_factory.h"
#include "condition_factory.h"
#include "action_factory.h"
#include "director.h"
#include "kc_director.h"
#include "kc_exec.h"
#include "parser_gen.h"
#include "lexer_gen.h"
#include "condition_and.h"
#include "action_list.h"
#include "rule.h"

#define B3_WORD_BUFFER_LEN 256

static wbk_logger_t logger = { "parser_gen" };

static wbk_b_t *g_b = NULL;
static wbk_kc_t *g_kc = NULL;
static char *g_text = NULL;
static char g_word[B3_WORD_BUFFER_LEN];
static b3_condition_and_t *g_condition_and = NULL;
static b3_condition_t *g_condition = NULL;
static b3_action_list_t *g_action_list = NULL;
static b3_action_t *g_action = NULL;

static wbk_mk_t
get_modifier(char *str);

static int
add_to_g_b(wbk_mk_t modifier, char key);

static int
add_to_kbman(wbk_kbman_t *kbman);

/**
 * Adds condition to condition_and and returns condition_and. If condition_and is NULL,
 * then a new condition and is allocated.
 */
static b3_condition_and_t *
add_to_condition_and(b3_condition_factory_t *condition_factory,
                     b3_condition_and_t *condition_and,
                     b3_condition_t *condition);

/**
 * Adds action to action_list and returns action_list. If action_list is NULL,
 * then a new action list is allocated.
 */
static b3_action_list_t *
add_to_action_list(b3_action_factory_t *action_factory,
                   b3_action_list_t *action_list,
                   b3_action_t *action);

static wbk_mk_t
get_modifier(char *str);

wbk_mk_t
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
add_to_kbman(wbk_kbman_t *kbman)
{
	wbk_kbman_add(kbman, g_kc);

	g_b = NULL;
	g_kc = NULL;
}

b3_condition_and_t *
add_to_condition_and(b3_condition_factory_t *condition_factory,
                     b3_condition_and_t *condition_and,
                     b3_condition_t *condition)
 {
   if (condition_and == NULL) {
     condition_and = b3_condition_factory_create_and(condition_factory);
   }

   b3_condition_and_add(condition_and, condition);

   return condition_and;
}

b3_action_list_t *
add_to_action_list(b3_action_factory_t *action_factory,
                   b3_action_list_t *action_list,
                   b3_action_t *action)
{
  if (action_list == NULL) {
    action_list = b3_action_factory_create_list(action_factory);
  }

  b3_action_list_add(action_list, action);

  return action_list;
}

int
yyerror(b3_kc_director_factory_t **kc_director_factory,
        b3_condition_factory_t **condition_factory,
        b3_action_factory_t **action_factory,
        b3_director_t **director,
        wbk_kbman_t **kbman,
        yyscan_t scanner,
        const char *msg) {
  wbk_logger_log(&logger, SEVERE, "Error during parsing the configuration file: %s\n", msg);

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
%parse-param { b3_condition_factory_t **condition_factory }
%parse-param { b3_action_factory_t **action_factory }
%parse-param { b3_director_t **director }
%parse-param { wbk_kbman_t **kbman }
%parse-param { yyscan_t scanner }

%union {
  char modifier_str[256];

	char key;
	char special;
}

%token <modifier_str>TOKEN_MODIFIER
%token <key>         TOKEN_KEY
%token               TOKEN_PLUS
%token               TOKEN_BRACKET_OPEN
%token               TOKEN_BRACKET_CLOSE
%token               TOKEN_EQUAL
%token               TOKEN_DOUBLE_QUOTES
%token               TOKEN_COMMA
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
%token               TOKEN_ENABLE
%token               TOKEN_FULLSCREEN
%token               TOKEN_EXEC
%token               TOKEN_SPLIT
%token               TOKEN_NO_STARTUP_ID
%token               TOKEN_TOGGLE
%token               TOKEN_TO
%token               TOKEN_OUTPUT
%token               TOKEN_FOR_WINDOW
%token               TOKEN_TITLE
%token               TOKEN_CLASS
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

statement:
bindsym
| for_window 
;

bindsym: TOKEN_BINDSYM TOKEN_SPACE binding TOKEN_SPACE bindsym-cmd
         { add_to_kbman(*kbman); }
       ;

binding: TOKEN_MODIFIER
         { add_to_g_b(get_modifier($1), 0); }
       | TOKEN_MODIFIER TOKEN_PLUS binding
         { add_to_g_b(get_modifier($1), 0); }
       | TOKEN_KEY
         { add_to_g_b(NOT_A_MODIFIER, tolower($1)); }
       | TOKEN_KEY TOKEN_PLUS binding
         { add_to_g_b(NOT_A_MODIFIER, tolower($1)); }
       ;

bindsym-cmd: bindsym-cmd-focus
       | bindsym-cmd-move
       | bindsym-cmd-workspace
       | bindsym-cmd-kill
       | bindsym-cmd-floating
       | bindsym-cmd-fullscreen
       | bindsym-cmd-exec
       | bindsym-cmd-split
       ;

bindsym-cmd-focus: TOKEN_FOCUS TOKEN_SPACE bindsym-cmd-focus-direction
         | TOKEN_FOCUS TOKEN_SPACE bindsym-cmd-focus-output
         ;

bindsym-cmd-focus-direction: TOKEN_UP
                     { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sawu(*kc_director_factory, g_b, *director); }
			       | TOKEN_DOWN
             { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sawd(*kc_director_factory, g_b, *director); }
		           | TOKEN_LEFT
			         { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sawl(*kc_director_factory, g_b, *director); }
                   | TOKEN_RIGHT
                   { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sawr(*kc_director_factory, g_b, *director); }
                   ;

bindsym-cmd-focus-output: TOKEN_OUTPUT TOKEN_SPACE TOKEN_UP
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sfmu(*kc_director_factory, g_b, *director); }
                | TOKEN_OUTPUT TOKEN_SPACE TOKEN_DOWN
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sfmd(*kc_director_factory, g_b, *director); }
                | TOKEN_OUTPUT TOKEN_SPACE TOKEN_LEFT
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sfml(*kc_director_factory, g_b, *director); }
                | TOKEN_OUTPUT TOKEN_SPACE TOKEN_RIGHT
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sfmr(*kc_director_factory, g_b, *director); }
                ;

bindsym-cmd-move: TOKEN_MOVE TOKEN_SPACE bindsym-cmd-move-direction
        | TOKEN_MOVE TOKEN_SPACE bindsym-cmd-move-container
        | TOKEN_MOVE TOKEN_SPACE bindsym-cmd-move-workspace
        ;

bindsym-cmd-move-direction: TOKEN_UP
                    { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawu(*kc_director_factory, g_b, *director); }
			      | TOKEN_DOWN
            { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawd(*kc_director_factory, g_b, *director); }
		          | TOKEN_LEFT
			        { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawl(*kc_director_factory, g_b, *director); }
                  | TOKEN_RIGHT
                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawr(*kc_director_factory, g_b, *director); }
                  ;

bindsym-cmd-move-container: TOKEN_CONTAINER TOKEN_SPACE TOKEN_TO TOKEN_SPACE bindsym-cmd-move-container-output-direction
                  | TOKEN_CONTAINER TOKEN_SPACE TOKEN_TO TOKEN_SPACE bindsym-cmd-move-container-workspace
                  ;

bindsym-cmd-move-container-output-direction: TOKEN_OUTPUT TOKEN_SPACE TOKEN_UP
{ g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwtmu(*kc_director_factory, g_b, *director); }
			                       | TOKEN_OUTPUT TOKEN_SPACE TOKEN_DOWN
                             { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwtmd(*kc_director_factory, g_b, *director); }
			                       | TOKEN_OUTPUT TOKEN_SPACE TOKEN_LEFT
                             { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwtml(*kc_director_factory, g_b, *director); }
			                       | TOKEN_OUTPUT TOKEN_SPACE TOKEN_RIGHT
                             { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwtmr(*kc_director_factory, g_b, *director); }
			                       ;

bindsym-cmd-move-container-workspace: TOKEN_WORKSPACE TOKEN_SPACE text
{ g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mawtw(*kc_director_factory, g_b, *director, g_text); free(g_text); g_text = NULL; }
			                        ;

bindsym-cmd-move-workspace: TOKEN_WORKSPACE TOKEN_SPACE TOKEN_TO TOKEN_SPACE TOKEN_OUTPUT TOKEN_SPACE bindsym-cmd-move-workspace-direction
                  ;

bindsym-cmd-move-workspace-direction: TOKEN_UP
			                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwu(*kc_director_factory, g_b, *director); }
			                | TOKEN_DOWN
			                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwd(*kc_director_factory, g_b, *director); }
			                | TOKEN_LEFT
			                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwl(*kc_director_factory, g_b, *director); }
			                | TOKEN_RIGHT
			                  { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_mfwr(*kc_director_factory, g_b, *director); }
			                ;

bindsym-cmd-workspace: TOKEN_WORKSPACE TOKEN_SPACE text
{ g_kc = (wbk_kc_t *) b3_kc_director_factory_create_cw(*kc_director_factory, g_b, *director, g_text); free(g_text); g_text = NULL; }
             ;

bindsym-cmd-kill: TOKEN_KILL
          { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_caw(*kc_director_factory, g_b, *director); }
        ;

bindsym-cmd-floating: TOKEN_FLOATING TOKEN_SPACE bindsym-cmd-floating-toggle
            ;

bindsym-cmd-floating-toggle: TOKEN_TOGGLE
                     { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_awtf(*kc_director_factory, g_b, *director); }
                   ;

bindsym-cmd-fullscreen: TOKEN_FULLSCREEN TOKEN_SPACE bindsym-cmd-fullscreen-toggle
              ;

bindsym-cmd-fullscreen-toggle: TOKEN_TOGGLE
                       { g_kc = (wbk_kc_t *) b3_kc_director_factory_create_tawf(*kc_director_factory, g_b, *director); }
                     ;

bindsym-cmd-exec: TOKEN_EXEC TOKEN_SPACE TOKEN_NO_STARTUP_ID TOKEN_SPACE text
        { g_kc = (wbk_kc_t *) b3_kc_exec_new(g_b, *director, ON_CURRENT_WS, g_text); g_text = NULL; }
        | TOKEN_EXEC TOKEN_SPACE text
        { g_kc = (wbk_kc_t *) b3_kc_exec_new(g_b, *director, ON_START_WS, g_text); g_text = NULL; }
        ;

bindsym-cmd-split: TOKEN_SPLIT TOKEN_SPACE TOKEN_KEY
{
  char msg[256];

  switch($3) {
  case 'h':
    g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sh(*kc_director_factory, g_b, *director);
    break;

  case 'v':
    g_kc = (wbk_kc_t *) b3_kc_director_factory_create_sv(*kc_director_factory, g_b, *director);
    break;

  default:
      sprintf(msg, "Unexpected token: %c", $3);
      yyerror(*kc_director_factory, *condition_factory, *action_factory, *director, *kbman, scanner, msg);
      YYERROR;
  }
}
                  ;

for_window:
  TOKEN_FOR_WINDOW TOKEN_SPACE TOKEN_BRACKET_OPEN for_window-conditions TOKEN_BRACKET_CLOSE TOKEN_SPACE for_window-actions
  { b3_director_add_rule(*director, b3_rule_new((b3_condition_t *) g_condition_and, (b3_action_t *) g_action_list)); g_condition_and = NULL; g_action_list = NULL; }
;

for_window-conditions:
  for_window-condition
  { g_condition_and = add_to_condition_and(*condition_factory, g_condition_and, g_condition); g_condition = NULL; }
| for_window-conditions TOKEN_SPACE for_window-condition
  { g_condition_and = add_to_condition_and(*condition_factory, g_condition_and, g_condition); g_condition = NULL; }
;

for_window-condition:
  TOKEN_TITLE TOKEN_EQUAL TOKEN_DOUBLE_QUOTES text TOKEN_DOUBLE_QUOTES
  { g_condition = (b3_condition_t *) b3_condition_factory_create_tc(*condition_factory, g_text); free(g_text); g_text = NULL; }
| TOKEN_CLASS TOKEN_EQUAL TOKEN_DOUBLE_QUOTES text TOKEN_DOUBLE_QUOTES
{ g_condition = (b3_condition_t *) b3_condition_factory_create_cc(*condition_factory, g_text); free(g_text); g_text = NULL; }
;

for_window-actions:
  for_window-action
  { g_action_list = add_to_action_list(*action_factory, g_action_list, g_action); g_action = NULL; }
| for_window-actions TOKEN_COMMA TOKEN_SPACE for_window-action
  { g_action_list = add_to_action_list(*action_factory, g_action_list, g_action); g_action = NULL; }
;

for_window-action:
  TOKEN_FLOATING TOKEN_SPACE TOKEN_ENABLE
  { g_action = (b3_action_t *) b3_action_factory_create_fa(*action_factory); }
| action-command
;

action-command:
  action-cmd-move
;

action-cmd-move:
//  TOKEN_MOVE TOKEN_SPACE bindsym-cmd-move-direction
  TOKEN_MOVE TOKEN_SPACE action-cmd-move-container
  //| TOKEN_MOVE TOKEN_SPACE bindsym-cmd-move-workspace
;

action-cmd-move-container:
// TOKEN_CONTAINER TOKEN_SPACE TOKEN_TO TOKEN_SPACE bindsym-cmd-move-container-output-direction
  //| TOKEN_CONTAINER TOKEN_SPACE TOKEN_TO TOKEN_SPACE bindsym-cmd-move-container-workspace
 TOKEN_CONTAINER TOKEN_SPACE TOKEN_TO TOKEN_SPACE action-cmd-move-container-workspace
;

action-cmd-move-container-workspace:
  TOKEN_WORKSPACE TOKEN_SPACE text
  { g_action = (b3_action_t *) b3_action_factory_create_mwtw(*action_factory, g_text); g_text = NULL; }
;

text: TOKEN_KEY
    { g_text = b3_add_c_to_s(g_text, $1); }
    | text TOKEN_KEY[K]
    { g_text = b3_add_c_to_s(g_text, $K); }
    | TOKEN_SPACE
    { g_text = b3_add_c_to_s(g_text, ' '); }
    | text TOKEN_SPACE
    { g_text = b3_add_c_to_s(g_text, ' '); }
    | TOKEN_PLUS
    { g_text = b3_add_c_to_s(g_text, '+'); }
    | text TOKEN_PLUS
    { g_text = b3_add_c_to_s(g_text, '+'); }
    | TOKEN_BRACKET_OPEN
    { g_text = b3_add_c_to_s(g_text, '['); }
    | text TOKEN_BRACKET_OPEN
    { g_text = b3_add_c_to_s(g_text, '['); }
    | TOKEN_BRACKET_CLOSE
    { g_text = b3_add_c_to_s(g_text, ']'); }
    | text TOKEN_BRACKET_CLOSE
    { g_text = b3_add_c_to_s(g_text, ']'); }
    | TOKEN_EQUAL
    { g_text = b3_add_c_to_s(g_text, '='); }
    | text TOKEN_EQUAL
    { g_text = b3_add_c_to_s(g_text, '='); }
    | TOKEN_DOUBLE_QUOTES
    { g_text = b3_add_c_to_s(g_text, '"'); }
    | text TOKEN_DOUBLE_QUOTES
    { g_text = b3_add_c_to_s(g_text, ','); }
    | TOKEN_COMMA
    { g_text = b3_add_c_to_s(g_text, ','); }
    | text TOKEN_COMMA
    { g_text = b3_add_c_to_s(g_text, '='); }
    | TOKEN_SPECIAL
    { g_text = b3_add_c_to_s(g_text, $1); }
    | text TOKEN_SPECIAL[S]
    { g_text = b3_add_c_to_s(g_text, $S); }
    | word-in-text
    { g_text = b3_add_s_to_s(g_text, g_word); }
    | text word-in-text
    { g_text = b3_add_s_to_s(g_text, g_word); }
    ;

word-in-text: TOKEN_MODIFIER
              { strcpy(g_word, $1); }
            | TOKEN_TOGGLE
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
            | TOKEN_ENABLE
              { strcpy(g_word, "enable"); }
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
            | TOKEN_FOR_WINDOW
              { strcpy(g_word, "for_window"); }
            | TOKEN_TITLE
              { strcpy(g_word, "title"); }
            | TOKEN_CLASS
              { strcpy(g_word, "class"); }
            ;

%%
