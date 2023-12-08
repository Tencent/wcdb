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

#pragma once

#include "ColumnType.hpp"
#include "StringView.hpp"
#include "UnsafeData.hpp"
#include "WCDBError.hpp"

namespace WCDB {

typedef struct SQLiteContext SQLiteContext;
typedef struct SQLiteValue SQLiteValue;

template<typename ScalarFunctionObject>
class ScalarFunctionTemplate;

class WCDB_API ScalarFunctionAPI {
    template<typename ScalarFunctionObject>
    friend class ScalarFunctionTemplate;

public:
    // Access value
    int getValueCount() const;
    ColumnType getValueType(int index = 0) const;
    int64_t getIntValue(int index = 0) const;
    double getFloatValue(int index = 0) const;
    const UnsafeStringView getTextValue(int index = 0) const;
    const UnsafeData getBlobValue(int index = 0) const;

    // Output result
    void setNullResult();
    void setIntResult(int64_t result);
    void setFloatResult(double result);
    void setTextResult(const UnsafeStringView& result);
    void setBlobResult(const UnsafeData& result);
    void setErrorResult(Error::Code code, const UnsafeStringView& msg);
    void setErrorResult(int code, const UnsafeStringView& msg);

protected:
    ScalarFunctionAPI(SQLiteContext* ctx, SQLiteValue** values, int valueNum);

    void* getUserData() const;

private:
    SQLiteContext* m_sqliteContext;
    SQLiteValue** m_values;
    int m_valueNum;
};

class WCDB_API AbstractScalarFunctionObject {
public:
    AbstractScalarFunctionObject(void* userContext, ScalarFunctionAPI& apiObj);
    virtual ~AbstractScalarFunctionObject() = 0;
    virtual void process(ScalarFunctionAPI& apiObj) = 0;
};

class WCDB_API ScalarFunctionModule final {
public:
    typedef void (*ScalarFunction)(SQLiteContext* pCtx, int nVal, SQLiteValue** apVal);
    ScalarFunctionModule() = delete;
    ScalarFunctionModule(const ScalarFunction& func, int parameterNum, bool deterministic, void* userCtx);
    ScalarFunction getFunc() const;
    int getParameterNum() const;
    bool isDeterminisic() const;
    void* getUserCtx() const;

private:
    ScalarFunction m_func;
    int m_parameterNum;
    bool m_deterministic;
    void* m_userCtx;
};

}; // namespace WCDB
