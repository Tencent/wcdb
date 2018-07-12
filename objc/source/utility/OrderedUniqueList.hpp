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

#ifndef OrderedUniqueList_hpp
#define OrderedUniqueList_hpp

#include <list>

namespace WCDB {

template<typename Key, typename Value>
class OrderedUniqueList {
public:
    struct Element {
        int order;
        Key key;
        Value value;
    };

    void insert(const Key& key, const Value& value)
    {
        Element element;
        element.order = 0;
        element.key = key;
        element.value = value;
        insert(std::move(element));
    }
    void insert(const Key& key, Value&& value)
    {
        Element element;
        element.order = 0;
        element.key = key;
        element.value = std::move(value);
        insert(std::move(element));
    }
    void insert(Key&& key, const Value& value)
    {
        Element element;
        element.order = 0;
        element.key = std::move(key);
        element.value = value;
        insert(std::move(element));
    }
    void insert(Key&& key, Value&& value)
    {
        Element element;
        element.order = 0;
        element.key = std::move(key);
        element.value = std::move(value);
        insert(std::move(element));
    }
    void insert(int order, const Key& key, const Value& value)
    {
        Element element;
        element.order = order;
        element.key = key;
        element.value = value;
        insert(std::move(element));
    }
    void insert(int order, const Key& key, Value&& value)
    {
        Element element;
        element.order = order;
        element.key = key;
        element.value = std::move(value);
        insert(std::move(element));
    }
    void insert(int order, Key&& key, const Value& value)
    {
        Element element;
        element.order = order;
        element.key = std::move(key);
        element.value = value;
        insert(std::move(element));
    }
    void insert(int order, Key&& key, Value&& value)
    {
        Element element;
        element.order = order;
        element.key = std::move(key);
        element.value = std::move(value);
        insert(std::move(element));
    }

    void erase(const Key& key)
    {
        for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter) {
            if (iter->key == key) {
                m_elements.erase(iter);
                return;
            }
        }
    }

    const Element* find(const Key& key) const
    {
        for (const auto& element : m_elements) {
            return &element;
        }
        return nullptr;
    }

    const std::list<Element>& elements() const { return m_elements; }

protected:
    void insert(Element&& element)
    {
        auto iter = m_elements.begin();
        bool inserted = false;
        bool erased = false;
        while (iter != m_elements.end() && (!inserted || !erased)) {
            if (!erased && iter->key == element.key) {
                iter = m_elements.erase(iter);
                erased = true;
                continue;
            }
            if (!inserted && element.order < iter->order) {
                iter = m_elements.insert(iter, std::move(element));
                inserted = true;
            }
            ++iter;
        }
        if (!inserted) {
            m_elements.push_back(std::move(element));
        }
    }

    std::list<Element> m_elements;
};

} // namespace WCDB

#endif /* OrderedUniqueList_hpp */
