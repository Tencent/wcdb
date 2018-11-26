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

#import "BackupTestCase.h"

@interface DepositTests : BackupTestCase

@end

@implementation DepositTests

- (void)test_deposite
{
    TestCaseObject* object = [[TestCaseObject alloc] init];
    object.isAutoIncrement = YES;
    object.content = [NSString randomString];
    int rowId = (int) self.objects.count;

    {
        // 1.
        TestCaseAssertTrue([self.database backup]);
        TestCaseAssertTrue([self.database deposit]);

        NSNumber* count = [self.database getValueFromStatement:WCDB::StatementSelect().select(TestCaseObject.allProperties.count()).from(self.tableName)].numberValue;
        TestCaseAssertTrue(count != nil);
        TestCaseAssertTrue(count.integerValue == 0);

        TestCaseAssertTrue([self.table insertObject:object]);
        ++rowId;
        TestCaseAssertTrue(object.lastInsertedRowID == rowId);
    }

    {
        // 2.
        TestCaseAssertTrue([self.database backup]);
        TestCaseAssertTrue([self.database deposit]);

        NSNumber* count = [self.database getValueFromStatement:WCDB::StatementSelect().select(TestCaseObject.allProperties.count()).from(self.tableName)].numberValue;
        TestCaseAssertTrue(count != nil);
        TestCaseAssertTrue(count.integerValue == 0);

        TestCaseAssertTrue([self.table insertObject:object]);
        ++rowId;
        TestCaseAssertTrue(object.lastInsertedRowID == rowId);
    }

    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.factory]);
    TestCaseAssertTrue([self.database removeDeposit]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.factory]);
}

@end
