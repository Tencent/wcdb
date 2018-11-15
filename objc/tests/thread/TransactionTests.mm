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

@interface TransactionTests : PreInsertedCRUDTestCase

@end

@implementation TransactionTests

#pragma mark - Database - Transaction
- (void)test_database_commit
{
    BOOL result = [self checkObjects:@[ self.object2 ]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             return [self.database runTransaction:^BOOL(WCTHandle* handle) {
                 return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
             }];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_database_rollback
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             __block BOOL unexpected = NO;
             if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return NO; // rollback
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_database_auto_rollback
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             __block BOOL unexpected = NO;
             [self.console disableSQLiteWrite];
             if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return YES; // commit failed
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             [self.console enableSQLiteWrite];
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Seperated Transaction
- (void)test_database_seperated_commit
{
    BOOL result = [self checkObjects:@[ self.object2 ]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             return [self.database beginTransaction]
                    && [self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                    && [self.database commitOrRollbackTransaction];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_database_seperated_auto_rollback
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             BOOL unexpected = NO;
             [self.console disableSQLiteWrite];
             if ([self.database beginTransaction]) {
                 if (![self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                     TESTCASE_FAILED
                     unexpected = YES;
                 }
                 if ([self.database commitOrRollbackTransaction]) { // commit failed
                     TESTCASE_FAILED
                     unexpected = YES;
                 }
             } else {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             [self.console enableSQLiteWrite];
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_database_seperated_rollback
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             if (![self.database beginTransaction]
                 || ![self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                 TESTCASE_FAILED
                 return NO;
             }
             [self.database rollbackTransaction];
             return YES;
         }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Nested Transaction
- (void)test_database_commit_nested
{
    BOOL result = [self checkObjects:@[ self.object2 ]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             return [self.database runNestedTransaction:^BOOL(WCTHandle* handle) {
                 return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
             }];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_database_rollback_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             __block BOOL unexpected = NO;
             if ([self.database runNestedTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return NO; // rollback
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_database_auto_rollback_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             __block BOOL unexpected = NO;
             [self.console disableSQLiteWrite];
             if ([self.database runNestedTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return YES; // commit failed
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             [self.console enableSQLiteWrite];
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Seperated Nested Transaction
- (void)test_database_seperated_commit_nested
{
    BOOL result = [self checkObjects:@[ self.object2 ]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             return [self.database beginNestedTransaction]
                    && [self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                    && [self.database commitOrRollbackNestedTransaction];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_database_seperated_auto_rollback_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             BOOL unexpected = NO;
             [self.console disableSQLiteWrite];
             if ([self.database beginNestedTransaction]) {
                 if (![self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                     TESTCASE_FAILED
                     unexpected = YES;
                 }
                 if ([self.database commitOrRollbackNestedTransaction]) { // commit failed
                     TESTCASE_FAILED
                     unexpected = YES;
                 }
             } else {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             [self.console enableSQLiteWrite];
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_database_seperated_rollback_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             if (![self.database beginNestedTransaction]
                 || ![self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                 TESTCASE_FAILED
                 return NO;
             }
             [self.database rollbackNestedTransaction];
             return YES;
         }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Transaction
- (void)test_handle_commit
{
    BOOL result = [self checkObjects:@[ self.object2 ]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             return [handle runTransaction:^BOOL(WCTHandle* handle) {
                 return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
             }];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_rollback
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             __block BOOL unexpected = NO;
             if ([handle runTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return NO; // rollback
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_auto_rollback
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             __block BOOL unexpected = NO;
             [self.console disableSQLiteWrite];
             if ([handle runTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return YES; // commit failed
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             [self.console enableSQLiteWrite];
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Seperated Transaction
- (void)test_handle_seperated_commit
{
    BOOL result = [self checkObjects:@[ self.object2 ]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             return [handle beginTransaction]
                    && [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                    && [handle commitOrRollbackTransaction];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_seperated_auto_rollback
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             BOOL unexpected = NO;
             [self.console disableSQLiteWrite];
             if ([handle beginTransaction]) {
                 if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                     TESTCASE_FAILED
                     unexpected = YES;
                 }
                 if ([handle commitOrRollbackTransaction]) { // commit failed
                     TESTCASE_FAILED
                     unexpected = YES;
                 }
             } else {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             [self.console enableSQLiteWrite];
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_seperated_rollback
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             if (![handle beginTransaction]
                 || ![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                 TESTCASE_FAILED
                 return NO;
             }
             [handle rollbackTransaction];
             return YES;
         }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Nested Transaction
- (void)test_handle_commit_nested
{
    BOOL result = [self checkObjects:@[ self.object2 ]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             return [handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                 return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
             }];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_rollback_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             __block BOOL unexpected = NO;
             if ([handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return NO; // rollback
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_auto_rollback_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             __block BOOL unexpected = NO;
             [self.console disableSQLiteWrite];
             if ([handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return YES; // commit failed
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             [self.console enableSQLiteWrite];
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Seperated Nested Transaction
- (void)test_handle_seperated_commit_nested
{
    BOOL result = [self checkObjects:@[ self.object2 ]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             return [handle beginNestedTransaction]
                    && [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                    && [handle commitOrRollbackNestedTransaction];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_seperated_auto_rollback_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             BOOL unexpected = NO;
             [self.console disableSQLiteWrite];
             if ([handle beginNestedTransaction]) {
                 if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                     TESTCASE_FAILED
                     unexpected = YES;
                 }
                 if ([handle commitOrRollbackNestedTransaction]) { // commit failed
                     TESTCASE_FAILED
                     unexpected = YES;
                 }
             } else {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             [self.console enableSQLiteWrite];
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_seperated_rollback_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             WCTHandle* handle = [self.database getHandle];
             if (![handle beginNestedTransaction]
                 || ![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                 TESTCASE_FAILED
                 return NO;
             }
             [handle rollbackNestedTransaction];
             return YES;
         }];
    TestCaseAssertTrue(result);
}

#pragma mark - Interspersed
- (void)test_commit_and_commit_nested
{
    BOOL result = [self checkObjects:@[]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT WCDBSavepoint_1", @"DELETE FROM main.testTable WHERE identifier == 2", @"RELEASE WCDBSavepoint_1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             return [self.database runTransaction:^BOOL(WCTHandle* handle) {
                 return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                        && [handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                               return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2];
                           }]; // commit, commit nested
             }];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_commit_and_rollback_nested
{
    BOOL result = [self checkObjects:@[ self.object2 ]
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT WCDBSavepoint_1", @"DELETE FROM main.testTable WHERE identifier == 2", @"ROLLBACK TO WCDBSavepoint_1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             __block BOOL unexpected = NO;
             if (![self.database runTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         unexpected = YES;
                     }
                     if ([handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                             if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2]) {
                                 TESTCASE_FAILED
                                 unexpected = YES;
                             }
                             return NO; // rollback nested
                         }]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return YES; // commit
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_rollback_and_commit_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT WCDBSavepoint_1", @"DELETE FROM main.testTable WHERE identifier == 2", @"RELEASE WCDBSavepoint_1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             __block BOOL unexpected = NO;
             if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     if (![handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                             return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2]; // commit nested
                         }]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return NO; // rollback
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

- (void)test_rollback_and_rollback_nested
{
    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT WCDBSavepoint_1", @"DELETE FROM main.testTable WHERE identifier == 2", @"ROLLBACK TO WCDBSavepoint_1", @"ROLLBACK" ]
         asExpectedAfterModification:^BOOL {
             __block BOOL unexpected = NO;
             if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
                     if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     if ([handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                             if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2]) {
                                 TESTCASE_FAILED
                                 unexpected = YES;
                             }
                             return NO; // rollback nested
                         }]) {
                         TESTCASE_FAILED
                         unexpected = YES;
                     }
                     return NO; // rollback
                 }]) {
                 TESTCASE_FAILED
                 unexpected = YES;
             }
             return !unexpected;
         }];
    TestCaseAssertTrue(result);
}

@end
