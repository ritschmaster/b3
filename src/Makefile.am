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

if DEBUG
AM_CFLAGS = -g -O0
AM_LDFLAGS = -g -O0 
endif

bin_PROGRAMS = b3 

noinst_LTLIBRARIES = libb3parser.la
noinst_LTLIBRARIES += libb3interpreter.la


libb3parser_la_SOURCES = win.c win.h # todo

libb3parser_la_CFLAGS = $(AM_CFLAGS)
libb3parser_la_CFLAGS += @collectionc_CFLAGS@
libb3parser_la_LDFLAGS = -pthread -mwindows -static 
libb3parser_la_LIBADD = @collectionc_LIBS@


libb3interpreter_la_SOURCES = director.c director.h
libb3interpreter_la_SOURCES += kc_director.c kc_director.h
libb3interpreter_la_SOURCES += kbman.c kbman.h
libb3interpreter_la_SOURCES += wsman.c wsman.h
libb3interpreter_la_SOURCES += monitor.c monitor.h
libb3interpreter_la_SOURCES += ws.c ws.h
libb3interpreter_la_SOURCES += winman.c winman.h
libb3interpreter_la_SOURCES += win.c win.h
libb3interpreter_la_SOURCES += win_watcher.c win_watcher.h
libb3interpreter_la_SOURCES += bar.c bar.h
libb3interpreter_la_SOURCES += win_factory.c win_factory.h
libb3interpreter_la_SOURCES += ws_factory.c ws_factory.h
libb3interpreter_la_SOURCES += wsman_factory.c wsman_factory.h
libb3interpreter_la_SOURCES += monitor_factory.c monitor_factory.h
libb3interpreter_la_SOURCES += kc_director_factory.c kc_director_factory.h
libb3interpreter_la_SOURCES += parser.c parser.h
libb3interpreter_la_SOURCES += til.c til.h
libb3interpreter_la_SOURCES += counter.c counter.h

libb3interpreter_la_CFLAGS = $(AM_CFLAGS)
libb3interpreter_la_CFLAGS += @collectionc_CFLAGS@ 
libb3interpreter_la_CFLAGS += @libwbkbase_CFLAGS@
libb3interpreter_la_LDFLAGS = -pthread -mwindows -static
libb3interpreter_la_LIBADD = @collectionc_LIBS@
libb3interpreter_la_LIBADD += @libwbkbase_LIBS@


b3_SOURCES = main.c

b3_CFLAGS = $(AM_CFLAGS)
b3_CFLAGS += @libwbkbase_CFLAGS@
b3_LDFLAGS =  -pthread -mwindows -static
b3_LDADD = libb3interpreter.la
b3_LDADD += @libwbkbase_LIBS@


