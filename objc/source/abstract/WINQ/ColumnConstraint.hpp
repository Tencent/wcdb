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

#ifndef ColumnConstraint_hpp
#define ColumnConstraint_hpp

#include <WCDB/Describable.hpp>

class ColumnConstraint : public DescribableWithLang<LangColumnConstraint> {
public:
    ColumnConstraint();
    ColumnConstraint(const std::string &name);

    ColumnConstraint &named(const std::string &name);

    ColumnConstraint &withPrimaryKey(
        const Order &order = Order::NotSet,
        const ConflictClause &conflictClause = ConflictClause::NotSet,
        bool autoIncrement = false);

    ColumnConstraint &
    withNotNull(const ConflictClause &conflictClause = ConflictClause::NotSet);

    ColumnConstraint &
    withUnique(const ConflictClause &conflictClause = ConflictClause::NotSet);

    ColumnConstraint &withChecking(const Expression &expression);

    ColumnConstraint &withDefault(const LiteralValue &literalValue);
    ColumnConstraint &withDefault(const Expression &expression);

    ColumnConstraint &withCollate(const std::string &collationName);

    ColumnConstraint &
    withForeignKeyClause(const ForeignKeyClause &foreignKeyClause);
};

#endif /* ColumnConstraint_hpp */
