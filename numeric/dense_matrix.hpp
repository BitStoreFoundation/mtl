// c++17 @Tarnakin V.D.
//this header has a description of the dense matrix container
#pragma once
#ifndef MTL_NUMERIC_DENSE_MATRIX_HPP
#define MTL_NUMERIC_DENSE_MATRIX_HPP
// mtl
#include "matrix_base.hpp"
// stl
#include <vector>
#include <array>

namespace mtl { namespace numeric {

template<
    typename _Ty = std::ptrdiff_t,
    std::size_t  = 3,
    vec_tag      = vec_tag::insert>
    class dense_vector;

template<
    typename    = std::ptrdiff_t,
    std::size_t = 3>
    class dense_matrix;
// matrix with size 2xn/3xn/4xn
template<typename _Ty>
    using matrix2_t = matrix<_Ty, 2>;
template<typename _Ty>
    using matrix3_t = matrix<_Ty, 3>;
template<typename _Ty>
    using matrix4_t = matrix<_Ty, 4>;
template<
    typename _Ty,
    std::size_t size>
    using matrix_n_t = dense_matrix<_Ty, size>;
// vector with 2/3/4 elements
template<
    typename _Ty,
    vec_tag tag = vec_tag::insert>
    using vector2_t = vector<_Ty, 2, tag>;
template<
    typename _Ty,
    vec_tag tag = vec_tag::insert>
    using vector3_t = vector<_Ty, 3, tag>;
template<
    typename _Ty,
    vec_tag tag = vec_tag::insert>
    using vector4_t = vector<_Ty, 4, tag>;
template<
    typename _Ty,
    std::size_t size,
    vec_tag tag = vec_tag::insert>
    using vector_n_t = dense_matrix<_Ty, size, tag>;
// deduction guide
template<
    typename _Ty, 
    typename ... _ArgsTy>
    dense_vector( _Ty, _ArgsTy... ) -> vector_n_t<_Ty, 1 + sizeof...( _ArgsTy )>;

template<
    typename _Ty, 
    size_t col_size>
    dense_matrix(std::initializer_list< vector_n_t<_Ty, col_size> > const&) -> matrix_n_t<_Ty, col_size>;
} // numeric
// private members types of vector
template<
    typename _Ty,
    std::size_t size>
    struct access_traits< numeric::vector_n_t<_Ty, size> >
    { using container_t = std::array<_Ty, size>; };
// private members types of vector
template<
    typename _Ty,
    std::size_t size>
    struct access_traits< numeric::vector_n_t<_Ty, size, numeric::vec_tag::view> >
    { using container_t = std::vector<_Ty>; };
// private members types of matrix
template<
    typename _Ty,
    std::size_t size>
    struct access_traits< numeric::matrix_n_t<_Ty, size> >
    { using container_t = std::vector<_Ty>; };

namespace numeric {

template<
    typename _DerivedTy, 
    typename _Ty,
    typename _AddMixingListTy,
    typename _TraitsTy = mixing_traits<_DerivedTy> >
    struct add_dense_operations_impl : public _AddMixingListTy
    {
      using accessor_t = typename _TraitsTy::accessor_t;
      // declare derived_ and base_container() = default
      MTL_DECL_PRIV_PTR_TO_THE_DERIVED_CLASS_AND_PUB_DEF_CONSTR( add_dense_operations_impl, accessor_t )

      bool operator == ( typename _TraitsTy::arg_t const& other )
      { return accessor_t::container( derived_ ) == accessor<typename _TraitsTy::arg_t>::container( other ); }

      _DerivedTy& operator *= ( _Ty const& value )
      { 
        auto &container = accessor_t::container( derived_ );
        for( auto &elem : container )
            elem *= value;
        return *derived_; 
      }

      _DerivedTy& operator += ( typename _TraitsTy::arg_t const& other )
      { 
        auto &container = accessor_t::container( derived_ );
        auto const* other_container = other.data();
        MTL_FOREACH( i, std::size( container_ ) )
            container[i] += other_container[i];
        return *derived_; 
      }

