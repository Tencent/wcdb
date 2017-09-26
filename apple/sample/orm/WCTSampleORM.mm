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

#import "WCTSampleORM.h"
#import "WCTSampleORM+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleORM

WCDB_IMPLEMENTATION(WCTSampleORM)
WCDB_SYNTHESIZE(WCTSampleORM, identifier)
WCDB_SYNTHESIZE_COLUMN(WCTSampleORM, desc, "description") //use "description" as column name in Database
WCDB_SYNTHESIZE_DEFAULT(WCTSampleORM, value, 1.0f)
WCDB_SYNTHESIZE_DEFAULT(WCTSampleORM, timestamp, WCTDefaultTypeCurrentTimestamp)
WCDB_SYNTHESIZE(WCTSampleORM, type)

WCDB_PRIMARY(WCTSampleORM, identifier)

@end
