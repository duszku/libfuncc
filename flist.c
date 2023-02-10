/**
 * @file
 * @brief Source file for @p flist module
 */

#include "flist.h"

/**
 * @brief Error-reporting macro
 *
 * This macro is intended to be used upon detection of unrecoverable error in
 * a standard library subroutine. It informs about location where the error
 * occured, prints error information and exits with a failure code.
 *
 * @param[in] X Subroutine that failed
 */
#define ERROR(X) do {                                       \
        fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__);    \
        perror((X));                                        \
        exit(EXIT_FAILURE);                                 \
} while (0);

/**
 * @brief Node of `flist`
 *
 * This structure works as both a node as well as the iterator. The `flist` is
 * implemented to be a doubly linked list storing data in form of void pointers
 * to mimic generics. Moreover, each node contains a two-flag bitfield which
 * store information used by `flist_free()` upon list deletion.
 *
 * @see flist_free()
 */
struct flist_iter {
        struct       flist_iter *next;  /**< @brief Next node */
        struct       flist_iter *prev;  /**< @brief Previous node */
        void        *data;              /**< @brief Pointer to the data */

        unsigned     is_stack : 1;      /**< @brief Is data stack-allocated? */
        unsigned     freeable : 1;      /**< @brief Should data be freed? */
};

/**
 * @brief A doubly linked list
 *
 * This structure is implemented to serve as an interface to a chain of
 * `flist_iter` nodes.
 *
 * @see flist_iter
 */
struct flist {
        struct       flist_iter *head;  /**< @brief Head of the list */
        struct       flist_iter *tail;  /**< @brief Tail of the list */
        size_t       len;               /**< @brief Length of the list */
};

typedef struct flist_iter *node;
typedef struct flist *list;

static list      new_list(void);
static node      new_node(void *, node, node, unsigned);

/* @cond INC_HEADER_DEFS_SEC */

struct flist *
flist_append(struct flist *l, void *dat, unsigned flags)
{
        struct   flist_iter *to_add;
        int      nil;

        nil     = l == NULL;
        to_add  = new_node(dat, nil ? NULL : l->tail, NULL, flags);

        if (nil) {
                l       = new_list();
                l->len  = 1;
                l->head = l->tail = to_add;
        } else {
                l->tail = l->tail->next = to_add;
                l->len++;
        }

        return l;
}

struct flist *
flist_prepend(struct flist *l, void *dat, unsigned flags)
{
        struct   flist_iter *to_add;
        int      nil;

        nil     = l == NULL;
        to_add  = new_node(dat, NULL, nil ? NULL : l->tail, flags);

        if (nil) {
                l       = new_list();
                l->len  = 1;
                l->head = l->tail = to_add;
        } else {
                l->head = l->head->prev = to_add;
                l->len++;
        }

        return l;
}

/* @endcond */
