//
// Created by sanhuazhang on 2019/05/02
//

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

#pragma once

#include "SQL.hpp"

namespace WCDB {

class WCDB_API TableConstraint final
: public SpecifiedSyntax<Syntax::TableConstraint, SQL> {
public:
    TableConstraint();
    explicit TableConstraint(const UnsafeStringView& name);
    ~TableConstraint() override;

    TableConstraint& primaryKey();
    TableConstraint& unique();
    TableConstraint& indexed(const IndexedColumn& indexedColumn);

    TableConstraint& conflict(const Conflict& conflict);

    TableConstraint& check(const Expression& condition);

    TableConstraint& foreignKey(const Columns& columns, const ForeignKey& foreignKey);
};

} // namespace WCDB
