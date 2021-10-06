// c++17 @Tarnakin V.D.
//this header has a description of the sparse matrix container
#pragma once
#ifndef MTL_SPARSE_MATRIX_HPP
#define MTL_SPARSE_MATRIX_HPP

#include "../base_mixing_templates.hpp"
#include "../type_traits.hpp"
#include "../algorithm.hpp"

#include <unordered_map>

#ifdef CXX_BUILDER_CXX17
# include <boost/optional.hpp>
# define MTL_NULLOPT boost::none
# define MTL_OPTIONAL(type) boost::optional<type>
#else
# include <optional>
# define MTL_NULLOPT std::nullopt
# define MTL_OPTIONAL(type) std::optional<type>
#endif

namespace mtl { namespace sparse {

template<
	  typename _Ty, 
    typename = elem_traits<typename std::remove_pointer<_Ty>::type> >
	  class sparse_vector;
// private members types of vector
template<typename _Ty>
	  struct private_types_list< sparse_vector<_Ty> >
	  {
	    using impl_t      = sparse_vector<_Ty>;
	    using container_t = std::unordered_map<size_t, _Ty>;
	  };
// vector accessor
	  MTL_DECLARE_ACCESSOR( vector_accessor )
// vector mixing
template<
	  typename _Ty,
	  typename _ElemTraitsTy>
	  using vec_mixing_list_t = mixing_list
    <
	    add_iterators< vector_accessor< private_types_list< sparse_vector<_Ty> > > >,
	    add_non_equalable< sparse_vector<_Ty>, sparse_vector<typename _ElemTraitsTy::type_t> >,
      add_non_equalable< sparse_vector<_Ty>, sparse_vector<typename _ElemTraitsTy::pointer_t> >,
      add_sum< sparse_vector<_Ty>, sparse_vector<typename _ElemTraitsTy::type_t> >,
      add_sum< sparse_vector<_Ty>, sparse_vector<typename _ElemTraitsTy::pointer_t> >,
      add_difference< sparse_vector<_Ty>, sparse_vector<typename _ElemTraitsTy::type_t> >,
      add_difference< sparse_vector<_Ty>, sparse_vector<typename _ElemTraitsTy::pointer_t> >,
      add_division_by_value< sparse_vector<_Ty>, _ElemTraitsTy, sparse_vector<typename _ElemTraitsTy::type_t> >,
	    add_division_by_value< sparse_vector<_Ty>, _ElemTraitsTy, sparse_vector<typename _ElemTraitsTy::pointer_t> >
    >;
// sparse vector container
template<
	  typename _Ty,
    typename _ElemTraitsTy>
	  class sparse_vector : public vec_mixing_list_t<_Ty, _ElemTraitsTy>
    {
	    using private_types_list_t = private_types_list< sparse_vector<_Ty> >;
	    using container_t          = typename private_types_list_t::container_t;

	    friend struct vector_accessor<private_types_list_t>;
      friend class  matrix<typename _ElemTraitsTy::type_t>;

      using o_size_t         = MTL_OPTIONAL(size_t);
public : // definitions
      using type_t           = typename _ElemTraitsTy::type_t;
      using pointer_t        = typename _ElemTraitsTy::pointer_t;
      using const_pointer_t  = const type_t*;
      using reference_t      = typename _ElemTraitsTy::reference_t;
      using init_list_t      = std::initializer_list<_Ty> const&;
	    using iterator_t       = typename container_t::iterator;
	    using const_iterator_t = typename container_t::const_iterator;
private :
      o_size_t    fst_indx_;
      o_size_t    lst_indx_;
      type_t      frequent_value_;
      container_t container_;
public :
      sparse_vector() = default;

      sparse_vector( size_t size ) 
        : vec_mixing_list_t<_Ty, _ElemTraitsTy>()
        , fst_indx_( MTL_NULLOPT )
        , lst_indx_( MTL_NULLOPT )
        , container_( size )
      { }

      sparse_vector( sparse_vector const& other )
        : container_( other.container_ )
      { }

      sparse_vector( sparse_vector && other ) noexcept
        : container_( std::move( other.container_ ) )
      { }

  template<
      typename _MatrixTy,
      typename _EnableTy = _Ty,
      is_pointer_t<_EnableTy> = true>
      sparse_vector( _MatrixTy const& matrix, size_t const& i )
        : sparse_vector()
      {
        size_t size( matrix.csize() );
        if( i >= size ) {
            throw MTL_EXCEPTION( "<sparse_vector::sparse_vector> : bad <matrix> line <index aka i> ( matrix[_index_][-] )" );
        }
        size_t j( 0 );
        fst_indx_ = i*size + j;
        lst_indx_ = i*size;
        for( ; j < size; j++ )
        {
            auto itr = matrix.container_.find( i*size + j );
            if( itr != std::end( matrix.container_ ) ) {
                container_[j] = &( itr->second );
            }
        }
      }

      sparse_vector( init_list_t list )
        : sparse_vector()
      {
        fst_indx_ = 0;
        lst_indx_ = list.size() - 1;
        for( auto const& col : list ) {
            container_[ ( *fst_indx_ )++ ] = col;
	      }
      }

