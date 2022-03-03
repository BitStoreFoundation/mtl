// c++17 @Tarnakin V.D.
//this header has a description of the algorithms
#pragma once
#ifndef MTL_ALGORITHM_HPP
#define MTL_ALGORITHM_HPP
// mtl
#include "exception.hpp"
// std
#include <algorithm>

namespace mtl {
// swap if
template<
    typename _Ty,
    typename _CompareTy>
    void swap_if( _Ty & l, _Ty & r, _CompareTy const& c )
    { if( c(l, r) ) std::swap( l, r ); }
// min value in matrix column
template<typename _MatrixTy>
    auto min_element( _MatrixTy const& m, std::size_t j ) -> decltype( std::begin( m ) )
    {
      if( m.empty() )
          throw exception_t("<mtl::min_element> : <matrix> is empty");
      if( m.csize() <= j )
          throw exception_t("<mtl::min_element> : <matrix.csize> <= <j_pos>");

      ++j;
      auto nxt = std::begin( m ) + j;
      auto min = nxt;

      for( std::size_t i(j); i < std::size( m ); i += j, nxt += j )
          if( min > nxt )
              min = nxt;

      return min;
    }
// max value in matrix column
template<typename _MatrixTy>
    auto max_element( _MatrixTy const& m, std::size_t j ) -> decltype( std::begin( m ) )
    {
      if( m.empty() )
          throw exception_t("<mtl::max_element> : <matrix> is empty");

      if( m.csize() <= j )
          throw exception_t("<mtl::max_element> : <matrix.csize> <= <j>");

      ++j;
      auto nxt = std::begin( m ) + j;
      auto max = nxt;

      for( std::size_t i(j); i < std::size( m ); i += j, nxt += j )
          if( max < nxt )
              max = nxt;

      return max;
    }
// min & max value in matrix column
template<typename _MatrixTy>
    auto minmax_element( _MatrixTy const& m, std::size_t j )
      -> std::pair<decltype(min_element( m, j )), decltype(max_element( m, j ))>
    { return { min_element( m, j ), max_element( m, j ) }; }
} // mtl
#endif
