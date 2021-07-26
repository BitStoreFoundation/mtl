#pragma once
#ifndef IO_HPP
#define IO_HPP

#include "matrix.hpp"
#include <iostream>

namespace tvd {

namespace manips {
template<
	typename _ContTy,
	typename _DelimTy = const char*>
	struct print_manip {

	_ContTy const& v_;
	_DelimTy d_;

	print_manip(_ContTy const& v, _DelimTy d = ", ")
		: v_(v), d_(std::move(d))
		{
		}

		friend std::ostream& operator << (std::ostream& os, print_manip const& manip) {
            using namespace std;
			auto f = begin(manip.v_), l(end(manip.v_));

            os << "{ ";
            while (f != l)
                if ((os << *f) && (++f != l))
					os << manip.d_;
			return os << " }";
        }
    };

template<
	typename _ContTy,
	typename _DelimTy = const char*>
	manips::print_manip<_ContTy, _DelimTy> print(_ContTy const& deduce, _DelimTy delim = ", ") {
		return { deduce, std::move(delim) };
		}
}

template<
	typename _Ty,
	size_t size>
	std::ostream & operator << (std::ostream & ostream, vector<_Ty, size> vector)
		{
		ostream << "[" << size << "]" << "(";
		for(size_t i = 0; i < size - 1; i++)
			ostream << vector[i] << ", ";
		ostream << vector[size - 1] << ")" << std::endl;
		return ostream;
		}

template<
    typename _Ty,
	size_t csize>
	std::ostream & operator << (std::ostream & ostream, matrix<_Ty, csize> matrix)
		{
		auto size = matrix.size();
		ostream << "Rows : " << size << std::endl << "Cols : " << csize << std::endl;
		for(size_t i = 0; i < size; i++)
			{
			auto vector = matrix[i];
			ostream << vector;
			}
		return ostream;
		}
}

#endif
