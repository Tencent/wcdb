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

#include <WCDB/column_type.hpp>
#include <WCDB/describable.hpp>

namespace WCDB {

class LiteralValue : public Describable {
public:
    template <typename T>
    LiteralValue(
        const T &value,
        typename std::enable_if<std::is_arithmetic<T>::value ||
                                std::is_enum<T>::value>::type * = nullptr)
        : Describable(std::to_string(value))
    {
    }

    LiteralValue(const char *value);
    LiteralValue(const std::string &value);

    LiteralValue(const std::nullptr_t &value);

    LiteralValue(const typename ColumnTypeInfo<ColumnType::BLOB>::CType &value,
                 int size);

protected:
    static std::string
    stringByReplacingOccurrencesOfString(const std::string &origin,
                                         const std::string &target,
                                         const std::string &replacement);

    static std::string LiteralString(const std::string &value);
};

} // namespace WCDB

#endif /* literal_value_hpp */
