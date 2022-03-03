#pragma once
#ifndef MTL_TYPES_HPP
#define MTL_TYPES_HPP

#define MTL_PASTE_DETAIL(namespace) \
template<typename ... _ArgsTy> \
    using variant_t  = namespace::variant<_ArgsTy...>; \
template<typename _Ty> \
    using optional_t = namespace::optional<_Ty>;

#ifdef __BORLANDC__ 
// C++ Builder ver <= 10.4
// Incorrect working std::variant and std::optional
// Using boost::variant and boost::optional
# if __BORLANDC__ <= 0x0750 

# include <boost/variant.hpp>
# include <boost/optional.hpp>
# define MTL_NULLOPT boost::none
namespace mtl::detail {
    MTL_PASTE_DETAIL(boost)
}
// any version
# else

# include <variant>
# include <optional>
# define MTL_NULLOPT std::nullopt
namespace mtl::detail {
    MTL_PASTE_DETAIL(std)
}
# endif
// any ide
#else
#define MTL_NULLOPT std::nullopt
// paste types
namespace mtl::detail {
    MTL_PASTE_DETAIL(std)
}
#endif
// delete macro
#undef MTL_PASTE_DETAIL
#endif