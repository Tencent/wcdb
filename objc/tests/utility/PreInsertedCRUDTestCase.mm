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

#import "PreInsertedCRUDTestCase.h"

@implementation PreInsertedCRUDTestCase

- (void)setUp
{
    [super setUp];

    self.tableClass = TestCaseObject.class;
    TestCaseAssertTrue([self createTable]);

    _object1 = [[TestCaseObject alloc] init];
    _object1.identifier = 1;
    _object1.content = [NSString randomString];

    _object2 = [[TestCaseObject alloc] init];
    _object2.identifier = 2;
    _object2.content = [NSString randomString];

    _objects = @[ _object1, _object2 ];

    TestCaseAssertTrue([self.table insertObjects:_objects]);

    [self.database close];
}

- (void)tearDown
{
    TestCaseAssertTrue([self dropTable]);
    [super tearDown];
}

@end
