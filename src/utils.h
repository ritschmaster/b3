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
 * @author Richard Bäck
 * @date 2021-01-03
 * @brief File contains utility functions
 */

#ifndef B3_UTILS_H
#define B3_UTILS_H

/**
 * Adds a character to a dynamically allocated string. If the modified_str is
 * NULL, then it will be newly allocated.
 *
 * @param modified_str The modified string. Can be NULL.
 * @param new_c The character to add.
 * @return Either the reallocated modified_str or a new one if modified_str was NULL.
 */
extern char *
b3_add_c_to_s(char *modified_str, char new_c);

/**
 * Adds a string to a dynamically allocated string. If the modified_str is
 * NULL, then it will be newly allocated.
 *
 * @param modified_str The modified string. Can be NULL.
 * @param new_s The string to add.
 * @return Either the reallocated modified_str or a new one if modified_str was NULL.
 */
extern char *
b3_add_s_to_s(char *modified_str, const char *new_s);

#endif // B3_UTILS_H
