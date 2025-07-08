//
// Created by qiuwenchen on 2023/2/16.
//

/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "Macro.h"
#include <memory>
#include <type_traits>

namespace WCDB {

struct NullOpt_T {
    struct SecretTag {
        explicit SecretTag() = default;
    };
    constexpr explicit NullOpt_T(SecretTag, SecretTag) noexcept {}
};

constexpr NullOpt_T NullOpt{ NullOpt_T::SecretTag{}, NullOpt_T::SecretTag{} };

class WCDB_API _OptionalValueFlag {
    template<typename U>
    friend class Optional;

public:
    _OptionalValueFlag();

protected:
    mutable bool m_hasValue;
};

template<class T, bool = std::is_trivially_destructible<T>::value>
class _OptionalValueDestruct : public _OptionalValueFlag {};

template<class T>
class _OptionalValueDestruct<T, false> : public _OptionalValueFlag {
    template<typename U>
    friend class _OptionalValueStorage;
    template<typename U>
    friend class Optional;

protected:
    union {
        char m_nullState;
        T m_value;
    };

    void reset()
    {
        if (m_hasValue) {
            m_value.~T();
        }
        m_nullState = 0;
        m_hasValue = false;
    }

public:
    ~_OptionalValueDestruct()
    {
        if (m_hasValue) {
            m_value.~T();
        }
    }

    _OptionalValueDestruct() : _OptionalValueFlag(), m_nullState(0) {}
};

template<class T>
class _OptionalValueDestruct<T, true> : public _OptionalValueFlag {
    template<typename U>
    friend class _OptionalValueStorage;
    template<typename U>
    friend class Optional;

protected:
    union {
        mutable char m_nullState;
        mutable T m_value;
    };

    void reset()
    {
        m_nullState = 0;
        m_hasValue = false;
    }

public:
    _OptionalValueDestruct() : _OptionalValueFlag(), m_nullState(0) {}
};

template<class T>
class _OptionalValueStorage : public _OptionalValueDestruct<T> {
public:
    constexpr inline bool hasValue() const { return this->m_hasValue; };

    constexpr inline T& value() & { return this->m_value; }

    constexpr inline const T& value() const& { return this->m_value; }

    constexpr inline T&& value() && { return std::move(this->m_value); }

    constexpr inline const T&& value() const&&
    {
        return std::move(this->m_value);
    }

    template<class U>
    constexpr T valueOr(U&& v) const&
    {
        static_assert(std::is_copy_constructible<T>::value,
                      "Optional<T>::valueOr: T must be copy constructible");
        static_assert(std::is_convertible<U, T>::value,
                      "Optional<T>::valueOr: U must be convertible to T");
        return this->hasValue() ? this->value() : static_cast<T>(std::forward<U>(v));
    }

    template<class U>
    constexpr T valueOr(U&& v) &&
    {
        static_assert(std::is_move_constructible<T>::value,
                      "Optional<T>::valueOr: T must be copy constructible");
        static_assert(std::is_convertible<U, T>::value,
                      "Optional<T>::valueOr: U must be convertible to T");
        return this->hasValue() ? std::move(this->value()) :
                                  static_cast<T>(std::forward<U>(v));
    }

    constexpr T valueOrDefault() const&
    {
        static_assert(std::is_copy_constructible<T>::value,
                      "Optional<T>::valueOrDefault: T must be copy constructible");
        static_assert(std::is_default_constructible<T>::value,
                      "Optional<T>::valueOrDefault: T must be have default constructor");
        return this->hasValue() ? this->value() : T();
    }

    constexpr T valueOrDefault() &&
    {
        static_assert(std::is_move_constructible<T>::value,
                      "Optional<T>::valueOrDefault: T must be copy constructible");
        static_assert(std::is_default_constructible<T>::value,
                      "Optional<T>::valueOrDefault: T must be have default constructor");
        return this->hasValue() ? std::move(this->value()) : T();
    }

protected:
    template<class That>
    void construct(That&& value)
    {
        new ((void*) std::addressof(this->m_value)) T(std::forward<That>(value));
        this->m_hasValue = true;
    }

    template<class That>
    void constructFrom(That&& opt)
    {
        if (opt.hasValue()) {
            construct(std::forward<That>(opt).value());
        }
    }

