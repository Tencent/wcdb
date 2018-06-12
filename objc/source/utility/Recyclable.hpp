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

#ifndef Recyclable_hpp
#define Recyclable_hpp

#include <functional>

#pragma GCC visibility push(hidden)

namespace WCDB {

template <typename T>
class Recyclable {
public:
    typedef std::function<void(const T &)> OnRecycled;

    Recyclable(const T &value, const Recyclable::OnRecycled &onRecycled)
        : m_value(value)
        , m_onRecycled(onRecycled)
        , m_reference(new std::atomic<int>(0))
    {
        retain();
    }

    Recyclable(Recyclable &&other)
        : m_value(std::move(other.m_value))
        , m_onRecycled(std::move(other.m_onRecycled))
        , m_reference(std::move(other.m_reference))
    {
    }

    Recyclable(const Recyclable &other)
        : m_value(other.m_value)
        , m_onRecycled(other.m_onRecycled)
        , m_reference(other.m_reference)
    {
        retain();
    }

    Recyclable &operator=(const Recyclable &other)
    {
        other.retain();
        release();
        m_value = other.m_value;
        m_onRecycled = other.m_onRecycled;
        m_reference = other.m_reference;
        return *this;
    }

    Recyclable &operator=(Recyclable &&other)
    {
        other.retain();
        release();
        m_value = std::move(other.m_value);
        m_onRecycled = std::move(other.m_onRecycled);
        m_reference = std::move(other.m_reference);
        return *this;
    }

    Recyclable(
        const std::nullptr_t &,
        typename std::enable_if<
            std::is_convertible<std::nullptr_t, T>::value>::type * = nullptr)
        : m_value(nullptr)
        , m_onRecycled(nullptr)
        , m_reference(new std::atomic<int>(0))
    {
        retain();
    }

    typename std::enable_if<std::is_convertible<std::nullptr_t, T>::value,
                            Recyclable &>::type
    operator=(const std::nullptr_t &)
    {
        release();
        m_value = nullptr;
        m_reference = nullptr;
        m_onRecycled = nullptr;
        return *this;
    }

    virtual ~Recyclable() { release(); }

    constexpr T &operator->() const { return m_value; }
    const T &get() const { return m_value; }

    typename std::enable_if<std::is_convertible<std::nullptr_t, T>::value,
                            bool>::type
    operator!=(const std::nullptr_t &) const
    {
        return m_value != nullptr;
    }

    typename std::enable_if<std::is_convertible<std::nullptr_t, T>::value,
                            bool>::type
    operator==(const std::nullptr_t &) const
    {
        return m_value == nullptr;
    }

protected:
    Recyclable() //invalid
        : m_reference(new std::atomic<int>(0))
    {
    }

    void retain() const
    {
        if (m_reference) {
            ++(*m_reference);
        }
    }
    void release()
    {
        if (m_reference) {
            if (--(*m_reference) == 0) {
                delete m_reference;
                m_reference = nullptr;
                if (m_onRecycled) {
                    m_onRecycled(m_value);
                    m_onRecycled = nullptr;
                }
            }
        }
    }

    T m_value;
    mutable std::atomic<int> *m_reference;
    Recyclable::OnRecycled m_onRecycled;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* Recyclable_hpp */
