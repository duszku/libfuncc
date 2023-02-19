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
 * @file
 * @brief Source file for @p flist module
 * @todo Comment the code. All of it - documentation tells what it does but you
 * will still need to know how it works once you forget it all in two weeks as
 * always and then discover some weird bug trying to use it.
 */

#include "include/flist.h"

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
 * This structure works both as a node as well as the iterator. The `flist` is
 * implemented to be a doubly linked list storing data in form of void pointers
 * to mimic generics. Moreover, each node contains a two-flag bitfield which
 * stores information used by `flist_free()` upon list deletion.
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
 * Creates and returns a pointer to a new node of @a flist initialized with
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
        struct   flist_iter *to_add;    /* new node */
        int      nil;                   /* is passed list pointer NULL? */

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
        struct   flist_iter *to_add;    /* new node */
        int      nil;                   /* is passed list pointer NULL? */

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

        if (*lp == NULL)
                return;

        for (cur = (*lp)->head; cur != NULL; cur = tmp) {
                /* free data within node iff its dynalloced, non-null
                   and freeable/force flag is set */
                if (!cur->is_stack && (force || cur->freeable) && cur->data)
                        free(cur->data);

                tmp = cur->next;
                free(cur);
        }

        free(*lp);
        *lp = NULL;
}

void *
flist_head(struct flist *l)
{
        return l == NULL || l->head == NULL ? NULL : l->head->data;
}

void *
flist_head_inplace(struct flist *l)
{
        struct   flist_iter *tmp;
        void    *ret;

        ret = flist_head(l);
        tmp = l->head;

        if (tmp != NULL) {
                if (l->head->next != NULL) {
                        l->head = l->head->next;
                        l->head->prev = NULL;
                } else
                        l->head = l->tail = NULL;

                free(tmp);
                l->len--;
        }

        return ret;
}

struct flist *
flist_tail(struct flist *l, int deep, void *(*copy_c)(void *))
{
        return flist_drop(l, 1, deep, copy_c);
}

void
flist_tail_inplace(struct flist **lp, int force)
{
        flist_drop_inplace(lp, 1, force);
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
flist_map_inplace(struct flist *l, void *(*f)(void *), int force)
{
        void    *data;
        struct   flist_iter *cur;

        for (cur = l->head; cur != NULL; cur = cur->next) {
                data = f(cur->data);

                if (cur->data != data && data != NULL) {
                        if (!cur->is_stack && (cur->freeable || force))
                                free(cur->data);

                        cur->is_stack = 0;
                        cur->freeable = 1;
                        cur->data = data;
                }
        }
}

size_t
flist_length(struct flist *l)
{
        return l == NULL ? 0 : l->len;
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
        
        /* empty list contains nothing */
        if (l == NULL)
                return 0;

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
flist_filter_inplace(struct flist **lp, int (*f)(void *), int force)
{
        struct   flist_iter *cur, *tmp;

        for (cur = (*lp)->head; cur != NULL; cur = tmp) {
                tmp = cur->next;

                if (f(cur->data))
                        continue;

                if (cur->prev == NULL)
                        (*lp)->head = cur->next;
                else
                        cur->prev->next = cur->next;

                if (cur->next == NULL)
                        (*lp)->tail = cur->prev;
                else
                        cur->next->prev = cur->prev;

                if (!cur->is_stack && (force || cur->freeable) && cur->data)
                        free(cur->data);

                free(cur);
                (*lp)->len--;
        }

        if ((*lp)->len == 0)
                flist_free(lp, force);
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

void
flist_take_inplace(struct flist **lp, int n, int force)
{
        struct   flist_iter *cur, *tmp;
        int      i;

        if (n <= 0)
                flist_free(lp, force);

        if ((size_t)n >= flist_length(*lp))
                return;

        for (i = 0, cur = (*lp)->head; i < n; ++i, cur = cur->next)
                ;

        cur->prev->next = NULL;
        (*lp)->tail = cur->prev;

        for (; cur != NULL; cur = tmp) {
                tmp = cur->next;

                if (!cur->is_stack && (force || cur->freeable) && cur->data)
                        free(cur->data);
                free(cur);
                (*lp)->len--;
        }
}

struct flist *
flist_drop(struct flist *l, int n, int deep, void *(*copy_c)(void *))
{
        struct   flist *ret;
        struct   flist_iter *cur;
        int      i;

        if ((size_t)n >= flist_length(l))
                return NULL;

        for (i = 0, cur = l->head; i < n; ++i, cur = cur->next)
                ;

        ret = NULL;
        for (; cur != NULL; cur = cur->next) {
                if (deep && copy_c != NULL) {
                        ret = flist_append(ret, copy_c(cur->data),
                            ELEM_HEAP | ELEM_FREE);
                } else {
                        ret = flist_append(ret, cur->data,
                            cur->is_stack ? ELEM_STACK : ELEM_HEAP);
                }
        }

        return ret;
}

void
flist_drop_inplace(struct flist **lp, int n, int force)
{
        struct   flist_iter *cur, *tmp;
        int      i;

        if ((size_t)n >= flist_length(*lp)) {
                flist_free(lp, force);
                return;
        }

        for (i = 0, cur = (*lp)->head; i < n; ++i, cur = tmp) {
                tmp = cur->next;

                if (!cur->is_stack && (force || cur->freeable) && cur->data)
                        free(cur->data);
                free(cur);
                (*lp)->len--;
        }

        cur->prev = NULL;
        (*lp)->head = cur;
}

void *
flist_foldr(struct flist *l, void *x, void *(*f)(void *, void *))
{
        void    *acc, *tmp;
        struct   flist_iter *cur;

        if (l == NULL || l->tail == NULL)
                return x;

        acc = f(l->tail->data, x);
        for (cur = l->tail->prev; cur != NULL; cur = cur->prev, free(tmp)) {
                tmp = acc;
                acc = f(cur->data, tmp);
        }

        return acc;
}

struct flist *
new_list(void)
{
        struct   flist *ret;

        if ((ret = malloc(sizeof(struct flist))) == NULL)
                ERROR("malloc");

        /* set all flags to zero and all pointers to null */
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
