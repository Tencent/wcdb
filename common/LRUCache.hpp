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

#ifndef LRUCache_hpp
#define LRUCache_hpp

#include <list>
#include <unordered_map>

namespace WCDB {

template<typename Key, typename Value>
class LRUCache {
public:
    typedef typename std::pair<Key, Value> KeyValue;
    typedef typename std::list<KeyValue>::iterator Iterator;

    LRUCache(size_t maxSize = std::numeric_limits<size_t>::max())
    : m_maxSize(maxSize)
    {
    }

    void put(const Key& key, const Value& value)
    {
        auto it = m_map.find(key);
        m_list.push_front(KeyValue(key, value));
        if (it != m_map.end()) {
            m_list.erase(it->second);
            m_map.erase(it);
        }
        m_map[key] = m_list.begin();
        if (m_map.size() > m_maxSize) {
            purge();
        }
    }

    void purge(size_t count = 1)
    {
        for (size_t i = 0; i < count && !m_list.empty(); ++i) {
            auto last = m_list.end();
            --last;
            m_map.erase(last->first);
            m_list.pop_back();
        }
    }

    const Value& get(const Key& key)
    {
        auto it = m_map.find(key);
        assert(exists(key));
        m_list.splice(m_list.begin(), m_list, it->second);
        return it->second->second;
    }

    bool exists(const Key& key) const { return m_map.find(key) != m_map.end(); }

    size_t size() const { return m_map.size(); }

    bool empty() const { return m_map.empty(); }

protected:
    size_t m_maxSize;
    std::list<KeyValue> m_list;
    std::unordered_map<Key, Iterator> m_map;
};

} // namespace WCDB

#endif /* LRUCache_hpp */
