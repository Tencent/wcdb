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
#import <WCDB/WCDB.h>

@implementation ColumnCodingObject

WCDB_IMPLEMENTATION(ColumnCodingObject)
WCDB_SYNTHESIZE(ColumnCodingObject, int32Object)
WCDB_SYNTHESIZE(ColumnCodingObject, int64Object)
WCDB_SYNTHESIZE(ColumnCodingObject, doubleObject)
WCDB_SYNTHESIZE(ColumnCodingObject, stringObject)
WCDB_SYNTHESIZE(ColumnCodingObject, dataObject)

- (BOOL)isEqual:(NSObject*)object
{
    if (object.class != self.class) {
        return NO;
    }
    ColumnCodingObject* other = (ColumnCodingObject*) object;

    if (self.int32Object != nil) {
        if (![other.int32Object isEqual:other.int32Object]) {
            return NO;
        }
    } else {
        if (other.int32Object != nil) {
            return NO;
        }
    }

    if (self.int64Object != nil) {
        if (![other.int64Object isEqual:other.int64Object]) {
            return NO;
        }
    } else {
        if (other.int64Object != nil) {
            return NO;
        }
    }

    if (self.doubleObject != nil) {
        if (![other.doubleObject isEqual:other.doubleObject]) {
            return NO;
        }
    } else {
        if (other.doubleObject != nil) {
            return NO;
        }
    }

    if (self.stringObject != nil) {
        if (![other.stringObject isEqual:other.stringObject]) {
            return NO;
        }
    } else {
        if (other.stringObject != nil) {
            return NO;
        }
    }

    if (self.dataObject != nil) {
        if (![other.dataObject isEqual:other.dataObject]) {
            return NO;
        }
    } else {
        if (other.dataObject != nil) {
            return NO;
        }
    }

    return YES;
}

@end
