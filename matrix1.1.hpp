// c++17 @Tarnakin V.D.
//this header has a description of the matrix container
#pragma once
//#ifndef MATRIX_HPP
//#define MATRIX_HPP

#include "base_mixing_templates.hpp"
#include "type_traits.hpp"
#include <vector>

namespace tvd {

template<
	typename _MatrixTy,
	typename _ElemTraitsTy>
	using mtx_mixing_list_t = mixing_list
	<
	  add_iterators<_MatrixTy, elem_container<_ElemTraitsTy> >,
	  add_non_equalable<_MatrixTy>,
	  add_sum<_MatrixTy>,
	  add_difference<_MatrixTy>,
	  add_division_by_value<_MatrixTy, _ElemTraitsTy>
	>;

template<
    typename _Ty,
    size_t,
	typename = elem_traits<typename std::remove_pointer<_Ty>::type> >
    class vector;
// matrix container
template<
	typename _Ty = float,
	size_t col_size = 3,
	typename _ElemTraitsTy = elem_traits<_Ty>,
	typename _MixingListTy = mtx_mixing_list_t<matrix<_Ty, col_size>, _ElemTraitsTy> >
	class matrix final : public _MixingListTy
    {
	  static_assert(
		!std::is_pointer_v<_Ty>,
		"< tvd::matrix<_Ty, size_t> > : no specialization of class for pointer"
	  );
	  friend struct access<elem_container<_ElemTraitsTy> >;
	  friend class  vector<_Ty*, col_size>;
public :
	  using add_multiplying_by_value<matrix<_Ty, col_size>, _ElemTraitsTy>::operator*;
	  using ptrs_vector_t    = vector<_Ty*, col_size>;
	  using vector_t         = vector<_Ty, col_size>;
	  using type_t           = typename _ElemTraitsTy::type_t;
	  using pointer_t        = typename _ElemTraitsTy::pointer_t;
	  using const_pointer_t  = const type_t*;
	  using iterator_t       = typename elem_container<_ElemTraitsTy>::container_t::iterator;
	  using const_iterator_t = typename elem_container<_ElemTraitsTy>::container_t::const_iterator;
  template<typename Ty = _Ty>
	  using init_list_t = std::initializer_list<Ty> const&;
private :
	  mutable std::vector<_Ty> container_;
public :
	  matrix() = default;

	  matrix( matrix const & other )
        : container_( other.container_ )
      {
      }

	  matrix( matrix && other ) noexcept
        : container_( std::move( other.container_ ) )
      {
      }

	  explicit matrix( size_t const & size )
        : mtx_mixing_list_t<matrix<_Ty, col_size>, _ElemTraitsTy>()
        , container_( size*col_size )
      {
      }

	  matrix( init_list_t<> list )
        : mtx_mixing_list_t<matrix<_Ty, col_size>, _ElemTraitsTy>()
        , container_( list.size() )
      {
        if( col_size > list.size() || list.size()%col_size != 0 ) {
			throw TVD_EXCEPTION( "<matrix::matrix> : bad <initializer_list> size" );
        }
        std::copy( list.begin(), list.end(), container_.begin() );
      }

	  matrix( init_list_t<vector_t> list )
        : mtx_mixing_list_t<matrix<_Ty, col_size>, _ElemTraitsTy>()
        , container_( list.size() )
      {
        if( col_size > list.size() || list.size()%col_size != 0 ) {
			throw TVD_EXCEPTION( "<matrix::matrix> : bad <initializer_list> <size>" );
        }
        for( auto const & vector : list ) {
            insert( vector );
        }
      }

	  matrix( size_t const & size, pointer_t array )
        : matrix( size )
      {
        for( size_t i(0); i < std::size(container_); i++ ) {
			container_[i] = array[i];
        }
        delete array;
	  }

	  bool empty() const noexcept {
		return container_.empty();
	  }

	  pointer_t data() noexcept {
        return container_.data();
      }

	  const_pointer_t data() const noexcept {
        return container_.data();
      }

	  size_t size() const noexcept {
        return container_.size()/col_size;
      }

	  size_t csize() const noexcept {
        return col_size;
      }

	  void resize( size_t size )
      {
        if( size == this->size() ) {
            return;
        }
        if( size%col_size != 0 ) {
            throw EXCEPTION( "<matrix::resize> : bad new size, <size>%<col_size> != <0>" );
        }
        container_.resize( size*col_size );
      }

	  void push_front( vector_t vector ) {
        container_.insert( container_.begin(), vector.begin(), vector.end() );
      }

	  void push_back( vector_t vector ) {
        container_.insert( container_.end(), vector.begin(), vector.end() );
      }

