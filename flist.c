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

        unsigned     call_h : 1;      /**< @brief Call cleanup handler? */
        unsigned     prot_h : 1;      /**< @brief Call cleanup iff forced? */
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
        void       (*cl_hand)(void *); /**< @brief Cleanup handlers */
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

struct flist *
flist_copy(struct flist *l, void *(*copy_c)(void *))
{
        struct   flist_iter *cur;
        struct   flist *ret;

        for (ret = NULL, cur = l->head; cur != NULL; cur = cur->next) {
                if (copy_c == NULL) {
                        ret = flist_append(ret, cur->data,
                            cur->call_h ? FLIST_CLEANPROT | FLIST_CLEANABLE
                            : FLIST_DONTCLEAN);
                } else {
                        ret = flist_append(ret, copy_c(cur->data), 
                            FLIST_CLEANABLE);
                }
        }

        return ret;
}

void
flist_free(struct flist **lp, int force)
{
        struct   flist_iter *cur, *tmp;

        if (*lp == NULL)
                return;

        for (cur = (*lp)->head; cur != NULL; cur = tmp) {
                /* 
                 * Only call cleanup handler for nonnul, cleanable data when
                 * either it is not protected or force flag is set.
                 */
                if (cur->call_h && cur->data && (!cur->prot_h || force))
                        (*lp)->cl_hand(cur->data);

                tmp = cur->next;
                free(cur);
        }

        free(*lp);
        *lp = NULL;
}

void
flist_set_cleanup(struct flist *l, void (*handler)(void *))
{
        if (l == NULL || handler == NULL)
                return;

        l->cl_hand = handler;
}

void
flist_head(struct flist *l, int force)
{
        struct   flist_iter *cur, *tmp;

        if (l == NULL)
                return;

        for (cur = l->head->next; cur != NULL; cur = tmp) {
                if (cur->call_h && cur->data && (!cur->prot_h || force))
                        l->cl_hand(cur->data);

                tmp = cur->next;
                free(cur);
        }
}

void
flist_tail(struct flist **lp, int force)
{
        flist_drop(lp, 1, force);
}

void
flist_map(struct flist *l, void *(*f)(void *), int force)
{
        void    *data;
        struct   flist_iter *cur;

        for (cur = l->head; cur != NULL; cur = cur->next) {
                data = f(cur->data);

                if (cur->data != data && data != NULL) {
                        if (cur->call_h && cur->data && (!cur->prot_h || force))
                                l->cl_hand(cur->data);

                        cur->call_h = 1;
                        cur->prot_h = 0;
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
        
        /* empty list contains nothing (duh) */
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

void
flist_filter(struct flist **lp, int (*f)(void *), int force)
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

                if (cur->call_h && cur->data && (!cur->prot_h || force))
                        (*lp)->cl_hand(cur->data);

                free(cur);
                (*lp)->len--;
        }

        if ((*lp)->len == 0)
                flist_free(lp, force);
}

void
flist_take(struct flist **lp, int n, int force)
{
        struct   flist_iter *cur, *tmp;
        int      i;

        if (n <= 0) {
                flist_free(lp, force);
                return;
        }

        if ((size_t)n >= flist_length(*lp))
                return;

        for (i = 0, cur = (*lp)->head; i < n; ++i, cur = cur->next)
                ;

        cur->prev->next = NULL;
        (*lp)->tail = cur->prev;

        for (; cur != NULL; cur = tmp) {
                tmp = cur->next;

                if (cur->call_h && cur->data && (!cur->prot_h || force))
                        (*lp)->cl_hand(cur->data);

                free(cur);
                (*lp)->len--;
        }
}

void
flist_drop(struct flist **lp, int n, int force)
{
        struct   flist_iter *cur, *tmp;
        int      i;

        if ((size_t)n >= flist_length(*lp)) {
                flist_free(lp, force);
                return;
        }

        for (i = 0, cur = (*lp)->head; i < n; ++i, cur = tmp) {
                tmp = cur->next;

                if (cur->call_h && cur->data && (!cur->prot_h || force))
                        (*lp)->cl_hand(cur->data);

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
        for (cur = l->tail->prev; cur != NULL; cur = cur->prev) {
                tmp = acc;
                acc = f(cur->data, tmp);
                l->cl_hand(tmp);
        }

        return acc;
}

void *
flist_foldl(struct flist *l, void *x, void *(*f)(void *, void *))
{
        void    *acc, *tmp;
        struct   flist_iter *cur;

        if (l == NULL || l->tail == NULL)
                return x;

        acc = f(x, l->head->data);
        for (cur = l->head->next; cur != NULL; cur = cur->next) {
                tmp = acc;
                acc = f(tmp, cur->data);
                l->cl_hand(tmp);
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

        ret->cl_hand = free;

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
        ret->call_h   = (flags & FLIST_CLEANABLE) != 0 ? 1 : 0;
        ret->prot_h   = (flags & FLIST_CLEANPROT) != 0 ? 1 : 0;

        return ret;
}
