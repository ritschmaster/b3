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
else
AM_CFLAGS = -O2
AM_LDFLAGS = -O2
endif

SUBDIRS = src tests man

EXTRA_DIST = README README.md

clean-local: clean-release-zip

clean-release-zip:
	rm -rf @PACKAGE_NAME@-@PACKAGE_VERSION@
	rm -rf @PACKAGE_NAME@-@PACKAGE_VERSION@.zip

release-zip: clean-release-zip check
	mkdir @PACKAGE_NAME@-@PACKAGE_VERSION@
	mkdir @PACKAGE_NAME@-@PACKAGE_VERSION@/bin
	mkdir @PACKAGE_NAME@-@PACKAGE_VERSION@/share
	mkdir @PACKAGE_NAME@-@PACKAGE_VERSION@/share/man
	mkdir @PACKAGE_NAME@-@PACKAGE_VERSION@/share/man/man1

	cp README.md @PACKAGE_NAME@-@PACKAGE_VERSION@
	cp ChangeLog.md @PACKAGE_NAME@-@PACKAGE_VERSION@
	cp CompatibilityList.md @PACKAGE_NAME@-@PACKAGE_VERSION@

	cp src/b3.exe @PACKAGE_NAME@-@PACKAGE_VERSION@/bin
	cp tests/full.config @PACKAGE_NAME@-@PACKAGE_VERSION@/bin/config

	cp man/b3.1 @PACKAGE_NAME@-@PACKAGE_VERSION@/share/man/man1

	zip -r @PACKAGE_NAME@-@PACKAGE_VERSION@.zip @PACKAGE_NAME@-@PACKAGE_VERSION@