	  void insert( vector_t vector, size_t pos = 0 )
      {
        if( pos >= container_.size() ) {
			throw TVD_EXCEPTION( "<matrix::insert> : bad insert position" );
        }
        container_.insert( container_.begin() + pos*col_size, vector.begin(), vector.end() );
      }

	  void erase( size_t pos )
      {
        if( pos >= container_.size() ) {
			throw TVD_EXCEPTION( "<matrix::erase> : bad erase position" );
		}
        auto it( container_.begin() + pos*col_size );
        container_.erase( it, it + col_size );
      }
	  // overloads
	  bool operator == ( matrix const& other )
      {
        auto size = std::size( container_ );
        if( size != std::size( other.container_ ) ) {
            return false;
        }
        for( size_t i(0); i < size; i++ ) 
        {
            if( container_[i] != other.container_[i] ) {
                return false;
            }
        }
        return true;
      }

	  matrix & operator += ( matrix const& other )
      {
        for( size_t i(0); i < container_.size(); i++ ) {
            container_[i] += other.container_[i];
        }
        return *this;
      }

	  matrix & operator -= ( matrix const& other )
      {
        for( size_t i(0); i < container_.size(); i++ ) {
            container_[i] -= other.container_[i];
        }
        return *this;
      }

	  matrix & operator *= ( _Ty const& value )
      {
        for( auto & elem : container_ ) {
            elem *= value;
        }
        return *this;
      }

	  matrix & operator *= ( matrix const& other )
      {
        pointer_t array = multiply( other );
        for( size_t i(0); i < std::size(container_); i++ ) {
            container_[i] = array[i];
        }
        return *this;
      }

  template<size_t col_size_>
	  matrix<_Ty, col_size_> operator * ( matrix<_Ty, col_size_> const& other ) {
        return matrix<_Ty, col_size_>( size(), multiply( other ) );
      }

  template<
	  typename Ty,
	  typename _EnableTy = typename std::remove_pointer<Ty>::type,
	  is_same_t<_EnableTy, _Ty> = true>
	  matrix<_Ty, 1> operator * ( vector<Ty, col_size> const& other )
      {
        matrix<_Ty, 1> m( other.size() );
        for( size_t i(0); i < size(); i++ ) {
            m[i][0] = other[i];
        }
        return *this*m;
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
        if( container_.size() > list.size() ) {
			throw TVD_EXCEPTION( "<matrix::operator=> : bad <initializer_list> size" );
        }
        std::copy( list.begin(), list.end(), container_.begin() );
        return *this;
      }

	  matrix & operator = ( init_list_t<vector_t> list )
      {
        if( container_.size() > list.size() ) {
			throw TVD_EXCEPTION( "<matrix::operator=> : bad <initializer_list> size" );
        }
        for( auto const & vector : list ) {
            insert(vector);
        }
        return *this;
      }

	  ptrs_vector_t operator [] ( size_t const& i )
      {
        if( i >= container_.size() ) {
			throw TVD_EXCEPTION( "<matrix::operator[]> : <i> >= <col_size> | <matrix> is empty" );
        }
        return vector<_Ty*, col_size>(*this, i);
      }

	  vector_t operator [] ( size_t const& i ) const
      {
        if( i >= container_.size() ) {
			throw TVD_EXCEPTION( "<matrix> : <i> >= <col_size> | <matrix> is empty" );
        }
        vector_t vector;
        std::copy( container_.begin() + i*col_size, container_.begin() + col_size*(i + 1), vector.begin() );
        return vector;
      }
private :

  template<size_t col_size_>
	  pointer_t multiply( matrix<_Ty, col_size_> const& m )
      {
        if constexpr( std::is_pointer_v<_Ty> ) {
			static_assert(
			  false_v<_Ty>,
			  "<matrix::multiply> : no viable overloaded for pointer type"
			);
        }
        if( col_size != std::size(m) ) {
            throw EXCEPTION( "<matrix> : M1xN1 && M2xN2 -> N1 != M2" );
        }

        size_t size = std::size(container_);
        pointer_t m_res_ = new _Ty[col_size_*size];

        for( size_t i(0); i < size; i++ ) 
        {
            for( size_t j(0); j < col_size_; j++ ) 
            {
                for( size_t k(0); k < col_size; k++ ) 
                {
                    m_res_[i*col_size_ + j] += container_[i*col_size + k]*m[k][j];
                }
            }
        }
        return m_res_;
      }
	};

template<
	typename _Ty,
    size_t size,
    class _ElemTraitsTy>
	using vec_mixing_list_t = mixing_list
	<
	  add_iterators<vector<_Ty, size>, elem_container<elem_traits<_Ty> > >,
	  add_non_equalable<vector<_Ty, size>, vector<typename _ElemTraitsTy::type_t, size> >,
	  add_non_equalable<vector<_Ty, size>, vector<typename _ElemTraitsTy::pointer_t, size> >,
	  add_sum<vector<_Ty, size>, vector<typename _ElemTraitsTy::type_t, size> >,
	  add_sum<vector<_Ty, size>, vector<typename _ElemTraitsTy::pointer_t, size> >,
	  add_difference<vector<_Ty, size>, vector<typename _ElemTraitsTy::type_t, size> >,
	  add_difference<vector<_Ty, size>, vector<typename _ElemTraitsTy::pointer_t, size> >,
	  add_division_by_value<vector<_Ty, size>, _ElemTraitsTy, vector<typename _ElemTraitsTy::type_t, size> >
	>;

