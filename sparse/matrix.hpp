// c++17 @Tarnakin V.D.
//this header has a description of the sparse matrix container
#pragma once
#ifndef MTL_SPARSE_MATRIX_HPP
#define MTL_SPARSE_MATRIX_HPP

#include "../base_mixing_templates.hpp"
#include "../algorithm.hpp"
#include "../types.hpp"

namespace mtl { namespace sparse {

template<
    typename _Ty, 
    typename = elem_traits<typename std::remove_pointer_t <_Ty> > >
    class vector;

template<
    typename _Ty = float,
    typename = elem_traits<_Ty> >
    class matrix;
} // sparse
// private members types of vector
template<typename _Ty>
    struct private_types_list< sparse::vector<_Ty> >
    {
      using impl_t      = sparse::vector<_Ty>;
      using container_t = std::unordered_map<std::size_t, _Ty>;
    };
// private members types of matrix
template<typename _Ty>
    struct private_types_list< sparse::matrix<_Ty> >
    {
      using impl_t      = sparse::matrix<_Ty>;
      using container_t = sparse::vector<_Ty>;
    };

template<typename _Ty>
    _Ty frequent( sparse::vector<_Ty> v ) 
    {
      std::unordered_map<std::size_t, std::size_t> map;
      auto fst = std::begin( v );
      auto lst = std::end( v );
      for( ; fst != lst; fst++ )
      {
          if( map.find( fst->second ) == map.end() ) {
              map[fst->second] = 1;
          } else {
              map[fst->second]++;
          }
      }
      return std::max_element( std::begin( map ), std::end( map ), 
                               [](const auto& lhs, const auto& rhs) 
                               { return lhs.second < rhs.second; }  )->first;
    }

namespace sparse {
// generate struct vector_accessor
    MTL_DECLARE_ACCESSOR( vector )
// vector mixing
template<
    typename _Ty,
    typename _ElemTraitsTy>
    struct vec_mixing_list_t 
      : public mixing_list
      <
        add_iterators< vector_accessor< private_types_list< vector<_Ty> > > >,
        add_base_methods< vector_accessor< private_types_list< vector<_Ty> > > >,
        add_non_equalable< vector<_Ty>, vector<typename _ElemTraitsTy::type_t> >,
        add_non_equalable< vector<_Ty>, vector<typename _ElemTraitsTy::pointer_t> >,
        add_sum< vector<_Ty>, vector<typename _ElemTraitsTy::type_t> >,
        add_sum< vector<_Ty>, vector<typename _ElemTraitsTy::pointer_t> >,
        add_difference< vector<_Ty>, vector<typename _ElemTraitsTy::type_t> >,
        add_difference< vector<_Ty>, vector<typename _ElemTraitsTy::pointer_t> >,
        add_division_by_value< vector<_Ty>, _ElemTraitsTy, vector<typename _ElemTraitsTy::type_t> >,
        add_division_by_value< vector<_Ty>, _ElemTraitsTy, vector<typename _ElemTraitsTy::pointer_t> >
      >
    {
      using add_sum< vector<_Ty>, vector<typename _ElemTraitsTy::type_t> >::operator+;
      using add_sum< vector<_Ty>, vector<typename _ElemTraitsTy::pointer_t> >::operator+;
      using add_difference< vector<_Ty>, vector<typename _ElemTraitsTy::type_t> >::operator-;
      using add_difference< vector<_Ty>, vector<typename _ElemTraitsTy::pointer_t> >::operator-;
      using add_division_by_value< vector<_Ty>, _ElemTraitsTy, vector<typename _ElemTraitsTy::type_t> >::operator/=;
      using add_division_by_value< vector<_Ty>, _ElemTraitsTy, vector<typename _ElemTraitsTy::pointer_t> >::operator/;
    };

// sparse vector container
template<
    typename _Ty,
    typename _ElemTraitsTy>
    class vector final : public vec_mixing_list_t<_Ty, _ElemTraitsTy>
    {
      using private_types_list_t = private_types_list< vector<_Ty> >;
      using container_t          = typename private_types_list_t::container_t;

