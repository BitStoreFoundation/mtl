// c++17 @Tarnakin V.D.
//this header has a description of the matrix transforms
#pragma once
#ifndef MATH_HPP
#define MATH_HPP

#include "matrix.hpp"
#include "math_defines.hpp"
#include <cmath>

namespace tvd {

    constexpr bool check_angle(short const & angle) 
    {
    return ( angle >= -360 && angle <= 360 );
    }

    inline double i_to_rads(int const & angle) 
    {
    if( !check_angle(angle) ) {
        throw EXCEPTION( "bad angle" );
        }
    return angle*k_pi/180;
    }

template<
    typename _Ty,
    size_t col_size,
    size_t pos = 0,
    is_arithmetic_t<_Ty> = true >
    matrix<_Ty, col_size + pos> diag(vector<_Ty, col_size> const & v) 
    {
    matrix<_Ty, col_size + pos> m;
    if( pos < 0 ) {
        size_t i = std::fabs(pos);
        for(size_t k = 0; k < col_size; k++, i++) {
            size_t j = i - pos;
            m[i][j] = v[j];
            }
    } else if( pos > 0 ) {
        size_t j = std::fabs(pos);
        for(size_t k = 0; k < col_size; k++, j++) {
            size_t i = i - pos;
            m[i][j] = v[i];
            }
    } else {
        for(size_t i = 0; i < col_size; ) {
            m[i][i] = v[i];
            }
        }
    return m;
    }
// LU
template<
    typename _Ty,
    size_t col_size,
    is_arithmetic_t<_Ty> = true >
    std::pair<matrix<_Ty, col_size>, matrix<_Ty, col_size> > LU(matrix<_Ty, col_size> const & A) 
    {
    if( A.empty() ) {
        throw EXCEPTION("tvd::LU : matrix's is empty");
        }
    if( col_size != A.size() ) {
        throw EXCEPTION("tvd::LU : matrix's size != col_size");
        }
        
    typedef matrix<_Ty, col_size> matrix_nxn_t;
    matrix_nxn_t L(col_size);
    matrix_nxn_t U = A;

    for(size_t i(0); i < col_size; i++)
        for(size_t j(i); j < col_size; j++)
            L[j][i] = U[j][i]/U[i][i];

    for(size_t k(1); k < col_size; k++)
        {
        for(size_t i(k - 1); i < col_size; i++)
            for(size_t j(i); j < col_size; j++)
                L[j][i] = U[j][i]/U[i][i];
        for(size_t i(k); i < col_size; i++)
            for(size_t j(k - 1); j < col_size; j++)
                U[i][j] = U[i][j] - L[i][k - 1]*U[k - 1][j];
        }
    return { L, U };
    }

template<typename _Ty,
    is_arithmetic_t<_Ty> = true >
    void move(math_types::matrix_3xn_t<_Ty> & m_res, _Ty x0, _Ty y0, _Ty x1, _Ty y1) 
    { 
    math_types::matrix_3xn_t<_Ty> t_tr
    {   1,          0,       0,
        0,          1,       0,
        x1 - x0,    y1 - y0, 1   };

    m_res *= t_tr;
    }

template<typename _Ty>
    void move(math_types::matrix_3xn_t<_Ty> & m_res, _Ty x, _Ty y) 
    {
    move(m_res, 0, 0, x, y);
    }

template<
    typename _Ty,
    is_arithmetic_t<_Ty> = true >
    void scale(math_types::matrix_3xn_t<_Ty> & m_res, _Ty k_x, _Ty k_y) 
    {
    _Ty m = m_res[0][0]*(1 - k_x);
    _Ty l = m_res[0][1]*(1 - k_y);

    math_types::matrix_3xn_t<_Ty> t_scl
    {   k_x, 0,   0,
        0,   k_y, 0,
        m,   l,   1   };

    m_res *= t_scl;
    }

template<
    typename _Ty,
    is_arithmetic_t<_Ty> = true >
    void rotate(math_types::matrix_3xn_t<_Ty> & m_res, short g_ang, _Ty x, _Ty y) 
    {
    auto r_ang = i_to_rads(g_ang);
    _Ty sin = std::sin(r_ang);
    _Ty cos = std::cos(r_ang);

    math_types::matrix_3xn_t<_Ty> t_rot
    {   cos,                 sin,                 0,
       -sin,                 cos,                 0,
        x*(1 - cos) + y*sin, y*(1 - cos) - x*sin, 1   };

    m_res *= t_rot;
    }
}
#endif
