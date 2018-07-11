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

#ifndef ThreadLocal_hpp
#define ThreadLocal_hpp

/*
 * Thread-local storage
 *
 * Since Apple clang support C++11 thread_local from arm64+iOS 8 and armv7+iOS 9, this class can be a replacement.
 *
 * See also: http://stackoverflow.com/questions/28094794/why-does-apple-clang-disallow-c11-thread-local-when-official-clang-supports
 */

#include <WCDB/Recyclable.hpp>
#include <map>
#include <pthread.h>

#pragma GCC visibility push(hidden)

namespace WCDB {

class UntypedThreadLocal {
#pragma mark - Declare
private:
    class Info;
    typedef unsigned int Identifier;
    typedef std::map<Identifier, Info> Infos;
#pragma mark - Threaded Info
private:
    class Info {
    public:
        typedef std::function<void *(void)> Constructor;
        typedef std::function<void(void *)> Deconstructor;

        Info(const Recyclable<pthread_key_t> &key,
             const Constructor &constructor,
             const Deconstructor &deconstructor);
        ~Info();

        void *getOrCreate();

    protected:
        const Constructor m_constructor;
        const Deconstructor m_deconstructor;
        void *m_holder;
        Recyclable<pthread_key_t> m_key;
    };
#pragma mark - UntypedThreadLocal
public:
    UntypedThreadLocal();
    void *getOrCreate();

protected:
    virtual void *constructor() = 0;
    virtual void deconstructor(void *) = 0;

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
class ThreadLocal : public UntypedThreadLocal {
public:
    ThreadLocal() : UntypedThreadLocal() {}

    ThreadLocal(const T &defaultValue)
    : UntypedThreadLocal(), m_defaultValue(defaultValue)
    {
    }

    T *getOrCreate() { return (T *) UntypedThreadLocal::getOrCreate(); }

    void *constructor() override
    {
        T *value = new T;
        return value;
    }

    void deconstructor(void *value) override { delete (T *) value; }

protected:
    T m_defaultValue;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* ThreadLocal_hpp */
