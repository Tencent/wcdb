//
// Created by sanhuazhang on 2019/05/02
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

#include <memory>

namespace WCDB {

template<typename T>
class Cloneable {
public:
    virtual ~Cloneable() = default;
    virtual std::shared_ptr<T> clone() const = 0;
};

template<typename T>
class Shadow final {
    //    static_assert(std::is_base_of<Cloneable<T>, T>::value, "");
#pragma mark - NULL
public:
    Shadow() : m_payload(nullptr), m_isPrivate(true) {}

    Shadow(const std::nullptr_t&) : m_payload(nullptr), m_isPrivate(true) {}

    Shadow& operator=(const std::nullptr_t&)
    {
        m_payload = nullptr;
        m_isPrivate = true;
        return *this;
    }

    bool operator==(std::nullptr_t) const { return m_payload == nullptr; }
    bool operator!=(std::nullptr_t) const { return m_payload != nullptr; }

#pragma mark - Value
public:
    Shadow(const T& value) : m_payload(value.clone()), m_isPrivate(true) {}

    Shadow(std::shared_ptr<T>&& value)
    : m_payload(std::move(value)), m_isPrivate(true)
    {
        m_isPrivate = true;
    }

    Shadow& operator=(const T& value)
    {
        auto newPayload = value.clone();
        m_payload = std::shared_ptr<T>(newPayload, static_cast<T*>(newPayload.get()));
        m_isPrivate = true;
        return *this;
    }

    const T* get() const { return m_payload.get(); }

    T* get()
    {
        if (!m_isPrivate) {
            if (m_payload.use_count() > 1) {
                auto newPayload = m_payload->clone();
                m_payload
                = std::shared_ptr<T>(newPayload, static_cast<T*>(newPayload.get()));
                m_isPrivate = true;
            } else {
                m_isPrivate = false;
            }
        }
        return m_payload.get();
    }

#pragma mark - Shadow
public:
    Shadow(const Shadow<T>& other)
    : m_payload(other.m_payload), m_isPrivate(other.m_payload != nullptr ? false : true)
    {
        if (other.m_payload != nullptr) {
            other.m_isPrivate = false;
        }
    }

    Shadow(Shadow<T>&& other)
    : m_payload(std::move(other.m_payload)), m_isPrivate(other.m_isPrivate)
    {
    }

    Shadow& operator=(const Shadow<T>& other)
    {
        m_payload = other.m_payload;
        m_isPrivate = m_payload == nullptr;
        other.m_isPrivate = m_isPrivate;
        return *this;
    }

    Shadow& operator=(Shadow<T>&& other)
    {
        m_payload = std::move(other.m_payload);
        m_isPrivate = other.m_isPrivate;
        return *this;
    }

private:
    std::shared_ptr<T> m_payload;
    mutable bool m_isPrivate;
};

} // namespace WCDB
