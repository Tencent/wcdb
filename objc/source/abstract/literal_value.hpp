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

#ifndef literal_value_hpp
#define literal_value_hpp

#include <WCDB/convertible.hpp>
#include <WCDB/declare.hpp>

namespace WCDB {

class LiteralValue : public Describable {
public:
    template <typename T>
    LiteralValue(const T &value,
                 typename std::enable_if<ColumnIsNullType<T>::value, void>::type
                     * = nullptr)
        : Describable("NULL")
    {
    }

    template <typename T>
    LiteralValue(const T &value,
                 typename std::enable_if<ColumnIsFloatType<T>::value,
                                         void>::type * = nullptr)
        : Describable(
              std::to_string(ColumnIsFloatType<T>::asUnderlyingType(value)))
    {
    }

    template <typename T>
    LiteralValue(const T &value,
                 typename std::enable_if<ColumnIsInteger32Type<T>::value,
                                         void>::type * = nullptr)
        : Describable(
              std::to_string(ColumnIsInteger32Type<T>::asUnderlyingType(value)))
    {
    }

    template <typename T>
    LiteralValue(const T &value,
                 typename std::enable_if<ColumnIsInteger64Type<T>::value,
                                         void>::type * = nullptr)
        : Describable(
              std::to_string(ColumnIsInteger64Type<T>::asUnderlyingType(value)))
    {
    }

    template <typename T>
    LiteralValue(const T &value,
                 typename std::enable_if<ColumnIsTextType<T>::value, void>::type
                     * = nullptr)
        : Describable(LiteralValue::stringByAntiInjecting(
              ColumnIsTextType<T>::asUnderlyingType(value)))
    {
    }

    template <typename T>
    LiteralValue(const T &value,
                 typename std::enable_if<ColumnIsBLOBType<T>::value, void>::type
                     * = nullptr)
        : Describable(LiteralValue::stringByAntiInjecting(
              ColumnIsBLOBType<T>::asUnderlyingType(value)))
    {
    }

protected:
    static std::string stringByAntiInjecting(const char *origin);
    static std::string
    stringByAntiInjecting(const std::vector<unsigned char> &origin);
};

} // namespace WCDB

#endif /* literal_value_hpp */
