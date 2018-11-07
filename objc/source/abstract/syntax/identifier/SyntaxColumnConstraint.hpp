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

#ifndef _WCDB_SYNTAXCOLUMNCONSTRAINT_HPP
#define _WCDB_SYNTAXCOLUMNCONSTRAINT_HPP

#include <WCDB/SyntaxExpression.hpp>
#include <WCDB/SyntaxForeignKeyClause.hpp>
#include <WCDB/SyntaxIdentifier.hpp>
#include <WCDB/SyntaxLiteralValue.hpp>

namespace WCDB {

namespace Syntax {

class ColumnConstraint final : public Identifier {
#pragma mark - Lang
public:
    String name;
    enum class Switch {
        PrimaryKey,
        NotNull,
        Unique,
        Check,
        Default,
        Collate,
        ForeignKey,
    } switcher;

    bool useOrder = false;
    Order order;
    bool useConflict = false;
    Conflict conflict;
    bool autoIncrement = false;
    Expression expression;
    String collation;
    ForeignKeyClause foreignKeyClause;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::ColumnConstraint;
    Type getType() const override final;
    String getDescription() const override final;
    void iterate(const Iterator& iterator, void* parameter) override final;
};

} // namespace Syntax

} // namespace WCDB

#endif /* _WCDB_SYNTAXCOLUMNCONSTRAINT_HPP */
