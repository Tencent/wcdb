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

#include <memory>

namespace WCDB {

template<typename T>
class Shadow;

template<typename T>
class Cloneable {
public:
    virtual ~Cloneable() {}
    virtual std::unique_ptr<T> clone() const = 0;
};

template<typename T>
class Shadow final {
    //    static_assert(std::is_base_of<Cloneable<T>, T>::value, "");
#pragma mark - NULL
public:
    Shadow() : m_payload(nullptr) {}

    Shadow(const std::nullptr_t&) : m_payload(nullptr) {}

    Shadow& operator=(const std::nullptr_t&)
    {
        m_payload = nullptr;
        return *this;
    }

    bool operator==(std::nullptr_t) const { return m_payload == nullptr; }
    bool operator!=(std::nullptr_t) const { return m_payload != nullptr; }

#pragma mark - Value
public:
    Shadow(const T& value) : m_payload(value.clone()) {}

    Shadow& operator=(const T& value)
    {
        m_payload
        = std::move(std::unique_ptr<T>{ static_cast<T*>(value.clone().release()) });
        return *this;
    }

    T* get() const { return m_payload.get(); }

    constexpr T* operator->() const { return m_payload.get(); }

    T& operator*() const { return *m_payload.get(); }

#pragma mark - Unique
public:
    Shadow(std::unique_ptr<T>&& value) : m_payload(std::move(value)) {}

#pragma mark - Shadow
public:
    Shadow(const Shadow<T>& other)
    : m_payload(other.get() != nullptr ? std::unique_ptr<T>{ static_cast<T*>(
                                         other.m_payload.get()->clone().release()) } :
                                         nullptr)
    {
    }

    Shadow& operator=(const Shadow<T>& other)
    {
        if (other.get() != nullptr) {
            m_payload = std::move(std::unique_ptr<T>{
            static_cast<T*>(other.m_payload.get()->clone().release()) });
        } else {
            m_payload = nullptr;
        }
        return *this;
    }

private:
    std::unique_ptr<T> m_payload;
};

} // namespace WCDB
