#pragma once
#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP
// c++17 @Tarnakin V.D.
#include <type_traits>
#include <string>
#include <vector>
namespace tvd {
// метафункция для проверки типов строк
template<class _UnusedTy = void>
	struct is_string : std::false_type { };
// специализация шаблона для стандартных строк
template<>
	struct is_string<std::string> : std::true_type { };

template<
	typename _Ty,
	typename _PtrTy = _Ty*,
	typename _RefTy = _Ty&>
	struct elem_traits
	{
	typedef _Ty    type_t;
	typedef _PtrTy pointer_t;
	typedef _RefTy reference_t;
	};
// тип контейнера
template<class _ElemTraitsTy>
	struct elem_container
    {
	typedef _ElemTraitsTy                               elem_traits_t;
	typedef std::vector<typename elem_traits_t::type_t> container_t;
	};
// шаблонные переменные
template<typename>
	inline constexpr bool false_v = std::false_type::value;

template<typename>
	inline constexpr bool true_v = std::true_type::value;

template<class _Ty>
	inline constexpr bool is_string_v = is_string<_Ty>::value;
// шаблонные псевдонимы для SFINAE
template<typename _Ty>
    using is_arithmetic_t = std::enable_if_t<std::is_arithmetic_v<_Ty>, bool>;

template<typename _Ty>
    using is_pointer_t = std::enable_if_t<std::is_pointer_v<_Ty>, bool>;

template<typename _Ty>
    using is_type_t = std::enable_if_t<!std::is_pointer_v<_Ty>, bool>;

template<typename _Ty1, typename _Ty2>
    using is_same_t = std::enable_if_t<std::is_same_v<_Ty1, _Ty2>, bool>;

template<typename _Ty>
	using is_string_t = std::enable_if_t<is_string_v<_Ty>, bool>;

template<typename _FstTy,
	typename _SecTy>
	using is_base_of_t = std::enable_if_t<std::is_base_of_v<_FstTy, _SecTy>, bool>;
// шаблонные псевдонимы - обертки
template<typename _Ty>
	using ref_t = _Ty&;

template<typename _Ty>
    using const_ref_t = _Ty const&;

template<typename _Ty>
    using rv_ref_t = _Ty&&;

template<typename _Ty>
	using ptr_t = _Ty*;
}
#endif