      friend struct vector_accessor<private_types_list_t>;
      friend class  matrix<typename _ElemTraitsTy::type_t>;
public :
      using type_t            = typename _ElemTraitsTy::type_t;
      using opt_type_t        = detail::optional_t<type_t>;
      using pointer_t         = typename _ElemTraitsTy::pointer_t;
      using const_pointer_t   = const type_t*;
      using const_reference_t = type_t const&;
      using pair_t            = std::pair<std::size_t, type_t>;
      using init_list_t       = std::initializer_list<_Ty> const&;
      using iterator_t        = typename container_t::iterator;
      using const_iterator_t  = typename container_t::const_iterator;
private :
      bool        is_sparse_;
      opt_type_t  frequent_value_;
      std::size_t col_size_;
      container_t container_;
public :
      vector() = default;

      vector( std::size_t size, type_t value )
        : vec_mixing_list_t<_Ty, _ElemTraitsTy>()
        , is_sparse_( false )
        , frequent_value_()
        , col_size_( size )
        , container_( col_size_ )
      {
        for( std::size_t i(0); i < size; i++ )
            container_[i] = value;
        do_sparse(); 
      }

      vector( vector const& other )
      { _assign( other ); }

  template<
      typename Ty,
      typename _EnableTy = _Ty,
      std::enable_if_t<
        std::is_pointer_v<_EnableTy>, bool
      > = true >
      vector( matrix<Ty> const& matrix, std::size_t const& i )
        : vec_mixing_list_t<_Ty, _ElemTraitsTy>()
        , frequent_value_( matrix.container_.frequent() )
        , col_size_( matrix.col_size_ )
        , container_( col_size_ )
      {
        if( i >= col_size_ ) {
            throw detail::exception_t( "<sparse::vector::vector> : bad <matrix> line <index aka i> ( matrix[_index_][-] )" );
        }
        for( std::size_t j(0); j < col_size_; j++ )
        {
            auto itr = matrix.container_.find( i*col_size_ + j );
            if( itr != std::end( matrix.container_ ) ) {
                container_[j] = &itr->second;
            }
        }
      }

      vector( init_list_t list )
        : vector()
      { _assign( list ); }

      std::size_t size() const noexcept 
      { return col_size_; }

      std::size_t amount() const noexcept
      { return container_.size(); }

      bool sparse() const noexcept
      { return is_sparse_; }

      opt_type_t frequent() const noexcept 
      { return frequent_value_; }

      iterator_t find( std::size_t i )
      { return container_.find( i ); }

      const_iterator_t find( std::size_t i ) const
      { return container_.find( i ); }

  template<
      typename _EnableTy = _Ty,
      std::enable_if_t<
        std::is_arithmetic_v<_EnableTy>, bool
      > = true >
      bool do_sparse() noexcept
      {
        if( is_sparse_ ) return true;
        do_dense();
        auto fst = std::begin( container_ );
        auto lst = std::end( container_ );
        frequent_value_ = mtl::frequent( *this );
        auto sz = col_size_;
        is_sparse_ = erase_if( fst, lst, [this]( auto const& itr ) { return itr->second == this->frequent_value_; } );
        col_size_ = sz;
        return is_sparse_;
      }

  template<
      typename _EnableTy = _Ty,
      std::enable_if_t<
        std::is_arithmetic_v<_EnableTy>, bool
      > = true >
      bool do_dense() noexcept 
      {
        if( is_sparse_ ) {
            for( std::size_t i(0); i < col_size_; i++ )
                if( container_.find( i ) == container_.end() )
                    container_[i] = *frequent_value_;
            frequent_value_ = MTL_NULLOPT;
            is_sparse_ = false;
        }
        return true;
      }

  template<
      typename _EnableTy = _Ty,
      std::enable_if_t<
        std::is_arithmetic_v<_EnableTy>, bool
      > = true >
      void push_back( type_t value ) 
      { 
        container_[col_size_] = value;
        col_size_++;
        do_dense();
      }

  template<
      typename _EnableTy = _Ty,
      std::enable_if_t<
        std::is_arithmetic_v<_EnableTy>, bool
      > = true >
      void erase( std::size_t i )
      {
        do_dense();
        container_.erase( i );
        _align(); 
        --col_size_;
      }

