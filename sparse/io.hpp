#pragma once
#ifndef MTL_SPARSE_IO_HPP
#define MTL_SPARSE_IO_HPP

#include "matrix.hpp"
#include <iostream>

namespace mtl { namespace sparse {

template<typename _Ty>
    std::ostream & operator << ( std::ostream & o, vector<_Ty> v )
    {
      using std::endl;
      o << "sparse : [" << v.amount() << "]";
      if( v.sparse() ) {
          o << "{ ";
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
    std::ostream & operator << ( std::ostream & o, matrix<_Ty> m ) {
      using std::endl;
      o << "[" << m.size()  << "]" << endl;
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
          vector<_Ty> v = m[i];
          for( std::size_t j(0); j < std::size( v ); j++ )
              o << v[j] << " ";
          o << "}" << endl;
      }
      return o;
    }
} } // mtl::sparse
#endif
