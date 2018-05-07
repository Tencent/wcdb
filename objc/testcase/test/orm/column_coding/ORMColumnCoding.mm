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

#import "ORMColumnCoding.h"
#import "Convenience.h"
#import "ORMColumnCoding+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation ORMColumnCoding

WCDB_IMPLEMENTATION(ORMColumnCoding)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, int32Value, 0)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, int64Value, 0)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, uint32Value, 0)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, uint64Value, 0)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, floatValue, 0)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, doubleValue, 0)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, stringValue, nil)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, numberValue, nil)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, dateValue, nil)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, dataValue, nil)
WCDB_SYNTHESIZE_DEFAULT(ORMColumnCoding, arrayValue, nil)

- (BOOL)isEqualToObject:(ORMColumnCoding *)object
{
    if (![object isKindOfClass:self.class]) {
        return NO;
    }
    if (self.int32Value != object.int32Value) {
        return NO;
    }
    if (self.int64Value != object.int64Value) {
        return NO;
    }
    if (self.uint32Value != object.uint32Value) {
        return NO;
    }
    if (self.uint64Value != object.uint64Value) {
        return NO;
    }
    if (self.floatValue != object.floatValue) {
        return NO;
    }
    if (self.numberValue.doubleValue != object.numberValue.doubleValue) {
        return NO;
    }
    if (self.stringValue != nil || object.stringValue != nil) {
        if (![self.stringValue isEqualToString:object.stringValue]) {
            return NO;
        }
    }
    if (self.numberValue != nil || object.numberValue != nil) {
        if (![self.numberValue isEqualToNumber:object.numberValue]) {
            return NO;
        }
    }
    if (self.dataValue != nil || object.dataValue != nil) {
        if (![self.dataValue isEqualToData:object.dataValue]) {
            return NO;
        }
    }
    if (self.dateValue != nil || object.dateValue != nil) {
        if (self.dateValue.timeIntervalSince1970 != object.dateValue.timeIntervalSince1970) {
            return NO;
        }
    }
    if (self.arrayValue != nil || object.arrayValue != nil) {
        if (![self.arrayValue isEqualToArray:object.arrayValue]) {
            return NO;
        }
    }
    return YES;
}

@end
