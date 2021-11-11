#pragma once
#ifndef MTL_NUMERIC_SPARSE_HPP
#define MTL_NUMERIC_SPARSE_HPP
// mtl
#include "sparse_matrix.hpp"
// stl
#include <iostream>

namespace mtl::numeric {

template<typename _Ty>
    std::ostream & operator << ( std::ostream & o, vector_t<_Ty> v )
    {
      using std::endl;
      if( v.sparse() ) {
          o << "sparse : [" << v.amount() << "]{ ";
          for( auto const& elem : v )
          {
              if constexpr( std::is_pointer_v<_Ty> ) {
                  o << *elem.second << " ";
              } else {
                  o <<  elem.second << " ";
              }
          }
          o << "}" << endl;
      }
      auto size = std::size( v );
      o << "[" << size << "]";
      o << "{ ";
      for( std::size_t i(0); i < size; i++ )
          o << v[i] << " ";
      o << "}" << endl;/**/
      return o;
    }

template<typename _Ty>
    std::ostream & operator << ( std::ostream & o, matrix_t<_Ty> const& m ) {
      using std::endl;
      o << "[" << m.size()  << "]";
      o << "[" << m.csize() << "]" << endl;
      if( m.sparse() ) {
          o << "{ ";
          for( auto const& elem : m )
              o << elem.second << " ";
          o << "}" << endl;
      }
      for( std::size_t i(0); i < std::size( m ); i++ )
      {
          o << "{ ";
          auto v = m[i];
          for( std::size_t j(0); j < std::size( v ); j++ )
              o << v[j] << " ";
          o << "}" << endl;
      }
      return o;
    }
}
#endif