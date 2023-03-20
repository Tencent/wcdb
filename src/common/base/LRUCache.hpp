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

#include "Assertion.hpp"
#include <list>
#include <unordered_map>

namespace WCDB {

template<typename Key, typename Value>
class LRUCache {
protected:
    using KeyValue = typename std::pair<Key, Value>;
    using List = std::list<KeyValue>;
    using KeyValueIterator = typename List::iterator;
    using Map = std::map<Key, KeyValueIterator>;
    using MapIterator = typename Map::iterator;

public:
    LRUCache() = default;
    virtual ~LRUCache() = default;

    void put(const Key& key, const Value& value)
    {
        auto it = m_map.find(key);
        m_list.push_front(KeyValue(key, value));
        if (it != m_map.end()) {
            m_list.erase(it->second);
            m_map.erase(it);
        }
        m_map[key] = m_list.begin();
        if (shouldPurge()) {
            purge();
        }
    }

    void purge(size_t count = 1)
    {
        for (size_t i = 0; i < count && !m_list.empty(); ++i) {
            auto last = m_list.end();
            --last;
            willPurge(last->first, last->second);
            m_map.erase(last->first);
            m_list.pop_back();
        }
    }

    const Value& get(const Key& key)
    {
        auto it = m_map.find(key);
        WCTAssert(exists(key));
        retain(it);
        return it->second->second;
    }

    bool exists(const Key& key) const { return m_map.find(key) != m_map.end(); }

    size_t size() const { return m_map.size(); }

    bool empty() const { return m_map.empty(); }

protected:
    void retain(const MapIterator& it)
    {
        m_list.splice(m_list.begin(), m_list, it->second);
    }

    virtual bool shouldPurge() const = 0;
    virtual void willPurge(const Key& key, const Value& value)
    {
        WCDB_UNUSED(key);
        WCDB_UNUSED(value);
    }

    List m_list;
    Map m_map;
};

} // namespace WCDB
