#pragma once
#ifndef MTL_SPARSE_IO_HPP
#define MTL_SPARSE_IO_HPP

#include "matrix.hpp"
#include <iostream>

namespace mtl { namespace sparse {

template<typename _Ty>
    std::ostream & operator << ( std::ostream & o, sparse_vector<_Ty> v )
    {
      o << "[" << size << "]{";
      std::ostream_iterator<_Ty> out_itr ( o, " " );
      std::copy( v.begin(), v.end(), out_itr );
      o << "}" << std::endl;
      return o;
    }

template<typename _Ty>
    std::ostream & operator << ( std::ostream & o, matrix<_Ty> m ) {
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
} } // mtl::sparse
#endif
