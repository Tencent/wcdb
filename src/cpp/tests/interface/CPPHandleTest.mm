//
// Created by 陈秋文 on 2023/8/13.
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

#import "CPPTestCase.h"
#import <Foundation/Foundation.h>

@interface CPPHandleTests : CPPTableTestCase

@end

@implementation CPPHandleTests

- (void)test_cancellation_signal
{
    TestCaseAssertTrue([self createValueTable]);
    WCDB::MultiRowsValue rows = [Random.shared testCaseValuesWithCount:10000 startingFromIdentifier:0];
    TestCaseAssertTrue(self.database->insertRows(rows, self.columns, self.tableName.UTF8String));
    bool hasTestInterrupt = false;
    self.database->traceError([&](const WCDB::Error &error) {
        if (error.level != WCDB::Error::Level::Error) {
            return;
        }
        XCTAssertTrue(error.code() == WCDB::Error::Code::Interrupt);
        hasTestInterrupt = true;
    });
    WCDB::Handle::CancellationSignal signal;
    [self.dispatch async:^{
        WCDB::Handle handle = self.database->getHandle();
        handle.attachCancellationSignal(signal);
        auto allRows = handle.selectAllRow(self.columns, self.tableName.UTF8String);
        XCTAssertFalse(allRows.succeed());
        handle.invalidate();
    }];
    usleep(10000);
    signal.cancel();
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue(hasTestInterrupt);
    self.database->traceSQL(nullptr);
}

@end
