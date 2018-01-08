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

#include <WCDB/database.hpp>
#include <WCDB/error.hpp>

namespace WCDB {

RecyclableStatement Database::prepare(const Statement &statement, Error &error)
{
    if (statement.getStatementType() == Statement::Type::Transaction) {
        Error::ReportCore(getTag(), getPath(), Error::CoreOperation::Prepare,
                          Error::CoreCode::Misuse,
                          "Using [getTransaction] method to do a transaction",
                          &error);
        return RecyclableStatement(RecyclableHandle(nullptr, nullptr), nullptr);
    }
    RecyclableHandle handle = flowOut(error);
    return CoreBase::prepare(handle, statement, error);
}

bool Database::exec(const Statement &statement, Error &error)
{
    if (statement.getStatementType() == Statement::Type::Transaction) {
        Error::ReportCore(getTag(), getPath(), Error::CoreOperation::Exec,
                          Error::CoreCode::Misuse,
                          "Using [getTransaction] method to do a transaction",
                          &error);
        return false;
    }
    RecyclableHandle handle = flowOut(error);
    return CoreBase::exec(handle, statement, error);
}

bool Database::isTableExists(const std::string &tableName, Error &error)
{
    RecyclableHandle handle = flowOut(error);
    return CoreBase::isTableExists(handle, tableName, error);
}

} //namespace WCDB
