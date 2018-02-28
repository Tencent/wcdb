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

#ifndef LangColumnConstraint_hpp
#define LangColumnConstraint_hpp

#include <WCDB/LangCommon.h>

namespace WCDB {

namespace Lang {

class ColumnConstraint : public Lang {
public:
    ColumnConstraint();

    CopyOnWriteString name;
    enum class Type : int {
        NotSet,
        PrimaryKey,
        NotNull,
        Unique,
        Check,
        Default,
        Collate,
        ForeignKeyClause,
    };
    Type type;

    Order order;

    ConflictClause conflictClause;
    bool autoIncrement;

    CopyOnWriteLazyLang<Expr> expr;

    enum class DefaultSwitch {
        NotSet,
        LiteralValue,
        Expr,
    };
    DefaultSwitch defaultSwitcher;
    CopyOnWriteLazyLang<LiteralValue> literalValue;

    CopyOnWriteString collationName;

    CopyOnWriteLazyLang<ForeignKeyClause> foreignKeyClause;

    virtual CopyOnWriteString SQL() const override;
};

template <>
CopyOnWriteString
CopyOnWriteLazyLangList<ColumnConstraint>::calculatedDescription() const;

} // namespace Lang

} // namespace WCDB

#endif /* LangColumnConstraint_hpp */
