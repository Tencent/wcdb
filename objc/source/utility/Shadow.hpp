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

#ifndef _WCDB_SHADOW_HPP
#define _WCDB_SHADOW_HPP

#include <memory>

namespace WCDB {

template<typename T>
class Shadow;

template<typename T>
class Cloneable {
public:
    virtual ~Cloneable() {}
    virtual T* clone() const = 0;
};

template<typename T>
class Shadow final : public std::unique_ptr<T> {
public:
    //    static_assert(std::is_base_of<Cloneable<T>, T>::value, "");
    using std::unique_ptr<T>::unique_ptr;

    Shadow(const T& value) { value.clone(*this); }

    Shadow& operator=(const T& value)
    {
        this->reset(static_cast<T*>(value.clone()));
        return *this;
    }

    Shadow(const Shadow<T>& other)
    {
        if (other.get() != nullptr) {
            this->reset(static_cast<T*>(other->clone()));
        } else {
            this->reset(nullptr);
        }
    }

    Shadow& operator=(const Shadow<T>& other)
    {
        if (other.get() != nullptr) {
            this->reset(static_cast<T*>(other->clone()));
        } else {
            this->reset(nullptr);
        }
        return *this;
    }

    constexpr T* operator->() const { return this->get(); }
};

} // namespace WCDB

#endif /* _WCDB_SHADOW_HPP */
