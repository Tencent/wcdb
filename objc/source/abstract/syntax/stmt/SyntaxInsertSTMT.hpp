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

#ifndef SyntaxInsertSTMT_hpp
#define SyntaxInsertSTMT_hpp

#include <WCDB/SyntaxIdentifier.hpp>

namespace WCDB {

namespace Syntax {

class InsertSTMT : public Identifier {
#pragma mark - Lang
public:
    bool useWithClause = false;
    WithClause withClause;
    enum class Switch {
        Insert,
        InsertOrReplace,
        InsertOrRollback,
        InsertOrAbort,
        InsertOrFail,
        InsertOrIgnore,
    } switcher
    = Switch::Insert;
    Schema schema;
    String table;
    String alias;
    std::list<Column> columns;

    enum class SwitchValue {
        Values,
        Select,
        Default,
    } valueSwitcher;
    std::list<std::list<Expression>> expressionsValues;
    SelectSTMT select;

    bool useUpsertClause = false;
    UpsertClause upsertClause;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::InsertSTMT;
    Type getType() const override final;
    String getDescription() const override final;
    void iterate(const Iterator& iterator, void* parameter) override final;
};

} // namespace Syntax

} // namespace WCDB

#endif /* SyntaxInsertSTMT_hpp */
