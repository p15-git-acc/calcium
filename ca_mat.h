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
#include "ca_poly.h"

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

/* Shape */

CA_MAT_INLINE int
ca_mat_is_empty(const ca_mat_t mat)
{
    return (mat->r == 0) || (mat->c == 0);
}

CA_MAT_INLINE int
ca_mat_is_square(const ca_mat_t mat)
{
    return (mat->r == mat->c);
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

/* Special matrices */

void ca_mat_zero(ca_mat_t mat, ca_ctx_t ctx);
void ca_mat_one(ca_mat_t mat, ca_ctx_t ctx);
void ca_mat_ones(ca_mat_t mat, ca_ctx_t ctx);
void ca_mat_pascal(ca_mat_t mat, int triangular, ca_ctx_t ctx);
void ca_mat_stirling(ca_mat_t mat, int kind, ca_ctx_t ctx);
void ca_mat_hilbert(ca_mat_t mat, ca_ctx_t ctx);
void ca_mat_dft(ca_mat_t res, int type, ca_ctx_t ctx);

/* Comparisons and properties */

truth_t ca_mat_check_equal(const ca_mat_t A, const ca_mat_t B, ca_ctx_t ctx);
truth_t ca_mat_check_is_zero(const ca_mat_t A, ca_ctx_t ctx);
truth_t ca_mat_check_is_one(const ca_mat_t A, ca_ctx_t ctx);

/* Conjugate and transpose */

void ca_mat_transpose(ca_mat_t B, const ca_mat_t A, ca_ctx_t ctx);
void ca_mat_conjugate(ca_mat_t B, const ca_mat_t A, ca_ctx_t ctx);
void ca_mat_conjugate_transpose(ca_mat_t mat1, const ca_mat_t mat2, ca_ctx_t ctx);

/* Arithmetic */

void ca_mat_neg(ca_mat_t dest, const ca_mat_t src, ca_ctx_t ctx);
void ca_mat_add(ca_mat_t res, const ca_mat_t mat1, const ca_mat_t mat2, ca_ctx_t ctx);
void ca_mat_sub(ca_mat_t res, const ca_mat_t mat1, const ca_mat_t mat2, ca_ctx_t ctx);
void ca_mat_mul(ca_mat_t C, const ca_mat_t A, const ca_mat_t B, ca_ctx_t ctx);

/* Trace */

void ca_mat_trace(ca_t trace, const ca_mat_t mat, ca_ctx_t ctx);

/* Gaussian elimination and LU decomposition */

truth_t ca_mat_find_pivot(slong * pivot_row, const ca_mat_t mat, slong start_row, slong end_row, slong column, ca_ctx_t ctx);

CA_MAT_INLINE void
_ca_mat_swap_rows(ca_mat_t mat, slong * perm, slong r, slong s)
{
    if (r != s)
    {
        ca_ptr u;
        slong t;

        if (perm != NULL)
        {
            t = perm[s];
            perm[s] = perm[r];
            perm[r] = t;
        }

        u = mat->rows[s];
        mat->rows[s] = mat->rows[r];
        mat->rows[r] = u;
    }
}

truth_t ca_mat_nonsingular_lu(slong * P, ca_mat_t LU, const ca_mat_t A, ca_ctx_t ctx);
truth_t ca_mat_nonsingular_fflu(slong * P, ca_mat_t LU, ca_t den, const ca_mat_t A, ca_ctx_t ctx);

/* Determinant */

void ca_mat_det_berkowitz(ca_t det, const ca_mat_t A, ca_ctx_t ctx);
int ca_mat_det_lu(ca_t det, const ca_mat_t A, ca_ctx_t ctx);
int ca_mat_det_bareiss(ca_t det, const ca_mat_t A, ca_ctx_t ctx);
void ca_mat_det_cofactor(ca_t det, const ca_mat_t A, ca_ctx_t ctx);
void ca_mat_det(ca_t det, const ca_mat_t A, ca_ctx_t ctx);

/* Characteristic polynomial */

void _ca_mat_charpoly(ca_ptr cp, const ca_mat_t mat, ca_ctx_t ctx);
void ca_mat_charpoly(ca_poly_t cp, const ca_mat_t mat, ca_ctx_t ctx);
int ca_mat_companion(ca_mat_t A, const ca_poly_t poly, ca_ctx_t ctx);

#ifdef __cplusplus
}
#endif

#endif
