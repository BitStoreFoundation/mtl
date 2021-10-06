﻿// c++17 @Tarnakin V.D.
//this header has a description of the dense matrix container
#pragma once
#ifndef MTL_DENSE_MATRIX_HPP
#define MTL_DENSE_MATRIX_HPP

#include "../base_mixing_templates.hpp"
#include "../type_traits.hpp"

#include <vector>
#include <array>

namespace mtl { namespace dense {

template<
	  typename _Ty,
    size_t,
    typename = elem_traits<typename std::remove_pointer<_Ty>::type> >
	  class vector;
// private members types of vector
template<
	  typename _Ty,
	  size_t size>
  	struct private_types_list< vector<_Ty, size> >
	  {
	    using impl_t      = vector<_Ty, size>;
	    using container_t = std::array<_Ty, size>;
	  };
// vector accessor
	  MTL_DECLARE_ACCESSOR( vector_accessor )
// vector mixing
template<
	  typename _Ty,
	  size_t size,
	  typename _ElemTraitsTy>
	  using vec_mixing_list_t = mixing_list
    <
      add_base_methods< vector_accessor< private_types_list< vector<_Ty, size> > > >,
	    add_iterators< vector_accessor< private_types_list< vector<_Ty, size> > > >,
	    add_non_equalable< vector<_Ty, size>, vector<typename _ElemTraitsTy::type_t, size> >,
      add_non_equalable< vector<_Ty, size>, vector<typename _ElemTraitsTy::pointer_t, size> >,
      add_sum< vector<_Ty, size>, vector<typename _ElemTraitsTy::type_t, size> >,
      add_sum< vector<_Ty, size>, vector<typename _ElemTraitsTy::pointer_t, size> >,
      add_difference< vector<_Ty, size>, vector<typename _ElemTraitsTy::type_t, size> >,
      add_difference< vector<_Ty, size>, vector<typename _ElemTraitsTy::pointer_t, size> >,
      add_division_by_value< vector<_Ty, size>, _ElemTraitsTy, vector<typename _ElemTraitsTy::type_t, size> >,
	    add_division_by_value< vector<_Ty, size>, _ElemTraitsTy, vector<typename _ElemTraitsTy::pointer_t, size> >
    >;
// vector container
template<
	  typename _Ty,
	  size_t col_size,
    typename _ElemTraitsTy>
	  class vector final : public vec_mixing_list_t<_Ty, col_size, _ElemTraitsTy>
    {
      static_assert(
        !is_null_size_v<col_size>,
		    "< mtl::vector<_Ty, size_t> > : <col_size> == <0>"
	    );

	    using private_types_list_t = private_types_list< vector<_Ty, col_size> >;
	    using container_t          = typename private_types_list_t::container_t;

	    friend struct vector_accessor<private_types_list_t>;
public : // definitions
      using type_t           = typename _ElemTraitsTy::type_t;
      using pointer_t        = typename _ElemTraitsTy::pointer_t;
      using const_pointer_t  = const type_t*;
      using reference_t      = typename _ElemTraitsTy::reference_t;
      using init_list_t      = std::initializer_list<_Ty> const&;
	    using iterator_t       = typename container_t::iterator;
	    using const_iterator_t = typename container_t::const_iterator;
private :
      container_t container_;
public :
      vector() = default;

      vector( vector const& other )
        : container_( other.container_ )
      { }

      vector( vector && other ) noexcept
        : container_( std::move( other.container_ ) )
      { }

  template<
      typename _MatrixTy,
      typename _EnableTy = _Ty,
      is_pointer_t<_EnableTy> = true>
      vector( _MatrixTy const& matrix, size_t const& i )
        : vector()
      {
        if( i >= matrix.csize() )
            throw MTL_EXCEPTION( "<vector::vector> : bad <matrix> line <index aka i> ( matrix[_index_][-] )" );
        for( size_t j(0); j < col_size; j++ )
            container_[j] = &matrix.container_[i*col_size + j];
      }

      vector( init_list_t list )
        : vector()
      {
        if( col_size < list.size() )
            throw MTL_EXCEPTION( "<vector::vector> : bad initializer_list <size>" );
        size_t j( 0 );
        for( auto const& col : list )
            container_[j++] = col;
      }

