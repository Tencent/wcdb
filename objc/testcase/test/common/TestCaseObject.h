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

#import "Convenience.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@protocol TestCaseObjectProtocol

+ (instancetype)objectWithId:(int)i;

+ (NSArray *)objectsWithCount:(int)count;

+ (NSArray *)objectsWithCount:(int)count from:(int)from;

+ (WCTColumnCodingRow *)rowWithObject:(NSObject<TestCaseObjectProtocol> *)object
                         onProperties:(int)properties;

- (BOOL)isEqualToObject:(NSObject<TestCaseObjectProtocol> *)object;

- (BOOL)isEqualToObject:(NSObject<TestCaseObjectProtocol> *)object
           onProperties:(int)properties;

@end

typedef NS_OPTIONS(NSUInteger, TestCaseObjectProperties) {
    TestCaseObjectPropertiesVariable1 = 1 << 0,
    TestCaseObjectPropertiesVariable2 = 1 << 1,
    TestCaseObjectPropertiesVariable3 = 1 << 2,
};

@interface TestCaseObject : NSObject <TestCaseObjectProtocol>

@property(nonatomic, assign) int variable1;
@property(nonatomic, retain) NSString *variable2;
@property(nonatomic, assign) float variable3;

@end

@interface NSArray (TestCaseObject)

- (BOOL)isEqualToTestCaseObjects:(NSArray *)objects;

- (BOOL)isEqualToTestCaseObjects:(NSArray *)objects
                    onProperties:(int)properties;

@end

@interface NSArray (Row)

+ (NSComparator)ValueComparator;

@end
