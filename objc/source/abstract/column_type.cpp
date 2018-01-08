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

#include <WCDB/column_type.hpp>

namespace WCDB {

const char *ColumnTypeName(ColumnType type)
{
    switch (type) {
        case ColumnType::Integer32:
            return ColumnTypeInfo<ColumnType::Integer32>::name;
        case ColumnType::Integer64:
            return ColumnTypeInfo<ColumnType::Integer64>::name;
        case ColumnType::Float:
            return ColumnTypeInfo<ColumnType::Float>::name;
        case ColumnType::Text:
            return ColumnTypeInfo<ColumnType::Text>::name;
        case ColumnType::BLOB:
            return ColumnTypeInfo<ColumnType::BLOB>::name;
        default:
            break;
    }
    return ColumnTypeInfo<ColumnType::Null>::name;
}

} //namespace WCDB
