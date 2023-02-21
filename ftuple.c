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

        if (dim < 2)
                return NULL;

        if ((ret = malloc(sizeof(struct ftuple))) == NULL)
                return NULL;

        if ((ret = calloc(dim, sizeof(void *))) == NULL)
                return NULL;

        ret->dim = dim;
        
        va_start(args, dim);
        for (i = 0; i < dim; ++i)
                ret->arr[i] = va_arg(args, void *);
        va_end(args);

        return ret;
}

size_t
ftuple_dim(struct ftuple *t)
{
        return t == NULL ? 0 : t->dim;
}

void *
ftuple_fst(struct ftuple *t)
{
        return ftuple_nth(t, 1);
}

void *
ftuple_snd(struct ftuple *t)
{
        return ftuple_nth(t, 2);
}

void *
ftuple_nth(struct ftuple *t, int n)
{
        return t == NULL || n >= ftuple_dim(t) ? NULL : t->arr[n];
}
