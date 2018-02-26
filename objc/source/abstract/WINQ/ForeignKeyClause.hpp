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

#ifndef ForeignKeyClause_hpp
#define ForeignKeyClause_hpp

#include <WCDB/Describable.hpp>

namespace WCDB {

class ForeignKeyClause : public DescribableWithLang<lang::ForeignKeyClause> {
public:
    ForeignKeyClause(const std::string &foreignTable);

    ForeignKeyClause &byAddingColumn(const Column &column);

    ForeignKeyClause &byAddingColumns(const std::list<Column> &columns);

    ForeignKeyClause &onDeleteSetNull();
    ForeignKeyClause &onDeleteSetDefault();
    ForeignKeyClause &onDeleteCascade();
    ForeignKeyClause &onDeleteRestrict();
    ForeignKeyClause &onDeleteNoAction();

    ForeignKeyClause &onUpdateSetNull();
    ForeignKeyClause &onUpdateSetDefault();
    ForeignKeyClause &onUpdateCascade();
    ForeignKeyClause &onUpdateRestrict();
    ForeignKeyClause &onUpdateNoAction();

    ForeignKeyClause &byMatching(const std::string &name);

    ForeignKeyClause &deferrable();
    ForeignKeyClause &notDeferrable();
    ForeignKeyClause &initiallyImmediate();
    ForeignKeyClause &initiallyDeferred();

private:
    void addTriggerForUpdating(
        const lang::ForeignKeyClause::Trigger::Operation &operation);
    void addTriggerForDeleting(
        const lang::ForeignKeyClause::Trigger::Operation &operation);
    void addTriggerForMatching(const std::string &name);
    void addTrigger(const lang::ForeignKeyClause::Trigger &trigger);
};

} // namespace WCDB

#endif /* ForeignKeyClause_hpp */
