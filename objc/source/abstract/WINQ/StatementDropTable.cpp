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

StatementDropTable::StatementDropTable()
{
    getMutableLang().ifExists = true;
}

StatementDropTable &StatementDropTable::dropTable(const std::string &tableName)
{
    Lang::DropTableSTMT &lang = getMutableLang();
    lang.name.assign(tableName);
    return *this;
}

StatementDropTable &StatementDropTable::ifExists(bool ifExists)
{
    Lang::DropTableSTMT &lang = getMutableLang();
    lang.ifExists = ifExists;
    return *this;
}

StatementDropTable &
StatementDropTable::withSchema(const std::string &schemaName)
{
    Lang::DropTableSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

} // namespace WCDB
