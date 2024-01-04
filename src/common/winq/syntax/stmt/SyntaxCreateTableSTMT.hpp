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

#include "SyntaxColumnDef.hpp"
#include "SyntaxIdentifier.hpp"
#include "SyntaxSchema.hpp"
#include "SyntaxSelectSTMT.hpp"
#include "SyntaxTableConstraint.hpp"

namespace WCDB {

namespace Syntax {

class WCDB_API CreateTableSTMT final : public Identifier {
#pragma mark - Lang
public:
    ~CreateTableSTMT() override;

    bool temp = false;
    bool ifNotExists = false;
    Schema schema;
    StringView table;
    WCDB_SYNTAX_MAIN_UNION_ENUM(ColumnDefs, Select, );
    std::list<ColumnDef> columnDefs;
    std::list<TableConstraint> tableConstraints;
    bool withoutRowid = false;

    Optional<SelectSTMT> select;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::CreateTableSTMT;
    Type getType() const override final;
    bool describle(std::ostream& stream) const override final;
    void iterate(const Iterator& iterator, bool& stop) override final;
};

} // namespace Syntax

} // namespace WCDB
