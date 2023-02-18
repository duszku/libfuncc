/*
 * Unit testing all list-transforming subroutines:
 *  - flist_map()
 *  - flist_map_inplace()
 */

#include <assert.h>

#include <criterion/criterion.h>
#include <funcc/flist.h>

#define A_LEN 10

int      arr[A_LEN] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
struct   flist *list;

void         setup(void);
void         end(void);

void *
square(void *x)
{
        int *ret;

        assert((ret = malloc(sizeof(int))) != NULL);
        *ret = *((int *)x) * *((int *)x);
        return ret;
}

TestSuite(flist_tsform, .init=setup, .fini=end);

Test(flist_tsform, map)
{
        struct   flist *l2;
        void    *dat;
        int      i;

        l2 = flist_map(list, square);

        for (i = 0; i < A_LEN; ++i) {
                dat = flist_head_inplace(l2);
                cr_expect_eq(*((int *)dat), i * i);
                free(dat);
        }

        flist_free(&l2, 0);
}

void
setup(void)
{
        int i;

        for (list = NULL, i = 0; i < A_LEN; ++i)
                list = flist_append(list, arr + i, ELEM_STACK);
}

void
end(void)
{
        flist_free(&list, 1);
}