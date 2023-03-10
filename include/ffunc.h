#ifndef FFUNC_H_INCLUDED
#define FFUNC_H_INCLUDED

#ifndef _POSIX_C_SOURCE
# define _RM_POSIX_DECL
# define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>

#include "ftuple.h"

typedef struct ftuple *(*ffunc)(struct ftuple *);

struct fncb;

struct fncb     *ffunc_creat(ffunc, size_t);

#ifdef _RM_POSIX_DECL
# undef _POSIX_C_SOURCE
#endif

#endif /* FFUNC_H_INCLUDED */
