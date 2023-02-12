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

/**
 * @fn struct flist new_list(void)
 * @brief Creates new list
 *
 * @todo Allow for more flexible error handling
 *
 * Creates and returns a pointer to a new, empty list. Treats malloc failure as
 * an unrecoverable error (for now at least).
 */
static struct flist         *new_list(void);

/**
 * @fn struct flist_iter new_node(void *dat, struct flist_iter *prev, struct
 *  flist_iter *next, unsigned flags)
 * @brief Creates new node of a linked list
 *
 * Create and returns a pointer to a new node of @a flist initialized with
 * data passed as arguments. Treats malloc failure as an unrecoverable error
 * (for now at least).
 *
 * @todo Allow for more flexible error handling
 *
 * @param[in] dat Data to store in the node
 * @param[in] prev Pointer to previous node
 * @param[in] next Pointer to next node
 * @param[in] flags Flags as defined in @a flist_append()
 * @see flist_append()
 */
static struct flist_iter    *new_node(void *, struct flist_iter *,
    struct flist_iter *, unsigned);

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
        to_add  = new_node(dat, NULL, nil ? NULL : l->head, flags);

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

void
flist_free(struct flist **lp, int force)
{
        struct   flist_iter *cur, *tmp;

        for (cur = (*lp)->head; cur != NULL; cur = tmp) {
                if (!cur->is_stack && (force || cur->freeable))
                        free(cur->data);

                tmp = cur->next;
                free(cur);
        }

        free(*lp);
        *lp = NULL;
}

struct flist *
flist_map(struct flist *l, void *(*f)(void *))
{
        struct   flist *ret;
        struct   flist_iter *cur;

        for (ret = NULL, cur = l->head; cur != NULL; cur = cur->next)
                ret = flist_append(ret, f(cur->data), ELEM_HEAP | ELEM_FREE);

        return ret;
}

void
flist_map_inplace(struct flist *l, void (*f)(void *))
{
        struct   flist_iter *cur;

        for (cur = l->head; cur != NULL; cur = cur->next)
                f(cur->data);
}

size_t
flist_length(struct flist *l)
{
        return l->len;
}

void *
flist_find(struct flist *l, int (*f)(void *))
{
        struct   flist_iter *cur;

        for (cur = l->head; cur != NULL; cur = cur->next) {
                if (f(cur->data))
                        return cur->data;
        }

        return NULL;
}

int
flist_elem(struct flist *l, int (*cmp)(const void *, const void *),
    const void *x)
{
        struct   flist_iter *cur;

        for (cur = l->head; cur != NULL; cur = cur->next) {
                if (cmp(cur->data, x) == 0)
                        return 1;
        }

        return 0;
}

int
flist_any(struct flist *l, int (*f)(void *))
{
        return flist_find(l, f) != NULL;
}

int
flist_all(struct flist *l, int (*f)(void *))
{
        struct   flist_iter *cur;

        for (cur = l->head; cur != NULL; cur = cur->next) {
                if (!f(cur->data))
                        return 0;
        }

        return 1;
}

struct flist *
flist_filter(struct flist *l, int (*f)(void *), int deep,
    void *(*copy_c)(void *))
{
        struct   flist *ret;
        struct   flist_iter *cur;
        void    *cpy;

        for (ret = NULL, cur = l->head; cur != NULL; cur = cur->next) {
                if (!f(cur->data))
                        continue;

                if (deep && copy_c != NULL) {
                        cpy = copy_c(cur->data);
                        ret = flist_append(ret, cpy, ELEM_HEAP | ELEM_FREE);
                } else {
                        ret = flist_append(ret, cur->data, cur->is_stack ?
                            ELEM_STACK : ELEM_HEAP);
                }
        }

        return ret;
}

void
flist_filter_inplace(struct flist *l, int (*f)(void *), int force)
{
        struct   flist_iter *cur, *tmp;

        for (cur = l->head; cur != NULL; cur = tmp) {
                tmp = cur->next;

                if (f(cur->data))
                        continue;

                if (cur->prev == NULL)
                        l->head = cur->next;
                else
                        cur->prev->next = cur->next;

                if (cur->next == NULL)
                        l->tail = cur->prev;
                else
                        cur->next->prev = cur->prev;

                if (!cur->is_stack && (cur->freeable || force))
                        free(cur->data);

                free(cur);
                l->len--;
        }
}

struct flist *
flist_take(struct flist *l, int n, int deep, void *(*copy_c)(void *))
{
        struct   flist *ret;
        struct   flist_iter *cur;
        void    *cpy;
        int      i;

        ret = NULL;
        for (i = 0, cur = l->head; i < n && cur != NULL; ++i, cur = cur->next) {
                if (deep && copy_c != NULL) {
                        cpy = copy_c(cur->data);
                        ret = flist_append(ret, cpy, ELEM_HEAP | ELEM_FREE);
                } else {
                        ret = flist_append(ret, cur->data, cur->is_stack ?
                            ELEM_STACK : ELEM_HEAP);
                }
        }

        return ret;
}

struct flist *
new_list(void)
{
        struct   flist *ret;

        if ((ret = malloc(sizeof(struct flist))) == NULL)
                ERROR("malloc");

        memset(ret, 0x00, sizeof(struct flist));

        return ret;
}

struct flist_iter *
new_node(void *dat, struct flist_iter *prev, struct flist_iter *next,
    unsigned flags)
{
        struct   flist_iter *ret;

        if ((ret = malloc(sizeof(struct flist_iter))) == NULL)
                ERROR("malloc");

        ret->data     = dat;
        ret->next     = next;
        ret->prev     = prev;
        ret->is_stack = (flags & ELEM_STACK) != 0 ? 1 : 0;
        ret->freeable = !ret->is_stack && (flags & ELEM_FREE) != 0 ? 1 : 0;

        return ret;
}
