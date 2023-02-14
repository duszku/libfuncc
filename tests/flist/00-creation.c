/* flist creation test */

#include <assert.h>
#include <diwtho.h>
#include <flist.h>

TEST_START("flist list creation")
{
        struct   flist *list;
        int     *hp0, *hp1, *hp2;   /* heap-allocated data */
        int     *fr0, *fr1, *fr2;   /* heap-allocated, to be freed by flist */
        int      st0, st1, st2;     /* stack-allocated data */
        int      i;


        /* Preparing testing data */

        assert((hp0 = malloc(sizeof(int))) != NULL);
        assert((hp1 = malloc(sizeof(int))) != NULL);
        assert((hp2 = malloc(sizeof(int))) != NULL);

        assert((fr0 = malloc(sizeof(int))) != NULL);
        assert((fr1 = malloc(sizeof(int))) != NULL);
        assert((fr2 = malloc(sizeof(int))) != NULL);

        st0  = 0;
        st1  = 1;
        st2  = 2;
        *hp0 = 3;
        *hp1 = 4;
        *hp2 = 5;
        *fr0 = 6;
        *fr1 = 7;
        *fr2 = 8;

        list = NULL;


        /* Appending to the list, verifying length of the list */

        list = flist_append(list, &st0, ELEM_STACK);
        diwtho_expect_eq(flist_length(list), 1, "List length after appending");
        list = flist_append(list, &st1, ELEM_STACK);
        diwtho_expect_eq(flist_length(list), 2, "");
        list = flist_append(list, &st2, ELEM_STACK);
        diwtho_expect_eq(flist_length(list), 3, "");

        list = flist_append(list, hp0, ELEM_HEAP);
        diwtho_expect_eq(flist_length(list), 4, "");
        list = flist_append(list, hp1, ELEM_HEAP);
        diwtho_expect_eq(flist_length(list), 5, "");
        list = flist_append(list, hp2, ELEM_HEAP);
        diwtho_expect_eq(flist_length(list), 6, "");

        list = flist_append(list, fr0, ELEM_HEAP | ELEM_FREE);
        diwtho_expect_eq(flist_length(list), 7, "");
        list = flist_append(list, fr1, ELEM_HEAP | ELEM_FREE);
        diwtho_expect_eq(flist_length(list), 8, "");
        list = flist_append(list, fr2, ELEM_HEAP | ELEM_FREE);
        diwtho_expect_eq(flist_length(list), 9, "");


        /* Verifying list head and tail inplace */
        
        diwtho_expect_eq(*((int *)flist_head(list)), 0, "Traversing list");
        flist_tail_inplace(&list, 0);
        diwtho_expect_eq(flist_length(list), 8, "");

        for (i = 1; i < 9; ++i) {
                diwtho_expect_eq(*((int *)flist_head(list)), i, "");
                flist_tail_inplace(&list, 0);
                diwtho_expect_eq(flist_length(list), 8 - i, "");
        }
}
TEST_END();
