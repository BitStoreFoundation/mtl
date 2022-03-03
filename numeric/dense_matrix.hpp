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

namespace mtl::numeric {

template<
    typename _Ty     = std::ptrdiff_t,
    std::size_t      = 3,
    typename         = insert_t,
    typename _Traits = elem_traits<_Ty> >
    class dense_vector;

template<
    typename    = std::ptrdiff_t,
    std::size_t = 3>
    class dense_matrix;
// matrix with size 2xn/3xn/4xn
template<typename _Ty>
    using matrix2_t = dense_matrix<_Ty, 2>;
template<typename _Ty>
    using matrix3_t = dense_matrix<_Ty, 3>;
template<typename _Ty>
    using matrix4_t = dense_matrix<_Ty, 4>;
template<
    typename _Ty,
    std::size_t size>
    using matrix_n_t = dense_matrix<_Ty, size>;
// vector with 2/3/4 elements
template<
    typename _Ty,
    typename tag_ = insert_t>
    using vector2_t = dense_vector<_Ty, 2, tag_>;
template<
    typename _Ty,
    typename tag_ = insert_t>
    using vector3_t = dense_vector<_Ty, 3, tag_>;
template<
    typename _Ty,
    typename tag_ = insert_t>
    using vector4_t = dense_vector<_Ty, 4, tag_>;
template<
    typename _Ty,
    std::size_t size,
    typename tag_ = insert_t>
    using vector_n_t = dense_vector<_Ty, size, tag_>;
// deduction guide
template<
    typename _Ty, 
    typename ... _ArgsTy>
    dense_vector( _Ty, _ArgsTy... ) -> dense_vector<_Ty, 1 + sizeof...( _ArgsTy )>;
template<
    typename _Ty, 
    size_t col_size>
    dense_matrix(std::initializer_list< vector_n_t<_Ty, col_size> > const&) -> dense_matrix<_Ty, col_size>;

} namespace mtl {
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
    struct access_traits< numeric::vector_n_t<_Ty, size, numeric::view_t> >
    { using container_t = std::vector<_Ty>; };
// private members types of matrix
template<
    typename _Ty,
    std::size_t size>
    struct access_traits< numeric::matrix_n_t<_Ty, size> >
    { using container_t = std::vector<_Ty>; };

} namespace mtl::numeric {

template<
    typename _DerivedTy, 
    typename _Ty,
    typename tag_,
    typename _AddMixingListTy,
    typename _TraitsTy = mixing_traits<_DerivedTy>,
    typename _IndexTy  = index<_DerivedTy, tag_> >
    struct add_dense_operations_impl : public _AddMixingListTy
    {
      using accessor_t = typename _TraitsTy::accessor_t;
      // declare derived_ and add_dense_operations_impl() = default
      MTL_DECL_PRIV_PTR_TO_THE_DERIVED_CLASS_AND_PUB_DEF_CONSTR( add_dense_operations_impl, _DerivedTy )

      _DerivedTy& operator *= ( _Ty const& value )
      { 
        auto *array = derived_->data();
        _IndexTy index;
        MTL_FOREACH( i, derived_->size() )
            array[index( derived_, i )] *= value;
        return *derived_; 
      }

      _DerivedTy& operator /= ( _Ty const& value )
      {
        if( value == 0 )
            throw exception_t( "<dense_vector/=> : value is null" );
        return *this *= (1.0/value); 
      }

      _DerivedTy& operator += ( typename _TraitsTy::arg_t const& other )
      { 
        auto *array = derived_->data();
        auto const* other_array = other.data();
        _IndexTy index;
        MTL_FOREACH( i, derived_->size() )
            array[index( derived_, i )] += other_array[i];
        return *derived_; 
      }

      _DerivedTy& operator -= ( typename _TraitsTy::arg_t const& other )
      { 
        auto *array = derived_->data();
        auto const* other_array = other.data();
        _IndexTy index;
        MTL_FOREACH( i, derived_->size() )
            array[index( derived_, i )] -= other_array[i];
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
      add_iterators<_DerivedTy>,
      add_access_by_key<_DerivedTy>,
      add_dense_operations_impl<
        _DerivedTy,
        _Ty,
        default_t,
        add_default_dense_mixing_t<
          _DerivedTy, 
          _Ty
        >
      >
    >;
// vector container
template<
    typename _Ty,
    std::size_t size_,
    typename _Traits>
    class dense_vector<_Ty, size_, insert_t, _Traits> final
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
      using reference_t       = typename _Traits::reference_t;
      using const_reference_t = typename _Traits::const_reference_t;
      using insert_vector_t   = vector_n_t<type_t, size_>;
      using init_list_t       = std::initializer_list<type_t> const&;
      using container_t       = typename access_traits<insert_vector_t>::container_t;
private : 
      container_t container_;
public :
      dense_vector( type_t val = 0 )
      {
        for( auto &elem : container_ )
            elem = val;
      }

      dense_vector( insert_vector_t const& other )
        : container_( other.container_ )
      { }

      dense_vector( insert_vector_t && other ) noexcept
        : container_( std::move( other.container_ ) )
      { }

      dense_vector( init_list_t list )
      {
        if( size_ < std::size( list ) )
            throw exception_t( "<dense_vector> : bad initializer_list <size>" );
        std::size_t j( 0 );
        for( auto const& col : list )
            container_[j++] = col;
      }

      std::size_t size() const noexcept 
      { return size_; }

      auto data() noexcept -> decltype( container_.data() )
      { return container_.data(); }

      auto data() const noexcept -> decltype( container_.data() )
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

      bool operator == ( insert_vector_t const& other )
      { 
        MTL_FOREACH( i, std::size( container_ ) )
            if( container_[i] != other[i] )
                return false;
        return true; 
      }

      const_reference_t at( std::size_t const& j ) const
      { return container_.at( j ); }

      reference_t at( std::size_t const& j )
      { return container_.at( j ); }
  }; // end vector container

template<
    typename _DerivedTy,
    typename _Ty,
    std::size_t size_,
    typename _InsertVecTy = vector_n_t<_Ty, size_> >
    using add_dense_view_vec_mixing_t = mixing_list<
      add_access_by_key<
        _DerivedTy, default_t, default_t, 
        mixing_traits<_DerivedTy>,
        index<_DerivedTy, view_t>
      >,
      add_dense_operations_impl<
        _DerivedTy,
        _Ty,
        view_t,
        add_default_dense_mixing_t<
          _DerivedTy, 
          _Ty,
          _InsertVecTy
        >,
        mixing_traits<
          _DerivedTy, 
          _InsertVecTy
        >
      >
    >;

template<
    typename _Ty,
    std::size_t size_,
    typename _Traits>
    class dense_vector<_Ty, size_, view_t, _Traits> final 
      : public add_dense_view_vec_mixing_t<vector_n_t<_Ty, size_, view_t>, _Ty, size_> 
    {
      friend struct default_accessor< vector_n_t<_Ty, size_, view_t> >;
      friend struct dense_matrix<_Ty, size_>;

      using type_t          = _Ty;
      using view_vector_t   = vector_n_t<type_t, size_, view_t>;
      using insert_vector_t = vector_n_t<_Ty, size_>; 
      using container_t     = typename access_traits<view_vector_t>::container_t;

      container_t &container_;
      const std::size_t i_;
public :
      dense_vector() = delete;
      dense_vector(view_vector_t const&) = delete;
      view_vector_t& operator = (view_vector_t const&) = delete;

      std::size_t size() const
      { return size_; }

      std::size_t index() const
      { return i_; }

      auto begin() -> decltype( std::begin( container_ ) ) 
      { return std::begin( container_ ) + i_*size_; }

      auto end() -> decltype( std::end( container_ ) ) 
      { return std::end( container_ ) + (1 + i_)*size_; }

      auto begin() const -> decltype( std::cbegin( container_ ) ) 
      { return std::cbegin( container_ ) + i_*size_; }

      auto end() const -> decltype( std::cend( container_ ) ) 
      { return std::cend( container_ ) + (1 + i_)*size_; }

      auto cbegin() const -> decltype( std::cbegin( container_ ) ) 
      { return std::cbegin( container_ ) + i_*size_; }

      auto cend() const -> decltype( std::cend( container_ ) ) 
      { return std::cend( container_ ) + (1 + i_)*size_; }

      auto data() noexcept -> decltype( container_.data() )
      { return container_.data() + i_*size_; }

      auto data() const noexcept -> decltype( container_.data() )
      { return container_.data() + i_*size_; }

      operator vector_n_t<type_t, size_> () const
      { 
        vector_n_t<type_t, size_> v; 
        MTL_FOREACH( j, size_ )
            v[j] = container_[i_*size_ + j];
        return v;
      }

      bool operator == ( insert_vector_t const& other )
      { 
        MTL_FOREACH( j, std::size( container_ ) )
            if( container_[i_*size_ + j] != other[j] )
                return false;
        return true; 
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
    typename _ViewVecTy = vector_n_t<_Ty, size_, view_t> >
    using add_dense_mtx_mixing_t = mixing_list<
      add_iterators<_DerivedTy>,
      add_dense_operations_impl<
        _DerivedTy,
        _Ty,
        default_t,
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
    class dense_matrix final : public add_dense_mtx_mixing_t<matrix_n_t<_Ty, col_size_>, _Ty, col_size_>
    {
      static_assert(
        !std::is_pointer_v<_Ty>,
        "< mtl::dense_matrix> : no specialization of class for pointer"
      );

      static_assert(
        !is_null_size_v<col_size_>,
        "< mtl::dense_matrix> : <col_size_> == <0>"
      );

      friend struct default_accessor< dense_matrix<_Ty, col_size_> >;
      friend class  dense_vector<_Ty, col_size_, view_t>;

  template<typename Ty>
      using init_list_t = std::initializer_list<Ty> const&;
public :
      using type_t           = _Ty;
      using matrix_t         = matrix_n_t<type_t, col_size_>;
      using container_t      = typename access_traits<matrix_t>::container_t;
      using iterator_t       = typename container_t::iterator;
      using const_iterator_t = typename container_t::const_iterator;
      using insert_vector_t  = vector_n_t<type_t, col_size_>;
      using view_vector_t    = vector_n_t<type_t, col_size_, view_t>;
      using add_multiplying<matrix_t, mixing_traits<matrix_t, type_t> >::operator*=;
private :
      mutable container_t container_;
public :
      dense_matrix() = default;

      dense_matrix( matrix_t const& other )
        : container_( other.container_ )
      { }

      dense_matrix( matrix_t && other ) noexcept
        : container_( std::move( other.container_ ) )
      { }

      explicit dense_matrix( std::size_t size, std::size_t value = 0 )
        : container_( size*col_size_, value )
      { }

      dense_matrix( init_list_t<type_t> list )
        : container_( list )
      {
        auto size = std::size( list );
        if( col_size_ > size || size%col_size_ != 0 )
            throw exception_t( "<dense::matrix::matrix(init_list<_Ty>)> : bad <initializer_list> size" );
      }

      dense_matrix( init_list_t<insert_vector_t> list )
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

      bool operator == ( matrix_t const& other )
      { 
        if( std::size( container_ ) != std::size( other.container_ ) )
            return false;
        MTL_FOREACH( i, std::size( container_ ) )
            if( container_[i] != other.container_[i] )
                return false;
        return true; 
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

      matrix_t& operator = ( init_list_t<insert_vector_t> list )
      { return __assign( list ); }

      view_vector_t operator [] ( std::size_t const& i )
      {
        if( i >= std::size( container_ ) )
            throw exception_t( "<dense_matrix[]> : bad access" );
        return view_vector_t( *this, i );
      }

      const view_vector_t operator [] ( std::size_t const& i ) const
      {
        if( i >= std::size( container_ ) )
            throw exception_t( "<dense_matrix[]> : bad access" );
        return view_vector_t( *this, i );
      }
private :

  template<std::size_t col_size>
      matrix_n_t<type_t, col_size> __multiply( matrix_n_t<type_t, col_size> const& m )
      {
        if( col_size_ != std::size( m ) ) {
            throw exception_t( "<dense_matrix::__multiply> : col1 != row2" );
        }
        auto size = this->size();
        matrix_n_t<type_t, col_size> r_m( size );
        auto *res_container     = r_m.data();
        auto *const rht_container = m.data();
        MTL_FOREACH( i, size )
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
} // mtl::numeric
#endif
