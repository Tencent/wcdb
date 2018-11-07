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

#ifndef _WCDB_THREADLOCAL_HPP
#define _WCDB_THREADLOCAL_HPP

/*
 * Thread-local storage
 *
 * Since Apple clang support C++11 thread_local from arm64+iOS 8 and armv7+iOS 9, this class can be a replacement.
 *
 * See also: http://stackoverflow.com/questions/28094794/why-does-apple-clang-disallow-c11-thread-local-when-official-clang-supports
 */

#include <WCDB/Recyclable.hpp>
#include <map>
#include <memory>
#include <pthread.h>

namespace WCDB {

class UntypedThreadLocal {
#pragma mark - Declare
private:
    class Info;
    typedef unsigned int Identifier;
    typedef std::map<Identifier, Info> Infos;
    typedef std::function<std::shared_ptr<void>(void)> Constructor;
#pragma mark - Threaded Info
private:
    class Info final {
    public:
        Info(const Recyclable<pthread_key_t> &key, std::shared_ptr<void> &&value);

        void *get();

    protected:
        Recyclable<pthread_key_t> m_key;
        std::shared_ptr<void> m_value;
    };
#pragma mark - UntypedThreadLocal
public:
    UntypedThreadLocal();
    virtual ~UntypedThreadLocal();
    void *getOrCreate();

protected:
    virtual std::shared_ptr<void> constructor() = 0;

private:
    Recyclable<pthread_key_t> m_key;
    Identifier m_identifier;
#pragma mark - Shared
private:
    static Identifier nextIdentifier();
    static const Recyclable<pthread_key_t> &sharedKey();
#pragma mark - Helper
private:
    static void threadDeconstructor(void *p);
    static pthread_key_t constructKey();
    static void deconstructKey(pthread_key_t &key);
};

template<typename T>
class ThreadLocal final : public UntypedThreadLocal {
public:
    ThreadLocal(const typename std::enable_if<std::is_default_constructible<T>::value>::type * = nullptr)
    : UntypedThreadLocal()
    {
    }

    ThreadLocal(const T &defaultValue)
    : UntypedThreadLocal(), m_defaultValue(defaultValue)
    {
    }

    T *getOrCreate() { return (T *) UntypedThreadLocal::getOrCreate(); }

    std::shared_ptr<void> constructor() override final
    {
        return std::shared_ptr<void>(new T(m_defaultValue));
    }

protected:
    T m_defaultValue;
};

} //namespace WCDB

#endif /* _WCDB_THREADLOCAL_HPP */
