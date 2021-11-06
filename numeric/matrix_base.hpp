#pragma once
#ifndef MTL_NUMERIC_MATRIX_BASE_HPP
#define MTL_NUMERIC_MATRIX_BASE_HPP

#include "../base_mixing_templates.hpp"
#include "../types.hpp"
#include "../foreach.hpp"

namespace mtl {  namespace numeric {
// extension for the default accessor
template<typename _DerivedTy>
    struct view_vector_accessor : public default_accessor<_DerivedTy>
    {
      static std::size_t index( _DerivedTy *impl )
      { return impl->i_; }

      static std::size_t size( _DerivedTy *impl )
      { return impl->size_; }
    };
// view_vector access rule tag
    struct view_vec_rule_t { };
    struct mtx_rule_t { };
// vector tags
    enum class vec_tag
    { insert, view };
} // numeric
// view_vector access rule
template<
    typename _DerivedTy,
    typename _TraitsTy>
    struct access_rule<_DerivedTy, numeric::view_vec_rule_t, _TraitsTy>
    {
      using accessor_t  = typename _TraitsTy::accessor_t;

      access_rule() = default;

      auto operator () ( _DerivedTy *impl, std::size_t const& j )
        -> decltype( accessor_t::container( impl )[j] )&
      { 
        if( j >= impl->size() ) throw exception_t( "<view_vector::operator[]> : <j> >= <size>" );
        return accessor_t::container( impl ).at( accessor_t::index( impl )*accessor_t::size( impl ) + j ); 
      };

      auto operator () ( _DerivedTy *impl, std::size_t const& j ) const
       -> decltype( accessor_t::container( impl )[j] ) const&
      { 
        if( j >= impl->size() ) throw exception_t( "<view_vector::operator[]> : <j> >= <size>" );
        return accessor_t::container( impl ).at( accessor_t::index( impl )*accessor_t::size( impl ) + j ); 
      };
    };

template<
    typename _DerivedTy,
    typename _TraitsTy>
    struct access_rule<_DerivedTy, numeric::mtx_rule_t, _TraitsTy>
    {
      using accessor_t    = typename _TraitsTy::accessor_t;
      using view_vector_t = typename _TraitsTy::return_t;

      access_rule() = default;

      view_vector_t operator () ( _DerivedTy *impl, std::size_t const& i )
      { 
        if( i >= impl->size() ) throw exception_t( "<matrix::operator[]> : <i> >= <size>" );
        return view_vector_t( *impl, i ); 
      };

      const view_vector_t operator () ( _DerivedTy *impl, std::size_t const& i ) const
      { 
        if( i >= impl->size() ) throw exception_t( "<matrix::operator[]> : <i> >= <size>" );
        return view_vector_t( *impl, i ); 
      };
    };
} // mtl
#endif