  template<
      typename _CompareTy,
      typename _EnableTy = _Ty>
      typename std::enable_if<
        std::is_arithmetic_v<_EnableTy>,
        bool
      >::type erase_if( const_iterator_t fst, const_iterator_t lst, _CompareTy c )
      {
        do_dense();
        container_t container;
        while( fst != lst )
        {
            if( !c( fst ) ) {
                container.insert( *fst );  
            }
            ++fst;
        }
        if( !container.empty() ) {
            col_size_  = std::size( container );
            container_ = std::move( container ); 
            return true;
        }
        return false;
      }

  template<
      typename Ty,
      typename _EnableTy = _Ty>
      typename std::enable_if<
        std::is_arithmetic_v<_EnableTy>,
        vector<_EnableTy>
      >::type& merge( vector<Ty> const& other, bool is_sparse = false )
      { 
        if constexpr( !std::is_same_v<type_t, Ty> || !std::is_same_v<pointer_t, Ty> ) {
            static_assert("<sparse::vector::merge> : bad <Ty> of <other>");
        }
        const auto fst = std::begin( other.container_ );
        const auto lst = std::end( other.container_ );
        std::size_t i( col_size_ );
        col_size_ += other.col_size_;

        for( ; i < col_size_; i++ ) 
        {
            if( lst == other.container_.find( i ) ) {
                container_[i] = other.frequent_value_;
            } else {
                container_.emplace( { col_size_, other.container_[i] } );
                fst++;
            }
        }

        if( is_sparse ) {
            do_dense();
            is_sparse_ = do_sparse();
        }
        return *this;
      }

      vector& operator = ( vector const& other )
      {
        if( this == &other ) return *this;
        return _assign( other );
      }

      vector& operator = ( init_list_t list )
      { _assign( list ); }

  template<
      typename Ty,
      typename _EnableTy = _Ty,
      std::enable_if_t<
        std::is_pointer_v<_EnableTy>, bool
      > = true >
      operator vector<Ty> ()
      { return vector<Ty>().merge( *this, true ); }

      bool operator == ( vector<pointer_t> const& other )
      { return _equal( other ); }

      bool operator == ( vector<type_t> const& other )
      { return _equal( other ); }

      vector& operator += ( vector<pointer_t> const& other )
      { return _plus_equal( other ); }

      vector& operator += ( vector<type_t> const& other )
      { return _plus_equal( other ); }

      vector& operator -= ( vector<pointer_t> const& other )
      { return _minus_equal( other ); }

      vector& operator -= ( vector<type_t> const& other )
      { return _minus_equal( other );  }

      vector& operator *= ( type_t const& value )
      {
        for( auto &elem : container_ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
               *elem.second *= value;
            } else {
                elem.second *= value;
            }
        }
        *frequent_value_ *= value;
        return *this;
      }

      const_reference_t operator [] ( std::size_t const& j ) const
      { 
        if( j >= col_size_ ) {
            throw detail::exception_t( "<sparse::vector::operator[] const> : bad access" );
        }
        auto itr = container_.find( j );
        if( itr != container_.end() ) {
            if constexpr ( std::is_pointer_v<_Ty> ) {
                return *container_.at(j);
            } else {
                return  container_.at(j);
            }
        } else {
            return *frequent_value_;
        }
      }
private :
      vector& _assign( vector const& other )
      {
        is_sparse_      = other.is_sparse_;
        frequent_value_ = other.frequent_value_;
        col_size_       = other.col_size_;
        container_      = other.container_;
        return *this;
      }

      vector& _assign( init_list_t list )
      {
        container_.clear();
        auto fst = std::begin( list );
        auto lst = std::end( list );
        for( std::size_t i(0); fst != lst; fst++, i++ )
            container_[i] = *fst;
        col_size_ = std::size( container_ );
        do_sparse();
        return *this;
      }

  template<
      typename _EnableTy = _Ty,
      std::enable_if_t<
        std::is_arithmetic_v<_EnableTy>, bool
      > = true >
      void _align()
      {
        container_t container;
        for( std::size_t i(0), j(0); i < col_size_; i++ )
        {
            const auto itr = container_.find( i );
            if( itr != container_.end() ) {
                container[j++] = itr->second;
            }
        }
        if( !container.empty() ) {
            container_ = std::move( container );
        }
      }

