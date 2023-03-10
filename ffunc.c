#include "include/ffunc.h"

struct farg {
        enum  { FFUNC_RES, FFUNC_DAT } atp;
        union {
                struct   fncb *res;
                struct   ftuple *dat;
        } aarg;
};

struct fncb {
        ffunc    fn;
        struct   farg *args;
};

struct fncb *
ffunc_creat(ffunc fn, size_t argn)
{
        struct   fncb *ret;

        if ((ret = malloc(sizeof(struct fncb))) == NULL)
                return NULL;

        ret->args->atp = FFUNC_DAT;
        if ((ret->args->aarg.dat = ftuple_nuls(argn)) == NULL)
                return NULL;

        ret->fn = fn;

        return ret;
}
