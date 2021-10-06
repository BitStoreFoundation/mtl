// c++17 @Tarnakin V.D.
//this header has a description of the algorithms
#pragma once
#ifndef MTL_ALGORITHM_HPP
#define MTL_ALGORITHM_HPP

#include <algorithm>

namespace mtl {

template<typename _ItrTy>
    auto frequent( _ItrTy & fst, _ItrTy const& lst ) -> decltype( *fst )  
    {
      std::unordered_map<decltype( *fst ), size_t> unordered_map;
      while( fst != lst )
      {
          if( unordered_map.find( elem ) == unordered_map.end() ) {
              unordered_map[elem] = 1;
          } else {
              unordered_map[elem]++;
          }
          ++fst;
      }
      return *( std::min_element( std::begin( unordered_map ), std::end( unordered_map ) ) );
    }
// swap if
template<
    typename _Ty,
    typename _CompareTy>
    void swap_if( _Ty & l, _Ty & r, _CompareTy const& c )
    {
      if( !c(l, r) ) return;
      std::swap(l, r);
    }
// min value in matrix column
template<typename _MatrixTy>
    auto min_element( _MatrixTy const& m, size_t j_pos ) -> decltype( *m.begin() )
    {
      if( m.empty() ) {
          throw MTL_EXCEPTION("<mtl::min_element> : <matrix> is empty");
      }
      auto size = m.csize();
      if( size <= j_pos ) {
          throw MTL_EXCEPTION("<mtl::min_element> : <matrix.csize> <= <j_pos>");
      }
      auto first = m.cbegin();
      auto last = m.cend();
      first += j_pos;
      auto min = (*first);
      for(auto & fst(first); fst < last; fst += size)
      {
          if( min > (*fst) ) min = (*fst);
      }
      return min;
    }
// max value in matrix column
template<typename _MatrixTy>
    auto max_element( _MatrixTy const& m, size_t j_pos ) -> decltype( *m.begin() )
    {
      if( m.empty() ) {
          throw MTL_EXCEPTION("<mtl::max_element> : <matrix> is empty");
      }
      auto size = m.csize();
      if( size <= j_pos ) {
          throw MTL_EXCEPTION("<mtl::max_element> : <matrix.csize> <= <j_pos>");
      }
      auto first = m.cbegin();
      auto last = m.cend();
      auto max = (*first);
      first += j_pos;
      for(auto & fst(first); fst < last; fst += size)
      {
          if( max < (*fst) ) max = (*fst);
      }
      return max;
    }
// min & max value in matrix column
template<typename _MatrixTy>
    auto minmax_element( _MatrixTy const& m, size_t j_pos )
      -> std::pair<decltype( min_element( m, j_pos ) ), decltype( max_element( m, j_pos ) )>
    {
      return { min_element( m, j_pos ), max_element( m, j_pos ) };
    }
} // mtl
#endif
