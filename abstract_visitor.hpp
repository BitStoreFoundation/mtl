#pragma once
#ifndef TVD_ABSTRACT_VISITOR_HPP
#define TVD_ABSTRACT_VISITOR_HPP

namespace tvd {

template<class... _ArgsTy> 
    struct abstract_visitor : _ArgsTy ... { using _ArgsTy::operator() ...; };

template<class... _ArgsTy> 
    abstract_visitor( _ArgsTy ... ) -> abstract_visitor<_ArgsTy ... >;
}
#endif