      size_t size() const noexcept 
      { return col_size; }

      vector & operator = ( vector const& other )
      {
        if( this == &other ) return *this;
        container_ = other.container_;
        return *this;
      }

      vector & operator = ( vector && other ) noexcept
      {
        if( this == &other ) return *this;
        container_ = std::move( other.container_ );
        return *this;
      }

  template<
      typename Ty,
      typename _EnableTy = _Ty,
      is_pointer_t<_EnableTy> = true>
      operator vector<Ty, col_size> ()
      {
        vector<Ty, col_size> vector;
        for( size_t i(0); i < col_size; i++ )
            vector[i] = *container_[i];
        return vector;
      }

      bool operator == ( vector<pointer_t, col_size> const& other )
      {
        if( col_size != other.size() ) {
            return false;
        }
        for( size_t i(0); i < col_size; i++ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
                if( ( *container_[i] ) != other[i] ) {
                    return false;
                }
            } else {
                if( container_[i] != other[i] ) {
                    return false;
                }
            }
        }
        return true;
      }

      bool operator == ( vector<type_t, col_size> const& other )
      {
        if( col_size != other.size() ) {
            return false;
        }
        for( size_t i(0); i < col_size; i++ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
                if( ( *container_[i] ) != other[i] ) {
                    return false;
                }
            } else {
                if( container_[i] != other[i] ) {
                    return false;
                }
            }
        }
        return true;
      }

      vector & operator += ( vector<pointer_t, col_size> const& other )
      {
        for( size_t i(0); i < col_size; i++ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
             ( *container_[i] ) += other[i];
            } else {
                container_[i] += other[i];
            }
        }
        return *this;
      }

      vector & operator += ( vector<type_t, col_size> const& other )
      {
        for( size_t i(0); i < col_size; i++ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
             ( *container_[i] ) += other[i];
            } else {
                container_[i] += other[i];
            }
        }
        return *this;
      }

      vector & operator -= ( vector<pointer_t, col_size> const& other )
      {
        for( size_t i(0); i < col_size; i++ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
             ( *container_[i] ) -= other[i];
            } else {
                container_[i] -= other[i];
            }
        }
        return *this;
      }

      vector & operator -= ( vector<type_t, col_size> const& other )
      {
        for( size_t i(0); i < col_size; i++ )
        {
            if constexpr( std::is_pointer_v<_Ty> ) {
             ( *container_[i] ) -= other[i];
            } else {
                container_[i] -= other[i];
            }
        }
        return *this;
      }

      vector & operator *= ( type_t const& value )
      {
        for( auto & it : container_ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
             ( *it ) += value;
            } else {
                it += value;
            }
        }
        return *this;
      }

      reference_t operator [] ( size_t const& j )
      {
        if( j >= col_size ) {
            throw MTL_EXCEPTION( "<vector::operator[]> : bad access" );
        }
        if constexpr ( std::is_pointer_v<_Ty> ) {
            return *container_[j];
        } else {
            return container_[j];
        }
      }

      type_t operator [] ( size_t const& j ) const
      {
        if( j >= col_size ) {
            throw MTL_EXCEPTION( "<vector::operator[] const> : bad access" );
        }
        if constexpr(std::is_pointer_v<_Ty>) {
            return *container_[j];
        } else {
            return container_[j];
        }
      }
	  }; // end vector container
// matrix container
template<
	  typename _Ty = float,
	  size_t = 3,
	  typename = elem_traits<_Ty> >
	  class matrix;
// private members types of matrix
template<
	  typename _Ty,
	  size_t size>
	  struct private_types_list< matrix<_Ty, size> >
	  {
	    using impl_t      = matrix<_Ty, size>;
	    using container_t = std::vector<_Ty>;
	  };
// matrix accessor
	  MTL_DECLARE_ACCESSOR( matrix_accessor )
// matrix mixing list
template<
    typename _MatrixTy,
    typename _ElemTraitsTy>
    using mtx_mixing_list_t = mixing_list
    <
      add_base_methods< matrix_accessor< private_types_list<_MatrixTy> > >,
	    add_iterators< matrix_accessor< private_types_list<_MatrixTy> > >,
	    add_non_equalable< _MatrixTy >,
	    add_sum< _MatrixTy >,
      add_difference< _MatrixTy >,
      add_division_by_value< _MatrixTy, _ElemTraitsTy >
	  >;