      bool do_sparse() noexcept 
      {
        const auto fst = std::begin( container_ );
        const auto lst = std::end( container_ );
        auto freq = frequent( fst, lst );
        return erase_if( fst, lst, [&freq]( auto const& itr ) { return *itr == freq; } );
      }

  template<typename _EnableTy = _Ty,
      is_pointer_t<_EnableTy> = false>
      iterator_t erase( const_iterator_t itr ) 
      { return container_.erase( itr ); }

  template<
      typename _CompareTy,
      typename _EnableTy = _Ty,
      is_pointer_t<_EnableTy> = false>
      bool erase_if( const_iterator_t fst, const_iterator_t lst, _CompareTy c ) 
      {
        container_t container;
        while( fst != lst )
        {
            if( !c( fst ) ) {
                container_.emplace( fst );  
            }
            ++fst;
        }
        if( !container.empty() ) {
          container_ = std::move( container ); 
          return true;
        }
        return false;
      }

  template<
      typename _CompareTy,
      typename _EnableTy = _Ty,
      is_pointer_t<_EnableTy> = false>
      void merge( sparse_vector const& other, bool sparse = false ) 
      {
        auto size = size();
        if( size != std::size( other.container_ ) )
            throw MTL_EXCEPTION( "<sparse_vector::merge> : any size of <other.container_>" );
        auto fst = std::begin( other.container_ );
        auto lst = std::end( other.container_ );
        for( ; fst != lst; fst++, size++ )
            container_.emplace( { size, *fst } );
        if( sparse )
            do_sparse();
      }

      vector & operator = ( vector const& other )
      {
        if( this == &other )
            return *this;
        if( size != std::size( other.container_ ) )
            throw MTL_EXCEPTION( "<sparse_vector::operator=(&)> : any size of <other.container_>" );
        container_ = other.container_;
        return *this;
      }

      vector & operator = ( vector && other ) noexcept
      {
        if( this == &other )
            return *this;
        if( size != std::size( other.container_ ) )
            throw MTL_EXCEPTION( "<sparse_vector::operator=(&&)> : any size of <other.container_>" );
        container_ = std::move( other.container_ );
        return *this;
      }

  template<
      typename Ty,
      typename _EnableTy = _Ty,
      is_pointer_t<_EnableTy> = true>
      operator vector<Ty> ()
      {
        auto size = size();
        vector<Ty> vector( size );
        for( size_t i(0); i < size; i++ )
            vector[i] = *container_[i];
        return vector;
      }

      bool operator == ( vector<pointer_t> const& other )
      {
        auto size = size();
        if( size != other.size() ) {
            return false;
        }
        for( size_t i(0); i < size; i++ )
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

      bool operator == ( vector<type_t> const& other )
      {
        auto size = size();
        if( size != other.size() ) {
            return false;
        }
        for( size_t i(0); i < size; i++ )
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

      vector & operator += ( vector<pointer_t> const& other )
      {
        auto size();
        if( size != std::size( other.container_ ) ) {
            throw MTL_EXCEPTION( "<sparse_vector::merge> : any size of <other.container_>" );
        }
        for( size_t i(0); i < size; i++ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
             ( *container_[i] ) += other[i];
            } else {
                container_[i] += other[i];
            }
        }
        return *this;
      }

      vector & operator += ( vector<type_t> const& other )
      {
        auto size();
        if( size != std::size( other.container_ ) ) {
            throw MTL_EXCEPTION( "<sparse_vector::merge> : any size of <other.container_>" );
        }
        for( size_t i(0); i < size; i++ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
             ( *container_[i] ) += other[i];
            } else {
                container_[i] += other[i];
            }
        }
        return *this;
      }

      vector & operator -= ( vector<pointer_t> const& other )
      {
        auto size();
        if( size != std::size( other.container_ ) ) {
            throw MTL_EXCEPTION( "<sparse_vector::merge> : any size of <other.container_>" );
        }
        for( size_t i(0); i < size; i++ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
             ( *container_[i] ) -= other[i];
            } else {
                container_[i] -= other[i];
            }
        }
        return *this;
      }

