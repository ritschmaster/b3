################################################################################
# This file is part of b3.
#
# Copyright 2020-2021 Richard Paul Baeck <richard.baeck@mailbox.org>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################

AM_YFLAGS = -d

if DEBUG
AM_CFLAGS = -O0
AM_CFLAGS += -D DEBUG_ENABLED=1
AM_LDFLAGS = -g -O0
else
AM_CFLAGS = -O2
AM_LDFLAGS = -O2
endif

bin_PROGRAMS = b3

noinst_LTLIBRARIES = libb3parser.la
noinst_LTLIBRARIES += libb3interpreter.la


BUILT_SOURCES = lexer_gen.h
BUILT_SOURCES += lexer_gen.c
BUILT_SOURCES += parser_gen.h
BUILT_SOURCES += parser_gen.c

CLEANFILES = lexer_gen.h
CLEANFILES += lexer_gen.c
CLEANFILES += parser_gen.h
CLEANFILES += parser_gen.c

lexer_gen.h: Makefile
parser_gen.h: Makefile


libb3interpreter_la_SOURCES = director.c director.h
libb3interpreter_la_SOURCES += ws_switcher.c ws_switcher.h
libb3interpreter_la_SOURCES += director_ws_switcher.c director_ws_switcher.h
libb3interpreter_la_SOURCES += rule.c rule.h
libb3interpreter_la_SOURCES += condition.c condition.h
libb3interpreter_la_SOURCES += condition_and.c condition_and.h
libb3interpreter_la_SOURCES += pattern_condition.c pattern_condition.h
libb3interpreter_la_SOURCES += title_condition.c title_condition.h
libb3interpreter_la_SOURCES += class_condition.c class_condition.h
libb3interpreter_la_SOURCES += condition_factory.c condition_factory.h
libb3interpreter_la_SOURCES += action.c action.h
libb3interpreter_la_SOURCES += action_list.c action_list.h
libb3interpreter_la_SOURCES += floating_action.c floating_action.h
libb3interpreter_la_SOURCES += mwtw_action.c mwtw_action.h
libb3interpreter_la_SOURCES += action_factory.c action_factory.h
libb3interpreter_la_SOURCES += kc_director.c kc_director.h
libb3interpreter_la_SOURCES += kc_director_factory.c kc_director_factory.h
libb3interpreter_la_SOURCES += kc_exec.c kc_exec.h
libb3interpreter_la_SOURCES += ws.c ws.h
libb3interpreter_la_SOURCES += ws_factory.c ws_factory.h
libb3interpreter_la_SOURCES += wsman.c wsman.h
libb3interpreter_la_SOURCES += wsman_factory.c wsman_factory.h
libb3interpreter_la_SOURCES += monitor.c monitor.h
libb3interpreter_la_SOURCES += monitor_factory.c monitor_factory.h
libb3interpreter_la_SOURCES += winman.c winman.h
libb3interpreter_la_SOURCES += win.c win.h
libb3interpreter_la_SOURCES += win_factory.c win_factory.h
libb3interpreter_la_SOURCES += win_watcher.c win_watcher.h
libb3interpreter_la_SOURCES += bar.c bar.h
libb3interpreter_la_SOURCES += til.c til.h
libb3interpreter_la_SOURCES += counter.c counter.h
libb3interpreter_la_SOURCES += utils.c utils.h

libb3interpreter_la_CFLAGS = $(AM_CFLAGS)
libb3interpreter_la_CFLAGS += @collectionc_CFLAGS@
libb3interpreter_la_CFLAGS += @libw32bindkeys_CFLAGS@
libb3interpreter_la_CFLAGS += @libpcre_CFLAGS@

libb3interpreter_la_LDFLAGS = $(AM_LDFLAGS)
libb3interpreter_la_LDFLAGS += -mwindows
libb3interpreter_la_LDFLAGS += -static

libb3interpreter_la_LIBADD = @collectionc_LIBS@
libb3interpreter_la_LIBADD += @libw32bindkeys_LIBS@
libb3interpreter_la_LIBADD += @libpcre_LIBS@


nodist_libb3parser_la_SOURCES = lexer_gen.h lexer_gen.c
nodist_libb3parser_la_SOURCES += parser_gen.h parser_gen.c

libb3parser_la_SOURCES = lexer_gen.l lexer_gen.h lexer_gen.c
libb3parser_la_SOURCES += parser_gen.y parser_gen.h parser_gen.c
libb3parser_la_SOURCES += parser.h parser.c

libb3parser_la_CFLAGS = $(AM_CFLAGS)
libb3parser_la_CFLAGS += @collectionc_CFLAGS@

libb3parser_la_LDFLAGS = $(AM_LDFLAGS)
libb3parser_la_LDFLAGS += -mwindows
libb3parser_la_LDFLAGS += -static

libb3parser_la_LIBADD = libb3interpreter.la
libb3parser_la_LIBADD += @collectionc_LIBS@


b3_SOURCES = main.c

b3_CFLAGS = $(AM_CFLAGS)
b3_CFLAGS += @libw32bindkeys_CFLAGS@

b3_LDFLAGS = $(AM_LDFLAGS)
b3_LDFLAGS += -mwindows
b3_LDFLAGS += -static

b3_LDADD = libb3interpreter.la
b3_LDADD += libb3parser.la
b3_LDADD += @libw32bindkeys_LIBS@