// matrix container
template<
	  typename _Ty,
	  size_t col_size,
	  typename _ElemTraitsTy>
	  class matrix final : public mtx_mixing_list_t<matrix<_Ty, col_size>, _ElemTraitsTy>
	  {
      static_assert(
        !std::is_pointer_v<_Ty>,
		    "< mtl::matrix<_Ty, size_t> > : no specialization of class for pointer"
      );

      static_assert(
        !is_null_size_v<col_size>,
		    "< mtl::matrix<_Ty, size_t> > : <col_size> == <0>"
	    );

	    using private_types_list_t = private_types_list< matrix<_Ty, col_size> >;
	    using container_t          = typename private_types_list_t::container_t;

	    friend struct matrix_accessor<private_types_list_t>;
	    friend class  vector<_Ty*, col_size>;
public :
	    using add_multiplying_by_value<matrix<_Ty, col_size>, _ElemTraitsTy>::operator*;
      using ptrs_vector_t    = vector<_Ty*, col_size>;
      using vector_t         = vector<_Ty, col_size>;
      using type_t           = typename _ElemTraitsTy::type_t;
      using pointer_t        = typename _ElemTraitsTy::pointer_t;
	    using const_pointer_t  = const type_t*;
	    using iterator_t       = typename container_t::iterator;
	    using const_iterator_t = typename container_t::const_iterator;
  template<typename Ty = _Ty>
      using init_list_t = std::initializer_list<Ty> const&;
private :
      mutable container_t container_;
public :
      matrix() = default;

      matrix( matrix const & other )
        : container_( other.container_ )
      { }

      matrix( matrix && other ) noexcept
        : container_( std::move( other.container_ ) )
      { }

	    explicit matrix( size_t size, size_t value = 0 )
        : mtx_mixing_list_t<matrix<_Ty, col_size>, _ElemTraitsTy>()
		    , container_( size*col_size )
	    {
		    if( value )
			      for( size_t i(0); i < size*col_size; i++ )
				        container_[i] = value;
      }

      matrix( init_list_t<> list )
        : mtx_mixing_list_t<matrix<_Ty, col_size>, _ElemTraitsTy>()
        , container_( list.size() )
      { this->operator=( list ); }

      matrix( init_list_t<vector_t> list )
        : mtx_mixing_list_t<matrix<_Ty, col_size>, _ElemTraitsTy>()
        , container_( list.size() )
      { this->operator=( list ); }

      size_t size() const noexcept 
      { return std::size( container_ )/col_size; }

      size_t csize() const noexcept 
      { return col_size; }

      void resize( size_t size )
      {
        if( size == this->size() )
            return;
        if( size%col_size != 0 )
            throw MTL_EXCEPTION( "<matrix::resize> : bad new size, <size>%<col_size> != <0>" );
        container_.resize( size*col_size );
      }

      void merge( matrix const& other ) 
      {
        for( size_t i(0); i < std::size( other ); i++ )
            push_back( other[i] );
      }

      void push_front( vector_t vector ) 
      { container_.insert( container_.begin(), vector.begin(), vector.end() ); }

      void push_back( vector_t vector ) 
      { container_.insert( container_.end(), vector.begin(), vector.end() ); }

  // push vector to the end of matrix if
template<typename _CompareTy>
    bool push_back_if( vector_t const& v, _CompareTy const& c )
    {
      for( size_t i(0); i < std::size( m ); i++ ) 
          if( !c( m[i] ) ) 
              return false;
      push_back( v );
      return true;
    }

      void insert( vector_t vector, size_t i = 0 )
      {
        if( i >= container_.size() ) {
            throw MTL_EXCEPTION( "<matrix::insert> : bad insert position" );
        }
        container_.insert( container_.begin() + i*col_size, vector.begin(), vector.end() );
      }

      void erase( size_t i )
      {
        if( i >= container_.size() ) {
            throw MTL_EXCEPTION( "<matrix::erase> : bad erase position" );
        }
        auto it( container_.begin() + i*col_size );
        container_.erase( it, it + col_size );
      }
      // overloads
      bool operator == ( matrix const& other ) 
      { return container_ == other.container_; }

      matrix & operator += ( matrix const& other )
      {
        for( size_t i(0); i < container_.size(); i++ )
            container_[i] += other.container_[i];
        return *this;
      }

      matrix & operator -= ( matrix const& other )
      {
        for( size_t i(0); i < container_.size(); i++ )
            container_[i] -= other.container_[i];
        return *this;
      }

      matrix & operator *= ( _Ty const& value )
      {
        for( auto & elem : container_ )
            elem *= value;
        return *this;
      }

      matrix & operator *= ( matrix const& other )
      {
        matrix r( size() );
        multiply( r.data(), other );
        std::copy( r.begin(), r.end(), container_.begin() );
        return *this;
      }

  template<size_t col_size_>
      matrix<_Ty, col_size_> operator * ( matrix<_Ty, col_size_> const& other ) 
      {
        matrix<_Ty, col_size_> r( size() );
        multiply( r.data(), other );
        return r;
      }

      matrix & operator = ( matrix const& other )
      {
        if( this == &other ) return *this;
        container_ = other.container_;
        return *this;
      }

      matrix & operator = ( matrix && other ) noexcept
      {
        if( this == &other ) return *this;
        container_ = std::move( other.container_ );
        return *this;
      }

      matrix & operator = ( init_list_t<> list )
      {
        if( col_size > list.size() || list.size()%col_size != 0 )
            throw MTL_EXCEPTION( "<matrix::matrix> : bad <initializer_list> size" );
        std::copy( list.begin(), list.end(), container_.begin() );
        return *this;
      }

      matrix & operator = ( init_list_t<vector_t> list )
      {
        for( auto const& vector : list )
            insert( vector );
        return *this;
      }

      ptrs_vector_t operator [] ( size_t const& i )
      {
        if( i >= std::size( container_ ) )
            throw MTL_EXCEPTION( "<matrix::operator[]> : <i> >= <col_size> | <matrix> is empty" );
        return vector<_Ty*, col_size>( *this, i );
      }

      const ptrs_vector_t operator [] ( size_t const& i ) const
      {
        if( i >= std::size( container_ ) )
            throw MTL_EXCEPTION( "<matrix> : <i> >= <col_size> | <matrix> is empty" );
        return vector<_Ty*, col_size>( *this, i );
      }
private :

  template<size_t col_size_>
      void multiply( _Ty *r, matrix<_Ty, col_size_> const& m )
      {
        if( col_size != std::size( m ) ) {
            throw MTL_EXCEPTION( "<matrix::multiply> : col1 != row2" );
        }
        auto m_ = m.data();
        for( size_t i(0); i < std::size( container_ ); i++ )
        {
            for( size_t j(0); j < col_size_; j++ ) 
            {
                for( size_t k(0); k < col_size; k++ ) 
                {
                    r[i*col_size_ + j] += container_[i*col_size + k]*m_[k*col_size_ + j];
                }
            }
        }
      }
    }; // end matrix container
// deduction guide
template<
    typename _Ty, 
    typename... _ArgsTy>
    vector( _Ty, _ArgsTy... ) -> vector<_Ty, 1 + sizeof...( _ArgsTy )>;

template<
    typename _Ty, 
    size_t col_size>
    matrix(std::initializer_list< vector<_Ty, col_size> > const&) -> matrix<_Ty, col_size>;

template<
    typename _Ty, 
    size_t col_size>
    matrix(std::initializer_list< vector<_Ty*, col_size> > const&) -> matrix<_Ty, col_size>;
// matrix with size 2xn/3xn/4xn
template<typename _Ty>
    using matrix_2xn_t = matrix<_Ty, 2>;
template<typename _Ty>
    using matrix_3xn_t = matrix<_Ty, 3>;
template<typename _Ty>
    using matrix_4xn_t = matrix<_Ty, 4>;
// vector with 2/3/4 elements
template<typename _Ty>
    using vector2_t = vector<_Ty, 2>;
template<typename _Ty>
    using vector3_t = vector<_Ty, 3>;
template<typename _Ty>
    using vector4_t = vector<_Ty, 4>;
} } // mtl::dense
#endif