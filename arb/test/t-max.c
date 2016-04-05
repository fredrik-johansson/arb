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

    Copyright (C) 2016 Arb authors

******************************************************************************/

#include "arb.h"

/* sample (x, y) so that x /in y */
void _sample_arf_in_arb(arf_t x, arb_t y, flint_rand_t state)
{
    slong bits, prec, expbits;
    arf_t a, b;
    slong i, n;

    arf_init(a);
    arf_init(b);

    bits = 2 + n_randint(state, 1000);
    prec = 2 + n_randint(state, 1000);
    expbits = n_randint(state, 14);
    n = n_randint(state, 3);

    arf_randtest(x, state, bits, expbits);
    arf_set(a, x);
    arf_set(b, x);
    for (i = 0; i < n; i++)
    {
        arf_randtest(x, state, bits, expbits);
        arf_min(a, a, x);
        arf_max(b, b, x);
    }

    arb_set_interval_arf(y, a, b, prec);

    arf_clear(a);
    arf_clear(b);
}

int main()
{
    slong iter;
    flint_rand_t state;

    flint_printf("max....");
    fflush(stdout);
    flint_randinit(state);

    for (iter = 0; iter < 100000; iter++)
    {
        arf_t a, b, c;
        arb_t x, y, z;
        slong prec;

        arf_init(a);
        arf_init(b);
        arf_init(c);

        arb_init(x);
        arb_init(y);
        arb_init(z);

        _sample_arf_in_arb(a, x, state);
        _sample_arf_in_arb(b, y, state);

        prec = 2 + n_randint(state, 200);

        arf_max(c, a, b);
        arb_max(z, x, y, prec);

        if (!arb_contains_arf(x, a) ||
            !arb_contains_arf(y, b) ||
            !arb_contains_arf(z, c))
        {
            flint_printf("FAIL: containment\n\n");
            flint_printf("a = "); arf_print(a); flint_printf("\n\n");
            flint_printf("b = "); arf_print(b); flint_printf("\n\n");
            flint_printf("c = "); arf_print(c); flint_printf("\n\n");
            flint_printf("x = "); arb_print(x); flint_printf("\n\n");
            flint_printf("y = "); arb_print(y); flint_printf("\n\n");
            flint_printf("z = "); arb_print(z); flint_printf("\n\n");
            abort();
        }

        /* aliasing */
        {
            int alias;

            if (n_randint(state, 2))
            {
                arb_max(x, x, y, prec);
                alias = arb_equal(x, z);
            }
            else
            {
                arb_max(y, x, y, prec);
                alias = arb_equal(y, z);
            }

            if (!alias)
            {
                flint_printf("FAIL: aliasing\n\n");
                flint_printf("x = "); arb_print(x); flint_printf("\n\n");
                flint_printf("y = "); arb_print(y); flint_printf("\n\n");
                flint_printf("z = "); arb_print(z); flint_printf("\n\n");
                abort();
            }
        }

        arf_clear(a);
        arf_clear(b);
        arf_clear(c);

        arb_clear(x);
        arb_clear(y);
        arb_clear(z);
    }

    flint_randclear(state);
    flint_cleanup();
    flint_printf("PASS\n");
    return EXIT_SUCCESS;
}