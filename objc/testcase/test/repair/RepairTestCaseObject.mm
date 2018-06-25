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

#import "RepairTestCaseObject.h"
#import "Convenience.h"
#import "RepairTestCaseObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>
#import <XCTest/XCTest.h>

@implementation RepairTestCaseObject

WCDB_IMPLEMENTATION(RepairTestCaseObject)
WCDB_SYNTHESIZE(RepairTestCaseObject, boolValue)
WCDB_SYNTHESIZE(RepairTestCaseObject, int8Value)
WCDB_SYNTHESIZE(RepairTestCaseObject, int16Value)
WCDB_SYNTHESIZE(RepairTestCaseObject, int32Value)
WCDB_SYNTHESIZE(RepairTestCaseObject, int64Value)
WCDB_SYNTHESIZE(RepairTestCaseObject, doubleValue)
WCDB_SYNTHESIZE(RepairTestCaseObject, stringValue)
WCDB_SYNTHESIZE(RepairTestCaseObject, dataValue)
WCDB_SYNTHESIZE(RepairTestCaseObject, nullValue)

WCDB_PRIMARY_ASC_AUTO_INCREMENT(RepairTestCaseObject, int64Value)
WCDB_NOT_NULL(RepairTestCaseObject, dataValue)
WCDB_NOT_NULL(RepairTestCaseObject, stringValue)
WCDB_INDEX(RepairTestCaseObject, "_index2", int8Value)
WCDB_INDEX(RepairTestCaseObject, "_index2", int16Value)
WCDB_INDEX(RepairTestCaseObject, "_index3", int32Value)
WCDB_INDEX(RepairTestCaseObject, "_index3", stringValue)
WCDB_INDEX(RepairTestCaseObject, "_index3", doubleValue)

+ (RepairTestCaseObject *)randomObject
{
    RepairTestCaseObject *object = [[RepairTestCaseObject alloc] init];
    object.boolValue = [NSNumber randomBool];
    object.int8Value = [NSNumber randomInt8];
    object.int16Value = [NSNumber randomInt16];
    object.int32Value = [NSNumber randomInt32];
    object.int64Value = [NSNumber randomInt64];
    object.doubleValue = [NSNumber randomDouble];
    object.stringValue = [NSString randomString];
    if ([NSNumber randomBool]) {
        object.dataValue = [NSData randomDataWithLength:4096];
    } else {
        object.dataValue = [NSData randomDataWithLength:100];
    }
    object.nullValue = nil;
    return object;
}

+ (RepairTestCaseObject *)maxObject
{
    RepairTestCaseObject *object = [[RepairTestCaseObject alloc] init];
    object.boolValue = std::numeric_limits<bool>::max();
    object.int8Value = std::numeric_limits<int8_t>::max();
    object.int16Value = std::numeric_limits<int16_t>::max();
    object.int32Value = std::numeric_limits<int32_t>::max();
    object.int64Value = std::numeric_limits<int64_t>::max();
    object.doubleValue = std::numeric_limits<double>::max();
    object.stringValue = [NSString randomString];
    object.dataValue = [NSData randomData];
    object.nullValue = nil;
    return object;
}

+ (RepairTestCaseObject *)minObject
{
    RepairTestCaseObject *object = [[RepairTestCaseObject alloc] init];
    object.boolValue = std::numeric_limits<bool>::min();
    object.int8Value = std::numeric_limits<int8_t>::min();
    object.int16Value = std::numeric_limits<int16_t>::min();
    object.int32Value = std::numeric_limits<int32_t>::min();
    object.int64Value = std::numeric_limits<int64_t>::min();
    object.doubleValue = std::numeric_limits<double>::min();
    object.stringValue = [NSString randomString];
    object.dataValue = [NSData randomData];
    object.nullValue = nil;
    return object;
}

+ (RepairTestCaseObject *)zeroObject
{
    RepairTestCaseObject *object = [[RepairTestCaseObject alloc] init];
    object.boolValue = NO;
    object.int8Value = 0;
    object.int16Value = 0;
    object.int32Value = 0;
    object.int64Value = 0;
    object.doubleValue = 0;
    object.stringValue = @"";
    object.dataValue = [@"" dataUsingEncoding:NSASCIIStringEncoding];
    object.nullValue = nil;
    return object;
}

+ (NSArray<RepairTestCaseObject *> *)randomObjects
{
    int count = 0;
    do {
        count = [NSNumber randomUInt8];
    } while (count <= 3);
    return [self randomObjects:count];
}

+ (NSArray<RepairTestCaseObject *> *)randomObjects:(int)count
{
    XCTAssertGreaterThan(count, 3);

    NSMutableDictionary<NSNumber *, RepairTestCaseObject *> *objects = [[NSMutableDictionary<NSNumber *, RepairTestCaseObject *> alloc] initWithCapacity:count];
    int maxPos, minPos, zeroPos;
    do {
        maxPos = [NSNumber randomUInt] % count;
        minPos = [NSNumber randomUInt] % count;
        zeroPos = [NSNumber randomUInt] % count;
    } while (maxPos == minPos || minPos == zeroPos || zeroPos == maxPos);
    RepairTestCaseObject *maxObject = [self maxObject];
    RepairTestCaseObject *minObject = [self minObject];
    RepairTestCaseObject *zeroObject = [self zeroObject];
    for (int i = 0; i < count; ++i) {
        RepairTestCaseObject *object;
        if (i == maxPos) {
            object = maxObject;
        } else if (i == minPos) {
            object = minObject;
        } else if (i == zeroPos) {
            object = zeroObject;
        } else {
            do {
                object = [self randomObject];
            } while (object.int64Value == maxObject.int64Value || object.int64Value == minObject.int64Value || object.int64Value == zeroObject.int64Value || [objects objectForKey:@(object.int64Value)] != nil);
        }
        [objects setObject:object forKey:@(object.int64Value)];
    }
    return objects.allValues;
}

- (BOOL)isEqualToRepairTestCaseObject:(RepairTestCaseObject *)object
{
    if (self.boolValue != object.boolValue) {
        return NO;
    }
    if (self.int8Value != object.int8Value) {
        return NO;
    }
    if (self.int16Value != object.int16Value) {
        return NO;
    }
    if (self.int32Value != object.int32Value) {
        return NO;
    }
    if (self.int64Value != object.int64Value) {
        return NO;
    }
    if (self.doubleValue != object.doubleValue) {
        return NO;
    }
    if (self.nullValue != nil || object.nullValue != nil) {
        return NO;
    }
    if (self.stringValue != nil) {
        if (![object.stringValue isEqualToString:self.stringValue]) {
            return NO;
        }
    } else {
        if (object.stringValue != nil) {
            return NO;
        }
    }
    if (self.dataValue != nil) {
        if (![object.dataValue isEqualToData:self.dataValue]) {
            return NO;
        }
    } else {
        if (object.dataValue != nil) {
            return NO;
        }
    }
    return YES;
}

@end
