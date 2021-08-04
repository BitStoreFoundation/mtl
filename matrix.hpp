﻿// c++17 @Tarnakin V.D.
//this header has a description of the matrix container
#pragma once
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "base_mixing_templates.hpp"
#include "type_traits.hpp"
#include <vector>
#include <array>

namespace tvd {
// mixing for container class
template<
    class _DerivedTy,
    class _ElemContainerTy>
    class add_base_methods
    {
public :
    using derived_t        = _DerivedTy;
    using type_t           = typename _ElemContainerTy::elem_traits_t::type_t;
    using pointer_t        = typename _ElemContainerTy::elem_traits_t::pointer_t;
    using iterator_t       = typename _ElemContainerTy::container_t::iterator;
    using const_iterator_t = typename _ElemContainerTy::container_t::const_iterator;
private :
    derived_t *derived_;
public :
    add_base_methods()
        : derived_(static_cast<derived_t*>(this))
        {
        }
    // begin/end
    const_iterator_t cbegin() const {
        return derived_->container_.cbegin();
        }

    const_iterator_t cend() const {
        return derived_->container_.cend();
        }

    const_iterator_t begin() const {
        return derived_->container_.begin();
        }

    const_iterator_t end() const {
        return derived_->container_.end();
        }

    iterator_t begin() {
        return derived_->container_.begin();
        }

    iterator_t end() {
        return derived_->container_.end();
        }

