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

#ifndef concurrent_list_hpp
#define concurrent_list_hpp

#include <WCDB/spin.hpp>
#include <list>

namespace WCDB {

template <typename T>
class ConcurrentList {
public:
    using ElementType = std::shared_ptr<T>;
    ConcurrentList(size_t capacityCap) : m_capacityCap(capacityCap) {}

    size_t getCapacityCap() const
    {
        SpinLockGuard<Spin> lockGuard(m_spin);
        return m_capacityCap;
    }

    bool pushBack(const ElementType &value)
    {
        SpinLockGuard<Spin> lockGuard(m_spin);
        if (m_list.size() < m_capacityCap) {
            m_list.push_back(value);
            return true;
        }
        return false;
    }

    bool pushFront(const ElementType &value)
    {
        SpinLockGuard<Spin> lockGuard(m_spin);
        if (m_list.size() < m_capacityCap) {
            m_list.push_front(value);
            return true;
        }
        return false;
    }

    ElementType popBack()
    {
        SpinLockGuard<Spin> lockGuard(m_spin);
        if (m_list.empty()) {
            return nullptr;
        }
        ElementType value = m_list.back();
        m_list.pop_back();
        return value;
    }

    ElementType popFront()
    {
        SpinLockGuard<Spin> lockGuard(m_spin);
        if (m_list.empty()) {
            return nullptr;
        }
        ElementType value = m_list.front();
        m_list.pop_front();
        return value;
    }

    bool isEmpty() const
    {
        SpinLockGuard<Spin> lockGuard(m_spin);
        return m_list.empty();
    }

    size_t size() const
    {
        SpinLockGuard<Spin> lockGuard(m_spin);
        return m_list.size();
    }

    size_t clear()
    {
        SpinLockGuard<Spin> lockGuard(m_spin);
        size_t size = m_list.size();
        m_list.clear();
        return size;
    }

protected:
    std::list<ElementType> m_list;
    size_t m_capacityCap;
    mutable Spin m_spin;
};

} //namespace WCDB

#endif /* concurrent_list_hpp */
