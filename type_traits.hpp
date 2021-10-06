// c++17 @Tarnakin V.D.
#pragma once
#ifndef MTL_TYPE_TRAITS_HPP
#define MTL_TYPE_TRAITS_HPP

#include <type_traits>

namespace mtl {

template<typename = void>
    struct is_string : std::false_type { };

template<>
    struct is_string<const char*> : std::true_type { };

template<size_t size>
    struct is_null_size : std::false_type { };

template<>
    struct is_null_size<0> : std::true_type { };

template<
    typename, 
    typename = void>
    struct is_method_exists: std::false_type { };
// _Ty::data() 
template<typename _Ty>
    using method_data_t = std::enable_if_t<
      std::is_same_v< decltype( std::declval<_Ty>().data() ), void > 
    >;
// is _Ty::data() exists 
template<typename _Ty>
    struct is_method_exists< _Ty, method_data_t<_Ty> > : std::true_type { };

template<
    typename _Ty,
    typename _MethodTy>
    constexpr bool is_method_exists_v = is_method_exists<_Ty, _MethodTy>::value;

template<
    typename _Ty,
    typename _PtrTy = _Ty*,
    typename _RefTy = _Ty&>
    struct elem_traits
    {
      using type_t      = _Ty;
      using pointer_t   = _PtrTy;
      using reference_t = _RefTy;
    };

template<typename>
    inline constexpr bool false_v = std::false_type::value;

template<typename>
    inline constexpr bool true_v = std::true_type::value;

template<class _Ty>
    inline constexpr bool is_string_v = is_string<_Ty>::value;

template<size_t size>
    inline constexpr bool is_null_size_v = is_null_size<size>::value;

template<
    typename _Ty,
    typename _MethodTy>>
    using is_method_exists_t = std::enable_if_t<is_method_exists_v<_Ty, _MethodTy>, bool>;

template<typename _Ty>
    using is_arithmetic_t = std::enable_if_t<std::is_arithmetic_v<_Ty>, bool>;

template<typename _Ty>
    using is_pointer_t = std::enable_if_t<std::is_pointer_v<_Ty>, bool>;

template<typename _Ty>
    using is_type_t = std::enable_if_t<!std::is_pointer_v<_Ty>, bool>;

template<
    typename _FstTy,
    typename _SecTy>
    using is_same_t = std::enable_if_t<std::is_same_v<_FstTy, _SecTy>, bool>;

template<typename _Ty>
    using is_string_t = std::enable_if_t<is_string_v<_Ty>, bool>;

template<
    typename _FstTy,
    typename _SecTy>
    using is_base_of_t = std::enable_if_t<std::is_base_of_v<_FstTy, _SecTy>, bool>;
} // mtl
#endif
