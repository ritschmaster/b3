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

typedef enum b3_winman_mode_s
{
	VERTICAL,
	HORIZONTAL,
	UNSPECIFIED
} b3_winman_mode_t;

typedef enum b3_winman_get_rel_e
{
 	PREVIOUS = 0,
    NEXT
} b3_winman_get_rel_t;

typedef struct b3_winman_s b3_winman_t;

struct b3_winman_s {
	int (*b3_winman_free)(b3_winman_t *winman);
	void (*b3_winman_traverse)(b3_winman_t *winman,
							   void visitor(b3_winman_t *winman, void *data),
							   void *data);
	int (*b3_winman_add_winman)(b3_winman_t *root, b3_winman_t *winman);
	int (*b3_winman_remove_winman)(b3_winman_t *root, b3_winman_t *winman);
	Array *(*b3_winman_get_winman_arr)(b3_winman_t *winman);
	int (*b3_winman_set_win)(b3_winman_t *winman, b3_win_t *win);
	b3_win_t *(*b3_winman_get_win)(b3_winman_t *winman);
	b3_winman_mode_t (*b3_winman_get_mode)(b3_winman_t *winman);
	b3_winman_t *(*b3_winman_get_parent)(b3_winman_t *root, b3_winman_t *winman);
	b3_winman_t *(*b3_winman_contains_win)(b3_winman_t *winman, const b3_win_t *win);
	b3_winman_t *(*b3_winman_get_winman_rel_to_winman)(b3_winman_t *root,
													   b3_winman_t *winman,
													   b3_winman_get_rel_t direction,
													   char rolling);
	int (*b3_winman_is_empty)(b3_winman_t *root, char check_deep);
	int (*b3_winman_reorg)(b3_winman_t *winman);
	b3_win_t *(*b3_winman_get_maximized)(b3_winman_t *winman);

	/**
	 * Array of b3_winman_t
	 */
	Array *winman_arr;

	b3_win_t *win;

	b3_winman_mode_t mode;
};

/**
 * @brief Creates a new window manager object
 * @return A new window manager object or NULL if allocation failed
 */
extern b3_winman_t *
b3_winman_new(b3_winman_mode_t mode);

/**
 * @brief Deletes a window manager object
 * @return Non-0 if the deletion failed
 */
extern int
b3_winman_free(b3_winman_t *winman);

/**
 * Traverses a window manager in pre-order.
 *
 * @param visitor The visitor of each node.
 * @param data A data object which is passed in to the visitor.
 */
extern void
b3_winman_traverse(b3_winman_t *winman,
					void visitor(b3_winman_t *winman, void *data),
					void *data);

/**
 * Add another window manager object to a window manager object.
 *
 * @param root The window manager object to which the window manager will be
 * added.
 * @param winman Will be stored within the window manager. The object
 * will be freed by the window manager.
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_winman_add_winman(b3_winman_t *root, b3_winman_t *winman);

/**
 * Remove a window manager object from a window manager object. It uses a
 * reference comparison to check for equality. It only removes the window
 * manager winman from the first level of the window manager root. If
 * winman would occur on a deeper level of root, then that is ignored!
 *
 * @param root The window manager from which the window manager will be removed.
 * @param winman Will be removed from the root window manager. Free the removed
 * window manager by yourself.
 * @return 0 if removed. Non-0 otherwise.
 */
extern int
b3_winman_remove_winman(b3_winman_t *root, b3_winman_t *winman);

/**
  * Get the window managers of a window manager.
  *
  * @return A pointer to the window managers. Do not free that memory.
  */
extern Array *
b3_winman_get_winman_arr(b3_winman_t *winman);

/**
 * Sets the window instance to the window manger.

 *
 * @param win Will be stored within the window manager. The object will not be
 * freed, free it by yourself!
 * @return 0 if added. Non-0 otherwise.
 */
extern int
b3_winman_set_win(b3_winman_t *winman, b3_win_t *win);

/**
  * Get the windows of the window manager.
  *
kk  * @return A pointer to the window. Free that memory only if you then call
  * b3_winman_set_win(winman, NULL)!
  */
extern b3_win_t *
b3_winman_get_win(b3_winman_t *winman);

extern b3_winman_mode_t
b3_winman_get_mode(b3_winman_t *winman);

/**
 * Searches for the parent of a window manager instance within a window
 * manager. It uses a reference comparison to check for equality. The search
 * runs through the entire tree.
 *
 * @param root The window manager to search in
 * @param winman The window manager to search for
 * @return The window manager instance containing the searched window manager -
 * if found. NULL otherwise. Do not free the returned window manager!
 */
extern b3_winman_t *
b3_winman_get_parent(b3_winman_t *root, b3_winman_t *winman);

/**
 * Searches for the window instance within the window manager and its
 * child nodes. It uses the b3_win_compare() function to check for equality.
 *
 * @param win A window which might be contained within the window manager
 * @return NULL if the window is not within the window manager. If the window is
 * within the window manager then the window manager is returned which actually
 * contains the window. Do not free it!
 */
extern b3_winman_t *
b3_winman_contains_win(b3_winman_t *winman, const b3_win_t *win);

/**
 * Retrieves the window manager in the direction given by parameter direction
 * relative to the window manager given by the parameter winman in the window
 * manager given by root. The search is performed only on the first level of the
 * parameter root.
 *
 * The window manager given by the parameter winman must be already available
 * in the window manager given by the parameter root (reference check).
 *
 * @param rolling If non-0, then the window starting from the other end of the
 * window manager is used
 * @return The window manager if found. NULL otherwise. Do not free the returned
 * window manager!
 */
extern b3_winman_t *
b3_winman_get_winman_rel_to_winman(b3_winman_t *root,
								   b3_winman_t *winman,
								   b3_winman_get_rel_t direction,
								   char rolling);

/**
 * If root does not contain any window manager containing a window, then root is empty.
 *
 * @param root The window manager to check for emptiness.
 * @param check_deeply If non-0, then the tree is checked deeply. Otherwise only
 * the first level of root is checked.
 * @return Non-0 if root contains any window manager containing a window.
 */
extern int
b3_winman_is_empty(b3_winman_t *root, char check_deeply);

/**
 * Re-organize the window manager tree internally by removing empty subtrees.
 */
extern int
b3_winman_reorg(b3_winman_t *winman);

/**
 * @return NULL if no window is maxmized. Otherwise the maximized window is returned.
 */
extern b3_win_t *
b3_winman_get_maximized(b3_winman_t *winman);

#endif // B3_WINMAN_H
