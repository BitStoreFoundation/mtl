// @Tarnakin V.D.
// this header has an exception macro & headrs
#pragma once
#ifndef MTL_EXCEPTION_HPP
#define MTL_EXCEPTION_HPP
// use std exception
# ifndef MTL_USE_ANY_EXCEPTION
#  include <exception>
#  include <stdexcept>
namespace mtl { namespace detail {
   using exception_t = std::runtime_error;
} }
# endif
#endif
