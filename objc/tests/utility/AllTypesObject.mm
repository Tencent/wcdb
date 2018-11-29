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

#import "AllTypesObject.h"
#import "AllTypesObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation AllTypesObject

WCDB_IMPLEMENTATION(AllTypesObject)

WCDB_SYNTHESIZE(AllTypesObject, intValue)
WCDB_SYNTHESIZE(AllTypesObject, unsignedIntValue)
WCDB_SYNTHESIZE(AllTypesObject, int32Value)
WCDB_SYNTHESIZE(AllTypesObject, int64Value)
WCDB_SYNTHESIZE(AllTypesObject, uint32Value)
WCDB_SYNTHESIZE(AllTypesObject, uint64Value)
WCDB_SYNTHESIZE(AllTypesObject, integerValue)
WCDB_SYNTHESIZE(AllTypesObject, uintegerValue)

WCDB_SYNTHESIZE(AllTypesObject, floatValue)
WCDB_SYNTHESIZE(AllTypesObject, doubleValue)
WCDB_SYNTHESIZE(AllTypesObject, numberValue)
WCDB_SYNTHESIZE(AllTypesObject, dateValue)

WCDB_SYNTHESIZE(AllTypesObject, cstringValue)
WCDB_SYNTHESIZE(AllTypesObject, cppStringValue)
WCDB_SYNTHESIZE(AllTypesObject, stringValue)

WCDB_SYNTHESIZE(AllTypesObject, dataValue)
WCDB_SYNTHESIZE(AllTypesObject, codingValue)

@end