  template<typename Ty>
      bool _equal( vector<Ty> const& other )
      {
        if( col_size_ != std::size(other) )
        {   return false;   }
        if( other.sparse() && frequent_value_ != other.frequent() )
        {   return false;   }
        for( std::size_t i(0); i < col_size_; i++ )
        {
            const auto itr = container_.find( i );
            if( itr != std::end( container_ ) ) {
                if constexpr ( std::is_pointer_v<_Ty> ) {
                    if( ( *itr->second ) != other[i] ) {
                        return false;
                    }
                } else {
                    if( itr->second != other[i] ) {
                        return false;
                    }
                }
            } else {
                if( frequent_value_ != other[i] ) {
                    return false;
                }
            }
        }
        return true;
      }

  template<typename Ty>
      vector & _plus_equal( vector<Ty> const& other )
      {
        do_dense();
        if( col_size_ != std::size(other) ) {
            throw detail::exception_t( "<sparse::vector::_plus_equal> : any size of <other.container_>" );
        }
        for( std::size_t i(0); i < col_size_; i++ )
        {
            if constexpr ( std::is_pointer_v<_Ty> ) {
             ( *container_[i] ) += other[i];
            } else {
                container_[i] += other[i];
            }
        }
        do_sparse();
        return *this;
      }

  template<typename Ty>
      vector & _minus_equal( vector<Ty> const& other )
      {
        do_dense();
        if( col_size_ != std::size(other) ) {
            throw detail::exception_t( "<sparse::vector::_minus_equal> : any size of <other.container_>" );
        }
        for( std::size_t i(0); i < col_size_; i++ )
        {
            if constexpr( std::is_pointer_v<_Ty> ) {
             ( *container_[i] ) -= other[i];
            } else {
                container_[i] -= other[i];
            }
        }
        do_sparse();
        return *this;
      }
  }; // end sparse vector container
// generate struct matrix_accessor
    MTL_DECLARE_ACCESSOR( matrix )
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
      friend class  vector<_Ty*>;

  template<typename Ty = _Ty>
      using init_list_t = std::initializer_list<Ty> const&;
public :
      using add_multiplying_by_value<matrix<_Ty>, _ElemTraitsTy>::operator*;
      using type_t           = typename _ElemTraitsTy::type_t;
      using pointer_t        = typename _ElemTraitsTy::pointer_t;
      using const_pointer_t  = const type_t*;
      using iterator_t       = typename container_t::iterator_t;
      using const_iterator_t = typename container_t::const_iterator_t;
      using ptrs_vector_t    = vector<pointer_t>;
      using vector_t         = vector<type_t>;
private :
      std::size_t         size_;
      std::size_t         col_size_;
      mutable container_t container_;
public :
      matrix() = default;

      matrix( matrix const& other )
        : container_( other.container_ )
      { }

      matrix( matrix && other ) noexcept
        : container_( std::move( other.container_ ) )
      { }

      matrix( std::size_t size, std::size_t col_size, std::size_t value = 0 )
        : mtx_mixing_list_t<matrix<_Ty>, _ElemTraitsTy>()
        , size_( size )
        , col_size_( col_size )
        , container_( size*col_size_, value )
      { }

      matrix( init_list_t<> list )
        : mtx_mixing_list_t<matrix<_Ty>, _ElemTraitsTy>()
        , size_( 1 )
        , col_size_( list.size() )
        , container_( list )
      { }

      matrix( init_list_t<vector_t> list )
        : mtx_mixing_list_t<matrix<_Ty>, _ElemTraitsTy>()
        , size_( list.size() )
        , col_size_( std::begin( list )->size() )
        , container_()
      { this->operator=( list ); }

      std::size_t size() const noexcept 
      { return size_; }

      std::size_t csize() const noexcept 
      { return col_size_; }
      /*
      void resize( std::size_t size )
      {
        if( size == size_ )
            return;
        container_.resize( size );
      }*/

