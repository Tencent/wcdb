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

#include <WCDB/WINQ.h>

namespace WCDB {

ForeignKeyClause::ForeignKeyClause(const std::string &foreignTable)
{
    lang::ForeignKeyClause &lang = getMutableLang();
    lang.foreignTable.assign(foreignTable);
}

ForeignKeyClause &ForeignKeyClause::byAddingColumn(const Column &column)
{
    lang::ForeignKeyClause &lang = getMutableLang();
    lang.columns.append(column.getLang());
    return *this;
}

ForeignKeyClause &
ForeignKeyClause::byAddingColumns(const std::list<Column> &columns)
{
    lang::ForeignKeyClause &lang = getMutableLang();
    for (const Column &column : columns) {
        lang.columns.append(column.getLang());
    }
    return *this;
}

ForeignKeyClause &ForeignKeyClause::onDeleteSetNull()
{
    addTriggerForDeleting(lang::ForeignKeyClause::Trigger::Operation::SetNull);
    return *this;
}
ForeignKeyClause &ForeignKeyClause::onDeleteSetDefault()
{
    addTriggerForDeleting(
        lang::ForeignKeyClause::Trigger::Operation::SetDefault);
    return *this;
}
ForeignKeyClause &ForeignKeyClause::onDeleteCascade()
{
    addTriggerForDeleting(lang::ForeignKeyClause::Trigger::Operation::Cascade);
    return *this;
}
ForeignKeyClause &ForeignKeyClause::onDeleteRestrict()
{
    addTriggerForDeleting(lang::ForeignKeyClause::Trigger::Operation::Restrict);
    return *this;
}
ForeignKeyClause &ForeignKeyClause::onDeleteNoAction()
{
    addTriggerForDeleting(lang::ForeignKeyClause::Trigger::Operation::NoAction);
    return *this;
}

ForeignKeyClause &ForeignKeyClause::onUpdateSetNull()
{
    addTriggerForUpdating(lang::ForeignKeyClause::Trigger::Operation::SetNull);
    return *this;
}
ForeignKeyClause &ForeignKeyClause::onUpdateSetDefault()
{
    addTriggerForUpdating(
        lang::ForeignKeyClause::Trigger::Operation::SetDefault);
    return *this;
}
ForeignKeyClause &ForeignKeyClause::onUpdateCascade()
{
    addTriggerForUpdating(lang::ForeignKeyClause::Trigger::Operation::Cascade);
    return *this;
}
ForeignKeyClause &ForeignKeyClause::onUpdateRestrict()
{
    addTriggerForUpdating(lang::ForeignKeyClause::Trigger::Operation::Restrict);
    return *this;
}
ForeignKeyClause &ForeignKeyClause::onUpdateNoAction()
{
    addTriggerForUpdating(lang::ForeignKeyClause::Trigger::Operation::NoAction);
    return *this;
}

ForeignKeyClause &ForeignKeyClause::byMatching(const std::string &name)
{
    addTriggerForMatching(name);
    return *this;
}

ForeignKeyClause &ForeignKeyClause::deferrable()
{
    lang::ForeignKeyClause &lang = getMutableLang();
    lang.doDeferrable = true;
    lang.notDeferrable = false;
    return *this;
}

ForeignKeyClause &ForeignKeyClause::notDeferrable()
{
    lang::ForeignKeyClause &lang = getMutableLang();
    lang.doDeferrable = true;
    lang.notDeferrable = true;
    return *this;
}

ForeignKeyClause &ForeignKeyClause::initiallyImmediate()
{
    lang::ForeignKeyClause &lang = getMutableLang();
    lang.initiallySwitcher = lang::ForeignKeyClause::InitiallySwitch::Immediate;
    return *this;
}

ForeignKeyClause &ForeignKeyClause::initiallyDeferred()
{
    lang::ForeignKeyClause &lang = getMutableLang();
    lang.initiallySwitcher = lang::ForeignKeyClause::InitiallySwitch::Deferred;
    return *this;
}

void ForeignKeyClause::addTriggerForUpdating(
    const lang::ForeignKeyClause::Trigger::Operation &operation)
{
    lang::ForeignKeyClause::Trigger trigger;
    trigger.match = false;
    trigger.update = true;
    trigger.operation = operation;
    addTrigger(trigger);
}

void ForeignKeyClause::addTriggerForDeleting(
    const lang::ForeignKeyClause::Trigger::Operation &operation)
{
    lang::ForeignKeyClause::Trigger trigger;
    trigger.match = false;
    trigger.update = false;
    trigger.operation = operation;
    addTrigger(trigger);
}

void ForeignKeyClause::addTriggerForMatching(const std::string &name)
{
    lang::ForeignKeyClause::Trigger trigger;
    trigger.match = true;
    trigger.name.assign(name);
    addTrigger(trigger);
}

void ForeignKeyClause::addTrigger(
    const lang::ForeignKeyClause::Trigger &trigger)
{
    lang::copy_on_write_lazy_lang<lang::ForeignKeyClause::Trigger> cowTrigger;
    cowTrigger.assign(trigger);
    getMutableLang().triggers.append(cowTrigger);
}

} // namespace WCDB
