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

#import "WTCAssert.h"
#import <WINQ/abstract.h>

@interface WTCStatementDropTriggerTests : XCTestCase

@end

@implementation WTCStatementDropTriggerTests

- (void)testStatementDropTrigger
{
    std::string schemaName = "testSchema";
    std::string triggerName = "testTrigger";

    WINQAssertEqual(WCDB::StatementDropTrigger()
                        .dropTrigger(triggerName, true)
                        .withSchema(schemaName),
                    @"DROP TRIGGER IF EXISTS testSchema.testTrigger");

    WINQAssertEqual(WCDB::StatementDropTrigger()
                        .dropTrigger(triggerName, true),
                    @"DROP TRIGGER IF EXISTS testTrigger");

    WINQAssertEqual(WCDB::StatementDropTrigger()
                        .dropTrigger(triggerName, false)
                        .withSchema(schemaName),
                    @"DROP TRIGGER testSchema.testTrigger");

    //Default
    WINQAssertEqual(WCDB::StatementDropTrigger()
                        .dropTrigger(triggerName)
                        .withSchema(schemaName),
                    @"DROP TRIGGER IF EXISTS testSchema.testTrigger");
}

@end
