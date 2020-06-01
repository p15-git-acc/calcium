/*
    Copyright (C) 2020 Fredrik Johansson

    This file is part of Calcium.

    Calcium is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#ifndef CA_H
#define CA_H

#ifdef CA_INLINES_C
#define CA_INLINE
#else
#define CA_INLINE static __inline__
#endif

#include <stdio.h>
#include "flint/flint.h"
#include "nf.h"
#include "nf_elem.h"
#include "calcium.h"
#include "qqbar.h"
#include "fmpz_mpoly_q.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Number object *************************************************************/

typedef union
{
    fmpq q;                           /* rational number */
    nf_elem_struct nf;                /* algebraic number field element */
    fmpz_mpoly_q_struct * mpoly_q;    /* generic field element */
}
ca_elem_struct;

typedef struct
{
    ulong field;
    ca_elem_struct elem;
}
ca_struct;

typedef ca_struct ca_t[1];

#define CA_FMPQ(x)         (&((x)->elem.q))
#define CA_MPOLY_Q(x)      (&(((x)->elem.mpoly_q)[0]))
#define CA_NF_ELEM(x)      (&((x)->elem.nf))
#define CA_FMPQ_NUMREF(x)  (fmpq_numref(CA_FMPQ(x)))
#define CA_FMPQ_DENREF(x)  (fmpq_denref(CA_FMPQ(x)))

/* We always allocate QQ and QQ(i), with field index 0 and 1 */
#define CA_FIELD_ID_QQ       0
#define CA_FIELD_ID_QQ_I     1

/* Bits added to the top of field_id to encode nonnumbers */
#define CA_UNKNOWN        (UWORD(1) << (FLINT_BITS - 1))
#define CA_UNDEFINED      (UWORD(1) << (FLINT_BITS - 2))
#define CA_UNSIGNED_INF   (UWORD(1) << (FLINT_BITS - 3))
#define CA_SIGNED_INF     (UWORD(1) << (FLINT_BITS - 4))
#define CA_SPECIAL        (CA_UNKNOWN | CA_UNDEFINED | CA_UNSIGNED_INF | CA_SIGNED_INF)

#define CA_IS_SPECIAL(x)  ((x)->field & CA_SPECIAL)

/* Extension object **********************************************************/

/* There are currently two kinds of extension elements: algebraic numbers,
   and symbolic functions. */
typedef enum
{
    CA_EXT_QQBAR,
    CA_EXT_FUNCTION
}
ca_extension_type;

typedef struct
{
    qqbar_struct x;     /* qqbar element */
    nf_struct nf;       /* antic number field for fast arithmetic */
}
ca_extension_data_qqbar;

typedef struct
{
    ulong func;             /* f = F_Pi, F_Exp, ... */
    slong num_args;         /* n */
    ca_struct * args;       /* x1, ..., xn */
    acb_struct enclosure;   /* Numerical enclosure of f(x1,...,xn) */
}
ca_extension_data_function;

typedef union
{
    ca_extension_data_qqbar qqbar;
    ca_extension_data_function function;
}
ca_extension_data_struct;

typedef struct
{
    ca_extension_type type;
    char * string;
    ca_extension_data_struct data;
}
ca_extension_struct;

typedef ca_extension_struct ca_extension_t[1];

/* Field object **************************************************************/

typedef enum
{
    CA_FIELD_TYPE_QQ,       /* field elements are represented as fmpq_t */
    CA_FIELD_TYPE_NF,       /* field elements are represented as nf_elem_t */
    CA_FIELD_TYPE_MPOLY_Q   /* field elements are represented as fmpz_mpoly_q_t */
}
ca_field_type_t;

/* todo: make a union */
typedef struct
{
    fmpz_mpoly_ctx_struct mctx;  /* todo: should perhaps be a reference to a fixed table of precomputed contexts */
    ca_field_type_t type;
    ca_extension_struct * nf_ext;
    ca_extension_struct ** ext;
    slong len;
    fmpz_mpoly_struct * ideal;
    slong ideal_len;
}
ca_field_struct;

typedef ca_field_struct ca_field_t[1];

#define CA_FIELD_MCTX(K) (&((K)->mctx))
#define CA_FIELD_NF(K) (&((K)->nf_ext->data.qqbar.nf))
#define CA_FIELD_NF_QQBAR(K) (&((K)->nf_ext->data.qqbar.x))

/* Context object ************************************************************/

/* todo: needs to be **extensions */
/* todo: merge field and extension objects? */

