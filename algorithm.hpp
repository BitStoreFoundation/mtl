#pragma once
#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include "matrix_view.hpp"
#include "condition.hpp"
#include <algorithm>
#include <optional>

namespace tvd {

template<
    class _Ty,
    size_t size,
    class _ConditionTy>
    bool insert(matrix<_Ty, size> & m, vector<_Ty, size> const& v, _ConditionTy const& c) noexcept 
    {
    for(size_t i(0); i < std::size(m); i++)
        if( !c(m[i]) ) return false;
    m.push_back( v );
    return true;
    }

template<typename _Ty>
	std::optional<matrix<int, 3> > lee_neumann( matrix_view<_Ty> const& map,
                                                int const& x_from, int const& y_from,
                                                int const& x_to,   int const& y_to,
                                                _Ty const& blank                      )
    {
    if( out_of_range(map, y_from, x_from) || out_of_range(map, y_to, x_to) ) {
        throw EXCEPTION("tvd::lee_neumann : out of range");
        }
    if( map[y_from][x_from] != blank || map[y_to][x_to] != blank ) {
        return std::nullopt;
        }
        
    typedef math_types::matrix_3xn_t<int> matrix_3xn_t;
    typedef math_types::vector4_t<int>    vector4_t;
    typedef math_types::vector3_t<int>    vector3_t;
        
    matrix_3xn_t    way = { y_from, x_from, 0 };
    const vector4_t dx  = { 1, 0, -1,  0 };
    const vector4_t dy  = { 0, 1,  0, -1 };
    bool            stop;

    auto wave_propagation = [&map,  &blank,
                             &way,  &stop, 
                             &dx,   &dy,
                             &x_to, &y_to  ]( const auto y, const auto x, const auto & d )
        -> int
        {
        for( size_t i(0); i < 4; i++ )
            {
            int iy = y + dy[i], ix = x + dx[i];
            if ( !out_of_range(map, iy, ix) &&
                 iy          >= 0           &&
                 ix          >= 0           &&
                 map[iy][ix] == blank          )
                {
                if( insert( way, { iy, ix, static_cast<int>(d) + 1 },
                    [&ix, &iy]( auto const& v )
                    { return !(v[0] == iy && v[1] == ix); }) )
                    {
                    if( iy == y_to && ix == x_to)
                        {
                        return std::size(way) - 1;
                        }
                    stop = false;
                    }
                }
            }
        return 0;
        };

    int y_end(0);
    size_t d(0);

    do {
        stop = true;
        for( size_t i(0); i < std::size(way); i++ ) 
            {
            if( way[i][2] == d ) {
                y_end = wave_propagation(way[i][0], way[i][1], d);
                }
            if( y_end ) {
                break;
                }
            }
        d++;
    } while( !stop );

    if( !y_end ) return std::nullopt;

    auto neighbour = [&dx, &dy](auto const& curr_v, auto const& last_v)
        {
        return ( ( last_v[0] - 1 == curr_v[0]   || last_v[0] + 1 == curr_v[0] ) &&
                 ( last_v[1]     == curr_v[1] ) ||
                 ( last_v[1] - 1 == curr_v[1]   || last_v[1] + 1 == curr_v[1] ) &&
                 ( last_v[0]     == curr_v[0] )                                    );
        };

    int x(x_to), y(y_to);
    vector3_t last_v( way[y_end] );
    d = way[y_end][2] - 1;
    y_end--;
    matrix_3xn_t min_w;
    min_w.push_back( { x_to, y_to, 1 } );

    while( d > 0 ) {
        auto l = d;
        for( size_t i(y_end); i != 0; i-- ) 
            if( way[i][2] == d && neighbour(way[i], last_v) ) {
                min_w.push_back( { way[i][1], way[i][0], 1 } );
                last_v = way[i];
                d--;
                break;
                }
        if( l == d ) return std::nullopt;
        }

    min_w.push_back( { x_from, y_from, 1 } );
    std::optional<matrix_3xn_t> o_min_w;
    o_min_w.emplace( min_w );
    return o_min_w;
    }
// swap if
template<
    typename _Ty,
    typename _ConditionTy>
    void swap_if(_Ty & l, _Ty & r, _ConditionTy const& condition) 
    {
    if( !condition(l, r) ) return;
    std::swap(l, r);
    }
// min value in matrix column
template<
    typename _Ty,
    size_t size>
    _Ty min(matrix<_Ty, size> const& m, size_t j_pos) 
    {
    if(m.empty()) {
        throw EXCEPTION("tvd::min : matrix's is empty");
        }
    if(size <= j_pos) {
        throw EXCEPTION("tvd::min : matrix's col_size <= j_pos");
        }
    auto first = m.begin();
    auto last = m.end();
    first += j_pos;
    _Ty min = (*first);
    for(auto & fst = first; fst < last; fst += size) 
        if(min > (*fst)) min = (*fst);
    return min;
    }
// max value in matrix column
template<
    typename _Ty,
    size_t size>
    _Ty max(matrix<_Ty, size> const& m, size_t j_pos)
    {
    if(m.empty()) {
        throw EXCEPTION("tvd::max : matrix's is empty");
        } 
    if(size <= j_pos) {
        throw EXCEPTION("tvd::max : matrix's col_size <= j_pos");
        }
    auto first = m.begin();
    auto last = m.end();
    _Ty max = (*first);
    first += j_pos;
    for(auto & fst = first; fst < last; fst += size)
        if(max < (*fst)) max = (*fst);
    return max;
    }
// min & max value in matrix column
template<
    typename _Ty,
    size_t size>
    std::pair<_Ty, _Ty> minmax(matrix<_Ty, size> const& m, size_t j_pos) 
    {
    return { min(m, j_pos), max(m, j_pos) };
    }
}
#endif
