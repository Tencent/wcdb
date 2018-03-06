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

#ifndef CopyOnWrite_hpp
#define CopyOnWrite_hpp

#include <assert.h>
#include <memory>

template <typename T>
class CopyOnWrite {
public:
    CopyOnWrite() : m_shared(nullptr) {}

    CopyOnWrite(const T &t) : m_shared(new T(t)) {}

    void assign(const T &t)
    {
        willProbablyChange();
        m_shared.reset(new T(t));
    }

    void assign(const CopyOnWrite<T> &t)
    {
        willProbablyChange();
        m_shared = t.m_shared;
    }

    template <typename U>
    void assign(const CopyOnWrite<U> &u)
    {
        willProbablyChange();
        m_shared = std::static_pointer_cast<T>(u.m_shared);
    }

    void clear()
    {
        willProbablyChange();
        m_shared = nullptr;
    }

    bool empty() const { return m_shared == nullptr; }

    const T &get() const
    {
        assert(!empty());
        return *(m_shared.get());
    }

    T &get_or_copy()
    {
        willProbablyChange();
        if (!m_shared.unique()) {
            if (empty()) {
                m_shared.reset(new T);
            } else {
                m_shared.reset(new T(*m_shared.get()));
            }
        }
        return *(m_shared.get());
    }

    bool equal(const CopyOnWrite<T> &other) const
    {
        return m_shared.get() == other.m_shared.get();
    }

protected:
    virtual void willProbablyChange(){};

private:
    std::shared_ptr<T> m_shared;

    template <typename D>
    friend class CopyOnWrite;
};

#endif /* CopyOnWrite_hpp */
