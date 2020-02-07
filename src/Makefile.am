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
AM_CFLAGS = -g3 -O0
AM_LDFLAGS = -g3 -O0
endif

bin_PROGRAMS = b3 scratch

scratch_SOURCES = scratch.c
scratch_LDFLAGS = --static -mwindows

noinst_LTLIBRARIES = libb3base.la
noinst_LTLIBRARIES += libb3parser.la
noinst_LTLIBRARIES += libb3interpreter.la


libb3parser_la_SOURCES = win.c win.h # todo

libb3interpreter_la_CFLAGS = @collectionc_CFLAGS@
libb3interpreter_la_LDFLAGS = --static -mwindows
libb3interpreter_la_LIBADD = @collectionc_LIBS@


libb3base_la_SOURCES = win.c win.h
libb3base_la_SOURCES += ws.c ws.h
libb3base_la_SOURCES += wsman.c wsman.h
libb3base_la_SOURCES += bar.c bar.h
libb3base_la_SOURCES += barman.c barman.h
libb3base_la_SOURCES += til.c til.h

libb3base_la_CFLAGS = @collectionc_CFLAGS@
libb3base_la_LDFLAGS = --static 
libb3base_la_LIBADD = @collectionc_LIBS@


libb3interpreter_la_SOURCES = win.c win.h #todo 

libb3interpreter_la_CFLAGS = @collectionc_CFLAGS@
libb3interpreter_la_LDFLAGS = --static -mwindows
libb3interpreter_la_LIBADD = @collectionc_LIBS@


b3_SOURCES = main.c

b3_LDFLAGS = --static -mwindows
b3_LDADD = libb3parser.la libb3interpreter.la


