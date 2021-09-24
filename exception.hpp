// @Tarnakin V.D.
// this header has an exception macro & headrs
#pragma once
#ifndef TVD_EXCEPTION_HPP
#define TVD_EXCEPTION_HPP
// use std exception
# ifndef TVD_EXCEPTION
#  include <exception>
#  include <stdexcept>
#  define TVD_EXCEPTION(message) std::runtime_error(message)
# endif
#endif