    template<class That>
    void assignFrom(That&& opt)
    {
        if (this->m_hasValue == opt.hasValue()) {
            if (this->m_hasValue) {
                this->m_value = std::forward<That>(opt).value();
            }
        } else {
            if (this->m_hasValue) {
                this->reset();
            } else {
                construct(std::forward<That>(opt).value());
            }
        }
    }
};

template<class T, bool = std::is_trivially_copy_constructible<T>::value>
class _OptionalCopyBase : public _OptionalValueStorage<T> {
public:
    using _OptionalValueStorage<T>::_OptionalValueStorage;
};

template<class T>
class _OptionalCopyBase<T, false> : public _OptionalValueStorage<T> {
public:
    using _OptionalValueStorage<T>::_OptionalValueStorage;

    _OptionalCopyBase() = default;

    _OptionalCopyBase(const _OptionalCopyBase& opt)
    {
        this->constructFrom(opt);
    }

    _OptionalCopyBase(_OptionalCopyBase&&) = default;
    _OptionalCopyBase& operator=(const _OptionalCopyBase&) = default;
    _OptionalCopyBase& operator=(_OptionalCopyBase&&) = default;
};

template<class T, bool = std::is_trivially_move_constructible<T>::value>
class _OptionalMoveBase : public _OptionalCopyBase<T> {
public:
    using _OptionalCopyBase<T>::_OptionalCopyBase;
};

template<class T>
class _OptionalMoveBase<T, false> : public _OptionalCopyBase<T> {
public:
    using _OptionalCopyBase<T>::_OptionalCopyBase;
    _OptionalMoveBase() = default;
    _OptionalMoveBase(const _OptionalMoveBase&) = default;

    _OptionalMoveBase(_OptionalMoveBase&& opt)
    {
        this->constructFrom(std::move(opt));
    }

    _OptionalMoveBase& operator=(const _OptionalMoveBase&) = default;
    _OptionalMoveBase& operator=(_OptionalMoveBase&&) = default;
};

template<class T,
         bool = std::is_trivially_destructible<T>::value && std::is_trivially_copy_constructible<T>::value
                && std::is_trivially_copy_assignable<T>::value>
class _OptionalCopyAssignBase : public _OptionalMoveBase<T> {
public:
    using _OptionalMoveBase<T>::_OptionalMoveBase;
};

template<class T>
class _OptionalCopyAssignBase<T, false> : public _OptionalMoveBase<T> {
public:
    using _OptionalMoveBase<T>::_OptionalMoveBase;
    _OptionalCopyAssignBase() = default;
    _OptionalCopyAssignBase(const _OptionalCopyAssignBase&) = default;
    _OptionalCopyAssignBase(_OptionalCopyAssignBase&&) = default;

    _OptionalCopyAssignBase& operator=(const _OptionalCopyAssignBase& opt)
    {
        this->assignFrom(opt);
        return *this;
    }

    _OptionalCopyAssignBase& operator=(_OptionalCopyAssignBase&&) = default;
};

template<class T,
         bool = std::is_trivially_destructible<T>::value && std::is_trivially_move_constructible<T>::value
                && std::is_trivially_move_assignable<T>::value>
class _OptionalMoveAssignBase : public _OptionalCopyAssignBase<T> {
public:
    using _OptionalCopyAssignBase<T>::_OptionalCopyAssignBase;
};

template<class T>
class _OptionalMoveAssignBase<T, false> : public _OptionalCopyAssignBase<T> {
public:
    using _OptionalCopyAssignBase<T>::_OptionalCopyAssignBase;

    _OptionalMoveAssignBase() = default;
    _OptionalMoveAssignBase(const _OptionalMoveAssignBase& __opt) = default;
    _OptionalMoveAssignBase(_OptionalMoveAssignBase&&) = default;
    _OptionalMoveAssignBase& operator=(const _OptionalMoveAssignBase&) = default;

    _OptionalMoveAssignBase& operator=(_OptionalMoveAssignBase&& opt)
    {
        this->assignFrom(std::move(opt));
        return *this;
    }
};

template<typename T>
class Optional : public _OptionalMoveAssignBase<T> {
    static_assert(std::is_object<T>::value,
                  "instantiation of optional with a non-object type is undefined behavior");

public:
    typedef T ValueType;
#pragma mark - Interface

