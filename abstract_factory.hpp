#pragma once
#ifndef TVD_ABSTRACT_FACTORY_HPP
#define TVD_ABSTRACT_FACTORY_HPP

#include <unordered_map>
#include <functional>

#ifdef CXX_BUILDER_CXX17
# include <boost/variant.hpp>
# include <boost/optional.hpp>
# define TVD_NULLOPT boost::none
#else
# include <variant>
# include <optional>
# define TVD_NULLOPT std::nullopt
#endif

namespace tvd {

template<
    typename _KeyTy,
    typename ... _ArgsTy>
    class abstract_factory
    {
public :
    using key_t       = _KeyTy;
#ifdef CXX_BUILDER_CXX17
    using var_t       = boost::variant<std::shared_ptr<_ArgsTy> ... >;
    using o_var_t     = boost::optional<var_t>;
#else
    using var_t       = std::variant<std::shared_ptr<_ArgsTy> ... >;
    using o_var_t     = std::optional<var_t>;
#endif
    using creator_t   = std::function<var_t()>;
    using creators_t  = std::unordered_map<key_t, creator_t>;
    using init_list_t = std::initializer_list< std::pair<key_t, creator_t> > const&;
private :
    creators_t creators_;
public :
    abstract_factory() = default;
    abstract_factory(abstract_factory const&) = default;
    abstract_factory& operator = (abstract_factory const&) = default;

    abstract_factory( init_list_t list ) 
      {
        for(auto const& pair : list) {
            creators_.emplace( pair );
        }
      }

    void register_class( key_t const& by_key, creator_t const& creator ) 
      {
        if( !creator ) {
            throw EXCEPTION( "<abstract_factory::register_class> : <creator> is empty wrap of functional object" );
        }
        if( creators_.find( by_key ) != creators_.end() ) {
            throw EXCEPTION( "<abstract_factory::register_class> : <by_key> already exsist" );
        }
        creators_[by_key] = creator;
      }

template<class _ObjTy>
    void register_class( key_t const& by_key ) 
      {
        if( creators_.find( by_key ) != creators_.end() ) {
            throw EXCEPTION( "<abstract_factory::register_class> : <by_key> already exsist" );
        }
        creators_[by_key] = &creator<_ObjTy>;
      }

    o_var_t creat( key_t const& by_key ) 
      {
        auto it = creators_.find( by_key );
        return it != creators_.end() ? it->second() : TVD_NULLOPT;
      }
private :

template<class _ObjTy>
    static var_t creator() { 
        return std::make_shared<_ObjTy>(); 
      } 
    };
}
#undef TVD_NULLOPT
#endif
