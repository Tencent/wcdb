//
// Created by sanhuazhang on 2018/07/23
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
#include <atomic>
#include <functional>
#include <memory>

namespace WCDB {

class WCDB_API Referenced {
public:
    Referenced();
    Referenced(const std::nullptr_t &);
    virtual ~Referenced() = 0;

protected:
    void reset(const Referenced &other);
    void reset(const std::nullptr_t &);

    void retain() const;
    void release();
    virtual void onUnreferenced() = 0;

private:
    mutable std::shared_ptr<std::atomic<int>> m_reference;
};

template<typename T>
class Recyclable : public Referenced {
public:
    typedef std::function<void(T &)> OnRecycled;

    Recyclable() : Referenced(nullptr), m_value(nullptr), m_onRecycled(nullptr)
    {
    }

    Recyclable(const std::nullptr_t &) : Recyclable() {}

    Recyclable(const T &value, const Recyclable::OnRecycled &onRecycled)
    : Referenced(), m_value(value), m_onRecycled(onRecycled)
    {
        retain();
    }

    Recyclable(const Recyclable &other)
    : Referenced(other), m_value(other.m_value), m_onRecycled(other.m_onRecycled)
    {
        retain();
    }

    Recyclable(Recyclable &&other)
    : Referenced(other), m_value(std::move(other.m_value)), m_onRecycled(other.m_onRecycled)
    {
        retain();
        other = nullptr;
    }

    Recyclable &operator=(const Recyclable &other)
    {
        reset(other);
        m_value = other.m_value;
        m_onRecycled = other.m_onRecycled;
        return *this;
    }

    Recyclable &operator=(Recyclable &&other)
    {
        reset(other);
        m_value = std::move(other.m_value);
        m_onRecycled = other.m_onRecycled;
        other = nullptr;
        return *this;
    }

    Recyclable &operator=(const std::nullptr_t &)
    {
        reset(nullptr);
        m_value = nullptr;
        m_onRecycled = nullptr;
        return *this;
    }

    bool operator==(const Recyclable &other) const
    {
        return m_value == other.m_value;
    }

    bool operator==(const std::nullptr_t &) const { return m_value == nullptr; }
    bool operator!=(const std::nullptr_t &) const { return m_value != nullptr; }

    ~Recyclable() override { release(); }

    constexpr const T &operator->() const { return m_value; }
    constexpr T &operator->() { return m_value; }
    T &get() { return m_value; }
    const T &get() const { return m_value; }

protected:
    void onUnreferenced() override final
    {
        if (m_onRecycled != nullptr) {
            m_onRecycled(m_value);
            m_onRecycled = nullptr;
        }
    }

    T m_value;
    Recyclable::OnRecycled m_onRecycled;
};

} //namespace WCDB