      matrix & assign( matrix const& other ) 
      {
        size_      = other.size_;
        container_ = other.container_;
        return *this;
      }

      matrix & assign( init_list_t<> list ) 
      {
        if( col_size_ > list.size() || list.size()%col_size_ != 0 )
            throw detail::exception_t( "<sparse::matrix::assign(init_list_t<_Ty>)> : bad <initializer_list> size" );
        container_ = list;
        return *this;
      }

      matrix & assign( init_list_t<vector_t> list )
      {
        auto fst = std::begin( list );
        auto lst = std::end( list );
        for( ; fst != lst; fst++ )
        {
            if( fst->size() != col_size_ ) {
                throw detail::exception_t( "<sparse::matrix::assign(init_list_t<vector_t>)> : bad <size> of <vector>" );
            }
            container_.merge( *fst );
        }
        container_.do_sparse();
      }

      matrix & move( matrix const& other ) 
      {
        size_      = std::move( other.size_ );
        container_ = std::move( other.container_ );
        return *this;
      }

      void merge( matrix const& other )
      {
        for( std::size_t i(0); i < std::size( other ); i++ )
            push_back( other[i] );
      }
/*
      void push_front( vector_t vector ) 
      { insert( vector, 0 ); }*/
      
      void push_back( vector_t vector ) 
      { container_.merge( vector ); ++size_; }
/*
      void insert( vector_t vector, std::size_t i = size_ )
      {
        if( i > size_ )
            throw detail::exception_t( "<sparse::matrix::insert> : bad insert position" );
        container_.do_dense();
        container_.insert( container_.begin() + i*col_size_, vector.begin(), vector.end() );
        ++size_;
      }

      void erase( std::size_t i )
      {
        if( i >= size_ )
            throw detail::exception_t( "<sparse::matrix::erase> : bad erase position" );
        auto it( container_.begin() + i*col_size_ );
        container_.erase( it, it + col_size_ );
        --size_;
      }*/
      // overloads
      bool operator == ( matrix const& other ) 
      { return container_ == other.container_; }

      matrix & operator += ( matrix const& other )
      {
        container_ += other.container_;
        return *this;
      }

      matrix & operator -= ( matrix const& other )
      {
        container_ -= other.container_;
        return *this;
      }

      matrix & operator *= ( _Ty const& value )
      {
        container_ *= value;
        return *this;
      }

      matrix & operator *= ( matrix const& other )
      {
        matrix r( size_ );
        _multiply( r.data(), other );
        std::copy( r.begin(), r.end(), container_.begin() );
        return *this;
      }

      matrix operator * ( matrix const& other ) 
      {
        matrix r( size() );
        _multiply( r.data(), other );
        return r;
      }

      matrix & operator = ( matrix const& other )
      {
        if( this == &other ) return *this;
        return assign( other );
      }

      matrix & operator = ( matrix && other ) noexcept
      {
        if( this == &other ) return *this;
        return move( other );
      }

      matrix & operator = ( init_list_t<> list )
      { return assign( list ); }

      matrix & operator = ( init_list_t<vector_t> list )
      { return assign( list ); }

      ptrs_vector_t operator [] ( std::size_t const& i )
      {
        if( i >= size_ )
            throw detail::exception_t( "<sparse::matrix::operator[]> : <i> >= <col_size_> | <matrix> is empty" );
        return vector<_Ty*>( *this, i );
      }

      const ptrs_vector_t operator [] ( std::size_t const& i ) const
      {
        if( i >= size_ )
            throw detail::exception_t( "<sparse::matrix::operator[]const> : <i> >= <col_size_> | <matrix> is empty" );
        return vector<_Ty*>( *this, i );
      }
private :

      void _multiply( matrix & r, matrix const& lft, matrix const& rht )
      { // rewrite
        if( lft.csize() != rht.size() )
            throw detail::exception_t( "<sparse::matrix::multiply> : col1 != row2" );
        for( std::size_t i(0); i < lft.size(); i++ )
            for( std::size_t j(0); j < rht.csize(); j++ ) 
                for( std::size_t k(0); k < lft.csize(); k++ ) 
                    r[i][j] += lft[i][k]*rht[k][j];
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