    inline bool succeed() const { return this->m_hasValue; }
    inline bool failed() const { return !this->m_hasValue; };

    constexpr inline T* operator->() { return &this->m_value; }

    constexpr inline const T* operator->() const { return &this->m_value; }

    T& getOrCreate()
    {
        if (!this->m_hasValue) {
            new ((void*) std::addressof(this->m_value)) T();
            this->m_hasValue = true;
        }
        return this->m_value;
    }

    const T& getOrCreate() const
    {
        if (!this->m_hasValue) {
            new ((void*) std::addressof(this->m_value)) T();
            this->m_hasValue = true;
        }
        return this->m_value;
    }

#pragma mark - Constructor

    Optional() : _OptionalMoveAssignBase<T>() {}

    Optional(const NullOpt_T&) : _OptionalMoveAssignBase<T>() {}

    template<typename U, typename Enable = void>
    struct Constructable : public std::false_type {};

    template<typename U>
    struct Constructable<U, typename std::enable_if<!std::is_base_of<Optional, U>::value && std::is_constructible<T, U>::value>::type>
    : public std::true_type {};

    template<typename U, typename Enable = typename std::enable_if<Constructable<U>::value && std::is_copy_constructible<U>::value>::type>
    Optional(const Optional<U>& opt)
    {
        this->constructFrom(opt);
    }

    template<typename U, typename Enable = typename std::enable_if<Constructable<U>::value && std::is_move_constructible<U>::value>::type>
    Optional(Optional<U>&& opt)
    {
        this->constructFrom(std::move(opt));
    }

    template<typename U, typename Enable = typename std::enable_if<Constructable<U>::value && std::is_copy_constructible<U>::value>::type>
    Optional(const U& value)
    {
        this->construct(value);
    }

    template<typename U, typename Enable = typename std::enable_if<Constructable<U>::value && std::is_move_constructible<U>::value>::type>
    Optional(U&& value)
    {
        this->construct(std::forward<U>(value));
    }

#pragma mark - Assignment

    Optional<T>& operator=(const NullOpt_T&)
    {
        this->reset();
        return *this;
    }

    template<typename U, typename Enable = typename std::enable_if<Constructable<U>::value && std::is_copy_assignable<U>::value>::type>
    Optional<T>& operator=(const Optional<U>& other)
    {
        this->assignFrom(other);
        return *this;
    }

    template<typename U, typename Enable = typename std::enable_if<Constructable<U>::value && std::is_move_assignable<U>::value>::type>
    Optional<T>& operator=(Optional<U>&& other)
    {
        this->assignFrom(std::move(other));
        return *this;
    }

    template<typename U, typename Enable = typename std::enable_if<Constructable<U>::value>::type>
    Optional<T>& operator=(U&& value)
    {
        if (this->m_hasValue) {
            this->m_value = std::forward<U>(value);
        } else {
            this->construct(std::forward<U>(value));
        }
        return *this;
    }

    bool operator==(const NullOpt_T&) const { return !this->m_hasValue; }

    bool operator!=(const NullOpt_T&) const { return this->m_hasValue; }

    template<typename U, typename Enable = typename std::enable_if<Constructable<U>::value>::type>
    bool operator==(const Optional<U>& other) const
    {
        if (this->m_hasValue != other.m_hasValue) {
            return false;
        } else if (!this->m_hasValue) {
            return true;
        }
        return this->m_value == other.m_value;
    }

    template<typename U, typename Enable = typename std::enable_if<Constructable<U>::value>::type>
    bool operator!=(const Optional<U>& other) const
    {
        if (this->m_hasValue != other.m_hasValue) {
            return true;
        } else if (this->m_hasValue) {
            return false;
        }
        return this->m_value != other.m_value;
    }

    inline constexpr const ValueType& operator*() const& noexcept
    {
        return this->m_value;
    }

    inline constexpr ValueType& operator*() & noexcept { return this->m_value; }

    inline constexpr ValueType&& operator*() && noexcept
    {
        return std::move(this->m_value);
    }

    inline constexpr const ValueType&& operator*() const&& noexcept
    {
        return std::move(this->m_value);
    }
};

template<class T>
struct IsWCDBOptional : std::false_type {};

template<class T>
struct IsWCDBOptional<Optional<T>> : std::true_type {};

} // namespace WCDB