typedef struct
{
    ca_field_struct * fields;
    slong fields_len;
    slong fields_alloc;

    ca_extension_struct * extensions;
    slong extensions_len;
    slong extensions_alloc;
}
ca_ctx_struct;

typedef ca_ctx_struct ca_ctx_t[1];

/* Context management */

void ca_ctx_init(ca_ctx_t ctx);

void ca_ctx_clear(ca_ctx_t ctx);

void ca_ctx_print(const ca_ctx_t ctx);

/* Extension and field methods */

void ca_extension_init_qqbar(ca_extension_t ext, const qqbar_t x);

void ca_extension_init_const(ca_extension_t ext, ulong func);

void ca_extension_init_fx(ca_extension_t ext, ulong func, const ca_t x);

void ca_extension_clear(ca_extension_t ext);

void ca_extension_print(const ca_extension_t ext);

void ca_field_init_qq(ca_field_t K);

void ca_field_init_nf(ca_field_t K, ca_extension_struct * ext);

void ca_field_init_mpoly_q(ca_field_t K, slong len);

void ca_field_clear(ca_field_t K);

void ca_field_set_ext(ca_field_t K, slong i, ca_extension_struct * ext);

void ca_field_print(const ca_field_t K);

/* Numbers */

void ca_init(ca_t x, ca_ctx_t ctx);

void ca_clear(ca_t x, ca_ctx_t ctx);

void ca_swap(ca_t x, ca_t y, ca_ctx_t ctx);

void _ca_make_field_element(ca_t x, slong i, ca_ctx_t ctx);

CA_INLINE void
_ca_make_fmpq(ca_t x, ca_ctx_t ctx)
{
    if (x->field != CA_FIELD_ID_QQ)
        _ca_make_field_element(x, CA_FIELD_ID_QQ, ctx);
}

void ca_set(ca_t res, const ca_t x, ca_ctx_t ctx);

void ca_zero(ca_t x, ca_ctx_t ctx);
void ca_one(ca_t x, ca_ctx_t ctx);

void ca_set_si(ca_t x, slong v, ca_ctx_t ctx);
void ca_set_ui(ca_t x, ulong v, ca_ctx_t ctx);
void ca_set_fmpz(ca_t x, const fmpz_t v, ca_ctx_t ctx);
void ca_set_fmpq(ca_t x, const fmpq_t v, ca_ctx_t ctx);

void ca_i(ca_t x, ca_ctx_t ctx);

void ca_unknown(ca_t x, ca_ctx_t ctx);

void ca_undefined(ca_t x, ca_ctx_t ctx);
void ca_uinf(ca_t x, ca_ctx_t ctx);
void ca_pos_inf(ca_t x, ca_ctx_t ctx);
void ca_neg_inf(ca_t x, ca_ctx_t ctx);
void ca_pos_i_inf(ca_t x, ca_ctx_t ctx);
void ca_neg_i_inf(ca_t x, ca_ctx_t ctx);

void ca_set_qqbar(ca_t res, const qqbar_t x, ca_ctx_t ctx);

void ca_print(ca_t x, ca_ctx_t ctx);

/* Representation properties */

CA_INLINE int ca_is_unknown(const ca_t x, ca_ctx_t ctx)
{
    return x->field == CA_UNKNOWN;
}

/* Value predicates and comparisons */

truth_t ca_check_is_number(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_zero(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_one(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_neg_one(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_i(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_neg_i(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_algebraic(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_rational(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_integer(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_real(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_imaginary(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_nonreal(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_undefined(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_infinity(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_uinf(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_signed_inf(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_pos_inf(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_neg_inf(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_pos_i_inf(const ca_t x, ca_ctx_t ctx);
truth_t ca_check_is_neg_i_inf(const ca_t x, ca_ctx_t ctx);

truth_t ca_check_equal(const ca_t x, const ca_t y, ca_ctx_t ctx);
truth_t ca_check_lt(const ca_t x, const ca_t y, ca_ctx_t ctx);
truth_t ca_check_le(const ca_t x, const ca_t y, ca_ctx_t ctx);
truth_t ca_check_gt(const ca_t x, const ca_t y, ca_ctx_t ctx);
truth_t ca_check_ge(const ca_t x, const ca_t y, ca_ctx_t ctx);

/* Arithmetic */

void ca_neg(ca_t res, const ca_t x, ca_ctx_t ctx);

#ifdef __cplusplus
}
#endif

#endif
