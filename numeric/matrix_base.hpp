#pragma once
#ifndef MTL_NUMERIC_MATRIX_BASE_HPP
#define MTL_NUMERIC_MATRIX_BASE_HPP

#include "../basic_mixing_templates.hpp"
#include "../types.hpp"
#include "../foreach.hpp"

namespace mtl::numeric {
// vector tags
    struct insert_t { };
    struct view_t { };
    struct mtx_t { };

} namespace mtl {

template<typename _DerivedTy>
    struct index<_DerivedTy, numeric::view_t>
    { 
      std::size_t operator () ( _DerivedTy *const impl, std::size_t const& j )
      { 
        auto size = impl->size();
        if( j >= size ) throw exception_t( "<view_vector::operator[]> : bad access" );
        return impl->index()*size + j; 
      }
    };
}
#endif