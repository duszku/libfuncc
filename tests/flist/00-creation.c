/* flist creation test
 *
 * Coverage: flist_append(), flist_prepend(), flist_length(), flist_head(),
 *           flist_tail_inplace(), flist_free()
 */

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

        fprintf(stderr, "\n\n\tCompile this with address/leak sanitizer.");
        fprintf(stderr, "\tIf LeakSanitizier does not complain at the end then \
it means that flist_free() works as intended.\n\n");


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
        diwtho_assert_neq(list, NULL, "List not NULL after appending");
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


        /* Verifying flist_free() and flist_tail_inplace() work (if leak
         * sanitizer does not scream at the end, then all fr?'s have been
         * successfuly freed) */

        flist_free(&list, 0);

        diwtho_assert_eq(list, NULL, "List freeing");
        diwtho_expect_eq(*hp0, 3, "");
        diwtho_expect_eq(*hp1, 4, "");
        diwtho_expect_eq(*hp2, 5, "");


        /* Preparing for flist_prepend() test */

        assert((fr0 = malloc(sizeof(int))) != NULL);
        assert((fr1 = malloc(sizeof(int))) != NULL);
        assert((fr2 = malloc(sizeof(int))) != NULL);

        *fr0 = 6;
        *fr1 = 7;
        *fr2 = 8;


        /* flist_prepend() testing */

        list = flist_prepend(list, &st0, ELEM_STACK);
        diwtho_assert_neq(list, NULL, "List not NULL after prepending");
        diwtho_expect_eq(flist_length(list), 1, "List length after prepending");
        list = flist_prepend(list, &st1, ELEM_STACK);
        diwtho_expect_eq(flist_length(list), 2, "");
        list = flist_prepend(list, &st2, ELEM_STACK);
        diwtho_expect_eq(flist_length(list), 3, "");

        list = flist_prepend(list, hp0, ELEM_HEAP);
        diwtho_expect_eq(flist_length(list), 4, "");
        list = flist_prepend(list, hp1, ELEM_HEAP);
        diwtho_expect_eq(flist_length(list), 5, "");
        list = flist_prepend(list, hp2, ELEM_HEAP);
        diwtho_expect_eq(flist_length(list), 6, "");

        list = flist_prepend(list, fr0, ELEM_HEAP | ELEM_FREE);
        diwtho_expect_eq(flist_length(list), 7, "");
        list = flist_prepend(list, fr1, ELEM_HEAP | ELEM_FREE);
        diwtho_expect_eq(flist_length(list), 8, "");
        list = flist_prepend(list, fr2, ELEM_HEAP | ELEM_FREE);
        diwtho_expect_eq(flist_length(list), 9, "");

        diwtho_expect_eq(*((int *)flist_head(list)), 8, "Traversing list");
        flist_tail_inplace(&list, 1);
        diwtho_expect_eq(flist_length(list), 8, "");

        for (i = 1; i < 9; ++i) {
                diwtho_expect_eq(*((int *)flist_head(list)), 8 - i, "");
                flist_tail_inplace(&list, 1);
                diwtho_expect_eq(flist_length(list), 8 - i, "");
        }

        flist_free(&list, 1);

        /* Preparing for flist_free() force flag test */

        assert((hp0 = malloc(sizeof(int))) != NULL);
        assert((hp1 = malloc(sizeof(int))) != NULL);
        assert((hp2 = malloc(sizeof(int))) != NULL);

        *hp0 = 3;
        *hp1 = 4;
        *hp2 = 5;

        list = flist_prepend(list, hp0, ELEM_HEAP);
        list = flist_prepend(list, hp1, ELEM_HEAP);
        list = flist_prepend(list, hp2, ELEM_HEAP);

        flist_free(&list, 1);

        diwtho_assert_eq(list, NULL, "List freeing with force flag");
}
TEST_END();
