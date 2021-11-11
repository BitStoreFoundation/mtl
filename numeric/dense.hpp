#pragma once
#ifndef MTL_NUMERIC_DENSE_IO_HPP
#define MTL_NUMERIC_DENSE_IO_HPP
// mtl
#include "dense_matrix.hpp"
// stl
#include <iostream>

namespace mtl::numeric {

template<
    typename _Ty,
    size_t size>
    std::ostream & operator << ( std::ostream & o, vector_n_t<_Ty, size> v )
    {
      o << "[" << size << "]{";
      std::ostream_iterator<_Ty> out_itr ( o, " " );
      std::copy( v.begin(), v.end(), out_itr );
      o << "}" << std::endl;
      return o;
    } 

template<
    typename _Ty,
    size_t size>
    std::ostream & operator << ( std::ostream & o, matrix_n_t<_Ty, size> m ) {
	  using std::endl;
	  o << "[" << m.size()  << "]" << endl;
	  o << "[" << m.csize() << "]" << endl;
	  for(size_t i = 0; i < std::size( m ); i++)
	  {
		  o << "{ ";
		  std::ostream_iterator<_Ty> out_itr ( o, " " );
		  vector<_Ty, size> v = m[i];
		  std::copy( v.begin(), v.end(), out_itr );
		  o << "}" << endl;
	  }
	  return o;
	}
} }
#endif