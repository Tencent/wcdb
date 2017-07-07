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

#include <WCDB/foreign_key.hpp>

namespace WCDB {

ForeignKey::ForeignKey(const std::string &foreignTableName,
                       const std::list<const std::string> &columnNames)
    : Describable("REFERENCES " + foreignTableName)
{
    if (!columnNames.empty()) {
        m_description.append(" (");
        joinDescribableList(columnNames);
        m_description.append(")");
    }
}

std::string ForeignKey::actionName(Action action)
{
    switch (action) {
        case Action::SetNull:
            return "SET NULL";
            break;
        case Action::SetDefault:
            return "SET DEFAULT";
            break;
        case Action::Cascade:
            return "CASCADE";
            break;
        case Action::Restrict:
            return "RESTRICT";
            break;
        case Action::NoAction:
            return "NO ACTION";
            break;
    }
}

ForeignKey &ForeignKey::onDelete(Action action)
{
    m_description.append(" ON DELETE " + actionName(action));
    return *this;
}

ForeignKey &ForeignKey::onUpdate(Action action)
{
    m_description.append(" ON UPDATE " + actionName(action));
    return *this;
}

ForeignKey &ForeignKey::match(const std::string &name)
{
    m_description.append(" MATCH " + name);
    return *this;
}

std::string ForeignKey::deferrableName(Deferrable deferrable)
{
    switch (deferrable) {
        case Deferrable::Deferred:
            return "INITIALLY DEFERRED";
            break;
        case Deferrable::Immediate:
            return "INITIALLY IMMEDIATE";
            break;
        case Deferrable::None:
            return "";
            break;
    }
}

ForeignKey &ForeignKey::deferrable(Deferrable deferrable)
{
    m_description.append(" DEFERRABLE " + deferrableName(deferrable));
    return *this;
}

ForeignKey &ForeignKey::notDeferrable(Deferrable deferrable)
{
    m_description.append(" NOT DEFERRABLE " + deferrableName(deferrable));
    return *this;
}

} // namespace WCDB
