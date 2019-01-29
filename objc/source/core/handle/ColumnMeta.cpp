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

#include <WCDB/ColumnMeta.hpp>

namespace WCDB {

ColumnMeta::ColumnMeta(int id_, String name_, String type_, bool notnull_, int primary_)
: id(id_), name(name_), type(type_), notnull(notnull_), primary(primary_)
{
}

int ColumnMeta::getIndexOfIntegerPrimary(const std::vector<ColumnMeta>& columnMetas)
{
    bool multiPrimary = false;
    int indexOfIntegerPrimary = -1;
    for (const ColumnMeta& columnMeta : columnMetas) {
        int pk = columnMeta.primary;
        if (pk > 0) {
            if (pk > 1) {
                return -1;
            } else if (Syntax::isIntegerType(columnMeta.type)) {
                indexOfIntegerPrimary = (int) columnMetas.size() - 1;
            }
        }
    }
    return indexOfIntegerPrimary;
}

} // namespace WCDB
