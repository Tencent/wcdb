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

#include <WCDB/Assertion.hpp>
#include <WCDB/MigrationInitializerHandle.hpp>

namespace WCDB {

const Error& MigrationInitializerHandle::getError() const
{
    return Handle::getError();
}

std::pair<bool, std::set<std::string>> MigrationInitializerHandle::getAllExistingTables()
{
    WCTInnerAssert(!isPrepared());
    WCTInnerAssert(!isInTransaction());

    if (!lazyOpen()) {
        return { false, {} };
    }

    return getUnorderedValues(
    StatementSelect().select(Column("table")).from("sqlite_master").where(Column("type") == "table"),
    0);
}

std::pair<bool, std::set<std::string>>
MigrationInitializerHandle::getAllColumns(const std::string& table, const std::string& database)
{
    WCTInnerAssert(!isPrepared());
    WCTInnerAssert(!isInTransaction());

    WCTInnerAssert(!table.empty());

    if (!lazyOpen()) {
        return { false, {} };
    }

    std::string schema = MigrationInfo::getSchemaForDatabase(database);
    if (!database.empty()) {
        markAsIgnorable(SQLITE_ERROR);
        // Adoptable for duplicate attach
        if (!execute(StatementAttach().attach(database).as(schema))
            && getResultCode() != SQLITE_ERROR) {
            return { false, {} };
        }
        markAsUnignorable();
    }

    bool succeed, exists;
    std::tie(succeed, exists) = tableExists(TableOrSubquery(table).withSchema(schema));
    if (!exists) {
        // return {true, {}} if the schema.table does not exist.
        return { succeed, {} };
    }
    return getUnorderedColumnsWithTable(table, schema);
}

void MigrationInitializerHandle::setError(const Error& error)
{
    Handle::ErrorProne::setError(error);
}

bool MigrationInitializerHandle::lazyOpen()
{
    if (isOpened()) {
        return true;
    }
    return open();
}
} // namespace WCDB
