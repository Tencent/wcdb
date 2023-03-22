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

CPPLiteralValue WCDBLiteralValueCreateWithInt64(long long value)
{
    return WCDBCreateCPPBridgedObject(CPPLiteralValue, new WCDB::LiteralValue((int64_t)value));
}

CPPLiteralValue WCDBLiteralValueCreateWithUInt64(unsigned long long value)
{
    return WCDBCreateCPPBridgedObject(CPPLiteralValue, new WCDB::LiteralValue((uint64_t)value));
}

CPPLiteralValue WCDBLiteralValueCreateWithDouble(double value)
{
    return WCDBCreateCPPBridgedObject(CPPLiteralValue, new WCDB::LiteralValue(value));
}

CPPLiteralValue WCDBLiteralValueCreateWithNull()
{
    return WCDBCreateCPPBridgedObject(CPPLiteralValue, new WCDB::LiteralValue(nullptr));
}

CPPLiteralValue WCDBLiteralValueCreateWithBool(bool value)
{
    return WCDBCreateCPPBridgedObject(CPPLiteralValue, new WCDB::LiteralValue(value));
}

CPPLiteralValue WCDBLiteralValueCreateWithString(const char* _Nullable value)
{
    return WCDBCreateCPPBridgedObject(
    CPPLiteralValue, new WCDB::LiteralValue(WCDB::StringView(value)));
}

CPPLiteralValue WCDBLiteralValueCreateWithCurrentTime()
{
    return WCDBCreateCPPBridgedObject(CPPLiteralValue, WCDB::LiteralValue::currentTime());
}

CPPLiteralValue WCDBLiteralValueCreateWithCurrentDate()
{
    return WCDBCreateCPPBridgedObject(CPPLiteralValue, WCDB::LiteralValue::currentDate());
}

CPPLiteralValue WCDBLiteralValueCreateWithCurrentTimestamp()
{
    return WCDBCreateCPPBridgedObject(CPPLiteralValue,
                                      WCDB::LiteralValue::currentTimestamp());
}
