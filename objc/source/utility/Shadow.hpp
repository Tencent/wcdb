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

#ifndef Shadow_hpp
#define Shadow_hpp

#include <memory>

namespace WCDB {

template<typename RealType>
class Shadow;

template<typename BaseType>
class Shadow : public std::unique_ptr<BaseType> {
public:
    using std::unique_ptr<BaseType>::unique_ptr;

    Shadow(BaseType* value) : std::unique_ptr<BaseType>(value) {}

    Shadow(const BaseType& value)
    : std::unique_ptr<BaseType>(new BaseType(value))
    {
    }

    Shadow& operator=(const BaseType& value)
    {
        this->reset(new BaseType(value));
        return *this;
    }

    Shadow(const Shadow<BaseType>& shadow)
    : std::unique_ptr<BaseType>(shadow.get() != nullptr ? new BaseType(*shadow.get()) : nullptr)
    {
    }

    Shadow& operator=(const Shadow<BaseType>& shadow)
    {
        this->reset(shadow.get() != nullptr ? new BaseType(*shadow.get()) : nullptr);
        return *this;
    }

    constexpr BaseType* operator->() const { return this->get(); }
};

} // namespace WCDB

#endif /* Shadow_hpp */
