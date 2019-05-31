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

/*
 * Unique list is a data structure that
 * 1. constrained by unique key.
 * 2. sorted by ascending order.
 *
 * The performance is not good while
 * 1. Both insert/find/erase are O(N)
 * 2. Initialization is O(N^2)
 *
 * It's designed for tiny dataset less than 20 in usual thus... well.
 */

#include <list>

namespace WCDB {

template<typename Key, typename Value, typename Order>
class UniqueListElement {
private:
    typedef UniqueListElement<Key, Value, Order> SelfType;

public:
    UniqueListElement(const Key& key, const Value& value, const Order& order)
    : m_key(key), m_value(value), m_order(order)
    {
    }

    const Key& key() const { return m_key; }

    Value& value() { return m_value; }

    const Value& value() const { return m_value; }

    const Order& order() const { return m_order; }

    bool operator==(const SelfType& other) const
    {
        return m_order == other.m_order && m_key == other.m_key
               && m_value == other.m_value;
    }

private:
    Key m_key;
    Value m_value;
    Order m_order;
};

// Small order first
template<typename Key, typename Value, typename Order = int>
class UniqueList : private std::list<UniqueListElement<Key, Value, Order>> {
protected:
    typedef UniqueListElement<Key, Value, Order> Element;
    typedef std::list<Element> SuperType;
    typedef UniqueList<Key, Value, Order> SelfType;
    typedef typename SuperType::iterator Iterator;

public:
    UniqueList() {}

    UniqueList(const std::initializer_list<Element>& elements)
    {
        for (const auto& element : elements) {
            insert(element.key(), element.value(), element.order());
        }
    }

    UniqueList(const SelfType& elements)
    {
        for (const auto& element : elements) {
            insert(element.key(), element.value(), element.order());
        }
    }

    void insert(const Key& key, const Value& value, const Order& order)
    {
        Iterator iter = this->begin();
        bool inserted = false;
        bool erased = false;
        while (iter != this->end() && (!inserted || !erased)) {
            if (!erased && iter->key() == key) {
                iter = this->SuperType::erase(iter);
                erased = true;
                continue;
            }
            if (!inserted && order < iter->order()) {
                iter = this->SuperType::insert(iter, Element(key, value, order));
                inserted = true;
            }
            ++iter;
        }
        if (!inserted) {
            this->push_back(Element(key, value, order));
        }
    }

    Iterator erase(const Key& key)
    {
        Iterator iter = find(key);
        if (iter != this->end()) {
            iter = this->SuperType::erase(iter);
        }
        return iter;
    }

    Iterator find(const Key& key)
    {
        return std::find_if(
        this->begin(),
        this->end(),
        std::bind(&UniqueList<Key, Value, Order>::findKey, key, std::placeholders::_1));
    }

    bool operator==(const SelfType& other) const
    {
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    bool operator!=(const SelfType& other) const
    {
        return !std::equal(begin(), end(), other.begin(), other.end());
    }

    using SuperType::begin;
    using SuperType::end;
    using SuperType::clear;
    using SuperType::size;
    using SuperType::empty;
    using SuperType::back;
    using SuperType::front;
    using SuperType::erase;

private:
    static bool findKey(const Key& key, const Element& element)
    {
        return element.key() == key;
    }
    static bool compareKey(const Element& left, const Element& right)
    {
        return left.key() < right.key();
    }
};

} // namespace WCDB
