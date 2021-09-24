#pragma once
#ifndef TVD_IO_HPP
#define TVD_IO_HPP

#include "matrix.hpp"
#include "matrix_view.hpp"

#include <iostream>

namespace tvd {

template<
    typename _Ty,
    size_t size>
    std::ostream & operator << ( std::ostream & o, vector<_Ty, size> v )
    {
      o << "[" << size << "]{";
      std::ostream_iterator<_Ty> out_itr ( o, ", ");
      std::copy( v.begin(), v.end(), out_itr );
      o << "}" << std::endl;
      return o;
    }

template<typename _Ty>
    std::ostream & operator << ( std::ostream & o, matrix_view<_Ty> m )
    {
      using std::endl;
      o << "[" << m.size()  << "]" << endl;
      o << "[" << m.csize() << "]" << endl;
      for(size_t i = 0; i < std::size( m ); i++)
      {
          o << "{ ";
          std::ostream_iterator<_Ty> out_itr ( o, ", ");
          std::copy( m[i].begin(), m[i].end(), out_itr );
          o << " }" << endl;
      }
      return o;
    }

template<
    typename _Ty,
    size_t size>
    std::ostream & operator << ( std::ostream & o, matrix<_Ty, size> m ) {
      return o << matrix_view<_Ty>( m );
    }
} // tvd
#endif
