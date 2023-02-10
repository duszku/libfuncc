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
 * @fn struct flist *flist_free(struct flist **lp, int force)
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
 * @fn void flist_genside(struct flist *l, void (*f)(void *))
 * @brief Generate a sequence of side effects using @p l
 *
 * Traverses list @p l and applies unpure function @p f with consecutive
 * elements of the list. This is intended to be used for things such as printing
 * or modifying the list in-place.
 *
 * @param[in] l Source list
 * @param[in] f Side effect generator
 */
void             flist_genside(struct flist *, void (*)(void *));

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

struct flist    *flist_filter(struct flist *, int (*)(void *));
struct flist    *flist_filter_inplace(struct flist *, int (*)(void *));
struct flist    *flist_take(struct flist *, int);
struct flist    *flist_drop(struct flist *, int);

#ifdef RM_POSIX_DECL
# undef _POSIX_C_SOURCE
#endif

#endif /* FLIST_H_INCLUDED */