      _DerivedTy& operator -= ( typename _TraitsTy::arg_t const& other )
      { 
        auto &container = accessor_t::container( derived_ );
        auto const* other_container = other.data();
        MTL_FOREACH( i, std::size( container_ ) )
            container[i] -= other_container[i];
        return *derived_; 
      }
    };

template<
    typename _DerivedTy,
    typename _Ty,
    typename _OtherTy = _DerivedTy,
    typename _Traits1 = mixing_traits<_DerivedTy, _OtherTy>,
    typename _Traits2 = mixing_traits<_DerivedTy, _Ty, _OtherTy> >
    using add_default_dense_mixing_t = mixing_list
    <
      add_basic_methods<_DerivedTy>,
      add_iterators<_DerivedTy>,
      add_non_equalable<_OtherTy>,
      add_sum<_DerivedTy, _Traits1>,
      add_difference<_DerivedTy, _Traits1>,
      add_multiplying<_DerivedTy, _Traits2>,
      add_division<_DerivedTy, _Traits2>
    >;

template<
    typename _DerivedTy,
    typename _Ty>
    using add_dense_insert_vec_mixing_t = mixing_list<
      add_access_by_key<_DerivedTy>,
      add_dense_operations_impl<
        _DerivedTy,
        _Ty,
        add_default_dense_mixing_t<
          _DerivedTy, 
          _Ty
        >
      >
    >;
// vector container
template<
    typename _Ty,
    std::size_t size_>
    class dense_vector<_Ty, size_, vec_tag::insert> final
      : public add_dense_insert_vec_mixing_t<vector_n_t<_Ty, size_>, _Ty>
    {
      static_assert(
        !std::is_pointer_v<_Ty>,
        "<mtl::numeric::dense_vector> : no specialization of class for pointer"
      );

      static_assert(
        !is_null_size_v<size_>,
        "<mtl::numeric::dense_vector> : <size_> == <0>"
      );

      friend struct default_accessor< vector_n_t<_Ty, size_> >;
public :
      using type_t            = _Ty;
      using insert_vector_t   = vector_n_t<type_t, size_>;
      using init_list_t       = std::initializer_list<type_t> const&;
      using container_t       = typename access_traits<insert_vector_t>::container_t;
private : 
      container_t container_;
public :
      dense_vector() = default;

      dense_vector( insert_vector_t const& other )
        : container_( other.container_ )
      { }

      dense_vector( insert_vector_t && other ) noexcept
        : container_( std::move( other.container_ ) )
      { }

      dense_vector( init_list_t list )
      {
        if( size_ < std::size( list ) )
            throw detail::exception_t( "<dense_vector> : bad initializer_list <size>" );
        std::size_t j( 0 );
        for( auto const& col : list )
            container_[j++] = col;
      }

      std::size_t size() const noexcept 
      { return size_; }

      auto data() noexcept
        -> decltype( container_.data() )
      { return container_.data(); }

      auto data() const noexcept
        -> decltype( container_.data() )
      { return container_.data(); }

      insert_vector_t& operator = ( insert_vector_t const& other )
      {
        if( this == &other ) return *this;
        container_ = other.container_;
        return *this;
      }

      insert_vector_t& operator = ( insert_vector_t && other ) noexcept
      {
        if( this == &other ) return *this;
        container_ = std::move( other.container_ );
        return *this;
      }
  }; // end vector container

template<
    typename _DerivedTy,
    typename _Ty,
    std::size_t size_,
    typename _InsertVecTy = vector_n_t<_Ty, size_> >
    using add_dense_view_vec_mixing_t = mixing_list<
      add_access_by_key<
        _DerivedTy, with_const_t, view_vec_rule_t, 
        mixing_traits<
          _DerivedTy, 
          _DerivedTy, 
          _Ty> 
        > 
      >,
      add_dense_operations_impl<
        _DerivedTy,
        _Ty,
        add_default_dense_mixing_t<
          _DerivedTy, 
          _Ty,
          _InsertVecTy
        >,
        mixing_traits<_DerivedTy, _InsertVecTy>
      >
    >;

template<
    typename _Ty,
    std::size_t size_>
    class dense_vector<_Ty, size_, vec_tag::view> final 
      : public add_dense_view_vec_mixing_t<vector_n_t<_Ty, size_, vec_tag::view>, _Ty, size_> 
    {
      friend struct default_accessor< vector_n_t<_Ty, size_, vec_tag::view> >;
      friend struct view_vector_accessor< vector_n_t<_Ty, size_, vec_tag::view> >;
      friend class  dense_matrix<_Ty, size_>;

      using type_t        = _Ty;
      using view_vector_t = vector_n_t<type_t, size_, vec_tag::view>;
      using container_t   = typename access_traits<view_vector_t>::container_t;

      container_t container_;
      const std::size_t i_;
public :
      dense_vector() = delete;
      dense_vector(view_vector_t const&) = delete;
      view_vector_t& operator = (view_vector_t const&) = delete;

      operator vector_n_t<std::remove_const<type_t>, size_> ()
      { 
        vector_n_t<std::remove_const<type_t>, size_> v; 
        MTL_FOREACH( i, size_ )
            v[i] = container_[i_*size_ + j];
        return v;
      }
private :
      dense_vector( matrix_n_t<type_t, size_> const& matrix, std::size_t const& i )
        : container_( matrix.container_ )
        , i_( i )
      {
        if( i_ >= std::size( matrix ) )
            throw exception_t( "<numeric::dense_vector::dense_vector> : <i_> >= size(<matrix>) )" );
      }
    };

template<
    typename _DerivedTy,
    typename _Ty,
    std::size_t size_,
    typename _ViewVecTy = vector_n_t<_Ty, size_, vec_tag::view> >
    using add_dense_mtx_mixing_t = mixing_list<
      add_access_by_key<
        _DerivedTy, with_const_t, mtx_rule_t, 
        mixing_traits<
          _DerivedTy, 
          _DerivedTy, 
          _ViewVecTy 
        > 
      >,
      add_dense_operations_impl<
        _DerivedTy,
        _Ty,
        add_default_dense_mixing_t<
          _DerivedTy, 
          _Ty
        >
      >
    >;
// matrix container
template<
    typename _Ty,
    std::size_t col_size_>
    class matrix final : public add_dense_mtx_mixing_t<matrix_n_t<_Ty, col_size_>, _Ty, col_size_>
    {
      static_assert(
        !std::is_pointer_v<_Ty>,
        "< mtl::dense_matrix> : no specialization of class for pointer"
      );

