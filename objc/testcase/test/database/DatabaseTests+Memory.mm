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

#import "TestCaseCommon.h"
#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
#import <UIKit/UIKit.h>
#endif // TARGET_OS_IPHONE && !TARGET_OS_WATCH

@interface DatabaseTests_Memory : TestCase

@end

@implementation DatabaseTests_Memory

- (void)test_purge
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    XCTAssertTrue([database canOpen]);
    XCTAssertTrue([database isOpened]);
    [database purge];
    XCTAssertFalse([database isOpened]);

    WCTHandle *handle = [database getHandle];
    [handle execute:WCDB::StatementBegin::deferred()];
    XCTAssertNotNil(handle);
    [database purge];
    XCTAssertTrue([database isOpened]);
    [handle finalizeHandle];
}

- (void)test_purge_all
{
    WCTDatabase *database1 = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:@"1"]];
    XCTAssertTrue([database1 canOpen]);
    XCTAssertTrue([database1 isOpened]);
    WCTDatabase *database2 = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:@"2"]];
    XCTAssertTrue([database2 canOpen]);
    XCTAssertTrue([database2 isOpened]);

    [WCTDatabase PurgeInAllDatabases];
    XCTAssertFalse([database1 isOpened]);
    XCTAssertFalse([database2 isOpened]);

    WCTHandle *handle = [database1 getHandle];
    [handle execute:WCDB::StatementBegin::deferred()];
    XCTAssertTrue([database2 canOpen]);
    [WCTDatabase PurgeInAllDatabases];
    XCTAssertTrue([database1 isOpened]);
    XCTAssertFalse([database2 isOpened]);
    [handle finalizeHandle];
}

#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
- (void)test_low_memory_purge
{
    NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];

    WCTDatabase *database1 = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:@"1"]];
    XCTAssertTrue([database1 canOpen]);
    XCTAssertTrue([database1 isOpened]);
    WCTDatabase *database2 = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:@"2"]];
    XCTAssertTrue([database2 canOpen]);
    XCTAssertTrue([database2 isOpened]);

    [notificationCenter postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    XCTAssertFalse([database1 isOpened]);
    XCTAssertFalse([database2 isOpened]);

    WCTHandle *handle = [database1 getHandle];
    [handle execute:WCDB::StatementBegin::deferred()];
    XCTAssertTrue([database2 canOpen]);
    [notificationCenter postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    XCTAssertTrue([database1 isOpened]);
    XCTAssertFalse([database2 isOpened]);
    [handle finalizeHandle];
}
#endif // TARGET_OS_IPHONE && !TARGET_OS_WATCH

@end
