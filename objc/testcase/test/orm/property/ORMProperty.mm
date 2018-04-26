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

#import "ORMProperty.h"
#import "ORMProperty+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation ORMProperty

WCDB_IMPLEMENTATION(ORMProperty)
WCDB_SYNTHESIZE(ORMProperty, property1)
WCDB_SYNTHESIZE_COLUMN(ORMProperty, property2, "renamedProperty2")
WCDB_SYNTHESIZE_DEFAULT(ORMProperty, property3, "defaultString")
WCDB_SYNTHESIZE_COLUMN_DEFAULT(ORMProperty, property4, "renamedProperty4", 1.1)

WCDB_PRIMARY(ORMProperty, property1)

@end
