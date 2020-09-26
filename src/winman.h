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

/**
 * @author Richard Bäck
 * @date 2020-05-08
 * @brief File contains the window manager class definition
 */

#ifndef B3_WINMAN_H
#define B3_WINMAN_H

#include <collectc/array.h>

#include "win.h"

typedef enum b3_winman_type_s
{
	/**
	 * If a window manager is an inner node, then it contains window managers,
	 * but no windows.
	 */
	INNER_NODE,

	/**
	 * If a window manager is a leaf, then it contains windows, but no other
	 * window managers.
	 */
	LEAF
} b3_winman_type_t;

typedef enum b3_winman_mode_s
{
	VERTICAL,
	HORIZONTAL
} b3_winman_mode_t;

typedef struct b3_winman_s
{
	/**
	 * Array of b3_winman_t
	 */
	Array *winman_arr;

	/**
	 * Array of b3_win_t
	 */
	Array *win_arr;

	b3_winman_type_t type;

	b3_winman_mode_t mode;
} b3_winman_t;

/**
 * @brief Creates a new window manager object
 * @return A new window manager object or NULL if allocation failed
 */
extern b3_winman_t *
b3_winman_new(b3_winman_mode_t mode);

/**
 * @brief creates a copy of a window manager object. The contents of winman_arr
 * and win_arr are only copied shallow.
 * @return A new window manager object or NULL if an error occurred
 */
extern b3_winman_t *
b3_winman_copy(b3_winman_t *winman);

/**
 * @brief Deletes a window manager object
 * @return Non-0 if the deletion failed
 */
extern int
b3_winman_free(b3_winman_t *winman);

/**
  * @brief Get the window managers of the window manager
  * @return A pointer to the window managers. Do not free that memory.
  */
extern Array *
b3_winman_get_winman_arr(b3_winman_t *winman);

/**
 * @param other Will be stored within the window manager. The object will be
 * freed by the window manager.
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_winman_add_winman(b3_winman_t *winman, b3_winman_t *other);

/**
 * Returns the first window found.
 */
extern b3_win_t *
b3_winman_get_next_window(b3_winman_t *winman);

/**
  * @brief Get the windows of the window manager
  * @return A pointer to the windows. Do not free that memory.
  */
extern Array *
b3_winman_get_win_arr(b3_winman_t *winman);

/**
 * @brief Adds a window instance to the window manger, ignoring its child nodes
 * @param win Will be stored within the window manager. The object will not be
 * freed, free it by yourself!
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_winman_add_win(b3_winman_t *winman, b3_win_t *win);

/**
 * @brief Removes a window instance within the window manager and its child
 * nodes
 * @param win The object will not be freed.
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_winman_remove_win(b3_winman_t *winman, const b3_win_t *win);

extern b3_winman_mode_t
b3_winman_get_mode(b3_winman_t *winman);

extern int
b3_winman_set_mode(b3_winman_t *winman, b3_winman_mode_t mode);

extern b3_winman_type_t
b3_winman_get_type(b3_winman_t *winman);

/**
 * @brief Searches for a window instance within the window manager and its
 * child nodes. It uses the b3_win_compare() function to check for equality.
 * @param win A window which might be contained within the window manager
 * @return NULL if the window is not within the window manager. If the window is
 * within the window manager then the window manager is returned which actually
 * contains the window. Do not free it!
 */
extern b3_winman_t *
b3_winman_contains_win(b3_winman_t *winman, const b3_win_t *win);

/**
 * @brief Searches for a window instance within the window manager ignoring
 * child nodes. It uses the b3_win_compare() function to check for equality.
 * @param win A window which might be contained within the window manager
 * @return The window instance stored in the window manager - if found. NULL
 * otherwise. Do not free the returned window!
*/
extern b3_win_t *
b3_winman_contains_win_leaf(b3_winman_t *winman, const b3_win_t *win);

/**
 * Searches for the paranet of a window manager instance within a window
 * manager. It uses a reference comparison to check for equality. The search
 * runs through the entire tree.
 *
 * @param winman The window manager to search in
 * @param other The window manager to search for
 * @return The window manager instance containing the searched window manager -
 * if found. NULL otherwise. Do not free the returned window manager!
 */
extern b3_winman_t *
b3_winman_find_parent_of_winman(b3_winman_t *winman, const b3_winman_t *other);

/**
 * Searches for the parent of a window manager instance within a window manager.
 * It uses a reference comparison to check for equality. The search is only
 * performed on the first level instead of the entire tree.
 *
 * @param winman The window manager to search in
 * @param other The window manager to search for
 * @return The position of the window manager instance containing the searched
 * window manager - if found. -1 otherwise.
 */
extern int
b3_winman_find_winman_at(b3_winman_t *winman, const b3_winman_t *other);

/**
 * @brief Traverses a window manager in pre-order.
 * @param visitor The visitor of each node.
 */
extern void
b3_winman_traverse(b3_winman_t *winman, void visitor(b3_winman_t *winman));

/**
 * @brief Converts a node to an inner node.
 * @return The new leaf created from the current window manager object that will
 * then be an inner node. NULL if anything failed.
 */
extern b3_winman_t *
b3_winman_to_inner_node(b3_winman_t *winman);

/**
 * @brief Converts a node to a leaf.
 * @return 0 if success. Non-0 otherwise.
 */
extern int
b3_winman_to_leaf(b3_winman_t *winman);

/**
 * @brief Re-organizes the window manager object by deleting un-needed inner
 * nodes and leaves.
 */
extern int
b3_winman_reorg(b3_winman_t *winman);

/**
 * @return Non-0 if any window is currently maximized. 0 otherwise.
 */
extern int
b3_winman_any_win_has_state(b3_winman_t *winman, b3_win_state_t state);

#endif // B3_WINMAN_H
