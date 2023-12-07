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

#include "SyntaxIdentifier.hpp"

namespace WCDB {

namespace Syntax {

class WCDB_API ForeignKeyClause final : public Identifier {
#pragma mark - Lang
public:
    ~ForeignKeyClause() override final;

    StringView foreignTable;
    std::list<Column> columns;
    enum class Switch : signed char {
        OnDeleteSetNull = 1,
        OnDeleteSetDefault,
        OnDeleteCascade,
        OnDeleteRestrict,
        OnDeleteNoAction,
        OnUpdateSetNull,
        OnUpdateSetDefault,
        OnUpdateCascade,
        OnUpdateRestrict,
        OnUpdateNoAction,
    };
    std::list<Switch> switchers;
    WCDB_SYNTAX_ENUM_UNION(MatchType, matchType);

    WCDB_SYNTAX_UNION_ENUM(Deferrable,
                           deferrable,
                           DeferrableInitiallyDeferred,
                           DeferrableInitiallyImmediate,
                           Deferrable,
                           NotDeferrableInitiallyDeferred,
                           NotDeferrableInitiallyImmediate,
                           NotDeferrable, );

    bool isValid() const override final;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::ForeignKeyClause;
    Type getType() const override final;
    bool describle(std::ostream& stream) const override final;
    void iterate(const Iterator& iterator, bool& stop) override final;
};

} // namespace Syntax

} // namespace WCDB
