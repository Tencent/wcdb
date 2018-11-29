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

@interface RetrieveTests : BackupTestCase

@end

@implementation RetrieveTests

- (BOOL)checkObjectsRetrieved
{
    return [self checkObjects:self.objects
                       andSQL:@"SELECT identifier, content FROM main.testTable"
        asExpectedBySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
            return [self.table getObjects];
        }];
}

- (BOOL)checkObjectsNotRetrieved
{
    return [self checkObjects:@[]
                       andSQL:@"SELECT type, name, tbl_name, rootpage, sql FROM main.sqlite_master WHERE name == 'testTable'"
        asExpectedBySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
            return [self.database getObjectsOfClass:WCTMaster.class fromTable:WCTMaster.tableName where:WCTMaster.name == self.tableName];
        }];
}

- (BOOL)checkRetrieve
{
    __block double lastPercentage = 0;
    __block BOOL sanity = YES;
    double score = [self.database retrieve:^(double percentage, double increment) {
        if (percentage - lastPercentage != increment
            || increment <= 0) {
            TESTCASE_FAILED
            sanity = NO;
        }
        lastPercentage = percentage;
    }];
    return sanity && score == 1.0 && lastPercentage == 1.0;
}

- (BOOL)checkRetrieveFailed
{
    __block double lastPercentage = 0;
    __block BOOL sanity = YES;
    double score = [self.database retrieve:^(double percentage, double increment) {
        if (percentage - lastPercentage != increment
            || increment <= 0) {
            TESTCASE_FAILED
            sanity = NO;
        }
        lastPercentage = percentage;
    }];
    return sanity && score != 1.0 && lastPercentage == 1.0;
}

- (BOOL)tryToMakeHeaderCorrupted
{
    if (![self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]) {
        TESTCASE_FAILED
        return NO;
    }
    __block BOOL result = NO;
    [self.database close:^{
        NSFileHandle *fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:self.path];
        if (!fileHandle) {
            TESTCASE_FAILED
            return;
        }
        [fileHandle writeData:[NSData randomDataWithLength:self.headerSize]];
        [fileHandle closeFile];
        result = YES;
    }];
    return result;
}

#pragma mark - Non-Corrupted
- (void)test_retrieve_with_backup_and_deposit
{
    TestCaseAssertTrue([self.database backup]);
    TestCaseAssertTrue([self.database deposit]);

    TestCaseAssertTrue([self checkRetrieve]);
    TestCaseAssertTrue([self checkObjectsRetrieved]);
}

- (void)test_retrieve_with_backup_and_without_deposit
{
    TestCaseAssertTrue([self.database backup]);

    TestCaseAssertTrue([self checkRetrieve]);
    TestCaseAssertTrue([self checkObjectsRetrieved]);
}

- (void)test_retrieve_without_backup_and_with_deposit
{
    TestCaseAssertTrue([self.database deposit]);

    TestCaseAssertTrue([self checkRetrieve]);
    TestCaseAssertTrue([self checkObjectsRetrieved]);
}

- (void)test_retrieve_without_backup_and_deposite
{
    TestCaseAssertTrue([self checkRetrieve]);
    TestCaseAssertTrue([self checkObjectsRetrieved]);
}

#pragma mark - Corrupted
- (void)test_retrieve_corrupted_with_backup_and_deposit
{
    TestCaseAssertTrue([self.database backup]);

    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    TestCaseAssertTrue([self.database deposit]);

    TestCaseAssertTrue([self checkRetrieve]);

    TestCaseAssertTrue([self checkObjectsRetrieved]);
}

- (void)test_retrieve_corrupted_with_backup_and_without_deposit
{
    TestCaseAssertTrue([self.database backup]);

    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    TestCaseAssertTrue([self checkRetrieve]);

    TestCaseAssertTrue([self checkObjectsRetrieved]);
}

- (void)test_retrieve_corrupted_without_backup_and_with_deposit
{
    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    TestCaseAssertTrue([self.database deposit]);

    TestCaseAssertTrue([self checkRetrieveFailed]);

    TestCaseAssertTrue([self checkObjectsNotRetrieved]);
}

- (void)test_retrieve_corrupted_without_backup_and_deposite
{
    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    TestCaseAssertTrue([self checkRetrieveFailed]);

    TestCaseAssertTrue([self checkObjectsNotRetrieved]);
}

@end
