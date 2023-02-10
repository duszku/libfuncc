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
# define RM_POSIX_DECL
# define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEM_STACK  0x0 /**< @brief Element is stack-allocated */
#define ELEM_HEAP   0x1 /**< @brief Element is heap-allocated */
#define ELEM_FREE   0x2 /**< @brief Free this element upon list deletion */

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
 * @fn struct flist *flist_free(struct flist **lp, int force)
 * @brief Frees list pointed to by `lp`
 *
 * Traverses list pointed to by `lp` and frees all of its nodes. If given node
 * contains heap-allocated data marked with `ELEM_FREE` flag, then data itself
 * is also freed. This behaviour can be changed by passing a nonzero value as
 * the `force` argument. Then all heap-allocated data will be freed. At the end
 * list is set to NULL so that the variable can be reused.
 *
 * @param[in,out] lp Pointer to the target list
 * @param[in] force Boolean, set to nonzero should all heap-allocated elements
 * be freed.
 */
void             flist_free(struct flist **, int);

#ifdef RM_POSIX_DECL
# undef _POSIX_C_SOURCE
#endif

#endif /* FLIST_H_INCLUDED */
