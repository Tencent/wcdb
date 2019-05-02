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

#import "NewlyCreatedTableIndexObject.h"
#import "NewlyCreatedTableIndexObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation NewlyCreatedTableIndexObject

WCDB_IMPLEMENTATION(NewlyCreatedTableIndexObject)

// bind renamedValue to the old column "value"
WCDB_SYNTHESIZE_COLUMN(renamedValue, "value")
WCDB_SYNTHESIZE(newValue)

// index will not be created for non-newly-created-table
WCDB_INDEX_FOR_NEWLY_CREATED_TABLE_ONLY("_index")
WCDB_INDEX(@"_index", renamedValue)

@end