      static_assert(
        !is_null_size_v<col_size_>,
        "< mtl::dense_matrix> : <col_size_> == <0>"
      );

      using private_types_list_t = private_types_list< matrix<_Ty, col_size_> >;
      using container_t          = typename private_types_list_t::container_t;

      friend struct default_accessor< dense_matrix<_Ty, col_szie_> >;
      friend class  dense_vector<_Ty, col_size_>;

  template<typename Ty>
      using init_list_t = std::initializer_list<Ty> const&;
public :
      using type_t           = _Ty;
      using matrix_t         = matrix_n_t<type_t, col_size_>;
      using add_multiplying_by_value<matrix<type_t, col_size_>, type_t>::operator*;
      using container_t      = typename access_traits<matrix_t>::container_t;
      using iterator_t       = typename container_t::iterator;
      using const_iterator_t = typename container_t::const_iterator;
      using insert_vector_t  = vector_n_t<type_t, col_size_>;
      using view_vector_t    = vector_n_t<type_t, col_size_, vec_tag::insert>;
private :
      mutable container_t container_;
public :
      matrix() = default;

      matrix( matrix_t const& other )
        : container_( other.container_ )
      { }

      matrix( matrix_t && other ) noexcept
        : container_( std::move( other.container_ ) )
      { }

      explicit matrix( std::size_t size, std::size_t value = 0 )
        : container_( size*col_size_, value )
      { }

      matrix( init_list_t<type_t> list )
        : container_( list )
      {
        auto size = std::size( list );
        if( col_size_ > size || size%col_size_ != 0 )
            throw exception_t( "<dense::matrix::matrix(init_list<_Ty>)> : bad <initializer_list> size" );
      }

      matrix( init_list_t<insert_vector_t> list )
        : container_( std::size( list ) )
      { __assign( list ); }

      std::size_t size() const noexcept 
      { return std::size( container_ )/col_size_; }

      std::size_t csize() const noexcept 
      { return col_size_; }

