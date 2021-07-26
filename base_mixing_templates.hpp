// c++17 @Tarnakin V.D.
#pragma once
#ifndef BASE_MIXING_TEMPLATES_HPP
#define BASE_MIXING_TEMPLATES_HPP

#include "exception.hpp"

namespace tvd {

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
