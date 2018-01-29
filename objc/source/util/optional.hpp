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

#ifndef optional_hpp
#define optional_hpp

#include <memory>

namespace WCDB {

//Simple optional class

template <typename T>
class Optional {
    static_assert(!std::is_same<T, std::nullptr_t>::value,
                  "Optional std::nullptr_t is not permitted");
    static_assert(!std::is_same<T, Optional>::value,
                  "Optional Optional is not permitted");

protected:
    class OptionalStorage {
    public:
        OptionalStorage(const T &value);
        T value;
    };

public:
    Optional(const T &value);
    Optional();

    operator bool() const;

    bool isValid() const;

    const T &operator*() const;

    T &operator*();

    const T &value() const;

    T &value();

protected:
    std::shared_ptr<OptionalStorage> m_storage;
};

} //namespace WCDB

#endif /* optional_hpp */
