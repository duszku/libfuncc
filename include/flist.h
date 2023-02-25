/*
 * MIT License
 *
 * Copyright (c) 2023 https://github.com/duszku
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @defgroup flist flist
 * @ingroup flist.h
 * @ingroup flist.c
 *
 * A functional, haskell-like linked list module coupled with number of
 * subroutines.
 */

/**
 * @file
 * @brief Header file for the @p flist module
 */

#ifndef FLIST_H_INCLUDED
#define FLIST_H_INCLUDED

#ifndef _POSIX_C_SOURCE
# define _RM_POSIX_DECL
# define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ftuple.h"

#define FLIST_DONTCLEAN 0x0 /**< @brief Inflag, cleanup handler not called */
#define FLIST_CLEANABLE 0x1 /**< @brief Inflag, cleanup handler called */
#define FLIST_CLEANPROT 0x2 /**< @brief Inflag, cleanup handler can be called */

struct flist;

/**
 * @fn struct flist *flist_append(struct flist *l, void *dat, unsigned flags)
 * @brief Appends element to a list
 *
 * Creates new list node with data @p dat using flagset @p flags. If @p l is
 * NULL, new list is created and said node is added to it to become its sole
 * element. Otherwise this new node is appended to the end of @p l.
 *
 * @param[in] l Target list
 * @param[in] dat Data to insert
 * @param[in] flags Flags to add
 */
struct flist    *flist_append(struct flist *, void *, unsigned);

/**
 * @fn struct flist *flist_prepend(struct flist *l, void *dat, unsigned flags)
 * @brief Prepends element to a list
 * @see flist_append()
 */
struct flist    *flist_prepend(struct flist *, void *, unsigned);

/**
 * @fn struct flist *flist_copy(struct flist *l, void *(*copy_c)(void *))
 * @brief Creates copy of list @p l
 *
 * If copy is intended to be shallow, pass NULL as @p copy_c. Then pointers to
 * data located in each node will be naively copied to the new list, but if any
 * of them had @a FLIST_CLEANABLE flag set, @a FLIST_CLEANPROT will be added (so
 * that such elements will not be freed twice).
 *
 * To make a deep copy, @p copy_c has to be a dynamically-allocating copy
 * constructor. All elements of the newly created list will have the
 * @a FLIST_CLEANABLE flag set.
 *
 * @param[in] l Source list
 * @param[in] copy_c Copy constructor, pass NULL if shallow copy suffices
 */
struct flist    *flist_copy(struct flist *, void *(*)(void *));

/**
 * @fn void flist_free(struct flist **lp, int force)
 * @brief Frees list pointed to by @p lp
 *
 * Traverses list pointed to by @p lp and frees all of its nodes. If given node
 * contains heap-allocated data marked with @a FLIST_CLEANABLE flag, then data
 * itself is also freed. This behaviour can be changed by passing a nonzero
 * value as the @p force argument. Then all heap-allocated data will be freed.
 * At the end list is set to NULL so that the variable can be reused.
 *
 * @param[in,out] lp Pointer to the target list
 * @param[in] force Boolean, set to nonzero should all heap-allocated elements
 * be freed.
 */
void             flist_free(struct flist **, int);

/**
 * @fn void flist_set_cleanup(struct flist *l, void (*handler)(void *))
 * @brief Change cleanup handler for list @p l
 *
 * When @a flist_append() and @a flist_prepend() create new lists they set
 * cleanup handler for them to standard @a free() function. However if data
 * stored in the list is not a simple type (for example it is a struct storing
 * pointers to dynamically allocated data) it may be necessary to change the
 * cleanup handler in order to avoid memory leaks.
 *
 * @param[in] l Target list
 * @param[in] handler New cleanup handler
 */
void             flist_set_cleanup(struct flist *, void (*)(void *));

/**
 * @fn void flist_head(struct flist *l, int force)
 * @brief Drops all but the first element of the list
 *
 * This is equivallent to @a flist_take() called with @p n set to one.
 *
 * @param[in] l Source list
 * @param[in] force Same as in @a flist_free()
 */
void             flist_head(struct flist *, int);

/**
 * @fn void flist_tail(struct flist **lp, int force)
 * @brief Removes first element of the list
 *
 * This is equivallent to @a flist_drop() called with @p n set to one.
 *
 * @param[in] l Pointer to the target list
 * @param[in] force Same as in @a flist_free()
 */
void             flist_tail(struct flist **, int);

/**
 * @fn void flist_map(struct flist *l, void *(*f)(void *), int force)
 * @brief Apply @p f with all elements of @p l and put the result in place of
 * the arguments.
 *
 * Traverses list @p l and applies unpure function @p f with consecutive
 * elements of the list. This is intended to be used for things such as printing
 * or modifying the list in-place. Functions that do not change the list still
 * need to return NULL or their input (be an identity function), otherwise
 * contents of the list will be removed.
 *
 * @param[in] l Source list
 * @param[in] f Side effect generator
 * @param[in] force Set to nonzero should old elements be removed
 */
