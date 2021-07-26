#pragma once
#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include "exception.hpp"
#include "type_traits.hpp"
#include <algorithm>

namespace tvd {
// swap if
template<typename _Ty,
	typename _ConditionTy>
	void swap_if(_Ty & first, _Ty & second, _ConditionTy condition) {
		if(!condition(first, second)) return;
		std::swap(first, second);
	}
// min value in matrix column
template<
	template<typename,
		size_t>
		class _MatrixTy,
	typename _Ty,
	size_t size,
	is_arithmetic_t<_Ty> = true>
	_Ty min(_MatrixTy<_Ty, size> const & m, size_t col_pos) {
		if(m.empty()) {
			throw EXCEPTION("tvd::LU : matrix's is empty");
        }
        if(size <= col_pos) {
			throw EXCEPTION("tvd::min : matrix's col_size <= col_pos");
        }
        auto first = m.begin();
        auto last = m.end();
        first += col_pos;
        _Ty min = (*first);
        for(auto & fst = first; fst < last; fst += size) {
            if(min > (*fst)) min = (*fst);
        }
		return min;
	}
// max value in matrix column
template<
    template<typename,
		size_t>
		class _MatrixTy,
	typename _Ty,
	size_t size,
	is_arithmetic_t<_Ty> = true>
	_Ty max(_MatrixTy<_Ty, size> const & m, size_t col_pos) {
        if(m.empty()) {
			throw EXCEPTION("tvd::LU : matrix's is empty");
        } 
        if(size <= col_pos) {
            throw EXCEPTION("tvd::max : matrix's col_size <= col_pos");
        }
        auto first = m.begin();
        auto last = m.end();
        _Ty max = (*first);
        first += col_pos;
        for(auto & fst = first; fst < last; fst += size) {
            if(max < (*fst)) max = (*fst);
        }
		return max;
	}
// min & max value in matrix column
template<
    template<typename,
		size_t>
		class _MatrixTy,
	typename _Ty,
	size_t size,
	is_arithmetic_t<_Ty> = true>
	std::pair<_Ty, _Ty> minmax(_MatrixTy<_Ty, size> const & m, size_t col_pos) {
        return std::pair { min(m, col_pos), max(m, col_pos) };
	}
}
#endif
