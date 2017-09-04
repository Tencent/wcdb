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
 * Since Apple clang support C++11 thread_local from arm64+iOS 8 and armv7+iOS 9, this class can be a replacement.
 *
 * See also: http://stackoverflow.com/questions/28094794/why-does-apple-clang-disallow-c11-thread-local-when-official-clang-supports
 */

#include <pthread.h>

namespace WCDB {

template <typename T>
class ThreadLocal {
public:
    ThreadLocal() : m_defaultValue(nullptr)
    {
        pthread_key_create(&m_key, [](void *value) { delete (T *) value; });
    }

    ThreadLocal(const T &defaultValue) : m_defaultValue(new T(defaultValue))
    {
        pthread_key_create(&m_key, [](void *value) { delete (T *) value; });
    }

    T *get()
    {
        T *value = (T *) pthread_getspecific(m_key);
        if (value) {
            return value;
        }
        value = new T;
        if (m_defaultValue) {
            *value = *m_defaultValue;
        }
        pthread_setspecific(m_key, value);
        return value;
    }

    ~ThreadLocal()
    {
        if (m_defaultValue) {
            delete m_defaultValue;
            m_defaultValue = nullptr;
        }
        pthread_key_delete(m_key);
    }

protected:
    pthread_key_t m_key;
    T *m_defaultValue;
};

} //namespace WCDB

#endif /* thread_local_hpp */
