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

#import "MigrationTestCase.h"

@interface MigrationTests : MigrationTestCase

@end

@implementation MigrationTests

- (void)setUp
{
    self.isCrossDatabaseMigration = NO;
    [super setUp];
}

- (void)test_filter
{
    WCTDatabase *filter = [[WCTDatabase alloc] initWithPath:[self.path stringByAppendingString:@"_filter"]];
    __block int tested = 0;
    NSString *expectedTableName = self.tableName;
    [filter filterMigration:^(WCTMigrationUserInfo *userInfo) {
        if ([userInfo.table isEqualToString:expectedTableName]) {
            ++tested;
        }
    }];
    TestCaseAssertTrue([filter getObjectOfClass:TestCaseObject.class fromTable:self.tableName] == nil);
    TestCaseAssertTrue([filter getObjectOfClass:TestCaseObject.class fromTable:self.tableName] == nil);
    TestCaseAssertTrue(tested == 1);
}

- (void)test_notification
{
    __block BOOL tableMigrated = NO;
    __block BOOL migrated = NO;
    NSString *expectedTableName = self.tableName;
    [self.database setNotificationWhenMigrated:^(WCTMigrationBaseInfo *info) {
        if (info == nil) {
            migrated = YES;
        } else if ([info.table isEqualToString:expectedTableName]) {
            tableMigrated = YES;
        }
    }];

    BOOL done = NO;
    BOOL succeed;
    do {
        TestCaseAssertFalse(tableMigrated);
        TestCaseAssertFalse(migrated);
        succeed = [self.database stepMigration:YES done:done];
    } while (succeed && !done);
    TestCaseAssertTrue(succeed);
    TestCaseAssertTrue(done);
    TestCaseAssertTrue(tableMigrated);
    TestCaseAssertTrue(migrated);
}

@end
