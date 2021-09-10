#pragma once
#ifndef ABSTRACT_FACTORY_HPP
#define ABSTRACT_FACTORY_HPP

#include <unordered_map>
#include <functional>
#include <optional>
#include <boost/variant.hpp>

namespace tvd {

template<
    typename _KeyTy,
    typename ... _ArgsTy>
    class abstract_factory
    {
public :
    using key_t       = _KeyTy;
    using var_t       = boost::variant<std::shared_ptr<_ArgsTy> ... >;
    using o_var_t     = std::optional<var_t>;
    using creator_t   = std::function<var_t()>;
    using hash_tbl_t  = std::unordered_map<key_t, creator_t>;
    using init_list_t = std::initializer_list< std::pair<key_t, creator_t> > const&;
private :
    hash_tbl_t hash_tbl_;
public :
    abstract_factory() = default;
    abstract_factory(abstract_factory const&) = default;
    abstract_factory& operator = (abstract_factory const&) = default;

    abstract_factory( init_list_t list ) 
        {
        for(auto const& pair : list)
            hash_tbl_.insert( pair );
        }

    void registerClass( key_t const& key, creator_t const& create ) 
        {
        if( !create ) {
            throw EXCEPTION("<abstract_factory::registerClass> : <create> is empty wrap of functional object");
            }
        auto it = hash_tbl_.find( key );
        if( it != hash_tbl_.end() ) {
            throw EXCEPTION("<abstract_factory::registerClass> : <key> already exsist");
            }
        hash_tbl_.emplace( key, create );
        }

template<class _ObjTy>
    void registerClass( key_t const& key ) 
        {
        auto it = hash_tbl_.find( key );
        if( it != hash_tbl_.end() ) {
            throw EXCEPTION("<abstract_factory::registerClass> : <key> already exsist");
            }
        hash_tbl_.emplace( key, &create<_ObjTy> );
        }

    o_var_t operator () ( key_t const& key ) 
        {
        auto it = hash_tbl_.find( key );
        return it != hash_tbl_.end() ? it->second() : std::nullopt;
        }
private :

template<class _ObjTy>
    static var_t create() { return std::make_shared<_ObjTy>(); } 
    };
}
#endif
