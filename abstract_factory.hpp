#pragma once
#ifndef ABSTRACT_FACTORY_HPP
#define ABSTRACT_FACTORY_HPP

#include <unordered_map>
#include <functional>

namespace tvd {

template<
	class _IdTy,
	class _UnusedTy>
	class abstract_factory { };
/*
	*–егиструет классы и
	*производит экземпл€ры по id
*/
template<
	class _IdTy,
	class _BaseTy,
	typename ... _ArgsTy>
	class abstract_factory<_IdTy, _BaseTy(_ArgsTy ...)>
	{
public :
	using id_t        = _IdTy;
	using base_ptr_t  = _BaseTy*;
	using creator_t   = std::function<base_ptr_t(_ArgsTy ...)>;
	using init_list_t = std::initializer_list<std::pair<id_t, creator_t> > const&;
private :
	using hash_tbl_t = std::unordered_map<id_t, creator_t>;
	hash_tbl_t hash_tbl_;
public :
	abstract_factory() = default;
    abstract_factory(abstract_factory const&) = default;
	abstract_factory& operator = (abstract_factory const&) = default;
	
	abstract_factory(init_list_t list) {
		for(auto const & pair : list)
			hash_tbl_.insert(pair);
		}

	void registerClass(id_t const & id, creator_t const & create) {
		if(!create) {
			throw EXCEPTION("abstract_factory : creator_t = null");
			}
		auto it = hash_tbl_.find(id);
		if(it != hash_tbl_.end()) {
			throw EXCEPTION("abstract_factory : id already exsist");
			}
		hash_tbl_.emplace(id, std::move(create));
		}

	template<class _DerivedTy>
		void registerClass(id_t const & id) {
			auto it = hash_tbl_.find(id);
			if(it != hash_tbl_.end()) {
				throw EXCEPTION("abstract_factory : id already exsist");
				}
			hash_tbl_.emplace(id, &create<_DerivedTy>);
			}

	base_ptr_t create(id_t const & id, _ArgsTy ... args) {
		auto it = hash_tbl_.find(id);
		return it != hash_tbl_.end() ? it->second(args ...) : nullptr;
		}
		
private :

	template<class _DerivedTy>
		static base_ptr_t create(_ArgsTy ... args) {
			return new _DerivedTy(args ...);
			}
    };
}
#endif
