/*
 * Unit testing all predicate-based subroutines:
 *  - `flist_all()`
 *  - `flist_any()`
 *  - `flist_filter()`
 *  - `flist_filter_inplace()`
 *  - `flist_find()`
 */

#include <assert.h>

#include <criterion/criterion.h>
#include <funcc/flist.h>

#define A_LEN 20

int arr[A_LEN]
    = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

struct   flist *list;

void         setup(void);
void         end(void);

int is_odd(void *x) { return *((int *)x) % 2 == 1; }
int is_evn(void *x) { return !is_odd(x); }
int lt_20(void *x)  { return *((int *)x) < 20; }
int geq_20(void *x) { return !lt_20(x); }

void *
copy_c(void *x)
{
        int *ret;

        assert((ret = malloc(sizeof(int))) != NULL);
        *ret = *((int *)x);
        return ret;
}

TestSuite(flist_preds, .init=setup, .fini=end);

Test(flist_preds, filter_shallow)
{
        struct   flist *l2;
        int      i;

        l2 = flist_filter(list, is_odd, 0, NULL);

        cr_expect_eq(flist_length(l2), A_LEN >> 1);
        for (i = 0; i < A_LEN >> 1; ++i)
                cr_expect(is_odd(flist_head_inplace(l2)));

        flist_free(&l2, 0);

        l2 = flist_filter(list, is_evn, 0, NULL);

        cr_expect_eq(flist_length(l2), A_LEN >> 1);
        for (i = 0; i < A_LEN >> 1; ++i)
                cr_expect(is_evn(flist_head_inplace(l2)));

        flist_free(&l2, 0);

        /* Filtering nothing */
        l2 = flist_filter(list, lt_20, 0, NULL);

        cr_expect_eq(flist_length(l2), flist_length(list));
        for (i = 0; i < A_LEN >> 1; ++i)
                cr_expect_eq(flist_head_inplace(list), flist_head_inplace(l2));

        flist_free(&l2, 0); end(); setup();

        /* Filtering everything */
        l2 = flist_filter(list, geq_20, 0, NULL);

        cr_expect_eq(flist_length(l2), 0);
        cr_expect_null(l2);
}

void
setup(void)
{
        int  i;

        for (list = NULL, i = 0; i < A_LEN; ++i)
                list = flist_append(list, arr + i, ELEM_STACK);
}

void
end(void)
{
        flist_free(&list, 1);
}
