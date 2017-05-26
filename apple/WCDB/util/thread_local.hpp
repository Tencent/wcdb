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

#ifndef thread_local_hpp
#define thread_local_hpp

/*
 * Thread-local storage
 *
 * Since Apple clang disallow C++11 thread_local in iOS 7 or lower, this class can be a replacement.
 * Remove it while deploy target is greater than iOS 7
 *
 * See also: http://stackoverflow.com/questions/28094794/why-does-apple-clang-disallow-c11-thread-local-when-official-clang-supports
 */

#include <pthread.h>

namespace WCDB {

template <typename T>
class ThreadLocal
{
public:
    typedef std::function<T*(void)> Constructor;
    typedef std::function<void(T*)> Destructor;
    ThreadLocal(const Constructor& constructor, Destructor destructor)
    : construct(constructor)
    {
        pthread_key_create(&m_key, destructor.template target<void(void*)>());
    }

    T* get()
    {
        T* value = (T*)pthread_getspecific(m_key);
        if (value) {
            return value;
        }
        value = construct();
        pthread_setspecific(m_key, value);
        return value;
    }

    ~ThreadLocal()
    {
        pthread_key_delete(m_key);
    }
protected:
    const Constructor construct;
    pthread_key_t m_key;
};

}//namespace WCDB

#endif /* thread_local_hpp */
