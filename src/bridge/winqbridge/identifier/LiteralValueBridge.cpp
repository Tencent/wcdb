//
// Created by qiuwenchen on 2022/5/26.
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

#include "LiteralValueBridge.h"
#include "LiteralValue.hpp"
#include "ObjectBridge.hpp"
#include <cstdint>

CPPLiteralValue WCDBLiteralValueCreate(CPPCommonValue value)
{
    switch (value.type) {
    case WCDBBridgedType_Null:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPLiteralValue, WCDB::LiteralValue, nullptr);
    case WCDBBridgedType_Bool:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPLiteralValue, WCDB::LiteralValue, (bool) value.intValue);
    case WCDBBridgedType_Int:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPLiteralValue, WCDB::LiteralValue, (int64_t) value.intValue);
    case WCDBBridgedType_UInt:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPLiteralValue, WCDB::LiteralValue, (uint64_t) value.intValue);
    case WCDBBridgedType_Double:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPLiteralValue, WCDB::LiteralValue, value.doubleValue);
    case WCDBBridgedType_String:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPLiteralValue,
        WCDB::LiteralValue,
        WCDB::StringView((const char*) value.intValue));
    default:
        assert(0);
    }
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPLiteralValue, WCDB::LiteralValue, nullptr);
}

CPPLiteralValue WCDBLiteralValueCreateWithInt64(long long value)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPLiteralValue, WCDB::LiteralValue, (int64_t) value);
}

CPPLiteralValue WCDBLiteralValueCreateWithUInt64(unsigned long long value)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPLiteralValue, WCDB::LiteralValue, (uint64_t) value);
}

CPPLiteralValue WCDBLiteralValueCreateWithDouble(double value)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPLiteralValue, WCDB::LiteralValue, value);
}

CPPLiteralValue WCDBLiteralValueCreateWithNull()
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPLiteralValue, WCDB::LiteralValue, nullptr);
}

CPPLiteralValue WCDBLiteralValueCreateWithBool(bool value)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPLiteralValue, WCDB::LiteralValue, value);
}

CPPLiteralValue WCDBLiteralValueCreateWithString(const char* _Nullable value)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPLiteralValue, WCDB::LiteralValue, WCDB::StringView(value));
}

CPPLiteralValue WCDBLiteralValueCreateWithCurrentTime()
{
    return WCDBCreateCPPBridgedObjectByCopy(CPPLiteralValue,
                                            WCDB::LiteralValue::currentTime());
}

CPPLiteralValue WCDBLiteralValueCreateWithCurrentDate()
{
    return WCDBCreateCPPBridgedObjectByCopy(CPPLiteralValue,
                                            WCDB::LiteralValue::currentDate());
}

CPPLiteralValue WCDBLiteralValueCreateWithCurrentTimestamp()
{
    return WCDBCreateCPPBridgedObjectByCopy(
    CPPLiteralValue, WCDB::LiteralValue::currentTimestamp());
}
