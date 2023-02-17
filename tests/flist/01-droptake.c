/*
 * Unit testing all subroutines concerning dropping or taking elements of flist:
 *  - flist_tail()
 *  - flist_tail_inplace()
 *  - flist_drop()
 *  - flist_drop_inplace()
 *  - flist_take()
 *  - flist_take_inplace()
 *
 * For this to work properly one needs libfuncc.so produced by the DEBUG target
 * as well as -lasan and -lubsan flags as we rely on the AddressSanitizer to
 * detect problems with memory management.
 */

#include <assert.h>

#include <criterion/criterion.h>
#include <funcc/flist.h>

struct   flist *list;
int     *fr, *hp, st;

/* Test init and fini subroutines */
void         setup(void);
void         end(void);

void *
copy_c(void *x)
{
        int *ret;

        assert((ret = malloc(sizeof(int))) != NULL);
        *ret = *((int *)x);
        return ret;
}

TestSuite(flist_droptake, .init=setup, .fini=end);

Test(flist_droptake, tail)
{
        struct   flist *l2;

        l2 = flist_tail(list, 0, NULL);

        cr_expect_eq(flist_length(l2), flist_length(list) - 1);
        cr_expect_eq(flist_head(l2), hp);

        flist_free(&l2, 0);

        l2 = flist_tail(list, 1, copy_c);

        cr_expect_eq(flist_length(l2), flist_length(list) - 1);
        cr_expect_neq(flist_head(l2), hp);
        cr_expect_eq(*((int *)flist_head(l2)), *((int *)hp));

        flist_free(&l2, 0);
}

Test(flist_droptake, tail_inplace)
{
        size_t   size;

        /* Without force flag */
        size = flist_length(list);
        flist_tail_inplace(&list, 0);

        cr_expect_eq(flist_length(list), size - 1);
        cr_expect_eq(*((int *)flist_head(list)), 1);

        flist_tail_inplace(&list, 0);

        cr_expect_eq(flist_length(list), size - 2);
        cr_expect_eq(*((int *)hp), 1); /* checking if sanitizer screams */
        cr_expect_eq(*((int *)flist_head(list)), 2);

        end();
        setup();

        /* With force flag */
        flist_tail_inplace(&list, 1);
        flist_tail_inplace(&list, 1);

        hp = NULL;
}

Test(flist_droptake, drop)
{
        struct   flist *l2;

        l2 = flist_drop(list, 2, 0, NULL);

        /* Simple dropping behaviour */
        cr_expect_eq(flist_length(l2), flist_length(list) - 2);
        cr_expect_eq(flist_head(l2), &st);

        flist_free(&l2, 0);

        /* Dropping nothing */
        l2 = flist_drop(list, 0, 0, NULL);

        cr_expect_eq(flist_length(l2), flist_length(list));
        cr_expect_eq(flist_head(l2), flist_head(list));

        flist_free(&l2, 0);

        /* Dropping everything */
        l2 = flist_drop(list, flist_length(list), 0, NULL);

        cr_expect_eq(flist_length(l2), 0);
        cr_expect_eq(l2, NULL);

        flist_free(&l2, 0);

        /* Dropping with deep copying */
        l2 = flist_drop(list, 2, 1, copy_c);

        cr_expect_eq(*((int *)flist_head(l2)), st);
        cr_expect_neq(flist_head(l2), flist_head(list));

        flist_free(&l2, 0);
}

void
setup(void)
{
        list = NULL;

        assert((hp = malloc(sizeof(int))) != NULL);
        assert((fr = malloc(sizeof(int))) != NULL);

        st  = 2;
        *hp = 1;
        *fr = 0;

        list = flist_append(list, fr, ELEM_HEAP | ELEM_FREE);
        list = flist_append(list, hp, ELEM_HEAP);
        list = flist_append(list, &st, ELEM_STACK);
}

void
end(void)
{
        if (list == NULL && fr != NULL) {
                free(fr);
                fr = NULL;
        }

        if (list != NULL)
                flist_free(&list, 0);

        if (hp != NULL) {
                free(hp);
                hp = NULL;
        }
}
