/*
 * MIT License
 *
 * Copyright (c) 2023 https://github.com/duszku
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @defgroup ftuple ftuple
 * @ingroup ftuple.h
 * @ingroup ftuple.c
 *
 * Tuples module
 */

/**
 * @file
 * @brief Header file for the @p ftuple module
 */

#ifndef FTUPLE_H_INCLUDED
#define FTUPLE_H_INCLUDED

#ifndef _POSIX_C_SOURCE
# define _RM_POSIX_DECL
# define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdarg.h>

struct ftuple;

/**
 * @fn struct ftuple *ftuple_create(size_t dim, ...)
 * @brief Join @p dim elements into a tuple
 *
 * Returns NULL on error.
 *
 * @param[in] dim Size of the tuple
 * @param[in] ... Conseccutive elements of the tuple
 */
struct ftuple   *ftuple_create(size_t, ...);

/**
 * @fn struct ftuple *ftuple_nuls(size_t dim)
 * @brief Creates empty tuple of size @p dim
 *
 * Creates a tuple of size @p dim with every value set to NULL.
 *
 * @param[in] dim Size of the tuple
 */
struct ftuple   *ftuple_nuls(size_t);

/**
 * @fn void ftuple_free(struct ftuple **tp)
 * @brief Free tuple structure
 *
 * This does not free elements stored within the tuple! Should they be freed,
 * they first need to be retrieved and dealt with by the programmer. Tuple
 * pointed to by @p tp is then set to NULL.
 *
 * @param[in,out] tp Pointer to the target tuple
 */
void             ftuple_free(struct ftuple **);

/**
 * @fn size_t ftuple_dim(struct ftuple *t)
 * @brief Retrieve size of a tuple
 *
 * @param[in] t Target tuple
 */
size_t           ftuple_dim(struct ftuple *);

/**
 * @fn void *ftuple_fst(struct ftuple *t)
 * @brief Retrieve first element of @p t
 *
 * @param[in] t Source tuple
 */
void            *ftuple_fst(struct ftuple *);

/**
 * @fn void *ftuple_snd(struct ftuple *t)
 * @brief Retrieve second element of @p t
 *
 * @param[in] t Source tuple
 */
void            *ftuple_snd(struct ftuple *);

/**
 * @fn void *ftuple_nth(struct ftuple *t, int n)
 * @brief Retrieve @p n th element of @p t
 *
 * If @p n is greater then size of the tuple, NULL is returned instead.
 *
 * @param[in] t Source tuple
 * @param[in] n Posiiton to retrieve value from
 */
void            *ftuple_nth(struct ftuple *, size_t);

#ifdef _RM_POSIX_DECL
# undef _POSIX_C_SOURCE
#endif

#endif /* FTUPLE_H_INCLUDED */
