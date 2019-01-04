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
WCDB_SYNTHESIZE(IndexObject, index_)
WCDB_SYNTHESIZE(IndexObject, indexAsc)
WCDB_SYNTHESIZE(IndexObject, indexDesc)

WCDB_SYNTHESIZE(IndexObject, uniqueIndex)
WCDB_SYNTHESIZE(IndexObject, uniqueIndexAsc)
WCDB_SYNTHESIZE(IndexObject, uniqueIndexDesc)

WCDB_SYNTHESIZE(IndexObject, multiIndex)
WCDB_SYNTHESIZE(IndexObject, multiIndexAsc)
WCDB_SYNTHESIZE(IndexObject, multiIndexDesc)

WCDB_INDEX(IndexObject, @"_index", index_);
WCDB_INDEX(IndexObject, @"_index_asc", indexAsc);
WCDB_INDEX(IndexObject, @"_index_desc", indexDesc);

WCDB_UNIQUE_INDEX(IndexObject, @"_unique_index", uniqueIndex);
WCDB_UNIQUE_INDEX(IndexObject, @"_unique_index_asc", uniqueIndexAsc);
WCDB_UNIQUE_INDEX(IndexObject, @"_unique_index_desc", uniqueIndexDesc);

WCDB_INDEX(IndexObject, @"_multi_index", multiIndex);
WCDB_INDEX(IndexObject, @"_multi_index", multiIndexAsc);
WCDB_INDEX(IndexObject, @"_multi_index", multiIndexDesc);

@end
