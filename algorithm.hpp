#pragma once
#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include "matrix_view.hpp"
#include <algorithm>
#include <stack>
#include <functional>
#include <random>
#include <optional>

namespace tvd {

template<
    typename _LTy,
    typename _RTy>
    bool less(_LTy const& l, _RTy const& r) noexcept {
        return (l < r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool greater(_LTy const& l, _RTy const& r) noexcept {
        return (l > r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool less_or_equals(_LTy const& l, _RTy const& r) noexcept {
        return (l <= r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool greater_or_equals(_LTy const& l, _RTy const& r) noexcept {
        return (l >= r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool equals(_LTy const& l, _RTy const& r) noexcept {
        return (l == r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool non_equals(_LTy const& l, _RTy const& r) noexcept {
        return (l != r);
    }

template<class _ContainerTy>
    bool out_of_range(_ContainerTy const& c, size_t const& i) noexcept {
		return less_or_equals(std::size(c), i);
    }

template<typename _MatrixrTy>
    bool out_of_range(_MatrixrTy const& m, size_t const& i, size_t const& j) noexcept {
		return ( less_or_equals(m.size(), i) || less_or_equals(m.csize(), j) );
    }

template<typename _Ty>
	std::optional<matrix<size_t, 2> > min_by_backtracking(matrix_view<_Ty> const& map,
										                  size_t const& i_from, size_t const& j_from,
										                  size_t const& i_to,   size_t const& j_to,
									                      _Ty    const& no_pass                       )
    {
        if( out_of_range(map, i_from, j_from) || out_of_range(map, i_to, j_to) ) {
            throw EXCEPTION("tvd::min_by_backtracking : out of range");
        }

        typedef std::pair<size_t, size_t>        pair_t;
		typedef math_types::matrix_2xn_t<size_t> matrix_2xn_t;
		typedef std::stack<pair_t>               stack_pair_t;

        stack_pair_t stack;
		matrix_2xn_t min_w { i_from, j_from };
        pair_t       current { i_from, j_from };

        auto visited_cell = [&min_w](size_t const& i_ne, size_t const& j_ne) 
        {
            for(size_t i = 0; i < min_w.size(); i++)
                if(min_w[i][0] == i_ne && min_w[i][1] == j_ne) {
                    return true;
                }
            return false;
		};

		auto get_neighbours = [&no_pass, &min_w, &current, &visited_cell](auto const& map) -> matrix_2xn_t
		{
			matrix_2xn_t neighbours;
			size_t &i( current.first ), &j( current.second );
            const auto col_size = map.csize();
			const auto size = map.size();
		    if( less(j + 1, col_size) ) {
			    if( map[i][j + 1] != no_pass && !visited_cell(i, j + 1) ) {
					neighbours.push_back( { i, j + 1 } );
			    }
			}
		    if( less(i + 1, size) ) {
			    if( map[i + 1][j] != no_pass  && !visited_cell(i + 1, j) ) {
				    neighbours.push_back( { i + 1, j } );
			    }
		    }
		    if( non_equals(i, 0) ) {
			    if( map[i - 1][j] != no_pass  && !visited_cell(i - 1, j) ) {
				    neighbours.push_back( { i - 1, j } );
			    }
		    }
		    if( non_equals(j, 0) ) {
			    if( map[i][j - 1] != no_pass  && !visited_cell(i, j - 1) ) {
				    neighbours.push_back( { i, j - 1 } );
			    }
			}
			return neighbours;
		};

		while( !(current.first == i_to && current.second == j_to) ) {
			auto neighbours = get_neighbours(map);
			if( !neighbours.empty() ) {
				stack.push(current);
                std::random_device rd;
                std::mt19937 mersenne(rd());
                size_t i_r = mersenne()%neighbours.size();
				current = { neighbours[i_r][0], neighbours[i_r][1] };
			    min_w.push_back( { current.first, current.second } );
			} else if( !stack.empty() ) {
			    current = stack.top();
			    stack.pop();
		    } else {
				return std::nullopt;
			}
		}
        std::optional<matrix_2xn_t> min;
        min.emplace( min_w );
		return min;
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
