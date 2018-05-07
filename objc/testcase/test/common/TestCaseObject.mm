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

#import "TestCaseObject.h"
#import "Convenience.h"
#import "TestCaseObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation TestCaseObject

WCDB_IMPLEMENTATION(TestCaseObject)
WCDB_SYNTHESIZE(TestCaseObject, variable1)
WCDB_SYNTHESIZE(TestCaseObject, variable2)
WCDB_SYNTHESIZE(TestCaseObject, variable3)

WCDB_PRIMARY_ASC_AUTO_INCREMENT(TestCaseObject, variable1)

+ (instancetype)objectWithId:(int)i
{
    TestCaseObject *object = [[TestCaseObject alloc] init];
    object.variable1 = i;

    object.variable2 = [NSString randomString];
    object.variable3 = (float) rand() / (float) (RAND_MAX / 100);
    return object;
}

+ (NSArray<TestCaseObject *> *)objectsWithCount:(int)count
{
    return [self objectsWithCount:count from:0];
}

+ (NSArray<TestCaseObject *> *)objectsWithCount:(int)count from:(int)from
{
    NSMutableArray<TestCaseObject *> *objects = [[NSMutableArray<TestCaseObject *> alloc] init];
    for (int i = 0; i < count; ++i) {
        [objects addObject:[self objectWithId:i + from]];
    }
    return objects;
}

+ (WCTColumnCodingRow *)rowWithObject:(TestCaseObject *)object
                         onProperties:(int)properties
{
    assert([object isKindOfClass:self.class]);
    NSMutableArray *row = [NSMutableArray array];
    if (properties & TestCaseObjectPropertiesVariable1) {
        [row addObject:[[NSNumber alloc] initWithInt:object.variable1]];
    }
    if (properties & TestCaseObjectPropertiesVariable2) {
        [row addObject:object.variable2];
    }
    if (properties & TestCaseObjectPropertiesVariable3) {
        [row addObject:[[NSNumber alloc] initWithFloat:object.variable3]];
    }
    return row;
}

- (BOOL)isEqualToObject:(TestCaseObject *)object
{
    if (![object isKindOfClass:self.class]) {
        return NO;
    }
    return self.variable1 == object.variable1 && [self.variable2 isEqualToString:object.variable2] && self.variable3 == object.variable3;
}

- (NSUInteger)hash
{
    return [NSString stringWithFormat:@"%@_%d_%@_%f", NSStringFromClass(self.class), self.variable1, self.variable2, self.variable3].hash;
}

- (BOOL)isEqualToObject:(TestCaseObject *)object
           onProperties:(int)properties
{
    if (![object isKindOfClass:self.class]) {
        return NO;
    }
    if (properties & TestCaseObjectPropertiesVariable1 && self.variable1 != object.variable1) {
        return NO;
    }
    if (properties & TestCaseObjectPropertiesVariable2 && ![self.variable2 isEqualToString:object.variable2]) {
        return NO;
    }
    if (properties & TestCaseObjectPropertiesVariable3 && self.variable3 != object.variable3) {
        return NO;
    }
    return YES;
}

@end

@implementation NSArray (TestCaseObject)

- (BOOL)isEqualToTestCaseObjects:(NSArray<NSObject<TestCaseObjectProtocol> *> *)objects
{
    return [self isEqualToObjects:objects
                   withComparator:^BOOL(TestCaseObject *lhs, TestCaseObject *rhs) {
                     return [lhs isEqualToObject:rhs];
                   }];
}

- (BOOL)isEqualToTestCaseObjects:(NSArray<NSObject<TestCaseObjectProtocol> *> *)objects
                    onProperties:(int)properties
{
    return [self isEqualToObjects:objects
                   withComparator:^BOOL(NSObject<TestCaseObjectProtocol> *lhs, NSObject<TestCaseObjectProtocol> *rhs) {
                     return [lhs isEqualToObject:rhs onProperties:properties];
                   }];
}

@end

@implementation NSArray (Row)

+ (NSComparator)ValueComparator
{
    return ^NSComparisonResult(WCTValue *lhs, WCTValue *rhs) {
      if (lhs.valueType != rhs.valueType) {
          if (lhs.valueType > rhs.valueType) {
              return NSOrderedDescending;
          } else {
              return NSOrderedAscending;
          }
      }
      switch (lhs.valueType) {
          case WCTColumnTypeNull:
              return NSOrderedSame;
          case WCTColumnTypeInteger32:
          case WCTColumnTypeInteger64:
          case WCTColumnTypeFloat:
              return [lhs.numberValue compare:rhs.numberValue];
          case WCTColumnTypeString:
              return [lhs.stringValue compare:rhs.stringValue];
          default:
              assert(false);
              return NSOrderedSame;
      }
    };
}

@end
