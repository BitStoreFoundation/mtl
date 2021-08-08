// c++17 @Tarnakin V.D.
#pragma once
#ifndef BASE_MIXING_TEMPLATES_HPP
#define BASE_MIXING_TEMPLATES_HPP

#include "exception.hpp"

namespace tvd {

template<class ... _MixingTy>
    struct add_mixing : public _MixingTy ... { };

template<class _DerivedTy>
    struct access { };
// mixing for container class
template<
    class _DerivedTy,
    class _ElemContainerTy>
    class add_iterators
    {
public :
    using derived_t  = _DerivedTy;
    using iterator_t = typename _ElemContainerTy::container_t::iterator;
private :
    derived_t *derived_;
public :
    add_base_methods()
        : derived_(static_cast<derived_t*>(this))
        {
        }
    
    iterator_t begin() {
        return iterator_t( derived_->data() );
        }

    iterator_t end() {
        return iterator_t( derived_->data() + access::size<_DerivedTy>(derived_) );
        }
    };

template<
    class _DerivedTy,
    class _ElemContainerTy>
    class add_const_iterators
    {
public :
    using derived_t        = _DerivedTy;
    using const_iterator_t = typename _ElemContainerTy::container_t::const_iterator;
private :
    derived_t *derived_;
public :
    add_base_methods()
        : derived_(static_cast<derived_t*>(this))
        {
        }
    
    const_iterator_t begin() const {
        return const_iterator_t( derived_->data() );
        }

    const_iterator_t end() const {
        return const_iterator_t( derived_->data() + access<_DerivedTy>::size(derived_) );
        }

    const_iterator_t cbegin() const {
        return begin();
        }

    const_iterator_t cend() const {
        return end();
        }
    };

template<
    class _DerivedTy,
    class _AnyTy = _DerivedTy>
    struct add_non_equalable
    {
    virtual bool operator == (_AnyTy const&) = 0;

    bool operator != (_AnyTy const& other) {
        return !(*this == other);
        }
    };

template<
    class _DerivedTy,
    class _AnyTy = _DerivedTy>
    struct add_sum
    {
    virtual _DerivedTy& operator += (_AnyTy const&) = 0;

    _DerivedTy operator + (_AnyTy const & other) const {
        auto cpy = static_cast<const _DerivedTy&>(*this);
        return cpy += other;
        }
    };

template<
    class _DerivedTy,
    class _AnyTy = _DerivedTy>
    struct add_difference
    {
    virtual _DerivedTy& operator -= (_AnyTy const&) = 0;

    _DerivedTy operator - (_AnyTy const & other) const {
        auto cpy = static_cast<const _DerivedTy&>(*this);
        return cpy -= other;
        }
    };

template<
    class _DerivedTy,
    class _ElemTraitsTy,
    class _ReturnTy = _DerivedTy>
    struct add_multiplying_by_value
    {
    virtual _DerivedTy& operator *= (typename _ElemTraitsTy::type_t const&) = 0;

    _ReturnTy operator * (typename _ElemTraitsTy::type_t const & value) {
        _ReturnTy cpy = static_cast<_DerivedTy&>(*this);
        return cpy *= value;
        }
    };

template<
    class _DerivedTy,
    class _ElemTraitsTy,
    class _ReturnTy = _DerivedTy>
    struct add_division_by_value
        : public add_multiplying_by_value<_DerivedTy, _ElemTraitsTy, _ReturnTy>
    {

    _DerivedTy& operator /= (typename _ElemTraitsTy::type_t const & value) {
        if(value <= 0) {
            throw EXCEPTION("bad operation : value = 0");
            }
        return *this *= (1.0/value);
        }

    _ReturnTy operator / (typename _ElemTraitsTy::type_t const & value) const {
        if(value <= 0) {
            throw EXCEPTION("bad operation : value = 0");
            }
        _ReturnTy cpy = static_cast<const _DerivedTy&>(*this);
        return cpy *= (1.0/value);
        }
    };
}
#endif
