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

#import "WCTSampleORMIndex.h"
#import "WCTSampleORMIndex+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleORMIndex

WCDB_IMPLEMENTATION(WCTSampleORMIndex)
WCDB_SYNTHESIZE(WCTSampleORMIndex, indexProperty)
WCDB_SYNTHESIZE(WCTSampleORMIndex, uniqueIndexProperty)
WCDB_SYNTHESIZE(WCTSampleORMIndex, multiIndexPart1)
WCDB_SYNTHESIZE(WCTSampleORMIndex, multiIndexPart2)

//Index Name = Table Name + Index Subfix Name
WCDB_INDEX(WCTSampleORMIndex, "_indexSubfix", indexProperty)

WCDB_UNIQUE_INDEX(WCTSampleORMIndex, "_uniqueIndexSubfix", uniqueIndexProperty)

//Use the same index subfix name to combine multiple properties into a multi indexes constraint
WCDB_INDEX(WCTSampleORMIndex, "_multiIndexSubfix", multiIndexPart1)
WCDB_INDEX(WCTSampleORMIndex, "_multiIndexSubfix", multiIndexPart2)

@end
