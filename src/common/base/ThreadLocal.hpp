//
// Created by sanhuazhang on 2018/09/29
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

#include <atomic>
#include <map>
#include <memory>

namespace WCDB {

template<typename T>
class UntypedThreadLocal {
protected:
    typedef unsigned int Identifier;
    static Identifier nextIdentifier()
    {
        static std::atomic<Identifier>* s_identifier = new std::atomic<Identifier>(0);
        return ++(*s_identifier);
    }
    static std::map<Identifier, T>& threadedStorage()
    {
        thread_local std::unique_ptr<std::map<Identifier, T>> s_storage(
        new std::map<Identifier, T>());
        return *s_storage;
    }
};

template<typename T>
class ThreadLocal : public UntypedThreadLocal<T> {
public:
    using UntypedThreadLocal<T>::nextIdentifier;
    using UntypedThreadLocal<T>::threadedStorage;
    using Identifier = typename UntypedThreadLocal<T>::Identifier;
    ThreadLocal(const typename std::enable_if<std::is_default_constructible<T>::value>::type* = nullptr)
    : m_identifier(nextIdentifier()), m_default()
    {
    }

    ThreadLocal(const T& defaultValue)
    : m_identifier(nextIdentifier()), m_default(defaultValue)
    {
    }

    ThreadLocal(T&& defaultValue)
    : m_identifier(nextIdentifier()), m_default(std::move(defaultValue))
    {
    }

    T& getOrCreate()
    {
        auto& storage = threadedStorage();
        auto iter = storage.find(m_identifier);
        if (iter == storage.end()) {
            iter = storage.emplace(m_identifier, m_default).first;
        }
        return *(&iter->second);
    }

private:
    const Identifier m_identifier;
    const T m_default;
};

} //namespace WCDB
