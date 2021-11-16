// c++17 @Tarnakin V.D.
//this header has a description of the sparse matrix container
#pragma once
#ifndef MTL_NUMERIC_SPARSE_MATRIX_HPP
#define MTL_NUMERIC_SPARSE_MATRIX_HPP
// mtl
#include "matrix_base.hpp"
// stl
#include <unordered_map>

namespace mtl::numeric {
//
template<
    typename _Ty = std::ptrdiff_t, 
    typename     = insert_t,
    typename     = elem_traits<_Ty> >
    class sparse_vector;
//
template<typename = std::ptrdiff_t>
    class sparse_matrix;

template<typename _Ty = std::ptrdiff_t>
    using matrix_t = sparse_matrix<_Ty>;

template<
    typename _Ty  = std::ptrdiff_t,
    typename tag_ = insert_t>
    using vector_t = sparse_vector<_Ty, tag_>;
// deduction guide
template<typename _Ty>
    sparse_vector(std::initializer_list<_Ty> const&) -> sparse_vector<_Ty>;
template<typename _Ty>
    sparse_matrix(std::initializer_list<_Ty> const&) -> sparse_matrix<_Ty>;
template<typename _Ty>
    sparse_matrix(std::initializer_list< vector_t<_Ty> > const&) -> sparse_matrix<_Ty>;

} namespace mtl {
//
template<typename _Ty>
    struct access_traits< numeric::vector_t<_Ty> >
    { using container_t = std::unordered_map<std::size_t, _Ty>; };
//
template<typename _Ty>
    struct access_traits< numeric::vector_t<_Ty, numeric::view_t> > 
    { using container_t = numeric::vector_t<_Ty>; };
//
template<typename _Ty>
    struct access_traits< numeric::matrix_t<_Ty> >
    { using container_t = numeric::vector_t<_Ty>; };
//
template<
    typename _Ty,
    typename tag_>
    _Ty frequent( numeric::vector_t<_Ty, tag_> const& v ) 
    {
      std::unordered_map<_Ty, std::size_t> map;
      for( std::size_t i(0); i < std::size( v ); i++ )
      {   
          auto elem = v[i];
          if( map.find( elem ) == map.end() ) {
              map[elem] = 1;
          } else {
              map[elem]++;
          }
      }
      return std::max_element( std::begin( map ), std::end( map ), 
                               [](const auto& lhs, const auto& rhs) 
                               { return lhs.second < rhs.second; }  )->first;
    }
//
} namespace mtl::numeric {

template<
    typename _DerivedTy, 
    typename _Ty,
    typename tag_,
    typename _AddMixingListTy,
    typename _TraitsTy = mixing_traits<_DerivedTy>,
    typename _IndexTy  = index<_DerivedTy, tag_> >
    struct add_sparse_operations_impl : public _AddMixingListTy
    {
      using accessor_t = typename _TraitsTy::accessor_t;
      // declare derived_ and add_dense_operations_impl() = default
      MTL_DECL_PRIV_PTR_TO_THE_DERIVED_CLASS_AND_PUB_DEF_CONSTR( add_sparse_operations_impl, _DerivedTy )

      _DerivedTy& operator *= ( _Ty const& value )
      { 
        derived_->do_dense();
        auto &container = accessor_t::container( derived_ );
        _IndexTy index;
        MTL_FOREACH( i, derived_->size() )
            container.find( index( derived_, i ) )->second *= value;
        return derived_->do_sparse(); 
      }

      _DerivedTy& operator /= ( _Ty const& value )
      { 
        if( value == 0 )
            throw exception_t( "<sparse_vector/=> : value is null" );
        return *this *= (1.0/value); 
      }

      _DerivedTy& operator += ( typename _TraitsTy::arg_t const& other )
      { 
        if( derived_->size() != std::size( other ) )
            throw exception_t( "<sparse_vector+=> : bad size" );
        derived_->do_dense();
        auto &container = accessor_t::container( derived_ );
        _IndexTy index;
        MTL_FOREACH( i, derived_->size() )
            container.find( index( derived_, i ) )->second += other[i];
        return derived_->do_sparse(); 
      }

      _DerivedTy& operator -= ( typename _TraitsTy::arg_t const& other )
      { 
        if( derived_->size() != std::size( other ) )
            throw exception_t( "<sparse_vector+=> : bad size" );
        derived_->do_dense();
        auto &container = accessor_t::container( derived_ );
        _IndexTy index;
        MTL_FOREACH( i, derived_->size() )
            container.find( index( derived_, i ) )->second -= other[i];
        return derived_->do_sparse(); 
      }
    };

template<
    typename _DerivedTy,
    typename _Ty,
    typename _OtherTy = _DerivedTy,
    typename _Traits1 = mixing_traits<_DerivedTy, _OtherTy>,
    typename _Traits2 = mixing_traits<_DerivedTy, _Ty, _OtherTy> >
    using add_default_sparse_mixing_t = mixing_list
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
    using add_sparse_insert_vec_mixing_t = mixing_list<
      add_iterators<_DerivedTy, only_const_t>,
      add_sparse_operations_impl<
        _DerivedTy,
        _Ty,
        default_t,
        add_default_sparse_mixing_t<
          _DerivedTy, 
          _Ty
        >
      >
    >;
// sparse vector container
template<
    typename _Ty,
    typename _TraitsTy>
    class sparse_vector<_Ty, insert_t, _TraitsTy> final 
      : public add_sparse_insert_vec_mixing_t<vector_t<_Ty>, _Ty>
    { 
      static_assert(
        !std::is_pointer_v<_Ty>,
        "< mtl::numeric::sparse_vector> : no specialization of class for pointer"
      );

      friend struct default_accessor< vector_t<_Ty> >;
public :
      using type_t            = _Ty;
      using opt_type_t        = detail::optional_t<type_t>;
      using const_reference_t = typename _TraitsTy::const_reference_t;
      using init_list_t       = std::initializer_list<_Ty> const&;
      using insert_vector_t   = vector_t<type_t>;
      using container_t       = typename access_traits<insert_vector_t>::container_t;
      using iterator_t        = typename container_t::iterator;
      using const_iterator_t  = typename container_t::const_iterator;
private :
      bool        is_sparse_;
      opt_type_t  frequent_value_;
      std::size_t size_;
      container_t container_;
public :
      sparse_vector() = default;

      explicit sparse_vector( std::size_t size, type_t value = 0 )
        : is_sparse_( false )
        , frequent_value_()
        , size_( size )
        , container_( size_ )
      {
        MTL_FOREACH( i, size )
            container_[i] = value;
        do_sparse(); 
      }

      sparse_vector( insert_vector_t const& other )
      { __assign( other ); }

      sparse_vector( init_list_t list )
        : sparse_vector()
      { __assign( list ); }

      std::size_t size() const noexcept
      { return size_; }

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

      insert_vector_t& do_sparse()
      {
        if( is_sparse_ ) return *this;
        do_dense();
        auto fst = std::begin( container_ );
        auto lst = std::end( container_ );
        frequent_value_ = mtl::frequent( *this );
        auto sz = size_;
        is_sparse_ = erase_if( fst, lst, [this]( auto const& itr ) { return itr->second == this->frequent_value_; } );
        size_ = sz;
        return *this;
      }

      insert_vector_t& do_dense() 
      {
        if( !is_sparse_ ) return *this; 
        MTL_FOREACH( i, size_ )
            if( container_.find( i ) == container_.end() )
                container_[i] = *frequent_value_;
        frequent_value_ = MTL_NULLOPT;
        is_sparse_ = false;
        return *this;
      }

      void push_back( type_t value ) 
      { 
        container_[size_] = value;
        size_++;
        do_dense();
      }

      void erase( std::size_t i )
      {
        do_dense();
        container_.erase( i );
        __align(); 
        --size_;
      }

      bool erase( const_iterator_t fst, const_iterator_t lst )
      { return erase_if( fst, lst, []( auto const& itr ){ return true; } ); }

  template<typename _CompareTy>
      bool erase_if( const_iterator_t fst, const_iterator_t lst, _CompareTy c )
      {
        do_dense();
        container_t container;
        for( auto itr( container_.begin() ); itr != fst; itr++ )
            container.insert( *itr );
        for( ; fst != lst; fst++ )
            if( !c( fst ) )
                container.insert( *fst );  
        if( !container.empty() ) {
            size_      = std::size( container );
            container_ = std::move( container ); 
            return true;
        }
        return false;
      }

  template<typename tag>
      insert_vector_t& merge( vector_t<type_t, tag> const& other )
      {
        do_dense();
        auto i( size_ );
        size_ += std::size( other );
        for( std::size_t j(0); i < size_; i++, j++ ) 
            container_[i] = other[j];
        do_sparse();
        return *this;
      }

      insert_vector_t& operator = ( insert_vector_t const& other )
      {
        if( this == &other ) return *this;
        return __assign( other );
      }

      insert_vector_t& operator = ( init_list_t list )
      { __assign( list ); }

      bool operator == ( insert_vector_t const& other )
      { 
        if( size_ != std::size( other ) )
        {   return false;   }
        if( other.sparse() && frequent_value_ != other.frequent() )
        {   return false;   }
        MTL_FOREACH( i, size_ )
        {
            const auto itr = container_.find( i );
            if( itr != std::end( container_ ) ) {
                if( itr->second != other[i] ) {
                    return false;
                }
            } else {
                if( frequent_value_ != other[i] ) {
                    return false;
                }
            }
        }
        return true;
      }

      const_reference_t at( std::size_t const& j ) const
      {
        if( j >= size_ )
            throw exception_t( "<sparse_insert_vector[] const> : bad access" );
        auto itr = container_.find( j ); 
        return itr != container_.end() ? itr->second : *frequent_value_;
      }

      const_reference_t operator [] ( std::size_t const& j ) const
      { return at( j ); }
private :
      insert_vector_t& __assign( insert_vector_t const& other )
      {
        is_sparse_      = other.is_sparse_;
        frequent_value_ = other.frequent_value_;
        size_           = other.size_;
        container_      = other.container_;
        return *this;
      }

      insert_vector_t& __assign( init_list_t list )
      {
        container_.clear();
        auto fst = std::begin( list );
        auto lst = std::end( list );
        for( std::size_t i(0); fst != lst; fst++, i++ )
            container_[i] = *fst;
        size_ = std::size( container_ );
        do_sparse();
        return *this;
      }

      void __align()
      {
        container_t container;
        for( std::size_t i(0), j(0); i < size_; i++ )
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
    }; // end sparse vector container
// base class for matrix and vector<> 
template<
    typename _DerivedTy,
    typename _Ty,
    typename _TraitsTy = mixing_traits<_DerivedTy> >
    struct add_sparse_container_impl
    {
      using accessor_t = typename _TraitsTy::accessor_t;
      // declare derived_ and base_container() = default
      MTL_DECL_PRIV_PTR_TO_THE_DERIVED_CLASS_AND_PUB_DEF_CONSTR( add_sparse_container_impl, _DerivedTy )

      bool sparse() const noexcept
      { return accessor_t::container( derived_ ).sparse(); }

      auto frequent() const noexcept
        -> decltype( accessor_t::container( derived_ ).frequent() )
      { return accessor_t::container( derived_ ).frequent(); }
    };

template<
    typename _DerivedTy,
    typename _Ty,
    typename _InsertVecTy = vector_t<_Ty>,
    typename _Traits      = mixing_traits<_DerivedTy, _InsertVecTy> >
    using add_sparse_view_vec_mixing_t = mixing_list<
      add_access_by_key<
        _DerivedTy, only_const_t, default_t, 
        mixing_traits<_DerivedTy>,
        index<_DerivedTy, view_t> 
      >,
      add_sparse_operations_impl<
        _DerivedTy,
        _Ty,
        view_t,
        add_default_sparse_mixing_t<
          _DerivedTy, 
          _Ty,
          _InsertVecTy
        >,
        _Traits
      >,
      add_sparse_container_impl<
        _DerivedTy, _Ty, _Traits
      >
    >;
// matrix vector
template<
    typename _Ty,
    typename _ElemTraitsTy>
    class sparse_vector<_Ty, view_t, _ElemTraitsTy> final 
      : public add_sparse_view_vec_mixing_t<vector_t<_Ty, view_t>, _Ty>
    {
      friend struct default_accessor< vector_t<_Ty, view_t> >;
      friend struct sparse_matrix<_Ty>;

      using type_t          = _Ty;
      using view_vector_t   = vector_t<type_t, view_t>;
      using insert_vector_t = vector_t<type_t>;
      using container_t     = typename access_traits<view_vector_t>::container_t;
      
      container_t &container_; 
      const std::size_t i_;
      const std::size_t size_;
public :
      sparse_vector() = delete;
      sparse_vector(view_vector_t const&) = delete;
      view_vector_t& operator = (view_vector_t const&) = delete;

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

      std::size_t size() const noexcept
      { return size_; }

      std::size_t index() const
      { return i_; }

      view_vector_t& do_dense()
      { 
        container_.do_sparse();
        return *this; 
      }

      view_vector_t& do_sparse()
      { 
        container_.do_sparse();
        return *this;
      }

      operator vector_t<type_t> () const
      { return vector_t<type_t>().merge( *this ); }

      bool operator == ( insert_vector_t const& other )
      {
        MTL_FOREACH( j, std::size( container_ ) )
            if( container_[i_*size_ + j] != other[j] )
                return false;
        return true; 
      }
private :
      sparse_vector( matrix_t<type_t> const& matrix, std::size_t const& size, std::size_t const& i )
        : container_( matrix.container_ )
        , size_( size )
        , i_( i )
      {
        if( i_ >= std::size( matrix ) ) {
            throw exception_t( "<sparse_vector aka view_vector::view_vector> : <i_> >= size(<matrix>) )" );
        }
      }
    };

template<
    typename _DerivedTy,
    typename _Ty,
    typename _ViewVecTy = vector_t<_Ty, view_t> >
    using add_sparse_mtx_mixing_t = mixing_list<
      add_iterators<_DerivedTy, only_const_t>,
      add_sparse_container_impl<
        _DerivedTy, _Ty
      >,
      add_default_sparse_mixing_t<
        _DerivedTy, _Ty
      >
    >;
// matrix container
template<typename _Ty>
    class sparse_matrix final : public add_sparse_mtx_mixing_t<matrix_t<_Ty>, _Ty>
    {
      static_assert(
        !std::is_pointer_v<_Ty>,
        "<mtl::numeric::sparse_matrix> : no specialization of class for pointer"
      );

      friend struct default_accessor< matrix_t<_Ty> >;
      friend class  sparse_vector<_Ty, view_t>;

  template<typename Ty>
      using init_list_t = std::initializer_list<Ty> const&;
public :
      using type_t           = _Ty;
      using matrix_t         = matrix_t<type_t>;
      using container_t      = typename access_traits<matrix_t>::container_t;
      using iterator_t       = typename container_t::iterator_t;
      using const_iterator_t = typename container_t::const_iterator_t;
      using insert_vector_t  = vector_t<type_t>;
      using view_vector_t    = vector_t<type_t, view_t>;
private :
      std::size_t         col_size_;
      mutable container_t container_;
public :
      sparse_matrix() = default;

      sparse_matrix( matrix_t const& other )
      { __assign( other ); }

      explicit sparse_matrix( std::size_t const& size, std::size_t col_size = 3, std::size_t value = 0 )
        : col_size_( col_size )
        , container_( size*col_size_, value )
      { }

      sparse_matrix( init_list_t<type_t> list )
        : col_size_( std::size( list ) )
        , container_( list )
      { }

      sparse_matrix( init_list_t<insert_vector_t> list )
        : col_size_( std::begin( list )->size() )
        , container_()
      { __assign( list ); }

      std::size_t size() const noexcept 
      { return container_.size()/col_size_; }

      std::size_t csize() const noexcept 
      { return col_size_; }

      std::size_t amount() const noexcept 
      { return container_.amount(); }

      matrix_t& do_sparse()
      { 
        container_.do_sparse(); 
        return *this; 
      }

      matrix_t& do_dense()
      { 
        container_.do_dense(); 
        return *this; 
      }

      bool resize( std::size_t const& col_size )
      {
        if( col_size == col_size_ || 
            std::size( container_ )%col_size != 0 )
        { return false; }
        col_size_ = col_size;
        return true;
      }

      void merge( matrix_t const& other )
      {
        for( std::size_t i(0); i < std::size( other ); i++ )
            container_.merge( other[i] );
      }
      
      void push_back( insert_vector_t const& vector ) 
      { container_.merge( vector ); }

      void erase( std::size_t i )
      {
        if( i >= size() )
            throw exception_t( "<sparse_matrix::erase> : bad erase position" );
        auto itr( container_.begin() + i*col_size_ );
        container_.erase( itr, itr + col_size_ );
      }

      bool operator == ( matrix_t const& other )
      { return container_ == other.container_; }

      matrix_t& operator *= ( _Ty const& value )
      { 
        container_ *= value;
        return *this;
      }

      matrix_t& operator /= ( _Ty const& value )
      { 
        container_ /= value;
        return *this;
      }

      matrix_t& operator += ( matrix_t const& other )
      { 
        container_ += other.container_; 
        return *this;
      }

      matrix_t& operator -= ( matrix_t const& other )
      { 
        container_ -= other.container_; 
        return *this;
      }

      matrix_t& operator *= ( matrix_t const& other )
      { return __assign( __multiply( other ) ); }

      matrix_t operator * ( matrix_t const& other ) 
      { return __multiply( other ); }

      matrix_t& operator = ( matrix_t const& other )
      {
        if( this == &other ) return *this;
        return __assign( other );
      }

      matrix_t& operator = ( init_list_t<type_t> list )
      { return __assign( list ); }

      matrix_t& operator = ( init_list_t<insert_vector_t> list )
      { return __assign( list ); }

      view_vector_t operator [] ( std::size_t const& i )
      { 
        if( i >= size() ) throw exception_t( "<sparse_matrix[]> : bad access" );
        return view_vector_t( *this, col_size_, i ); 
      };

      const view_vector_t operator [] ( std::size_t const& i ) const
      { 
        if( i >= size() ) throw exception_t( "<sparse_matrix[]const> : bad access" );
        return view_vector_t( *this, col_size_, i ); 
      };
private :
      matrix_t __multiply( matrix_t const& rht )
      {
        if( col_size_ != std::size( rht ) )
            throw exception_t( "<sparse_matrix::__multiply> : col1 != row2" );
        auto &lft = *this;
        auto size = this->size();
        auto col_size = rht.csize();
        mtl::numeric::matrix_t<type_t> res_m( size, col_size );
        res_m.do_dense();
        MTL_FOREACH( i, size )
        {
            insert_vector_t res_v( res_m[i] );
            const insert_vector_t lft_v( lft[i] );
            MTL_FOREACH( j, col_size )
            {
                auto &val = res_v.find( j )->second;
                MTL_FOREACH( k, col_size_ )
                    val += lft_v[k]*rht[k][j];
            }
        }
        return res_m.do_sparse();
      }

      matrix_t& __assign( matrix_t const& other ) 
      {
        col_size_  = other.col_size_;
        container_ = other.container_;
        return *this;
      }

      matrix_t& __assign( init_list_t<type_t> list ) 
      {
        if( col_size_ > list.size() || list.size()%col_size_ != 0 )
            throw exception_t( "<sparse_matrix::assign(init_list_t<_Ty>)> : bad <initializer_list> size" );
        container_ = list;
        return *this;
      }

      matrix_t& __assign( init_list_t<insert_vector_t> list )
      {
        auto fst = std::begin( list );
        auto lst = std::end( list );
        for( ; fst != lst; fst++ )
        {
            if( fst->size() != col_size_ ) {
                throw exception_t( "<sparse_matrix::assign(init_list_t<vector_t>)> : bad <size> of <vector>" );
            }
            container_.merge( *fst );
        }
        return do_sparse();
      }
    }; // end matrix container
} // mtl::numeric
#endif