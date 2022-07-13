# ifndef CPPAD_LOCAL_OP_TANH_OP_HPP
# define CPPAD_LOCAL_OP_TANH_OP_HPP
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-21 Bradley M. Bell

CppAD is distributed under the terms of the
             Eclipse Public License Version 2.0.

This Source Code may also be made available under the following
Secondary License when the conditions for such availability set forth
in the Eclipse Public License, Version 2.0 are satisfied:
      GNU General Public License, Version 2.0 or later.
---------------------------------------------------------------------------- */


namespace CppAD { namespace local { // BEGIN_CPPAD_LOCAL_NAMESPACE


// See dev documentation: forward_unary_op
template <class Base>
void forward_tanh_op(
    size_t p           ,
    size_t q           ,
    size_t i_z         ,
    size_t i_x         ,
    size_t cap_order   ,
    Base*  taylor      )
{
    // check assumptions
    CPPAD_ASSERT_UNKNOWN( NumArg(TanOp) == 1 );
    CPPAD_ASSERT_UNKNOWN( NumRes(TanOp) == 2 );
    CPPAD_ASSERT_UNKNOWN( q < cap_order );
    CPPAD_ASSERT_UNKNOWN( p <= q );

    // Taylor coefficients corresponding to argument and result
    Base* x = taylor + i_x * cap_order;
    Base* z = taylor + i_z * cap_order;
    Base* y = z      -       cap_order;

    size_t k;
    if( p == 0 )
    {   z[0] = tanh( x[0] );
        y[0] = z[0] * z[0];
        p++;
    }
    for(size_t j = p; j <= q; j++)
    {   Base base_j = static_cast<Base>(double(j));

        z[j] = x[j];
        for(k = 1; k <= j; k++)
            z[j] -= Base(double(k)) * x[k] * y[j-k] / base_j;

        y[j] = z[0] * z[j];
        for(k = 1; k <= j; k++)
            y[j] += z[k] * z[j-k];
    }
}

// See dev documentation: forward_unary_op
template <class Base>
void forward_tanh_op_dir(
    size_t q           ,
    size_t r           ,
    size_t i_z         ,
    size_t i_x         ,
    size_t cap_order   ,
    Base*  taylor      )
{
    // check assumptions
    CPPAD_ASSERT_UNKNOWN( NumArg(TanOp) == 1 );
    CPPAD_ASSERT_UNKNOWN( NumRes(TanOp) == 2 );
    CPPAD_ASSERT_UNKNOWN( 0 < q );
    CPPAD_ASSERT_UNKNOWN( q < cap_order );

    // Taylor coefficients corresponding to argument and result
    size_t num_taylor_per_var = (cap_order-1) * r + 1;
    Base* x = taylor + i_x * num_taylor_per_var;
    Base* z = taylor + i_z * num_taylor_per_var;
    Base* y = z      -       num_taylor_per_var;

    size_t k;
    size_t m = (q-1) * r + 1;
    for(size_t ell = 0; ell < r; ell++)
    {   z[m+ell] = Base(double(q)) * ( x[m+ell] - x[m+ell] * y[0] );
        for(k = 1; k < q; k++)
            z[m+ell] -= Base(double(k)) * x[(k-1)*r+1+ell] * y[(q-k-1)*r+1+ell];
        z[m+ell] /= Base(double(q));
        //
        y[m+ell] = Base(2.0) * z[m+ell] * z[0];
        for(k = 1; k < q; k++)
            y[m+ell] += z[(k-1)*r+1+ell] * z[(q-k-1)*r+1+ell];
    }
}

// See dev documentation: forward_unary_op
template <class Base>
void forward_tanh_op_0(
    size_t i_z         ,
    size_t i_x         ,
    size_t cap_order   ,
    Base*  taylor      )
{
    // check assumptions
    CPPAD_ASSERT_UNKNOWN( NumArg(TanOp) == 1 );
    CPPAD_ASSERT_UNKNOWN( NumRes(TanOp) == 2 );
    CPPAD_ASSERT_UNKNOWN( 0 < cap_order );

    // Taylor coefficients corresponding to argument and result
    Base* x = taylor + i_x * cap_order;
    Base* z = taylor + i_z * cap_order;  // called z in documentation
    Base* y = z      -       cap_order;  // called y in documentation

    z[0] = tanh( x[0] );
    y[0] = z[0] * z[0];
}


// See dev documentation: reverse_unary_op
template <class Base>
void reverse_tanh_op(
    size_t      d            ,
    size_t      i_z          ,
    size_t      i_x          ,
    size_t      cap_order    ,
    const Base* taylor       ,
    size_t      nc_partial   ,
    Base*       partial      )
{
    // check assumptions
    CPPAD_ASSERT_UNKNOWN( NumArg(TanOp) == 1 );
    CPPAD_ASSERT_UNKNOWN( NumRes(TanOp) == 2 );
    CPPAD_ASSERT_UNKNOWN( d < cap_order );
    CPPAD_ASSERT_UNKNOWN( d < nc_partial );

    // Taylor coefficients and partials corresponding to argument
    const Base* x  = taylor  + i_x * cap_order;
    Base* px       = partial + i_x * nc_partial;

    // Taylor coefficients and partials corresponding to first result
    const Base* z  = taylor  + i_z * cap_order; // called z in doc
    Base* pz       = partial + i_z * nc_partial;

    // Taylor coefficients and partials corresponding to auxillary result
    const Base* y  = z  - cap_order; // called y in documentation
    Base* py       = pz - nc_partial;


    size_t j = d;
    size_t k;
    Base base_two(2);
    while(j)
    {
        px[j]   += pz[j];
        pz[j]   /= Base(double(j));
        for(k = 1; k <= j; k++)
        {   px[k]   -= azmul(pz[j], y[j-k]) * Base(double(k));
            py[j-k] -= azmul(pz[j], x[k]) * Base(double(k));
        }
        for(k = 0; k < j; k++)
            pz[k] += azmul(py[j-1], z[j-k-1]) * base_two;

        --j;
    }
    px[0] += azmul(pz[0], Base(1.0) - y[0]);
}

} } // END_CPPAD_LOCAL_NAMESPACE
# endif
