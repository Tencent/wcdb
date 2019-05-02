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

#import "Random+TestCaseObject.h"
#import "TestCaseObject+WCTTableCoding.h"

@implementation Random (TestCaseObject)

- (TestCaseObject*)testCaseObjectWithIdentifier:(int)identifier
{
    return [TestCaseObject objectWithIdentifier:identifier andContent:self.string];
}

- (TestCaseObject*)testCaseObjectWithAutoIncrement
{
    TestCaseObject* object = [TestCaseObject objectWithIdentifier:0 andContent:self.string];
    object.isAutoIncrement = YES;
    return object;
}

- (TestCaseObject*)autoIncrementTestCaseObject
{
    return [self autoIncrementTestCaseObjectWithIdentifier:0];
}

- (TestCaseObject*)autoIncrementTestCaseObjectWithIdentifier:(int)identifier
{
    TestCaseObject* object = [TestCaseObject objectWithIdentifier:identifier andContent:self.string];
    object.isAutoIncrement = YES;
    return object;
}

- (NSArray<TestCaseObject*>*)testCaseObjectsWithCount:(NSUInteger)count startingFromIdentifier:(int)identifier
{
    NSMutableArray<TestCaseObject*>* objects = [NSMutableArray arrayWithCapacity:count];
    for (NSUInteger i = identifier; i < identifier + count; ++i) {
        [objects addObject:[self testCaseObjectWithIdentifier:(int) i]];
    }
    return objects;
}

- (NSArray<TestCaseObject*>*)autoIncrementTestCaseObjectsWithCount:(NSUInteger)count
{
    NSMutableArray<TestCaseObject*>* objects = [NSMutableArray arrayWithCapacity:count];
    for (NSUInteger i = 0; i < count; ++i) {
        [objects addObject:[self autoIncrementTestCaseObject]];
    }
    return objects;
}

@end
