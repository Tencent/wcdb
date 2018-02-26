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

#ifndef sequantial_increasing_map_hpp
#define sequantial_increasing_map_hpp

#import <list>
#import <map>
#import <memory>

template <typename Key, typename Value, class Comparator = std::less<Key>>
class SequentialIncreasingMap {
public:
    using Map = std::map<Key, std::shared_ptr<Value>, Comparator>;
    using List = std::list<std::shared_ptr<Value>>;

    void append(const Key &key, const std::shared_ptr<Value> &value)
    {
        assert(m_map.find(key) == m_map.end());
        m_map[key] = value;
        m_list.push_back(value);
    }

    const Map &getMap() const { return m_map; }

    const List &getList() const { return m_list; }

protected:
    List m_list;
    Map m_map;
};

#endif /* sequantial_increasing_map_hpp */
