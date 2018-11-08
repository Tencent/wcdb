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

#import "TableConstraintObject.h"
#import "TableConstraintObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation TableConstraintObject

WCDB_IMPLEMENTATION(TableConstraintObject)
WCDB_SYNTHESIZE(TableConstraintObject, multiPrimary)
WCDB_SYNTHESIZE(TableConstraintObject, multiPrimaryAsc)
WCDB_SYNTHESIZE(TableConstraintObject, multiPrimaryDesc)

WCDB_SYNTHESIZE(TableConstraintObject, multiUnique)
WCDB_SYNTHESIZE(TableConstraintObject, multiUniqueAsc)
WCDB_SYNTHESIZE(TableConstraintObject, multiUniqueDesc)

WCDB_MULTI_PRIMARY(TableConstraintObject, @"_multi_primary", multiPrimary)
WCDB_MULTI_PRIMARY_ASC(TableConstraintObject, @"_multi_primary", multiPrimaryAsc)
WCDB_MULTI_PRIMARY_DESC(TableConstraintObject, @"_multi_primary", multiPrimaryDesc)

WCDB_MULTI_UNIQUE(TableConstraintObject, @"_multi_unique", multiUnique)
WCDB_MULTI_UNIQUE_ASC(TableConstraintObject, @"_multi_unique", multiUniqueAsc)
WCDB_MULTI_UNIQUE_DESC(TableConstraintObject, @"_multi_unique", multiUniqueDesc)

@end
