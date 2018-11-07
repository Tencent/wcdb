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

#import "BuiltinTypesObject.h"
#import "BuiltinTypesObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation BuiltinTypesObject

WCDB_IMPLEMENTATION(BuiltinTypesObject)

WCDB_SYNTHESIZE(BuiltinTypesObject, intValue)
WCDB_SYNTHESIZE(BuiltinTypesObject, unsignedIntValue)
WCDB_SYNTHESIZE(BuiltinTypesObject, int32Value)
WCDB_SYNTHESIZE(BuiltinTypesObject, int64Value)
WCDB_SYNTHESIZE(BuiltinTypesObject, uint32Value)
WCDB_SYNTHESIZE(BuiltinTypesObject, uint64Value)
WCDB_SYNTHESIZE(BuiltinTypesObject, integerValue)
WCDB_SYNTHESIZE(BuiltinTypesObject, uintegerValue)

WCDB_SYNTHESIZE(BuiltinTypesObject, floatValue)
WCDB_SYNTHESIZE(BuiltinTypesObject, doubleValue)
WCDB_SYNTHESIZE(BuiltinTypesObject, numberValue)
WCDB_SYNTHESIZE(BuiltinTypesObject, dateValue)

WCDB_SYNTHESIZE(BuiltinTypesObject, cstringValue)
WCDB_SYNTHESIZE(BuiltinTypesObject, cppStringValue)
WCDB_SYNTHESIZE(BuiltinTypesObject, stringValue)

WCDB_SYNTHESIZE(BuiltinTypesObject, dataValue)
WCDB_SYNTHESIZE(BuiltinTypesObject, codingValue)

@end
