#!/bin/bash
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

function clean
{
    make distclean
    rm -rf configure
    rm -rf m4
    rm -rf autom4te.cache
    rm -rf aclocal.m4
    rm -rf Makefile.in
    rm -rf src/Makefile.in
    rm -rf tests/Makefile.in
    rm -rf data/Makefile.in
}

function debug
{
    clean
    
    autoreconf -im 
    ./configure --enable-debug --enable-install-base 
    make
}

function release
{
    clean

    autoreconf -im 
    ./configure --enable-install-base 
    make
}

while getopts ":v" opt; do
    case "$opt" in
        "v" )
            verboseOn=1
            ;;
    esac
done

args=( $@ )

case "${args[$OPTIND - 1]}" in
    clean)
        clean
        ;;
    debug)
        debug
        ;;
    * )
        release
        ;;
esac
