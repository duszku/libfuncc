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
