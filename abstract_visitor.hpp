#pragma once
#ifndef ABSTRACT_VISITOR_HPP
#define ABSTRACT_VISITOR_HPP

#include <variant>

namespace tvd {

template<class... _ArgsTy> 
    struct abstract_visitor : _ArgsTy ... { using _ArgsTy::operator() ...; };

template<class... _ArgsTy> 
    abstract_visitor( _ArgsTy ... ) -> abstract_visitor<_ArgsTy ... >;
}
#endif