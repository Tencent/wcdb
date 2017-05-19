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

#include <sqlite3.h>
#include "recover.hpp"
#include <string>

Recover::Recover(sqlite3* handle)
: m_handle(handle)
{
}

std::shared_ptr<RecoverStatement> Recover::prepare(const std::string& tableName, const size_t& valuesCount)
{
    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO "+tableName+" VALUES("+std::string("?", valuesCount)+")";

    int rc = sqlite3_prepare_v2(m_handle, sql.c_str(), -1, &stmt, nullptr);
    if (rc==SQLITE_OK) {
        std::shared_ptr<RecoverStatement> recoverStatement(new RecoverStatement(stmt));
        if (recoverStatement) {
            return recoverStatement;
        }
        sqlite3_finalize(stmt);
    }
    return nullptr;
}

RecoverStatement::RecoverStatement(sqlite3_stmt* stmt)
: m_stmt(stmt)
{
}
    
bool RecoverStatement::execWithValues(const std::shared_ptr<Values>& values)
{
    for (size_t i = 0; i < values->size(); ++i) {
        sqlite3_reset(m_stmt);
        const Value& value = values->at(i);
        switch (values->at(i).getType()) {
            case ValueType::Integer:
                sqlite3_bind_int64(m_stmt, (int)i, value.getIntegerValue());
                break;
            case ValueType::Float:
                sqlite3_bind_double(m_stmt, (int)i, value.getFloatValue());
                break;
            case ValueType::Text:
                sqlite3_bind_text(m_stmt, (int)i, value.getTextValue(), (int)value.getSize(), SQLITE_STATIC);
                break;
            case ValueType::BLOB:
                sqlite3_bind_blob(m_stmt, (int)i, value.getBLOBValue(), (int)value.getSize(), SQLITE_STATIC);
                break;
            case ValueType::Null:
                sqlite3_bind_null(m_stmt, (int)i);
                break;
            default:
                break;
        }
    }
    return sqlite3_step(m_stmt)==SQLITE_OK;
}

RecoverStatement::~RecoverStatement()
{
    sqlite3_finalize(m_stmt);
}
