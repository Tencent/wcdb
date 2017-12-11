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

#ifndef column_def_hpp
#define column_def_hpp

#include <WCDB/column.hpp>
#include <WCDB/column_type.hpp>
#include <WCDB/declare.hpp>
#include <WCDB/describable.hpp>
#include <WCDB/foreign_key.hpp>
#include <WCDB/order_term.hpp>

namespace WCDB {

class ColumnDef : public Describable {
public:
    ColumnDef(const Column &column, ColumnType type);

    ColumnDef &makePrimary(OrderTerm term = OrderTerm::NotSet,
                           bool autoIncrement = false,
                           Conflict conflict = Conflict::NotSet);

    ColumnDef &makeDefault(const Expr &expr);

    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, ColumnDef &>::type
    makeDefault(const T &value)
    {
        m_description.append(" DEFAULT " + std::to_string(value));
        return *this;
    }

    ColumnDef &makeDefault(const char *value);

    ColumnDef &makeDefault(const std::string &value);

    ColumnDef &makeDefault(const std::nullptr_t &value);

    enum class DefaultType {
        CurrentTime,
        CurrentDate,
        CurrentTimestamp,
    };

    ColumnDef &makeDefault(DefaultType defaultType);

    ColumnDef &
    makeDefault(const typename ColumnTypeInfo<ColumnType::BLOB>::CType &value,
                int size);

    ColumnDef &makeNotNull();

    ColumnDef &makeUnique();

    ColumnDef &makeForeignKey(const ForeignKey &foreignKey);
};

} //namespace WCDB

#endif /* column_def_hpp */
