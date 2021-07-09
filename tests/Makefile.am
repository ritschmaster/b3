################################################################################
# This file is part of b3.
#
# Copyright 2020 Richard Paul Baeck <richard.baeck@mailbox.org>
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


#AM_CFLAGS = -I$(top_builddir)/src
AM_CFLAGS = -D TESTDATADIR=\"$(top_builddir)/tests\" -Wl,--allow-multiple-definition

if DEBUG
AM_CFLAGS += -g -O0
AM_LDFLAGS = -g -O0
else
AM_CFLAGS += -O2
AM_LDFLAGS = -O2
endif

TESTS = test_parser
TESTS += test_winman
TESTS += test_ws

check_PROGRAMS = test_parser
check_PROGRAMS += test_winman
check_PROGRAMS += test_ws

noinst_LTLIBRARIES = libb3test.la

libb3test_la_SOURCES = test.h
libb3test_la_SOURCES += test.c
libb3test_la_CFLAGS = @libw32bindkeys_CFLAGS@
libb3test_la_LIBADD = @libw32bindkeys_LIBS@

test_parser_SOURCES = test_parser.c
test_parser_CFLAGS = $(AM_CFLAGS)
test_parser_CFLAGS += @libw32bindkeys_CFLAGS@
test_parser_CFLAGS += @collectionc_CFLAGS@
test_parser_LDFLAGS = $(AM_LDFLAGS)
test_parser_LDFLAGS += -mwindows
test_parser_LDFLAGS += -static
test_parser_LDADD = libb3test.la
test_parser_LDADD += $(top_builddir)/src/libb3interpreter.la
test_parser_LDADD += $(top_builddir)/src/libb3parser.la
test_parser_LDADD += @libw32bindkeys_LIBS@
test_parser_LDADD += @collectionc_LIBS@

test_winman_SOURCES = test_winman.c
test_winman_CFLAGS = $(AM_CFLAGS)
test_winman_CFLAGS += @libw32bindkeys_CFLAGS@
test_winman_CFLAGS += @collectionc_CFLAGS@
test_winman_LDFLAGS = $(AM_LDFLAGS)
test_winman_LDFLAGS += -mwindows
test_winman_LDFLAGS += -static
test_winman_LDADD = libb3test.la
test_winman_LDADD += $(top_builddir)/src/libb3interpreter.la
test_winman_LDADD += $(top_builddir)/src/libb3parser.la
test_winman_LDADD += @libw32bindkeys_LIBS@
test_winman_LDADD += @collectionc_LIBS@

test_ws_SOURCES = test_ws.c
test_ws_CFLAGS = $(AM_CFLAGS)
test_ws_CFLAGS += @libw32bindkeys_CFLAGS@
test_ws_CFLAGS += @collectionc_CFLAGS@
test_ws_LDFLAGS = $(AM_LDFLAGS)
test_ws_LDFLAGS += -mwindows
test_ws_LDFLAGS += -static
test_ws_LDADD = libb3test.la
test_ws_LDADD += $(top_builddir)/src/libb3interpreter.la
test_ws_LDADD += $(top_builddir)/src/libb3parser.la
test_ws_LDADD += @libw32bindkeys_LIBS@
test_ws_LDADD += @collectionc_LIBS@
