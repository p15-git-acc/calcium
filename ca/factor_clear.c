/*
    Copyright (C) 2020 Fredrik Johansson

    This file is part of Calcium.

    Calcium is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "ca.h"

void
ca_factor_clear(ca_factor_t fac, ca_ctx_t ctx)
{
    if (fac->alloc != 0)
    {
        ca_vec_clear(fac->base, fac->alloc, ctx);
        ca_vec_clear(fac->exp, fac->alloc, ctx);
    }
}
