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

#import "IndexObject.h"
#import "IndexObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation IndexObject

WCDB_IMPLEMENTATION(IndexObject)
WCDB_SYNTHESIZE(index_)
WCDB_SYNTHESIZE(indexAsc)
WCDB_SYNTHESIZE(indexDesc)

WCDB_SYNTHESIZE(uniqueIndex)
WCDB_SYNTHESIZE(uniqueIndexAsc)
WCDB_SYNTHESIZE(uniqueIndexDesc)

WCDB_SYNTHESIZE(multiIndex)
WCDB_SYNTHESIZE(multiIndexAsc)
WCDB_SYNTHESIZE(multiIndexDesc)

WCDB_INDEX(@"_index", index_);
WCDB_INDEX(@"_index_asc", indexAsc);
WCDB_INDEX(@"_index_desc", indexDesc);

WCDB_UNIQUE_INDEX(@"_unique_index", uniqueIndex);
WCDB_UNIQUE_INDEX(@"_unique_index_asc", uniqueIndexAsc);
WCDB_UNIQUE_INDEX(@"_unique_index_desc", uniqueIndexDesc);

WCDB_INDEX(@"_multi_index", multiIndex);
WCDB_INDEX(@"_multi_index", multiIndexAsc);
WCDB_INDEX(@"_multi_index", multiIndexDesc);

@end
