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

#import "DatabaseTestCase.h"
#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
#import <UIKit/UIKit.h>
#endif

@interface MemoryTests : DatabaseTestCase

@end

@implementation MemoryTests

- (void)test_purge
{
    // acquire handle
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [self.database purge];
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)test_feature_purge_will_not_clear_active_handle
{
    // acquire handle and keep it
    WCTHandle* handle = [self.database getHandle];
    TestCaseAssertTrue([handle execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [self.database purge];
    TestCaseAssertTrue([self.database isOpened]);
}

- (void)test_purge_all
{
    // acquire handle
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [WCTDatabase purgeAll];
    TestCaseAssertFalse([self.database isOpened]);
}

#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
- (void)test_feature_auto_purge_when_memory_low
{
    // acquire handle
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    TestCaseAssertFalse([self.database isOpened]);
}
#endif

@end
