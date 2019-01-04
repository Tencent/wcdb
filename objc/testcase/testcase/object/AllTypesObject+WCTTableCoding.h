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
#import <WCDB/WCDB.h>

@interface AllTypesObject (WCTTableCoding) <WCTTableCoding>

WCDB_PROPERTY(type)

WCDB_PROPERTY(intValue)
WCDB_PROPERTY(unsignedIntValue)
WCDB_PROPERTY(int32Value)
WCDB_PROPERTY(int64Value)
WCDB_PROPERTY(uint32Value)
WCDB_PROPERTY(uint64Value)
WCDB_PROPERTY(integerValue)
WCDB_PROPERTY(uintegerValue)

WCDB_PROPERTY(floatValue)
WCDB_PROPERTY(doubleValue)
WCDB_PROPERTY(numberValue)
WCDB_PROPERTY(dateValue)

WCDB_PROPERTY(stringValue)

WCDB_PROPERTY(dataValue)
WCDB_PROPERTY(codingValue)

@end
