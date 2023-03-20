//
// Created by 陈秋文 on 2022/8/13.
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

#import "Random+CPPTestObject.h"
#import <Foundation/Foundation.h>

@implementation Random (CPPTestObject)

- (WCDB::MultiRowsValue)testCaseValuesWithCount:(NSUInteger)count startingFromIdentifier:(int64_t)identifier
{
    WCDB::MultiRowsValue result;
    for (int i = 0; i < count; i++) {
        WCDB::OneRowValue row;
        row.emplace_back(i + identifier);
        row.emplace_back(self.string.UTF8String);
        result.push_back(row);
    }
    return result;
}

- (WCDB::MultiRowsValue)autoIncrementTestCaseValuesWithCount:(NSUInteger)count
{
    WCDB::MultiRowsValue result;
    for (int i = 0; i < count; i++) {
        WCDB::OneRowValue row;
        row.emplace_back(i);
        row.emplace_back(self.string.UTF8String);
        result.push_back(row);
    }
    return result;
}

- (CPPTestCaseObject)testCaseObjectWithIdentifier:(int)identifier
{
    return CPPTestCaseObject(identifier, self.string.UTF8String);
}

- (CPPTestCaseObject)autoIncrementTestCaseObject
{
    return [self autoIncrementTestCaseObjectWithIdentifier:0];
}

- (CPPTestCaseObject)autoIncrementTestCaseObjectWithIdentifier:(int)identifier
{
    CPPTestCaseObject obj = CPPTestCaseObject::autoIncrementObject(self.string.UTF8String);
    obj.identifier = identifier;
    return obj;
}

- (WCDB::ValueArray<CPPTestCaseObject>)testCaseObjectsWithCount:(NSUInteger)count startingFromIdentifier:(int)identifier
{
    WCDB::ValueArray<CPPTestCaseObject> result;
    for (NSUInteger i = identifier; i < identifier + count; ++i) {
        result.push_back([self testCaseObjectWithIdentifier:(int) i]);
    }
    return result;
}

- (WCDB::ValueArray<CPPTestCaseObject>)autoIncrementTestCaseObjectsWithCount:(NSUInteger)count
{
    WCDB::ValueArray<CPPTestCaseObject> result;
    for (NSUInteger i = 0; i < count; ++i) {
        result.push_back([self testCaseObjectWithIdentifier:(int) i]);
    }
    return result;
}

@end