      vector & operator -= ( vector<type_t> const& other )
      {
        auto size();
        if( size != std::size( other.container_ ) ) {
            throw MTL_EXCEPTION( "<sparse_vector::merge> : any size of <other.container_>" );
        }
        for( size_t i(0); i < size; i++ )
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
        if( j >= size() ) {
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
        if( j >= size() ) {
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
	  typename = elem_traits<_Ty> >
	  class matrix;
// private members types of matrix
template<typename _Ty>
	  struct private_types_list< matrix<_Ty> >
	  {
	    using impl_t      = matrix<_Ty>;
	    using container_t = sparse_vector<_Ty>;
	  };
// matrix accessor
	  MTL_DECLARE_ACCESSOR( matrix_accessor )
// matrix mixing list
template<
    typename _MatrixTy,
    typename _ElemTraitsTy>
    using mtx_mixing_list_t = mixing_list
    <
	    add_iterators< matrix_accessor< private_types_list<_MatrixTy> > >,
	    add_non_equalable< _MatrixTy >,
	    add_sum< _MatrixTy >,
      add_difference< _MatrixTy >,
      add_division_by_value< _MatrixTy, _ElemTraitsTy >
	>;
// matrix container
template<
	  typename _Ty,
	  typename _ElemTraitsTy>
	  class matrix final : public mtx_mixing_list_t<matrix<_Ty>, _ElemTraitsTy>
	  {
      static_assert(
        !std::is_pointer_v<_Ty>,
		    "< mtl::sparse::matrix<_Ty> > : no specialization of class for pointer"
      );

	    using private_types_list_t = private_types_list< matrix<_Ty> >;
	    using container_t          = typename private_types_list_t::container_t;

	    friend struct matrix_accessor<private_types_list_t>;
	    friend class  sparse_vector<_Ty*>;
public : // definitions
      
private :
      size_t      col_size_;
      container_t container_;
public :
      matrix() = default;

      matrix( matrix const & other )
        : container_( other.container_ )
      { }

      matrix( matrix && other ) noexcept
        : container_( std::move( other.container_ ) )
      { }

	    matrix( size_t size, size_t col_size, size_t value = 0 )
        : mtx_mixing_list_t<matrix<_Ty>, _ElemTraitsTy>()
        , col_size_( col_size )
		    , container_( size*col_size )
	    {
		    if( value )
			      for( size_t i(0); i < size*col_size_; i++ )
				        container_[i] = value;
      }

      matrix( init_list_t<> list )
        : mtx_mixing_list_t<matrix<_Ty>, _ElemTraitsTy>()
        , col_size_( list.size() )
        , container_( list.size() )
      { this->operator=( list ); }

      matrix( init_list_t<vector_t> list )
        : mtx_mixing_list_t<matrix<_Ty>, _ElemTraitsTy>()
        , col_size_( std::begin( list )->size() )
        , container_( list.size() )
      { this->operator=( list ); }

      size_t size() const noexcept 
      { return std::size( container_ )/col_size_; }

      size_t csize() const noexcept 
      { return col_size_; }

      void resize( size_t size )
      {
        if( size == container_.size()  )
            return;
        container_.resize( size );
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

      void insert( vector_t vector, size_t i = 0 )
      {
        if( i >= container_.size() )
            throw MTL_EXCEPTION( "<matrix::insert> : bad insert position" );
        container_.insert( container_.begin() + i*col_size_, vector.begin(), vector.end() );
      }

      void erase( size_t i )
      {
        if( i >= container_.size() )
            throw MTL_EXCEPTION( "<matrix::erase> : bad erase position" );
        auto it( container_.begin() + i*col_size_ );
        container_.erase( it, it + col_size_ );
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

      matrix operator * ( matrix const& other ) 
      {
        matrix r( size() );
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
        if( col_size_ > list.size() || list.size()%col_size_ != 0 )
            throw MTL_EXCEPTION( "<matrix::operator=> : bad <initializer_list> size" );
        std::copy( list.begin(), list.end(), container_.begin() );
        return *this;
      }

      matrix & operator = ( init_list_t<vector_t> list )
      {
        auto nxt = std::begin( list ) + 1;
        auto lst = std::end( list );
        while( nxt != lst ) 
        {
            if( nxt->size() != col_size_ ) {
                throw MTL_EXCEPTION( "<matrix::operator=> : bad <size> of <vector>" );
            }
            insert( vector );
        }
        return *this;
      }

      ptrs_vector_t operator [] ( size_t const& i )
      {
        if( i >= std::size( container_ ) )
            throw MTL_EXCEPTION( "<matrix::operator[]> : <i> >= <col_size_> | <matrix> is empty" );
        return vector<_Ty*>( *this, i );
      }

      const ptrs_vector_t operator [] ( size_t const& i ) const
      {
        if( i >= std::size( container_ ) )
            throw MTL_EXCEPTION( "<matrix::operator[]> : <i> >= <col_size_> | <matrix> is empty" );
        return vector<_Ty*>( *this, i );
      }
private :

      void multiply( _Ty *r, matrix const& m )
      {
        if( col_size_ != std::size( m ) ) {
            throw MTL_EXCEPTION( "<matrix::multiply> : col1 != row2" );
        }
        auto m_ = m.data();
        auto col_size = m.csize();
        for( size_t i(0); i < std::size( container_ ); i++ )
        {
            for( size_t j(0); j < col_size; j++ ) 
            {
                for( size_t k(0); k < col_size_; k++ ) 
                {
                    r[i*col_size + j] += container_[i*col_size_ + k]*m_[k*col_size + j];
                }
            }
        }
      }
    }; // end matrix container
// deduction guide
template<typename _Ty>
    vector(std::initializer_list<_Ty> const&) -> vector<_Ty>;

template<typename _Ty>
    matrix(std::initializer_list<_Ty> const&) -> matrix<_Ty>;

template<typename _Ty>
    matrix(std::initializer_list< vector<_Ty> > const&) -> matrix<_Ty>;

template<typename _Ty>
    matrix(std::initializer_list< vector<_Ty*> > const&) -> matrix<_Ty>;
} } // mtl::sparse
#endif