    pointer_t data() const {
        return derived_->container_.data();
        }
    };

template<
    typename _Ty,
    size_t,
    class = elem_traits<typename std::remove_pointer<_Ty>::type> >
    class vector;

template<
    class _MatrixTy,
    class _ElemTraitsTy>
    class matrix_mixing
        : public add_base_methods<_MatrixTy, elem_container<_ElemTraitsTy> >
        , public add_non_equalable<_MatrixTy>
        , public add_sum<_MatrixTy>
        , public add_difference<_MatrixTy>
        , public add_division_by_value<_MatrixTy, _ElemTraitsTy> { };
// matrix container
template<
    typename _Ty = float,
    size_t col_size = 3,
    class _ElemTraitsTy = elem_traits<_Ty> >
    class matrix final
        : public matrix_mixing<matrix<_Ty, col_size>, _ElemTraitsTy>
    {
    static_assert(!std::is_pointer_v<_Ty>, "tvd::matrix<_Ty, size_t> : no specialization of class for pointer");
    friend class add_base_methods<matrix<_Ty, col_size>, elem_container<_ElemTraitsTy> >;
    friend class vector<_Ty*, col_size>;
public :
    using add_multiplying_by_value<matrix<_Ty, col_size>, _ElemTraitsTy>::operator*;
    using ptrs_vector_t = vector<_Ty*, col_size>;
    using vector_t      = vector<_Ty, col_size>;
    using type_t        = typename _ElemTraitsTy::type_t;
    using pointer_t     = typename _ElemTraitsTy::pointer_t;
    template<typename Ty = _Ty>
        using init_list_t = std::initializer_list<Ty> const&;
private :
    mutable std::vector<_Ty> container_;
public :
    matrix() = default;

    matrix(matrix const & other)
        : container_(other.container_)
        {
        }

    matrix(matrix && other) noexcept
        : container_(std::move(other.container_))
        {
        }

    explicit matrix(size_t const & size)
        : matrix_mixing<matrix<_Ty, col_size>, _ElemTraitsTy>()
        , container_(size*col_size)
        {
        }

    matrix(init_list_t<> list)
        : matrix_mixing<matrix<_Ty, col_size>, _ElemTraitsTy>()
        , container_(list.size())
        {
        if(col_size > list.size() || list.size()%col_size != 0) {
            throw EXCEPTION("matrix::matrix : bad initializer list size");
            }
        std::copy(list.begin(), list.end(), container_.begin());
        }

    matrix(init_list_t<vector_t> list)
        : matrix_mixing<matrix<_Ty, col_size>, _ElemTraitsTy>()
        , container_(list.size())
        {
        if(col_size > list.size() || list.size()%col_size != 0) {
            throw EXCEPTION("matrix::matrix : bad initializer list size");
            }
        for(auto const & vector : list)
            insert(vector);
        }

    matrix(size_t const & size, pointer_t array)
        : matrix(size)
        {
        for(size_t i = 0; i < container_.size(); i++)
            container_[i] = array[i];
        delete array;
        }

    size_t size() const noexcept {
        return container_.size()/col_size;
        }

    size_t csize() const noexcept {
        return col_size;
        }

    void resize(size_t size) {
        if(size == this->size()) {
            return;
            }
        if(size%col_size != 0) {
            throw EXCEPTION("matrix : bad new size, size % col_size != 0");
            }
        container_.resize(size*col_size);
        }

    void push_front(vector_t const & vector) {
        container_.insert(container_.begin(), vector.begin(), vector.end());
        }

    void push_back(vector_t const & vector) {
        container_.insert(container_.end(), vector.begin(), vector.end());
        }

    void insert(vector_t const & vector, size_t pos = 0) {
        if(pos >= container_.size()) {
            throw EXCEPTION("matrix : bad insert position");
            }
        container_.insert(container_.begin() + pos*col_size, vector.begin(), vector.end());
        }

    void erase(size_t pos) {
        if(pos >= container_.size()) {
            throw EXCEPTION("matrix : bad erase position");
            }
        auto it = container_.begin() + pos*col_size;
        container_.erase(it, it + col_size);
        }
    // overloads
    bool operator == (matrix const & other) {
        auto size = container_.size();
        if(size != other.container_.size()) {
            return false;
            }
        for(size_t i = 0; i < size; i++)
            if(container_[i] != other.container_[i]) {
                return false;
                }
        return true;
        }

    matrix & operator += (matrix const & other) {
        for(size_t i = 0; i < container_.size(); i++)
            container_[i] += other.container_[i];
        return *this;
        }

    matrix & operator -= (matrix const & other) {
        for(size_t i = 0; i < container_.size(); i++)
            container_[i] -= other.container_[i];
        return *this;
        }

    matrix & operator *= (_Ty const & value) {
        for(auto & it : container_)
            it *= value;
        return *this;
        }

    matrix & operator *= (matrix const & other) {
        pointer_t array = multiply(other);
        for(size_t i = 0; i < container_.size(); i++)
            container_[i] = array[i];
        return *this;
        }

    template<size_t col_size_>
        matrix<_Ty, col_size_> operator * (matrix<_Ty, col_size_> const & other) {
            return matrix<_Ty, col_size_>(size(), multiply(other));
            }

    template<
        typename Ty,
        typename _EnableTy = typename std::remove_pointer<Ty>::type,
        is_same_t<_EnableTy, _Ty> = true>
        matrix<_Ty, 1> operator * (vector<Ty, col_size> const & other) {
            matrix<_Ty, 1> m(other.size());
            for(size_t i = 0; i < size(); i++)
                m[i][0] = other[i];
            return *this*m;
            } /**/

    matrix & operator = (matrix const & other) {
        if(this == &other) return *this;
        container_ = other.container_;
        return *this;
        }

    matrix & operator = (matrix && other) noexcept {
        if(this == &other) return *this;
        container_ = std::move(other.container_);
        return *this;
        }

    matrix & operator = (init_list_t<> list) {
        if(container_.size() > list.size()) {
            throw EXCEPTION("matrix : container_.size() > list.size()");
            }
        std::copy(list.begin(), list.end(), container_.begin());
        return *this;
        }

    matrix & operator = (init_list_t<vector_t> list) {
        if(container_.size() > list.size()) {
            throw EXCEPTION("matrix : container_.size() > list.size()");
            }
        for(auto const & vector : list)
            insert(vector);
        return *this;
        }

    ptrs_vector_t operator [] (size_t const & i) {
        if(i >= container_.size()) {
            throw EXCEPTION("matrix : i >= col_size | matrix is empty");
            }
        return vector<_Ty*, col_size>(*this, i);
        }

    vector_t operator [] (size_t const & i) const {
        if(i >= container_.size()) {
            throw EXCEPTION("matrix : i >= col_size | matrix is empty");
            }
        vector_t vector;
        std::copy(container_.begin() + i*col_size, container_.begin() + col_size*(i + 1), vector.begin());
        return vector;
        }
private :

    template<size_t col_size_>
        pointer_t multiply(matrix<_Ty, col_size_> const & m)
            {
            if constexpr(std::is_pointer_v<_Ty>) {
                static_assert(false_v<_Ty>, "no viable overloaded for pointer type");
                }
            if(col_size != m.size()) {
                throw EXCEPTION("matrix : M1xN1 && M2xN2 -> H1 != M2");
                }
            size_t size = container_.size();
            pointer_t m_res_ = new _Ty[col_size_*size];
            for (size_t i = 0; i < size; i++)
                for (size_t j = 0; j < col_size_; j++)
                    for (size_t k = 0; k < col_size; k++) {
                        m_res_[i*col_size_ + j] += container_[i*col_size + k]*m[k][j];
                    }
            return m_res_;
            }
    };

template<
    typename _Ty,
    size_t size,
    class _ElemTraitsTy>
    class vector_mixing
        : public add_base_methods<vector<_Ty, size>, elem_container<elem_traits<_Ty> > >
        , public add_non_equalable<vector<_Ty, size>, vector<typename _ElemTraitsTy::type_t, size> >
        , public add_non_equalable<vector<_Ty, size>, vector<typename _ElemTraitsTy::pointer_t, size> >
        , public add_sum<vector<_Ty, size>, vector<typename _ElemTraitsTy::type_t, size> >
        , public add_sum<vector<_Ty, size>, vector<typename _ElemTraitsTy::pointer_t, size> >
        , public add_difference<vector<_Ty, size>, vector<typename _ElemTraitsTy::type_t, size> >
        , public add_difference<vector<_Ty, size>, vector<typename _ElemTraitsTy::pointer_t, size> >
        , public add_division_by_value<vector<_Ty, size>, _ElemTraitsTy, vector<typename _ElemTraitsTy::type_t, size> >
        {
        };
// vector container
template<
    typename _Ty,
    size_t col_size,
    class _ElemTraitsTy>
    class vector final
        : public vector_mixing<_Ty, col_size, _ElemTraitsTy>
    {
    friend class add_base_methods<vector<_Ty, col_size>, elem_container<elem_traits<_Ty> > >;
public : // definitions
    using type_t      = typename _ElemTraitsTy::type_t;
    using pointer_t   = typename _ElemTraitsTy::pointer_t;
    using reference_t = typename _ElemTraitsTy::reference_t;
    using init_list_t = std::initializer_list<_Ty> const&;
private :
    std::vector<_Ty> container_;
public :

    vector()
        : vector_mixing<_Ty, col_size, _ElemTraitsTy>()
        , container_(col_size)
        {
        }

    vector(vector const & other)
        : container_(other.container_)
        {
        }

    vector(vector && other) noexcept
        : container_(std::move(other.container_))
        {
        }

    template<
        typename _EnableTy = _Ty,
        is_pointer_t<_EnableTy> = false>
        vector(_Ty const & value)
            : vector()
            {
            for(auto & value_ : container_)
                value_ = value;
            }
    // проверить const
    template<
        typename Ty,
        typename _EnableTy = _Ty,
        is_pointer_t<_EnableTy> = true>
        vector(matrix<Ty, col_size> const & matrix, size_t const & i)
            : vector()
            {
            for(size_t j = 0; j < col_size; j++)
                {
                if constexpr(std::is_pointer_v<Ty>)
                    container_[j] = matrix.container_[i*col_size + j];
                else
                    container_[j] = &matrix.container_[i*col_size + j];
                }
            }

    vector(init_list_t list)
        : vector()
        {
        if(col_size < list.size()) {
            throw EXCEPTION("vector::vector : bad initializer_list size");
            }
        size_t j(0);
        for(auto const & col : list)
            container_[j++] = col;
        }

    size_t size() const noexcept {
        return col_size;
        }

    vector & operator = (vector const & other) {
        if(this == &other) return *this;
        container_ = other.container_;
        return *this;
        }

    vector & operator = (vector && other) noexcept {
        if(this == &other) return *this;
        container_ = std::move(other.container_);
        return *this;
        }

    template<typename Ty,
        typename _EnableTy = _Ty,
        is_pointer_t<_EnableTy> = true>
        operator vector<Ty, col_size> () {
            vector<Ty, col_size> vector;
            for(size_t i = 0; i < col_size; i++)
                vector[i] = *container_[i];
            return vector;
            }

    bool operator == (vector<pointer_t, col_size> const & other) {
        if(col_size != other.size()) {
            return false;
            }
        for(size_t i = 0; i < col_size; i++)
            {
            if constexpr(std::is_pointer_v<_Ty>)
                {
                if((*container_[i]) != other[i])
                    {
                    return false;
                    }
                }
            else
                {
                if(container_[i] != other[i])
                    {
                    return false;
                    }
                }
            }
        return true;
        }

    bool operator == (vector<type_t, col_size> const & other) {
        if(col_size != other.size()) {
            return false;
            }
        for(size_t i = 0; i < col_size; i++)
            {
            if constexpr(std::is_pointer_v<_Ty>)
                {
                if((*container_[i]) != other[i])
                    {
                    return false;
                    }
                }
            else
                {
                if(container_[i] != other[i])
                    {
                    return false;
                    }
                }
            }
        return true;
        }

    vector & operator += (vector<pointer_t, col_size> const & other) {
        for(size_t i = 0; i < col_size; i++)
            if constexpr(std::is_pointer_v<_Ty>)
                (*container_[i]) += other[i];
            else
                container_[i] += other[i];
        return *this;
        }

    vector & operator += (vector<type_t, col_size> const & other) {
        for(size_t i = 0; i < col_size; i++)
            if constexpr(std::is_pointer_v<_Ty>)
                (*container_[i]) += other[i];
            else
                container_[i] += other[i];
        return *this;
        }

    vector & operator -= (vector<pointer_t, col_size> const & other) {
        for(size_t i = 0; i < col_size; i++)
            if constexpr(std::is_pointer_v<_Ty>)
                (*container_[i]) -= other[i];
            else
                container_[i] -= other[i];
        return *this;
        }

    vector & operator -= (vector<type_t, col_size> const & other) {
        for(size_t i = 0; i < col_size; i++)
            if constexpr(std::is_pointer_v<_Ty>)
                (*container_[i]) -= other[i];
            else
                container_[i] -= other[i];
        return *this;
        }

    vector & operator *= (type_t const & value) {
        for(auto & it : container_)
            if constexpr(std::is_pointer_v<_Ty>)
                (*it) += value;
            else
                it += value;
        return *this;
        }

    reference_t operator [] (size_t const & j) {
        if(j >= col_size) {
            throw EXCEPTION("TMtxVector : bad access");
            }
        if constexpr(std::is_pointer_v<_Ty>) return *container_[j];
        else return container_[j];
        }

    type_t operator [] (size_t const & j) const {
        if(j >= col_size) {
            throw EXCEPTION("TMtxVector : bad access");
            }
        if constexpr(std::is_pointer_v<_Ty>) return *container_[j];
        else return container_[j];
        }
    };

// deduction guide
template<
    typename _Ty, 
    typename... _ArgsTy>
    vector(_Ty, _ArgsTy...) -> vector<_Ty, 1 + sizeof...(_ArgsTy)>;

template<
    typename _Ty, 
    size_t col_size>
    matrix(vector<_Ty, col_size>) -> matrix<_Ty, col_size>;
// definitions
namespace math_types {
// matrix with size 3xn/4xn
template<typename _Ty>
    using matrix_3xn_t = matrix<_Ty, 3>;
template<typename _Ty>
    using matrix_4xn_t = matrix<_Ty, 4>;
// vector with 3/4 elements
template<typename _Ty>
    using vector3_t = vector<_Ty, 3>;
template<typename _Ty>
    using vector4_t = vector<_Ty, 4>;
}
}
#endif