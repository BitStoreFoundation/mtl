#pragma once
#ifndef CONDITION_HPP
#define CONDITION_HPP

#include <cstddef>

namespace tvd {

template<
    typename _LTy,
    typename _RTy>
    bool less(_LTy const& l, _RTy const& r) noexcept 
    {
    return (l < r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool greater(_LTy const& l, _RTy const& r) noexcept 
    {
    return (l > r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool less_or_equals(_LTy const& l, _RTy const& r) noexcept 
    {
    return (l <= r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool greater_or_equals(_LTy const& l, _RTy const& r) noexcept 
    {
    return (l >= r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool equals(_LTy const& l, _RTy const& r) noexcept 
    {
    return (l == r);
    }

template<
    typename _LTy,
    typename _RTy>
    bool non_equals(_LTy const& l, _RTy const& r) noexcept 
    {
    return (l != r);
    }

template<class _ContainerTy>
    bool out_of_range(_ContainerTy const& c, size_t const& i) noexcept 
    {
	return c.size() <= i;
    }

template<class _MatrixTy>
    bool out_of_range(_MatrixTy const& m, size_t const& i, size_t const& j) noexcept 
    {
	return ( m.size() <= i || m.csize() <= j );
    }
}
#endif