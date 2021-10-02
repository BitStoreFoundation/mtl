// @Tarnakin V.D.
// this header has an exception macro & headrs
#pragma once
#ifndef MTL_EXCEPTION_HPP
#define MTL_EXCEPTION_HPP
// use std exception
# ifndef MTL_EXCEPTION
#  include <exception>
#  include <stdexcept>
#  define MTL_EXCEPTION(message) std::runtime_error( message )
# endif
#endif
