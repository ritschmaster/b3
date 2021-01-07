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

/**
 * @author Richard Bäck <richard.baeck@mailbox.org>
 * @date 2020-08-04
 * @brief File contains the tests for the parser class
 */

#include "../src/parser.h"

#include <w32bindkeys/datafinder.h>

#include "test.h"

#include "../src/win_factory.h"
#include "../src/ws_factory.h"
#include "../src/wsman_factory.h"
#include "../src/monitor_factory.h"
#include "../src/kc_director_factory.h"
#include "../src/condition_factory.h"
#include "../src/action_factory.h"

static wbk_datafinder_t *g_datafinder;

static b3_parser_t *g_parser = NULL;
static b3_director_t *g_director;
static b3_win_factory_t *g_win_factory;
static b3_ws_factory_t *g_ws_factory;
static b3_wsman_factory_t *g_wsman_factory;
static b3_monitor_factory_t *g_monitor_factory;
static b3_kc_director_factory_t *g_kc_director_factory;
static b3_condition_factory_t *g_condition_factory;
static b3_action_factory_t *g_action_factory;

static void
setup(void)
{
	g_datafinder = wbk_datafinder_new(TESTDATADIR);

	g_win_factory = b3_win_factory_new();
	g_ws_factory = b3_ws_factory_new();
	g_wsman_factory = b3_wsman_factory_new(g_ws_factory);
	g_monitor_factory = b3_monitor_factory_new(g_wsman_factory);
	g_kc_director_factory = b3_kc_director_factory_new();
	g_condition_factory = b3_condition_factory_new();
	g_action_factory = b3_action_factory_new();
	g_parser = b3_parser_new(g_kc_director_factory, g_condition_factory, g_action_factory);
	g_director = b3_director_new(g_monitor_factory);
}

static void
teardown(void)
{
	b3_director_free(g_director);
	b3_parser_free(g_parser);
	b3_kc_director_factory_free(g_kc_director_factory);
	b3_monitor_factory_free(g_monitor_factory);
	b3_wsman_factory_free(g_wsman_factory);
	b3_ws_factory_free(g_ws_factory);
	b3_win_factory_free(g_win_factory);

	wbk_datafinder_free(g_datafinder);
}

static int
test_parse_str_empty(void)
{
	wbk_kbman_t *kbman;

	kbman = b3_parser_parse_str(g_parser, g_director, "");

	if (kbman == NULL) {
		return 1;
	}

	wbk_kbman_free(kbman);

	return 0;
}

static int
test_parse_str_none(void)
{
	wbk_kbman_t *kbman;

	kbman = b3_parser_parse_str(g_parser, g_director, "\n \n   \n\n\t\t\n");

	if (kbman == NULL) {
		return 1;
	}

	wbk_kbman_free(kbman);

	return 0;
}

static int
test_parse_str_rules(void)
{
	wbk_kbman_t *kbman;
  char config[] = "for_window [title=\".*Microsoft Teams.*\"] floating enable\n"
    "\t\t\n    \n"
    "for_window [class=\"CabinetWClass\"] floating enable";

  kbman = b3_parser_parse_str(g_parser, g_director, config);

  if (kbman == NULL) {
		return 1;
	}

	wbk_kbman_free(kbman);

	return 0;
}

static int
test_parse_str(void)
{
	wbk_kbman_t *kbman;
	char config[] = "bindsym Mod1 move up\n"
			        "bindsym MOD4+b move left\n"
			        "bindsym c move right\n"
			        "bindsym c+a move down\n";

	kbman = b3_parser_parse_str(g_parser, g_director, config);

	if (kbman == NULL) {
		return 1;
	}

	wbk_kbman_free(kbman);

	return 0;
}

static int
test_parse_file_empty(void)
{
	char *filename;
	FILE *config_file;
	wbk_kbman_t *kbman;

	filename = wbk_datafinder_gen_path(g_datafinder, "empty.config");
	config_file = fopen(filename, "r");
	kbman = b3_parser_parse_file(g_parser, g_director, config_file);
	fclose(config_file);
	free(filename);

	if (kbman == NULL) {
		return 1;
	}

	wbk_kbman_free(kbman);

	return 0;
}

static int
test_parse_file_none(void)
{
	char *filename;
	FILE *config_file;
	wbk_kbman_t *kbman;

	filename = wbk_datafinder_gen_path(g_datafinder, "none.config");
	config_file = fopen(filename, "r");
	kbman = b3_parser_parse_file(g_parser, g_director, config_file);
	fclose(config_file);
	free(filename);

	if (kbman == NULL) {
		return 1;
	}

	wbk_kbman_free(kbman);

	return 0;
}

static int
test_parse_file_rules(void)
{
	char *filename;
	FILE *config_file;
	wbk_kbman_t *kbman;

	filename = wbk_datafinder_gen_path(g_datafinder, "rules.config");
	config_file = fopen(filename, "r");
	kbman = b3_parser_parse_file(g_parser, g_director, config_file);
	fclose(config_file);
	free(filename);

	if (kbman == NULL) {
		return 1;
	}

	wbk_kbman_free(kbman);

	return 0;
}

static int
test_parse_file(void)
{
	char *filename;
	FILE *config_file;
	wbk_kbman_t *kbman;

	filename = wbk_datafinder_gen_path(g_datafinder, "full.config");
	config_file = fopen(filename, "r");
	kbman = b3_parser_parse_file(g_parser, g_director, config_file);
	fclose(config_file);
	free(filename);

	if (kbman == NULL) {
		return 1;
	}

	wbk_kbman_free(kbman);

	return 0;
}

int
main(void)
{
	b3_test(setup, teardown, test_parse_str_empty);
	b3_test(setup, teardown, test_parse_str_none);
	b3_test(setup, teardown, test_parse_str_rules);
	b3_test(setup, teardown, test_parse_str);
	b3_test(setup, teardown, test_parse_file_empty);
	b3_test(setup, teardown, test_parse_file_none);
	b3_test(setup, teardown, test_parse_file_rules);
	b3_test(setup, teardown, test_parse_file);

	return 0;
}