      auto data() noexcept
        -> decltype( container_.data() )
      { return container_.data(); }

      auto data() const noexcept
        -> decltype( container_.data() )
      { return container_.data(); }

      void resize( std::size_t size )
      {
        if( size == this->size() )
            return;
        if( size%col_size_ != 0 )
            throw exception_t( "<matrix::resize> : bad new size, <size>%<col_size_> != <0>" );
        container_.resize( size*col_size_ );
      }

      void push_front( insert_vector_t v ) 
      { container_.insert( std::begin( container_ ), std::begin( v ), std::end( v ) ); }

      void push_back( insert_vector_t v ) 
      { container_.insert( std::end( container_ ), std::begin( v ), std::end( v ) ); }

  
  template<typename _CompareTy>
      bool push_back_if( insert_vector_t const& v, _CompareTy const& c )
      {
        auto const& m = *this;
        MTL_FOREACH( i, size() )
            if( !c( m[i] ) )
                return false;
        push_back( v );
        return true;
      }

      void insert( insert_vector_t v, std::size_t i = 0 )
      {
        if( i >= std::size( container_ ) )
            throw exception_t( "<matrix::insert> : bad insert position" );
        container_.insert( std::begin( container_ ) + i*col_size_, std::begin( v ), std::end( v ) );
      }

      void erase( std::size_t i )
      {
        if( i >= std::size( container_ ) )
            throw exception_t( "<matrix::erase> : bad erase position" );
        auto it( std::begin( container_ ) + i*col_size_ );
        container_.erase( it, it + col_size_ );
      }

      matrix_t& operator *= ( matrix_t const& other )
      { return __move( std::move( __multiply( other ) ) ); }

  template<std::size_t col_size>
      matrix_n_t<type_t, col_size> operator * ( matrix_n_t<type_t, col_size> const& other ) 
      { return __multiply( other ); }

      matrix_t& operator = ( matrix_t const& other )
      {         
        if( this == &other ) return *this;
        container_ = other.container_;
        return *this; 
      }

      matrix_t& operator = ( matrix_t && other ) noexcept
      { return __move( std::move( other ) ); }

      matrix_t& operator = ( init_list_t<type_t> list )
      { 
        auto size = std::size( list );
        if( col_size_ > size || size%col_size_ != 0 )
            throw exception_t( "<dense_matrix=> : bad <list> size" );
        std::copy( std::begin( list ), std::end( list ), std::begin( container_ ) );
        return *this;
      }

      matrix_t& operator = ( init_list_t<vector_t> list )
      { return __assign( other ); }

      view_vector_t operator [] ( std::size_t const& i )
      {
        if( i >= std::size( container_ ) )
            throw exception_t( "<dense_matrix[]> : <i> >= <col_size> | <matrix> is empty" );
        return view_vector_t( *this, i );
      }

      const view_vector_t operator [] ( std::size_t const& i ) const
      {
        if( i >= std::size( container_ ) )
            throw exception_t( "<dense_matrix[]> : <i> >= <col_size> | <matrix> is empty" );
        return view_vector_t( *this, i );
      }
private :

  template<std::size_t col_size>
      matrix_n_t<type_t, col_size> __multiply( matrix_n_t<type_t, col_size> const& m )
      {
        if( col_size_ != std::size( m ) ) {
            throw exception_t( "<dense_matrix::__multiply> : col1 != row2" );
        }
        matrix_n_t<type_t, col_size> r_m( size() );
        auto res_container     = r_m.data();
        const auto rht_container = m.data();
        MTL_FOREACH( i, std::size( r_container ) )
            MTL_FOREACH( j, col_size )
                MTL_FOREACH( k, col_size_ )
                    res_container[i*col_size + j] += container_[i*col_size + k]*rht_container[k*col_size + j];
        return r_m;
      }

      matrix_t& __move( matrix_t && other ) noexcept
      {
        if( this == &other ) return *this;
        container_ = std::move( other.container_ );
        return *this;
      }

      matrix_t& __assign( init_list_t<insert_vector_t> list )
      {
        for( auto const& v : list )
            push_back( v );
        return *this;
      }
    }; // end matrix container
} } // mtl::numeric
#endif
