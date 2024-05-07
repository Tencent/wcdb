//
// Created by 陈秋文 on 2023/11/25.
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

#include "ScalarFunctionModule.hpp"
#include "Assertion.hpp"
#include "SQLite.h"

namespace WCDB {

// Access value
ScalarFunctionAPI::ScalarFunctionAPI(SQLiteContext *ctx, SQLiteValue **values, int valueNum)
: m_sqliteContext(ctx), m_values(values), m_valueNum(valueNum)
{
}

int ScalarFunctionAPI::getValueCount() const
{
    return m_valueNum;
}

ColumnType ScalarFunctionAPI::getValueType(int index) const
{
    WCTAssert(index < m_valueNum);
    if (index < m_valueNum) {
        int type = sqlite3_value_type((sqlite3_value *) m_values[index]);
        switch (type) {
        case SQLITE_INTEGER:
            return ColumnType::Integer;
        case SQLITE_FLOAT:
            return ColumnType::Float;
        case SQLITE_BLOB:
            return ColumnType::BLOB;
        case SQLITE_TEXT:
            return ColumnType::Text;
        default:
            return ColumnType::Null;
        }
    } else {
        return ColumnType::Null;
    }
}

int64_t ScalarFunctionAPI::getIntValue(int index) const
{
    WCTAssert(index < m_valueNum);
    if (index < m_valueNum) {
        return sqlite3_value_int64((sqlite3_value *) m_values[index]);
    } else {
        return 0;
    }
}

double ScalarFunctionAPI::getFloatValue(int index) const
{
    WCTAssert(index < m_valueNum);
    if (index < m_valueNum) {
        return sqlite3_value_double((sqlite3_value *) m_values[index]);
    } else {
        return 0;
    }
}

const UnsafeStringView ScalarFunctionAPI::getTextValue(int index) const
{
    WCTAssert(index < m_valueNum);
    if (index < m_valueNum) {
        return UnsafeStringView(
        (const char *) sqlite3_value_text((sqlite3_value *) m_values[index]),
        sqlite3_value_bytes((sqlite3_value *) m_values[index]));
    } else {
        return UnsafeStringView();
    }
}

const UnsafeData ScalarFunctionAPI::getBlobValue(int index) const
{
    WCTAssert(index < m_valueNum);
    if (index < m_valueNum) {
        return UnsafeData(
        (unsigned char *) sqlite3_value_blob((sqlite3_value *) m_values[index]),
        sqlite3_value_bytes((sqlite3_value *) m_values[index]));
    } else {
        return UnsafeData();
    }
}

// Output result
void ScalarFunctionAPI::setNullResult()
{
    if (!m_sqliteContext) {
        return;
    }
    sqlite3_result_null((sqlite3_context *) m_sqliteContext);
}

void ScalarFunctionAPI::setFloatResult(double result)
{
    if (!m_sqliteContext) {
        return;
    }
    sqlite3_result_double((sqlite3_context *) m_sqliteContext, result);
}

void ScalarFunctionAPI::setIntResult(int64_t result)
{
    if (!m_sqliteContext) {
        return;
    }
    sqlite3_result_int64((sqlite3_context *) m_sqliteContext, result);
}

void ScalarFunctionAPI::setTextResult(const UnsafeStringView &result)
{
    if (!m_sqliteContext) {
        return;
    }
    sqlite3_result_text(
    (sqlite3_context *) m_sqliteContext, result.data(), (int) result.length(), SQLITE_TRANSIENT);
}

void ScalarFunctionAPI::setBlobResult(const UnsafeData &data)
{
    if (!m_sqliteContext) {
        return;
    }
    sqlite3_result_blob(
    (sqlite3_context *) m_sqliteContext, data.buffer(), (int) data.size(), SQLITE_TRANSIENT);
}

void ScalarFunctionAPI::setErrorResult(Error::Code code, const UnsafeStringView &msg)
{
    if (!m_sqliteContext) {
        return;
    }
    sqlite3_result_error(
    (sqlite3_context *) m_sqliteContext, msg.data(), (int) msg.length());
    sqlite3_result_error_code((sqlite3_context *) m_sqliteContext, (int) code);
}

void ScalarFunctionAPI::setErrorResult(int code, const UnsafeStringView &msg)
{
    if (!m_sqliteContext) {
        return;
    }
    sqlite3_result_error(
    (sqlite3_context *) m_sqliteContext, msg.data(), (int) msg.length());
    sqlite3_result_error_code((sqlite3_context *) m_sqliteContext, code);
}

void *ScalarFunctionAPI::getUserData() const
{
    if (!m_sqliteContext) {
        return nullptr;
    }
    return sqlite3_user_data((sqlite3_context *) m_sqliteContext);
}

AbstractScalarFunctionObject::AbstractScalarFunctionObject(void *userContext,
                                                           ScalarFunctionAPI &apiObj)
{
    WCDB_UNUSED(userContext);
    WCDB_UNUSED(apiObj);
}

AbstractScalarFunctionObject::~AbstractScalarFunctionObject() = default;

ScalarFunctionModule::ScalarFunctionModule(const ScalarFunction &func,
                                           int parameterNum,
                                           bool deterministic,
                                           void *userCtx)
: m_func(func), m_parameterNum(parameterNum), m_deterministic(deterministic), m_userCtx(userCtx)
{
}

ScalarFunctionModule::ScalarFunction ScalarFunctionModule::getFunc() const
{
    return m_func;
}

void *ScalarFunctionModule::getUserCtx() const
{
    return m_userCtx;
}

int ScalarFunctionModule::getParameterNum() const
{
    return m_parameterNum;
}

bool ScalarFunctionModule::isDeterminisic() const
{
    return m_deterministic;
}

} // namespace WCDB
