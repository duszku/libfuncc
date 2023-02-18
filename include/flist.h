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

#define ELEM_STACK  0x1 /**< @brief Element is stack-allocated */
#define ELEM_HEAP   0x2 /**< @brief Element is heap-allocated */
#define ELEM_FREE   0x4 /**< @brief Free this element upon list deletion */

struct flist;

/* Memory menagement and list construction subroutines */

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
 * @fn void flist_free(struct flist **lp, int force)
 * @brief Frees list pointed to by @p lp
 *
 * Traverses list pointed to by @p lp and frees all of its nodes. If given node
 * contains heap-allocated data marked with @a ELEM_FREE flag, then data itself
 * is also freed. This behaviour can be changed by passing a nonzero value as
 * the @p force argument. Then all heap-allocated data will be freed. At the end
 * list is set to NULL so that the variable can be reused.
 *
 * @param[in,out] lp Pointer to the target list
 * @param[in] force Boolean, set to nonzero should all heap-allocated elements
 * be freed.
 */
void             flist_free(struct flist **, int);

/**
 * @fn void *flist_head(struct flist *l)
 * @brief Returns first element of the list
 *
 * If list is empty, NULL is returned instead.
 *
 * @param[in] l Source list
 * @see flist_head_inplace()
 */
void            *flist_head(struct flist *);

/**
 * @fn void *flist_head_inplace(struct flist *l)
 * @brief Removes first node of the list and returns data it stored
 *
 * If list is empty, NULL is returned instead.
 *
 * @param[in] l Source list
 * @see flist_head()
 */
void            *flist_head_inplace(struct flist *);

/**
 * @fn struct flist *flist_tail(struct flist *l, int deep,
 * void *(*copy_c)(void *))
 * @brief Returns new list composed of all but the first element
 *
 * This is equivallent to calling @a flist_drop() with @p n set to one.
 *
 * @param[in] l Source list
 * @param[in] deep Nonzero should a deep copy be performed
 * @param[in] copy_c Copy constructor of the data stored in @p l
 * @see flist_drop()
 */
struct flist    *flist_tail(struct flist *, int, void *(*)(void *));

/**
 * @fn void flist_tail_inplace(struct flist **lp, int force)
 * @brief Removes first element of the list
 *
 * This is equivallent to @a flist_drop_inplace() called with @p n set to one.
 *
 * @param[in] l Pointer to the target list
 * @param[in] force Same as in @a flist_free()
 * @see flist_drop_inplace()
 */
void             flist_tail_inplace(struct flist **, int);

/**
 * @fn struct flist *flist_map(struct flist *l, void *(*f)(void *))
 * @brief Maps @p l to another list using @p f
 *
 * Creates a new list obtained by applying function @p f with consecutive
 * elements of list @p l. All nodes of resulting list are expected to be
 * dynamically allocated and are flagged with @a ELEM_FREE.
 *
 * @param[in] l Source list
 * @param[in] f Mapping function
 */
struct flist    *flist_map(struct flist *, void *(*)(void *));

/**
 * @fn void flist_map_inplace(struct flist *l, void *(*f)(void *), int force)
 * @brief Generate a sequence of side effects using @p l
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
void             flist_map_inplace(struct flist *, void *(*)(void *), int);

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
 * @fn struct flist *flist_filter(struct flist *l, int (*f)(void *), int deep,
 * void *(*copy_c)(void *))
 * @brief Filter out elements of @p l that do not satisfy predicate @p f
 *
 * This subroutine creates new list and leaves @p l untouched. For filtering
 * in-place see @a flist_filter_inplace(). If @p deep is nonzero and @p copy_c
 * is not NULL, filter performs deep copy of data within nodes of @p l and sets
 * @a ELEM_FREE flag for all elements of the newly created list.
 *
 * @param[in] l Target list
 * @param[in] f Predicate
 * @param[in] deep Set to nonzero should deep copy be performed
 * @param[in] copy_c Copy constructor for elements stored in the list
 * @see flist_filter_inplace()
 */
struct flist    *flist_filter(struct flist *, int (*)(void *), int,
    void *(*)(void *));

/**
 * @fn void flist_filter_inplace(struct flist **l, int (*f)(void *), int force)
 * @brief Filter out elements of @p l that do not satisfy predicate @p f
 * @b inplace
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
void             flist_filter_inplace(struct flist **, int (*)(void *), int);

/**
 * @fn struct flist *flist_take(struct flist *l, int n, int deep,
 * void *(*copy_c)(void *))
 * @brief Create new list with only first @p n elements of @p l
 *
 * If @p n is greater than or equal to the length of @p l then this subroutine
 * will simply create a copy of the list. If @p deep is zero or @p copy_c is
 * NULL, this will be a shallow copy.
 *
 * @param[in] l Source list
 * @param[in] n Number of elements to take
 * @param[in] deep Set to nonzero should deep copy be performed
 * @param[in] copy_c Copy constructor for elements stored in the list
 * @see flist_take_inplace()
 */
struct flist    *flist_take(struct flist *, int, int, void *(*)(void *));

/**
 * @fn void flist_take_inplace(struct flist **lp, int n, int force)
 * @brief Truncate the list to a given size
 *
 * If @p n is greater than or equal to the length of @p l then no changes will 
 * be applied. Taking zero elements is equivallent to freeing the list.
 *
 * @param[in] lp Pointer to the target list
 * @param[in] n Number of elements to take
 * @param[in] force Same as in @a flist_free()
 * @see flist_take()
 */
void             flist_take_inplace(struct flist **, int, int);

/**
 * @fn struct flist *flist_drop(struct flist *l, int n, int deep,
 * void *(*copy_c)(void *))
 * @brief Create new list obtained by removing first @p n elements of @p l.
 *
 * If @p n is greater than or equal to the length of @p l then this subroutine
 * will simply return NULL. If @p deep is zero or @p copy_c is NULL, the
 * returned list will contain shallow copies of data in @p l.
 *
 * @param[in] l Source list
 * @param[in] n Number of elements to drop
 * @param[in] deep Set to nonzero should deep copy be performed
 * @param[in] copy_c Copy constructor for elements stored in the list
 * @see flist_drop_inplace()
 */
struct flist    *flist_drop(struct flist *, int, int, void *(*)(void *));

/**
 * @fn void flist_drop_inplace(struct flist **lp, int n, int force)
 * @brief Remove first @p n elements from @p l
 *
 * Calling this with @p n greater or equal to the length of @p l is equivallent
 * to calling @a flist_free()
 *
 * @param[in] lp Pointer to the target list
 * @param[in] n Number of elements to drop
 * @param[in] force Same as in @a flist_free()
 * @see flist_drop()
 */
void             flist_drop_inplace(struct flist **, int, int);

#ifdef _RM_POSIX_DECL
# undef _POSIX_C_SOURCE
#endif

#endif /* FLIST_H_INCLUDED */
