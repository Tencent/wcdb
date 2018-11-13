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

#import "SingleDatabaseTestCase.h"

@interface DatabaseTests : SingleDatabaseTestCase

@end

@implementation DatabaseTests

- (void)test_tag
{
    TestCaseAssertNotEqual(self.database.tag, WCTInvalidTag);
    WCTDatabase* another = [[WCTDatabase alloc] initWithPath:self.path];
    TestCaseAssertEqual(self.database.tag, another.tag);
}

- (void)test_path
{
    TestCaseAssertTrue([self.database.path isEqualToString:self.path]);
}

- (void)test_existing
{
    WCTTag tag = self.database.tag;
    {
        WCTDatabase* existing = [[WCTDatabase alloc] initWithExistingPath:self.path];
        TestCaseAssertTrue(existing != nil);
        TestCaseAssertEqual(tag, existing.tag);
        [existing invalidate];
    }
    {
        WCTDatabase* existing = [[WCTDatabase alloc] initWithExistingTag:self.database.tag];
        TestCaseAssertTrue(existing != nil);
        TestCaseAssertEqual(tag, existing.tag);
        [existing invalidate];
    }
    [self.database invalidate];
    {
        WCTDatabase* existing = [[WCTDatabase alloc] initWithExistingPath:self.path];
        TestCaseAssertTrue(existing == nil);
    }
    {
        WCTDatabase* existing = [[WCTDatabase alloc] initWithExistingTag:tag];
        TestCaseAssertTrue(existing == nil);
    }
}

- (void)test_open
{
    TestCaseAssertFalse([self.database isOpened]);
    TestCaseAssertTrue([self.database canOpen]);
    TestCaseAssertTrue([self.database isOpened]);
}

@end
