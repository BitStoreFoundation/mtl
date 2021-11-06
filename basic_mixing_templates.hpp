// C++17 @Tarnakin V.D.
#pragma once
#ifndef MTL_BASIC_MIXING_TEMPLATES_HPP
#define MTL_BASIC_MIXING_TEMPLATES_HPP

#include "exception.hpp"
#include "type_traits.hpp"
// macro for declare reference to the derived class
#define MTL_DECL_PRIV_PTR_TO_THE_DERIVED_CLASS_AND_PUB_DEF_CONSTR(class_name, derived_type) \
private : \
    derived_type *derived_; \
public : \
    class_name() \
      : derived_( static_cast<derived_type*>( this ) ) \
    { }

namespace mtl {
// helpers
template<typename ... _ArgsTy>
    struct mixing_list : public _ArgsTy ... { };
//
template<typename _DerivedTy>
    struct access_traits { };

template<
    typename _DerivedTy,
    typename _TraitsTy = access_traits<_DerivedTy> >
    struct default_accessor
    {
      static typename _TraitsTy::container_t& container( _DerivedTy *impl )
      { return impl->container_; }
    };
// tags
    struct default_rule_t { };
    struct no_const_t { };
    struct only_const_t { }; 
    struct with_const_t { };
//
template<
    typename _DerivedTy,
    typename _ArgTy      = _DerivedTy,
    typename _ReturnTy   = _DerivedTy,
    typename _AccessorTy = default_accessor<_DerivedTy> >
    struct mixing_traits
    {
      using derived_t  = _DerivedTy;
      using return_t   = _ReturnTy;
      using accessor_t = _AccessorTy;
      using arg_t      = _ArgTy; 
    };

template<
    typename _DerivedTy,
    typename _RuleTagTy = default_rule_t,
    typename _TraitsTy  = mixing_traits<_DerivedTy> >
    struct access_rule { };

template<
    typename _DerivedTy,
    typename _TraitsTy>
    struct access_rule<_DerivedTy, default_rule_t, _TraitsTy>
    {
      using accessor_t = typename _OverloadTraitsTy::accessor_t;

      access_rule() = default;

      auto operator () ( _DerivedTy *impl, std::size_t const& key )
        -> decltype( accessor_t::container( impl )[key] )& 
      { return accessor_t::container( impl ).at( key ); };

      auto operator () ( _DerivedTy const* impl, std::size_t const& key ) const
        -> decltype( accessor_t::container( impl )[key] )& 
      { return accessor_t::container( impl ).at( key ); };
    };
// mixing for container class
template<
    typename _DerivedTy,
    typename _TagTy     = with_const_t,
    typename _RuleTagTy = default_rule_t,
    typename _TraitsTy  = mixing_traits<_DerivedTy>,
    typename _RuleTy    = access_rule<_DerivedTy, _RuleTagTy, _TraitsTy> >
    struct add_access_by_key
    {
      using accessor_t = typename _TraitsTy::accessor_t;
      // declare derived_ and add_access_by_key() = default
      MTL_DECL_PRIV_PTR_TO_THE_DERIVED_CLASS_AND_PUB_DEF_CONSTR( add_access_by_key, _DerivedTy )

  template<
      typename KeywordTy,
      typename TagTy = _TagTy,
      std::enable_if_t<
        std::is_same_v<TagTy, with_const_t> ||
        std::is_same_v<TagTy, no_const_t>, 
        bool> = true >
      auto operator [] ( KeywordTy const& key )
        -> decltype( _RuleTy()( derived_, key ) )
      { return _RuleTy()( derived_, key ); }

  template<
      typename KeywordTy,
      typename TagTy = _TagTy,
      std::enable_if_t<
        std::is_same_v<TagTy, with_const_t> ||
        std::is_same_v<TagTy, only_const_t>, 
        bool> = true >
      auto operator [] ( KeywordTy const& key ) const
        -> decltype( _RuleTy()( derived_, key ) )
      { return _RuleTy()( derived_, key ); }
    };

template<
    typename _DerivedTy,
    typename _TraitsTy = mixing_traits<_DerivedTy> >
    struct add_basic_methods
    {
      using accessor_t = typename _TraitsTy::accessor_t;
      // declare derived_ and add_basic_methods() = default
      MTL_DECL_PRIV_PTR_TO_THE_DERIVED_CLASS_AND_PUB_DEF_CONSTR( add_basic_methods, _DerivedTy )

      bool empty() const noexcept 
      { return accessor_t::container( derived_ ).empty(); }

