// @Tarnakin V.D.
// this header has an exception macro & headrs
#pragma once
#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
// use std exception
#ifndef EXCEPTION

#include <exception>
#include <stdexcept>
#define EXCEPTION(message) std::runtime_error(message)

#endif

#endif
