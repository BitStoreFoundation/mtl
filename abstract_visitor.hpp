#pragma once
#ifndef MTL_ABSTRACT_VISITOR_HPP
#define MTL_ABSTRACT_VISITOR_HPP

namespace mtl {

template<class... _ArgsTy> 
    struct abstract_visitor : _ArgsTy ... { using _ArgsTy::operator() ...; };

template<class... _ArgsTy> 
    abstract_visitor( _ArgsTy ... ) -> abstract_visitor<_ArgsTy ... >;
}
#endif