      void clear() noexcept 
      { accessor_t::container( derived_ ).clear(); }
    };

template<
    typename _DerivedTy,
    typename _TagTy    = with_const_t,
    typename _TraitsTy = mixing_traits<_DerivedTy> >
    struct add_iterators { };

template<    
    typename _DerivedTy,
    typename _TraitsTy>
    struct add_iterators<_DerivedTy, with_const_t, _TraitsTy>
    {
      using accessor_t = typename _TraitsTy::accessor_t;
      // declare derived_ and add_iterators() = default
      MTL_DECL_PRIV_PTR_TO_THE_DERIVED_CLASS_AND_PUB_DEF_CONSTR( add_iterators, _DerivedTy )

      auto begin()
        -> decltype( accessor_t::container( derived_ ).begin() ) 
      { return accessor_t::container( derived_ ).begin(); }

      auto end()
        -> decltype( accessor_t::container( derived_ ).end() ) 
      { return accessor_t::container( derived_ ).end(); }

      auto begin() const
        -> decltype( accessor_t::container( derived_ ).cbegin() ) 
      { return accessor_t::container( derived_ ).begin(); }

      auto end() const
        -> decltype( accessor_t::container( derived_ ).cend() ) 
      { return accessor_t::container( derived_ ).end(); }

      auto cbegin() const -> decltype( accessor_t::container( derived_ ).cbegin() ) 
      { return accessor_t::container( derived_ ).cbegin(); }

      auto cend() const -> decltype( accessor_t::container( derived_ ).cbegin() ) 
      { return accessor_t::container( derived_ ).cend(); }
    };

template<
    typename _DerivedTy,
    typename _TraitsTy>
    struct add_iterators<_DerivedTy, only_const_t, _TraitsTy>
    {
      using accessor_t = typename _TraitsTy::accessor_t;
      // declare derived_ and add_iterators() = default
      MTL_DECL_PRIV_PTR_TO_THE_DERIVED_CLASS_AND_PUB_DEF_CONSTR( add_iterators, _DerivedTy )

      auto begin() const
        -> decltype( accessor_t::container( derived_ ).cbegin() ) 
      { return accessor_t::container( derived_ ).begin(); }

      auto end() const
        -> decltype( accessor_t::container( derived_ ).cend() ) 
      { return accessor_t::container( derived_ ).end(); }

      auto cbegin() const -> decltype( accessor_t::container( derived_ ).cbegin() ) 
      { return accessor_t::container( derived_ ).cbegin(); }

      auto cend() const -> decltype( accessor_t::container( derived_ ).cbegin() ) 
      { return accessor_t::container( derived_ ).cend(); }
    };

template<typename _OtherTy>
    struct add_non_equalable
    {
      virtual bool operator == (_OtherTy const&) = 0;

      bool operator != ( _OtherTy const& other ) 
      { return !( *this == other ); }
    };

template<
    typename _DerivedTy,
    typename _TraitsTy = mixing_traits<_DerivedTy> >
    struct add_sum
    {
      using return_t = typename _TraitsTy::return_t;

      virtual _DerivedTy& operator += (typename _TraitsTy::arg_t const&) = 0;

      return_t operator + ( typename _TraitsTy::arg_t const& other ) const
      { return_t( static_cast<const _DerivedTy&>( *this ) ) += other; }
    };

template<
    typename _DerivedTy,
    typename _TraitsTy = mixing_traits<_DerivedTy> >
    struct add_difference
    {
      using return_t = typename _TraitsTy::return_t;

      virtual _DerivedTy& operator -= ( typename _TraitsTy::arg_t const& ) = 0;

      return_t operator - ( typename _TraitsTy::arg_t const& other ) const
      { return_t( static_cast<const _DerivedTy&>( *this ) ) -= other; }
    };

template<
    typename _DerivedTy,
    typename _TraitsTy = mixing_traits<_DerivedTy> >
    struct add_multiplying
    {
      using return_t = typename _TraitsTy::return_t;

      virtual _DerivedTy& operator *= (typename _TraitsTy::arg_t const&) = 0;

      return_t operator * ( typename _TraitsTy::arg_t const& other )
      { return return_t( static_cast<const _DerivedTy&>( *this ) ) *= other; }
    };

template<
    typename _DerivedTy,
    typename _TraitsTy = mixing_traits<_DerivedTy> >
    struct add_division
    {
      using return_t = typename _TraitsTy::return_t;

      virtual _DerivedTy& operator /= (typename _TraitsTy::arg_t const&) = 0;

      return_t operator / ( typename _TraitsTy::arg_t const& other ) const
      { return return_t( static_cast<const _DerivedTy&>( *this ) ) /= other; }
    };
} // mtl
#endif
