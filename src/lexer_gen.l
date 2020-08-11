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
 */

#include "kbman.h"
#include "parser_gen.h"

%}

%option outfile="lexer_gen.c" header-file="lexer_gen.h"
%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

MODIFIER        (MOD4|MOD1)
KEY             [a-zA-Z0-9]
PLUS            [+]
BINDSYM         bindsym
SPACE           [ ]+
EOL             [\n]+
EOF             \Z

%%

{MODIFIER}               { return TOKEN_MODIFIER; }
{KEY}                    { return TOKEN_KEY; }
{PLUS}                   { return TOKEN_PLUS; }
{BINDSYM}                { return TOKEN_BINDSYM; }
{SPACE}                  { return TOKEN_SPACE; }
{EOL}                    { return TOKEN_EOL; }
{EOF}                    { return 0; }

%%



