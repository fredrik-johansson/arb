/*=============================================================================

    This file is part of ARB.

    ARB is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ARB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ARB; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2014 Fredrik Johansson

******************************************************************************/

#include "arf.h"

int
arf_add_special(arf_t z, const arf_t x, const arf_t y, long prec, arf_rnd_t rnd)
{
    if (arf_is_zero(x))
    {
        if (arf_is_zero(y))
        {
            arf_zero(z);
            return 0;
        }
        else
            return arf_set_round(z, y, prec, rnd);
    }
    else if (arf_is_zero(y))
    {
        return arf_set_round(z, x, prec, rnd);
    }
    else if (arf_is_nan(x) || arf_is_nan(y)
        || (arf_is_pos_inf(x) && arf_is_neg_inf(y))
        || (arf_is_neg_inf(x) && arf_is_pos_inf(y)))
    {
        arf_nan(z);
        return 0;
    }
    else if (arf_is_special(x))
    {
        arf_set(z, x);
        return 0;
    }
    else
    {
        arf_set(z, y);
        return 0;
    }
}

int
arf_add(arf_ptr z, arf_srcptr x, arf_srcptr y, long prec, arf_rnd_t rnd)
{
    mp_size_t xn, yn;
    mp_srcptr xptr, yptr;
    long shift;

    if (arf_is_special(x) || arf_is_special(y))
    {
        return arf_add_special(z, x, y, prec, rnd);
    }

    shift = _fmpz_sub_small(ARF_EXPREF(x), ARF_EXPREF(y));

    if (shift < 0)
    {
        arf_srcptr __t;
        __t = x; x = y; y = __t;
        shift = -shift;
    }

    ARF_GET_MPN_READONLY(xptr, xn, x);
    ARF_GET_MPN_READONLY(yptr, yn, y);

    return _arf_add_mpn(z, xptr, xn, ARF_SGNBIT(x), ARF_EXPREF(x),
                           yptr, yn, ARF_SGNBIT(y), shift, prec, rnd);
}

int
arf_add_si(arf_ptr z, arf_srcptr x, long y, long prec, arf_rnd_t rnd)
{
    mp_size_t xn, yn;
    mp_srcptr xptr, yptr;
    mp_limb_t ytmp;
    int xsgnbit, ysgnbit;
    fmpz yexp;
    long shift;

    if (y == 0)
    {
        return arf_set_round(z, x, prec, rnd);
    }
    else if (arf_is_special(x))
    {
        if (arf_is_zero(x))
            return arf_set_round_si(z, y, prec, rnd);
        else
        {
            arf_set(z, x);
            return 0;
        }
    }

    ysgnbit = (y < 0);
    if (ysgnbit)
        ytmp = -y;
    else
        ytmp = y;
    yptr = &ytmp;
    yn = 1;
    yexp = FLINT_BITS;
    /* XXX: worth the trouble to top-align y? */

    shift = _fmpz_sub_small(ARF_EXPREF(x), &yexp);

    xsgnbit = ARF_SGNBIT(x);
    ARF_GET_MPN_READONLY(xptr, xn, x);

    if (shift >= 0)
        return _arf_add_mpn(z, xptr, xn, xsgnbit, ARF_EXPREF(x),
                               yptr, yn, ysgnbit, shift, prec, rnd);
    else
        return _arf_add_mpn(z, yptr, yn, ysgnbit, &yexp,
                               xptr, xn, xsgnbit, -shift, prec, rnd);
}

int
arf_add_ui(arf_ptr z, arf_srcptr x, ulong y, long prec, arf_rnd_t rnd)
{
    mp_size_t xn, yn;
    mp_srcptr xptr, yptr;
    mp_limb_t ytmp;
    int xsgnbit, ysgnbit;
    fmpz yexp;
    long shift;

    if (y == 0)
    {
        return arf_set_round(z, x, prec, rnd);
    }
    else if (arf_is_special(x))
    {
        if (arf_is_zero(x))
            return arf_set_round_ui(z, y, prec, rnd);
        else
        {
            arf_set(z, x);
            return 0;
        }
    }

    ysgnbit = 0;
    ytmp = y;
    yptr = &ytmp;
    yn = 1;

    count_leading_zeros(yexp, ytmp);
    yexp = FLINT_BITS;
    shift = _fmpz_sub_small(ARF_EXPREF(x), &yexp);

    xsgnbit = ARF_SGNBIT(x);
    ARF_GET_MPN_READONLY(xptr, xn, x);

    if (shift >= 0)
        return _arf_add_mpn(z, xptr, xn, xsgnbit, ARF_EXPREF(x),
                               yptr, yn, ysgnbit, shift, prec, rnd);
    else
        return _arf_add_mpn(z, yptr, yn, ysgnbit, &yexp,
                               xptr, xn, xsgnbit, -shift, prec, rnd);
}

int
arf_add_fmpz(arf_ptr z, arf_srcptr x, const fmpz_t y, long prec, arf_rnd_t rnd)
{
    mp_size_t xn, yn;
    mp_srcptr xptr, yptr;
    mp_limb_t ytmp;
    int xsgnbit, ysgnbit;
    fmpz yexp;
    long shift;

    if (fmpz_is_zero(y))
    {
        return arf_set_round(z, x, prec, rnd);
    }
    else if (arf_is_special(x))
    {
        if (arf_is_zero(x))
            return arf_set_round_fmpz(z, y, prec, rnd);
        else
        {
            arf_set(z, x);
            return 0;
        }
    }

    FMPZ_GET_MPN_READONLY(ysgnbit, yn, yptr, ytmp, *y)
    yexp = yn * FLINT_BITS;
    shift = _fmpz_sub_small(ARF_EXPREF(x), &yexp);

    xsgnbit = ARF_SGNBIT(x);
    ARF_GET_MPN_READONLY(xptr, xn, x);

    if (shift >= 0)
        return _arf_add_mpn(z, xptr, xn, xsgnbit, ARF_EXPREF(x),
                               yptr, yn, ysgnbit, shift, prec, rnd);
    else
        return _arf_add_mpn(z, yptr, yn, ysgnbit, &yexp,
                               xptr, xn, xsgnbit, -shift, prec, rnd);
}