void             flist_map(struct flist *, void *(*)(void *), int);

/**
 * @fn size_t flist_length(struct flist *l);
 * @brief Return length of the list
 *
 * Since length of the list is stored in memory, it does not need to be
 * calculated and therefore this is not a costly operation.
 *
 * @param[in] l Target list
 */
size_t           flist_length(struct flist *);

/**
 * @fn void *flist_find(struct flist *l, int (*f)(void *))
 * @brief Find first element satisfying predicate @p f
 *
 * Returns NULL if no such element was found
 *
 * @param[in] l Target list
 * @param[in] f Predicate
 */
void            *flist_find(struct flist *, int (*)(void *));

/**
 * @fn int flist_elem(struct flist *l, int (*cmp)(const void *, const void *),
 *  const void *x)
 * @brief Verify whether @p x is an element of @p l
 *
 * The @p cmp is expected to be a comparison function defined the same way as
 * comparison function needed for @a qsort() as defined in ANSI C90.
 *
 * @param[in] l Target list
 * @param[in] cmp Comparison function
 * @param[in] x Element we are looking for
 */
int              flist_elem(struct flist *, int (*)(const void *, const void *),
    const void *);

/**
 * @fn int flist_any(struct flist *l, int (*f)(void *))
 * @brief Verify whether any element of @p l satisfies predicate @p f
 *
 * This is equivallent to checking if @a flist_find() returned a non-NULL value.
 * @param[in] l Target list
 * @param[in] f Predicate
 * @see flist_find()
 */
int              flist_any(struct flist *, int (*)(void *));

/**
 * @fn int flist_all(struct flist *l, int (*f)(void *))
 * @brief Verify whether all elements of @p l satisfy predicate @p f
 *
 * This is equivallent to checking if @a flist_find() returned NULL or @a
 * flist_any() returned true for the logical negation of the predicate.
 *
 * @param[in] l Target list
 * @param[in] f Predicate
 * @see flist_find()
 * @see flist_any()
 */
int              flist_all(struct flist *, int (*)(void *));

/**
 * @fn void flist_filter(struct flist **l, int (*f)(void *), int force)
 * @brief Filter out elements of @p l that do not satisfy predicate @p f
 *
 * This subroutine removes from @p l nodes that do not satisfy @p f. Thus it
 * behaves somewhat similarly to @a flist_free() in the sense that it also
 * takes a @p force parameter. If no element satisfies @p f, then entire list is
 * freed.
 *
 * @param[in] lp Pointer to target list
 * @param[in] f Predicate
 * @param[in] force Same as in @a flist_free()
 * @see flist_free()
 */
void             flist_filter(struct flist **, int (*)(void *), int);

/**
 * @fn void flist_take(struct flist **lp, int n, int force)
 * @brief Truncate the list to a given size
 *
 * If @p n is greater than or equal to the length of @p l then no changes will 
 * be applied. Taking zero elements is equivallent to freeing the list.
 *
 * @param[in] lp Pointer to the target list
 * @param[in] n Number of elements to take
 * @param[in] force Same as in @a flist_free()
 */
void             flist_take(struct flist **, int, int);

/**
 * @fn void flist_drop(struct flist **lp, int n, int force)
 * @brief Remove first @p n elements from @p l
 *
 * Calling this with @p n greater or equal to the length of @p l is equivallent
 * to calling @a flist_free()
 *
 * @param[in] lp Pointer to the target list
 * @param[in] n Number of elements to drop
 * @param[in] force Same as in @a flist_free()
 */
void             flist_drop(struct flist **, int, int);

/**
 * @fn void *flist_foldr(struct flist *l, void *x, void *(*f)(void *, void *))
 * @brief Folds the list from the right
 *
 * Calculates value of @p f applied with @p x and last element of @p l, then of
 * @p f applied with the penultimate element and result of previous calculation
 * and so on until it reaches the end. @p f is expected to return heap-allocated
 * data that can be freed. @p l is unmodified.
 *
 * @param[in] l Source list
 * @param[in] x Starting element
 * @param[in] f Folding function
 */
void            *flist_foldr(struct flist *, void *, void *(*)(void *, void *));

/**
 * @fn void *flist_foldl(struct flist *l, void *x, void *(*f)(void *, void *))
 * @brief Folds the list from the left
 *
 * Analogous to @a flist_foldr()
 *
 * @param[in] l Source list
 * @param[in] x Starting element
 * @param[in] f Folding function
 */
void            *flist_foldl(struct flist *, void *, void *(*)(void *, void *));

/**
 * @fn void *flist_val_head(struct flist *l)
 * @brief Returns data stored in the head of the list
 *
 * If list is empty or @p l is NULL, then NULL is returned.
 *
 * @param[in] l Source list
 */
void            *flist_val_head(struct flist *);

#ifdef _RM_POSIX_DECL
# undef _POSIX_C_SOURCE
#endif

#endif /* FLIST_H_INCLUDED */
