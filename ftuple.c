#include "include/ftuple.h"

struct ftuple {
        void    **arr;
        size_t   dim;
};

struct ftuple *
ftuple_create(size_t dim, ...)
{
        size_t   i;
        va_list  args;
        struct   ftuple *ret;

        if (dim < 2 || (ret = malloc(sizeof(struct ftuple))) == NULL)
                return NULL;

        if ((ret->arr = calloc(dim, sizeof(void *))) == NULL) {
                free(ret);
                return NULL;
        }

        ret->dim = dim;
        
        va_start(args, dim);
        for (i = 0; i < dim; ++i)
                ret->arr[i] = va_arg(args, void *);
        va_end(args);

        return ret;
}

void
ftuple_free(struct ftuple **tp)
{
        free((*tp)->arr);
        free(*tp);

        *tp = NULL;
}

size_t
ftuple_dim(struct ftuple *t)
{
        return t == NULL ? 0 : t->dim;
}

void *
ftuple_fst(struct ftuple *t)
{
        return ftuple_nth(t, 0);
}

void *
ftuple_snd(struct ftuple *t)
{
        return ftuple_nth(t, 1);
}

void *
ftuple_nth(struct ftuple *t, size_t n)
{
        return t == NULL || n >= ftuple_dim(t) ? NULL : t->arr[n];
}
