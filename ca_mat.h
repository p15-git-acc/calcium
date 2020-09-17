/*
    Copyright (C) 2020 Fredrik Johansson

    This file is part of Calcium.

    Calcium is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#ifndef CA_MAT_H
#define CA_MAT_H

#ifdef CA_MAT_INLINES_C
#define CA_MAT_INLINE
#else
#define CA_MAT_INLINE static __inline__
#endif

#include <stdio.h>
#include "flint/flint.h"
#include "flint/fmpz_mat.h"
#include "flint/fmpq_mat.h"
#include "flint/perm.h"
#include "arb_mat.h"
#include "acb_mat.h"
#include "antic/nf.h"
#include "antic/nf_elem.h"
#include "ca.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Matrix object */

typedef struct
{
    ca_ptr entries;
    slong r;
    slong c;
    ca_ptr * rows;
}
ca_mat_struct;

typedef ca_mat_struct ca_mat_t[1];

#define ca_mat_entry(mat,i,j) ((mat)->rows[i] + (j))
#define ca_mat_nrows(mat) ((mat)->r)
#define ca_mat_ncols(mat) ((mat)->c)

CA_MAT_INLINE ca_ptr
ca_mat_entry_ptr(ca_mat_t mat, slong i, slong j)
{
    return ca_mat_entry(mat, i, j);
}

/* Memory management */

void ca_mat_init(ca_mat_t mat, slong r, slong c, ca_ctx_t ctx);

void ca_mat_clear(ca_mat_t mat, ca_ctx_t ctx);

CA_MAT_INLINE void
ca_mat_swap(ca_mat_t mat1, ca_mat_t mat2, ca_ctx_t ctx)
{
    ca_mat_struct t = *mat1;
    *mat1 = *mat2;
    *mat2 = t;
}

/* Window matrices */

void ca_mat_window_init(ca_mat_t window, const ca_mat_t mat, slong r1, slong c1, slong r2, slong c2, ca_ctx_t ctx);

CA_MAT_INLINE void
ca_mat_window_clear(ca_mat_t window, ca_ctx_t ctx)
{
    flint_free(window->rows);
}

/* Conversions */

void ca_mat_set(ca_mat_t dest, const ca_mat_t src, ca_ctx_t ctx);
void ca_mat_set_fmpz_mat(ca_mat_t dest, const fmpz_mat_t src, ca_ctx_t ctx);
void ca_mat_set_fmpq_mat(ca_mat_t dest, const fmpq_mat_t src, ca_ctx_t ctx);

/* Random generation */

void ca_mat_randtest(ca_mat_t mat, flint_rand_t state, slong len, slong bits, ca_ctx_t ctx);
void ca_mat_randtest_rational(ca_mat_t mat, flint_rand_t state, slong bits, ca_ctx_t ctx);

/* I/O */

void ca_mat_print(const ca_mat_t mat, ca_ctx_t ctx);
void ca_mat_printn(const ca_mat_t mat, slong digits, ca_ctx_t ctx);


#ifdef __cplusplus
}
#endif

#endif