	struct emty_list { };

// vector container
template<
	typename _Ty,
	size_t col_size,
	class _ElemTraitsTy>
	class vector final
	  : protected matrix<_Ty, col_size, _ElemTraitsTy, emty_list>
	  , public vec_mixing_list_t<_Ty, col_size, _ElemTraitsTy>
	{
	  friend struct access< elem_container< elem_traits<_Ty> > >;
	  using base =
public : // definitions
	  using type_t           = typename _ElemTraitsTy::type_t;
	  using pointer_t        = typename _ElemTraitsTy::pointer_t;
	  using const_pointer_t  = const type_t*;
	  using reference_t      = typename _ElemTraitsTy::reference_t;
	  using init_list_t      = std::initializer_list<_Ty> const&;
	  using iterator_t       = typename elem_container<_ElemTraitsTy>::container_t::iterator;
	  using const_iterator_t = typename elem_container<_ElemTraitsTy>::container_t::const_iterator;
private :
	  std::vector<_Ty> container_;
public :

	  vector()
        : vec_mixing_list_t<_Ty, col_size, _ElemTraitsTy>()
        , container_( col_size )
      {
      }

	  vector( vector const& other )
        : container_( other.container_ )
      {
      }

	  vector( vector && other ) noexcept
        : container_( std::move( other.container_ ) )
      {
      }

  template<
	  typename _EnableTy = _Ty,
	  is_pointer_t<_EnableTy> = false>
	  vector( _Ty const& value )
        : vector()
      {
        for( auto & value_ : container_ ) {
            value_ = value;
        }
      }
    
  template<
	  typename Ty,
	  typename _EnableTy = _Ty,
	  is_pointer_t<_EnableTy> = true>
	  vector( matrix<Ty, col_size> const& matrix, size_t const& i )
        : vector()
      {
        for( size_t j(0); j < col_size; j++ ) 
        {
            if constexpr( std::is_pointer_v<Ty> ) {
                container_[j] = matrix.container_[i*col_size + j];
            } else {
                container_[j] = &matrix.container_[i*col_size + j];
            }
        }
      }

	  vector( init_list_t list )
        : vector()
      {
        if( col_size < list.size() ) {
			throw TVD_EXCEPTION( "<vector::vector> : bad initializer_list <size>" );
        }
        size_t j( 0 );
        for( auto const& col : list ) {
            container_[j++] = col;
        }
	  }

	  bool empty() const noexcept {
		return container_.empty();
	  }

	  pointer_t data() noexcept {
        return container_.data();
      }

	  const_pointer_t data() const noexcept {
        return container_.data();
      }

	  size_t size() const noexcept {
        return col_size;
      }

	  vector & operator = ( vector const& other )
      {
        if( this == &other ) {
            return *this;
        }
        container_ = other.container_;
        return *this;
      }

	  vector & operator = ( vector && other ) noexcept
      {
        if( this == &other ) {
            return *this;
        }
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
        for( size_t i(0); i < col_size; i++ ) {
            vector[i] = *container_[i];
		}
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
			throw TVD_EXCEPTION( "<vector::operator[]> : bad access" );
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
			throw TVD_EXCEPTION( "<vector::operator[] const> : bad access" );
        }
        if constexpr(std::is_pointer_v<_Ty>) {
            return *container_[j];
        } else { 
            return container_[j];
        }
      }
    };

    // deduction guide
template<
    typename _Ty, 
    typename... _ArgsTy>
    vector(_Ty, _ArgsTy...) -> vector<_Ty, 1 + sizeof...(_ArgsTy)>;

template<
    typename _Ty, 
    size_t col_size>
    matrix(vector<_Ty, col_size>) -> matrix<_Ty, col_size>;
    // matrix detail
	namespace detail {
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
    } // detail
} // tvd
//#endif