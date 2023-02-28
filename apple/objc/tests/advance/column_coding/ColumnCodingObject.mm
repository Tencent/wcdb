//
// Created by sanhuazhang on 2019/05/02
//

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

#import "ColumnCodingObject.h"
#import "ColumnCodingObject+WCTTableCoding.h"
#import "TestCase.h"
#import <WCDB/WCDB.h>

@implementation ColumnCodingObject

WCDB_IMPLEMENTATION(ColumnCodingObject)
WCDB_SYNTHESIZE(integerObject)
WCDB_SYNTHESIZE(doubleObject)
WCDB_SYNTHESIZE(stringObject)
WCDB_SYNTHESIZE(dataObject)

- (BOOL)isEqual:(NSObject*)object
{
    if (object.class != self.class) {
        return NO;
    }
    ColumnCodingObject* other = (ColumnCodingObject*) object;

    return [NSObject isObject:self.integerObject nilEqualToObject:other.integerObject]
           && [NSObject isObject:self.doubleObject nilEqualToObject:other.doubleObject]
           && [NSObject isObject:self.stringObject nilEqualToObject:other.stringObject]
           && [NSObject isObject:self.dataObject nilEqualToObject:other.dataObject];
}

@end
