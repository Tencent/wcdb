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

#import <TestCase/CRUDTestCase.h>
#import <TestCase/TestCaseAssertion.h>

@implementation CRUDTestCase {
    TestCaseObject* _object1;
    TestCaseObject* _object2;
    NSArray<TestCaseObject*>* _objects;
}

- (TestCaseObject*)object1
{
    if (!_object1) {
        TestCaseObject* object = [[TestCaseObject alloc] init];
        object.identifier = 1;
        object.content = self.random.string;
        _object1 = object;
    }
    return _object1;
}

- (TestCaseObject*)object2
{
    if (!_object2) {
        TestCaseObject* object = [[TestCaseObject alloc] init];
        object.identifier = 2;
        object.content = self.random.string;
        _object2 = object;
    }
    return _object2;
}

- (NSArray<TestCaseObject*>*)objects
{
    if (!_objects) {
        _objects = @[ self.object1, self.object2 ];
    }
    return _objects;
}

- (void)insertPresetObjects
{
    TestCaseAssertTrue([self createTable]);
    TestCaseAssertTrue([self.table insertObjects:self.objects]);
    [self.database close];
}

@end
