/*
 * Unit testing all subroutines concerning creation, memory management and
 * structure of flist, namely:
 *  - flist_append()
 *  - flist_prepend()
 *  - flist_length()
 *  - flist_head()
 *  - flist_head_inplace()
 *  - flist_free()
 *
 * For this to work properly one needs libfunc.so produced by the DEBUG target
 * as well as -lasan and -lubsan flags as we rely on the LeakSanitizer to detect
 * problems with memory management.
 */

#include <assert.h>

#include <criterion/criterion.h>
#include <funcc/flist.h>

struct   flist *list;
int     *fr, *hp, st;

/* Test init and fini subroutines */
void         setup(void);
void         end(void);

TestSuite(flist_creation, .init=setup, .fini=end);

Test(flist_creation, appending)
{
        /* Appending elements, verifying that list grows properly */

        list = flist_append(list, &st, ELEM_STACK);

        cr_expect_eq(flist_length(list), 1);
        cr_expect_eq(*((int *)flist_head(list)), 2);

        list = flist_append(list, hp, ELEM_HEAP);

        cr_expect_eq(flist_length(list), 2);
        cr_expect_eq(*((int *)flist_head(list)), 2);

        list = flist_append(list, fr, ELEM_HEAP | ELEM_FREE);

        cr_expect_eq(flist_length(list), 3);
        cr_expect_eq(*((int *)flist_head(list)), 2);


        /* Freeing list without force, verifying it was cleaned as needed */

        flist_free(&list, 0);
        fr = NULL;

        cr_expect_eq(list, NULL);
        cr_expect_eq(flist_length(list), 0);
        cr_expect_eq(*hp, 1);
}

Test(flist_creation, prepending)
{
        /* Appending elements, verifying that list grows properly */

        list = flist_prepend(list, &st, ELEM_STACK);

        cr_expect_eq(flist_length(list), 1);
        cr_expect_eq(*((int *)flist_head(list)), 2);

        list = flist_prepend(list, hp, ELEM_HEAP);

        cr_expect_eq(flist_length(list), 2);
        cr_expect_eq(*((int *)flist_head(list)), 1);

        list = flist_prepend(list, fr, ELEM_HEAP | ELEM_FREE);

        cr_expect_eq(flist_length(list), 3);
        cr_expect_eq(*((int *)flist_head(list)), 0);


        /* Freeing list without force, verifying it was cleaned as needed */

        flist_free(&list, 1);
        fr = hp = NULL;

        cr_expect_eq(list, NULL);
        cr_expect_eq(flist_length(list), 0);
}

Test(flist_creation, list_structure)
{
#define DUPA printf("dupa\n")

        list = flist_append(list, fr, ELEM_HEAP | ELEM_FREE);
        list = flist_append(list, hp, ELEM_HEAP);
        list = flist_append(list, &st, ELEM_STACK);

        cr_expect_eq(*((int *)flist_head_inplace(list)), 0);
DUPA;
        cr_expect_eq(*((int *)flist_head_inplace(list)), 1);
DUPA;
        cr_expect_eq(*((int *)flist_head_inplace(list)), 2);
DUPA;

        flist_free(&list, 0);

        list = flist_prepend(list, fr, ELEM_HEAP | ELEM_FREE);
DUPA;
        list = flist_prepend(list, hp, ELEM_HEAP);
DUPA;
        list = flist_prepend(list, &st, ELEM_STACK);
DUPA;

        cr_expect_eq(*((int *)flist_head_inplace(list)), 2);
        cr_expect_eq(*((int *)flist_head_inplace(list)), 1);
        cr_expect_eq(*((int *)flist_head_inplace(list)), 0);
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
}

void
end(void)
{
        if (list != NULL)
                flist_free(&list, 0);

        if (hp != NULL) {
                free(hp);
                hp = NULL;
        }

        if (fr != NULL) {
                free(fr);
                fr = NULL;
        }
}
