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

#ifndef _WCDB_SYNTAXUPDATESTMT_HPP
#define _WCDB_SYNTAXUPDATESTMT_HPP

#include <WCDB/SyntaxIdentifier.hpp>

namespace WCDB {

namespace Syntax {

class UpdateSTMT final : public Identifier {
#pragma mark - Lang
public:
    bool useWithClause = false;
    WithClause withClause;
    bool useConflictAction = false;
    ConflictAction conflictAction;
    QualifiedTableName table;
    std::list<std::list<Column>> columnsList;
    std::list<Expression> expressions;
    bool useCondition = false;
    Expression condition;
    std::list<OrderingTerm> orderingTerms;
    bool useLimit = false;
    Expression limit;
    LimitParameterType limitParameterType = LimitParameterType::NotSet;
    Expression limitParameter;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::UpdateSTMT;
    Type getType() const override final;
    String getDescription() const override final;
    String getDescription(bool skipSchema) const;
    void iterate(const Iterator& iterator, bool& stop) override final;
};

} // namespace Syntax

} // namespace WCDB

#endif /* _WCDB_SYNTAXUPDATESTMT_HPP */
