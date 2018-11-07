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

#ifndef _WCDB_MODULEARGUMENT_HPP
#define _WCDB_MODULEARGUMENT_HPP

#include <WCDB/SQL.hpp>

namespace WCDB {

class ModuleArgument final : public TypedSyntax<Syntax::ModuleArgument, SQL> {
public:
    using TypedSyntax<Syntax::ModuleArgument, SQL>::TypedSyntax;
    ModuleArgument(const TableConstraint& tableConstraint);
    ModuleArgument(const ColumnDef& columnDef);
    ModuleArgument(const Expression& left, const Expression& right);
};

} // namespace WCDB

#endif /* _WCDB_MODULEARGUMENT_HPP */
