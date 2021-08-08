#pragma once
#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include "matrix_view.hpp"
#include <algorithm>
#include <stack>
#include <optional>
#include <functional>
#include <random>

namespace tvd {

template<typename _Ty>
    bool less(_Ty const& l, _Ty const& r) noexcept {
        return (l < r);
    }

template<typename _Ty>
    bool equal(_Ty const& l, _Ty const& r) noexcept {
        return (l == r);
    }

template<class _ContainerTy>
    bool out_of_range(_ContainerTy const& c, size_t const& i) noexcept {
        return (std::size(c) >= i);
    }

template<typename _MatrixrTy>
    bool out_of_range(_MatrixrTy const& m, size_t const& i, size_t const& j) noexcept {
        return (m.size() >= i || m.csize() >= j);
    }

template<typename _Ty>
    std::optional<matrix<_Ty, 2> > min_by_backtracking(matrix_view<_Ty> const& map, 
                                                       size_t const& i_from, size_t const& j_from,
                                                       size_t const& i_to,   size_t const& j_to,   
                                                       _Ty    const& no_pass                       ) 
    {
        if( out_of_range(m, i_from, j_from) || out_of_range(m, i_to, j_to) ) {
            throw EXCEPTION("tvd::min_by_backtracking : bad indexes");
        }

        typedef std::pait<size_t, size_t> pair_t;
        typedef matrix<_Ty, 2>            matrix_2xn_t;
        typedef std::stack<pair_t>        stack_pair_t;

        matrix_2xn_t min_w; 
        stack_pair_t stack( size*map.size() );
        pair_t current { i_from, j_from };

        auto get_neighbour = [&current, &no_pass, &min_w, &current](auto const& map) -> std::optional<pair_t>
        {
            matrix_2xn_t neighbours;
            size_t i = current.first; 
            size_t j = current.second;
		    if( !less(size, j + 1) ) {
			    if(map[i][j + 1] != no_pass) {
				    neighbours.push_back( { i, j + 1 } );
			    }
		    }
		    if( !less(size, i + 1) ) {
			    if(maze[i + 1][j] != no_pass) {
				    neighbours.push_back( { i + 1, j } );
			    }
		    }
		    if( !equal(i, 0) ) {
			    if(maze[i - 1][j] != no_pass) {
				    neighbours.push_back( { i - 1, j } );
			    }
		    }
		    if( !equal(j, 0) ) {
			    if(maze[i][j - 1] != no_pass) {
				    neighbours.push_back( { i, j - 1 } );
			    }
		    }
		    return neighbours.empty() ? std::nullopt : {  };
        };
        
        while(!(current.first == i_to && current.second == j_to)) {
		    auto neighbour = get_neighbour(map);
		    if(!neighbour) {
			    stack.push(current);
			    current = neighbour;
			    min_w.push_back( { current.first, current.second } );
		    } else if(!stack.empty()) {
			    current = stack.top();
			    stack.pop();
		    } else {
			    return std::nullopt;
		    }
	    }
	    return min_w;
    }
// swap if
template<
    typename _Ty,
    typename _ConditionTy>
    void swap_if(_Ty & l, _Ty & r, _ConditionTy const& condition) {
        if(!condition(l, r)) return;
        std::swap(l, r);
    }
// min value in matrix column
template<
    typename _Ty,
    size_t size>
    _Ty min(matrix<_Ty, size> const& m, size_t j_pos) {
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
        for(auto & fst = first; fst < last; fst += size) {
            if(min > (*fst)) min = (*fst);
        }
        return min;
    }
// max value in matrix column
template<
    typename _Ty,
    size_t size>
    _Ty max(matrix<_Ty, size> const& m, size_t j_pos) {
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
        for(auto & fst = first; fst < last; fst += size) {
            if(max < (*fst)) max = (*fst);
        }
        return max;
    }
// min & max value in matrix column
template<
    typename _Ty,
    size_t size>
    std::pair<_Ty, _Ty> minmax(matrix<_Ty, size> const& m, size_t j_pos) {
        return { min(m, j_pos), max(m, j_pos) };
    }
}
#endif
