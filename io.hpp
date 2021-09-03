#pragma once
#ifndef IO_HPP
#define IO_HPP

#include "matrix_view.hpp"
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
        auto f = std::begin(manip.v_), l = std::end(manip.v_);
        os << "{ ";
        while( f != l )
            if( (os << *f) && (++f != l) )
                os << manip.d_;
        return os << " }";
        }
    };
}

template<
    typename _ContTy,
    typename _DelimTy = const char*>
    manips::print_manip<_ContTy, _DelimTy> print(_ContTy const& deduce, _DelimTy delim = ", ") {
        return { deduce, std::move(delim) };
    }

template<typename _Ty>
	std::ostream & operator << (std::ostream & ostream, std::vector<_Ty> vector)
    {
		ostream << "[" << vector.size() << "]";
        ostream << manips::print_manip(vector) << std::endl;
		return ostream;
    }

template<
	typename _Ty,
	size_t size>
	std::ostream & operator << (std::ostream & ostream, vector<_Ty, size> vector)
    {
        ostream << "[" << size << "]";
        ostream << manips::print_manip(vector) << std::endl;
		return ostream;
    }

template<typename _Ty>
	std::ostream & operator << (std::ostream & ostream, matrix_view<_Ty> matrix)
    {
		auto size = matrix.size();
		ostream << "Rows : " << size << std::endl << "Cols : " << matrix.csize() << std::endl;
		for(size_t i = 0; i < size; i++)
        {
            ostream << manips::print_manip(matrix[i]) << std::endl;
        }
		return ostream;
    }

template<
	typename _Ty,
	size_t csize>
	std::ostream & operator << (std::ostream & ostream, matrix<_Ty, csize> matrix)
    {
        matrix_view<_Ty> m_v(matrix);
        ostream << m_v;
		return ostream;
    }
}
#endif
