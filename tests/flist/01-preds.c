/* flist predicate-based functions testing
 *
 * Coverage: flist_all(), flist_any(), flist_filter(), flist_filter_inplace()
 */

#include <assert.h>
#include <diwtho.h>
#include <flist.h>

#define AR_LEN 10

int
is_odd(void *x)
{
        return *((int *)x) % 2 != 0;
}

int
is_evn(void *x)
{
        return !is_odd(x);
}

int
lt_20(void *x)
{
        return *((int *)x) < 20;
}

int
geq_20(void *x)
{
        return !lt_20(x);
}

void *
copy_c(void *x)
{
        int *ret;

        assert((ret = malloc(sizeof(int))) != NULL);
        *ret = *((int *)x);
        return ret;
}

TEST_START("flist predicate-based functions")
{
        struct   flist *list_odd, *list_evn, *list_mx1, *list_mx2, *list_mx3;
        size_t   len;
        int      odd_n[AR_LEN] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 };
        int      evn_n[AR_LEN] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
        int     *hp0, *hp1;
        int      i;

        printf("\n\tCompile this with address/leak sanitizer. If leak\n");
        printf("\tsanitizier does not complain at the end then it\n");
        printf("\tmeans that flist_free() and filter_inplace() work\n\
\tas intended.\n\n");

        /* Preparing for testing */

        list_odd = list_evn = list_mx1 = list_mx2 = list_mx3 = NULL;

        for (i = 0; i < AR_LEN; ++i) {
                list_evn = flist_append(list_evn, evn_n + i, ELEM_STACK);
                list_odd = flist_append(list_odd, odd_n + i, ELEM_STACK);

                list_mx1 = flist_append(list_mx1, evn_n + i, ELEM_STACK);
                list_mx1 = flist_append(list_mx1, odd_n + i, ELEM_STACK);
        }


        /* Testing any() and all() */

        diwtho_expect(flist_all(list_odd, is_odd), "flist_all()");
        diwtho_expect(flist_all(list_evn, is_evn), "");
        diwtho_expect(flist_all(list_mx1, lt_20), "");

        diwtho_expect(!flist_any(list_odd, is_evn), "flist_any()");
        diwtho_expect(!flist_any(list_evn, is_odd), "");
        diwtho_expect(!flist_any(list_mx1, geq_20), "");

        flist_free(&list_odd, 1);
        flist_free(&list_evn, 1);

        /* Prepraring for filter() testing */

        assert((hp0 = malloc(sizeof(int))) != NULL);
        assert((hp1 = malloc(sizeof(int))) != NULL);

        *hp0 = 20;
        *hp1 = 21;

        flist_prepend(list_mx1, hp0, ELEM_HEAP);
        flist_prepend(list_mx1, hp1, ELEM_HEAP);


        /* Testing filter() */

        list_mx2 = flist_filter(list_mx1, is_odd, 0, NULL);
        list_mx3 = flist_filter(list_mx1, is_odd, 1, copy_c);

        /* are they different lists? */
        diwtho_expect_neq(list_mx1, list_mx2, "filter()");
        diwtho_expect_neq(list_mx1, list_mx3, "");

        /* are first values the same? */
        diwtho_expect_eq(*((int *)flist_head(list_mx1)),
            *((int *)flist_head(list_mx2)), "");
        diwtho_expect_eq(*((int *)flist_head(list_mx1)),
            *((int *)flist_head(list_mx3)), "");

        /* was deep copy only performed for list_mx3? */
        diwtho_expect_neq(flist_head(list_mx1), flist_head(list_mx3), "");
        diwtho_expect_eq(flist_head(list_mx1), flist_head(list_mx2), "");

        /* are values properly filtered? */
        diwtho_expect(flist_all(list_mx2, is_odd), "");
        diwtho_expect(flist_all(list_mx3, is_odd), "");
        diwtho_expect(flist_any(list_mx2, geq_20), "");

        /* are lengths updated properly? */
        len = flist_length(list_mx1);

        diwtho_expect_eq(flist_length(list_mx2), len >> 1, "");
        diwtho_expect_eq(flist_length(list_mx3), len >> 1, "");

        flist_free(&list_mx2, 0);
        flist_free(&list_mx3, 0);


        /* Testing filter_inplace() */

        flist_filter_inplace(list_mx1, is_evn, 0);

        diwtho_expect_eq(flist_length(list_mx1), len >> 1, "filter_inplace()");
        diwtho_expect(flist_all(list_mx1, is_evn), "");
        diwtho_expect(!flist_any(list_mx1, is_odd), "");

        flist_filter_inplace(list_mx1, lt_20, 1);

        diwtho_expect_eq(flist_length(list_mx1), (len >> 1) - 1, "");
        diwtho_expect(flist_all(list_mx1, lt_20), "");

        flist_free(&list_mx1, 0);
        free(hp1);
}
TEST_END();
