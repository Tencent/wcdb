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

#import "WCTSampleORMTableConstraint.h"
#import "WCTSampleORMTableConstraint+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleORMTableConstraint

WCDB_IMPLEMENTATION(WCTSampleORMTableConstraint)
WCDB_SYNTHESIZE(WCTSampleORMTableConstraint, primaryKeyPart1)
WCDB_SYNTHESIZE(WCTSampleORMTableConstraint, primaryKeyPart2)
WCDB_SYNTHESIZE(WCTSampleORMTableConstraint, uniqueKeyPart1)
WCDB_SYNTHESIZE(WCTSampleORMTableConstraint, uniqueKeyPart2)

//Use the same constraint name to combine multiple properties into a multi primary key constraint
WCDB_MULTI_PRIMARY_DESC(WCTSampleORMTableConstraint, "MultiPrimaryConstraint", primaryKeyPart1)
WCDB_MULTI_PRIMARY(WCTSampleORMTableConstraint, "MultiPrimaryConstraint", primaryKeyPart2)

//Use the same constraint name to combine multiple properties into a multi unique constraint
WCDB_MULTI_UNIQUE(WCTSampleORMTableConstraint, "MultiUniqueConstraint", uniqueKeyPart1)
WCDB_MULTI_UNIQUE_ASC(WCTSampleORMTableConstraint, "MultiUniqueConstraint", uniqueKeyPart2)

@end
