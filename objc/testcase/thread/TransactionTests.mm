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

#import <TestCase/TestCase.h>

@interface TransactionTests : CRUDTestCase

@end

@implementation TransactionTests

- (void)setUp
{
    [super setUp];
    [self insertPresetObjects];
}

#pragma mark - Database - Transaction
- (void)test_database_commit
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          return [self.database runTransaction:^BOOL(WCTHandle* handle) {
              return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
          }];
      }];
}

- (void)test_database_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          __block BOOL tested = NO;
          __block BOOL unexpected = NO;
          if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
                    tested = YES;
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return NO; // rollback
              }]) {
              TestCaseFailure();
              unexpected = YES;
          }
          return !unexpected && tested;
      }];
}

#ifdef DEBUG
- (void)test_database_auto_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          [WCTDatabase disableSQLiteWrite];
          if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
              tested = YES;
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return YES; // commit failed
              }]) {
              TestCaseFailure();
              unexpected = YES;
          } else {
              TestCaseAssertFalse([self.database isInTransaction]);
          }
          [WCTDatabase enableSQLiteWrite];
          return !unexpected && tested;
      }];
}
#endif

#pragma mark - Database - Seperated Transaction
- (void)test_database_seperated_commit
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          return [self.database beginTransaction]
                 && [self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                 && [self.database commitOrRollbackTransaction];
      }];
}

#ifdef DEBUG
- (void)test_database_seperated_auto_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          BOOL unexpected = NO;
          [WCTDatabase disableSQLiteWrite];
          if ([self.database beginTransaction]) {
              if (![self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                  TestCaseFailure();
                  unexpected = YES;
              }
              if ([self.database commitOrRollbackTransaction]) { // commit failed
                  TestCaseFailure();
                  unexpected = YES;
              } else {
                  TestCaseAssertFalse([self.database isInTransaction]);
              }
          } else {
              TestCaseFailure();
              unexpected = YES;
          }
          [WCTDatabase enableSQLiteWrite];
          return !unexpected;
      }];
}
#endif

- (void)test_database_seperated_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          if (![self.database beginTransaction]
              || ![self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
              TestCaseFailure();
              return NO;
          }
          [self.database rollbackTransaction];
          return YES;
      }];
}

#pragma mark - Database - Nested Transaction
- (void)test_database_commit_nested
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          return [self.database runNestedTransaction:^BOOL(WCTHandle* handle) {
              return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
          }];
      }];
}

- (void)test_database_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          if ([self.database runNestedTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return NO; // rollback
              }]) {
              TestCaseFailure();
              unexpected = YES;
          }
          return !unexpected;
      }];
}

#ifdef DEBUG
- (void)test_database_auto_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          [WCTDatabase disableSQLiteWrite];
          if ([self.database runNestedTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return YES; // commit failed
              }]) {
              TestCaseFailure();
              unexpected = YES;
          } else {
              TestCaseAssertFalse([self.database isInTransaction]);
          }
          [WCTDatabase enableSQLiteWrite];
          return !unexpected;
      }];
}
#endif

#pragma mark - Database - Seperated Nested Transaction
- (void)test_database_seperated_commit_nested
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          return [self.database beginNestedTransaction]
                 && [self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                 && [self.database commitOrRollbackNestedTransaction];
      }];
}

#ifdef DEBUG
- (void)test_database_seperated_auto_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          BOOL unexpected = NO;
          [WCTDatabase disableSQLiteWrite];
          if ([self.database beginNestedTransaction]) {
              if (![self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                  TestCaseFailure();
                  unexpected = YES;
              }
              if ([self.database commitOrRollbackNestedTransaction]) { // commit failed
                  TestCaseFailure();
                  unexpected = YES;
              } else {
                  TestCaseAssertFalse([self.database isInTransaction]);
              }
          } else {
              TestCaseFailure();
              unexpected = YES;
          }
          [WCTDatabase enableSQLiteWrite];
          return !unexpected;
      }];
}
#endif

- (void)test_database_seperated_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          if (![self.database beginNestedTransaction]
              || ![self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
              TestCaseFailure();
              return NO;
          }
          [self.database rollbackNestedTransaction];
          return YES;
      }];
}

#pragma mark - Database - Is In Transaction
- (void)test_database_is_in_transaction
{
    {
        TestCaseAssertFalse([self.database isInTransaction]);
        TestCaseAssertTrue([self.database beginTransaction]);
        TestCaseAssertTrue([self.database isInTransaction]);
        TestCaseAssertTrue([self.database commitOrRollbackTransaction]);
        TestCaseAssertFalse([self.database isInTransaction]);
    }
    {
        TestCaseAssertFalse([self.database isInTransaction]);
        TestCaseAssertTrue([self.database beginTransaction]);
        TestCaseAssertTrue([self.database isInTransaction]);
        [self.database rollbackTransaction];
        TestCaseAssertFalse([self.database isInTransaction]);
    }
}

#pragma mark - Handle - Transaction
- (void)test_handle_commit
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          return [handle runTransaction:^BOOL(WCTHandle* handle) {
              return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
          }];
      }];
}

- (void)test_handle_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          if ([handle runTransaction:^BOOL(WCTHandle* handle) {
              tested = YES;
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return NO; // rollback
              }]) {
              TestCaseFailure();
              unexpected = YES;
          }
          return !unexpected && tested;
      }];
}

#ifdef DEBUG
- (void)test_handle_auto_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          [WCTDatabase disableSQLiteWrite];
          if ([handle runTransaction:^BOOL(WCTHandle* handle) {
              tested = YES;
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return YES; // commit failed
              }]) {
              TestCaseFailure();
              unexpected = YES;
          } else {
              TestCaseAssertFalse([handle isInTransaction]);
          }
          [WCTDatabase enableSQLiteWrite];
          return !unexpected && tested;
      }];
}
#endif

#pragma mark - Handle - Seperated Transaction
- (void)test_handle_seperated_commit
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          return [handle beginTransaction]
                 && [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                 && [handle commitOrRollbackTransaction];
      }];
}

#ifdef DEBUG
- (void)test_handle_seperated_auto_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          BOOL unexpected = NO;
          [WCTDatabase disableSQLiteWrite];
          if ([handle beginTransaction]) {
              if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                  TestCaseFailure();
                  unexpected = YES;
              }
              if ([handle commitOrRollbackTransaction]) { // commit failed
                  TestCaseFailure();
                  unexpected = YES;
              } else {
                  TestCaseAssertFalse([handle isInTransaction]);
              }
          } else {
              TestCaseFailure();
              unexpected = YES;
          }
          [WCTDatabase enableSQLiteWrite];
          return !unexpected;
      }];
}
#endif

- (void)test_handle_seperated_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          if (![handle beginTransaction]
              || ![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
              TestCaseFailure();
              return NO;
          }
          [handle rollbackTransaction];
          return YES;
      }];
}

#pragma mark - Handle - Nested Transaction
- (void)test_handle_commit_nested
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          return [handle runNestedTransaction:^BOOL(WCTHandle* handle) {
              return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
          }];
      }];
}

- (void)test_handle_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          __block BOOL unexpected = NO;
          if ([handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return NO; // rollback
              }]) {
              TestCaseFailure();
              unexpected = YES;
          }
          return !unexpected;
      }];
}

#ifdef DEBUG
- (void)test_handle_auto_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          __block BOOL unexpected = NO;
          [WCTDatabase disableSQLiteWrite];
          if ([handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return YES; // commit failed
              }]) {
              TestCaseFailure();
              unexpected = YES;
          } else {
              TestCaseAssertFalse([handle isInTransaction]);
          }
          [WCTDatabase enableSQLiteWrite];
          return !unexpected;
      }];
}
#endif

#pragma mark - Handle - Seperated Nested Transaction
- (void)test_handle_seperated_commit_nested
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          return [handle beginNestedTransaction]
                 && [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                 && [handle commitOrRollbackNestedTransaction];
      }];
}

#ifdef DEBUG
- (void)test_handle_seperated_auto_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          BOOL unexpected = NO;
          [WCTDatabase disableSQLiteWrite];
          if ([handle beginNestedTransaction]) {
              if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                  TestCaseFailure();
                  unexpected = YES;
              }
              if ([handle commitOrRollbackNestedTransaction]) { // commit failed
                  TestCaseFailure();
                  unexpected = YES;
              } else {
                  TestCaseAssertFalse([handle isInTransaction]);
              }
          } else {
              TestCaseFailure();
              unexpected = YES;
          }
          [WCTDatabase enableSQLiteWrite];
          return !unexpected;
      }];
}
#endif

- (void)test_handle_seperated_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          if (![handle beginNestedTransaction]
              || ![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
              TestCaseFailure();
              return NO;
          }
          [handle rollbackNestedTransaction];
          return YES;
      }];
}

#pragma mark - Database - Is In Transaction
- (void)test_handle_is_in_transaction
{
    WCTHandle* handle = [self.database getHandle];
    {
        TestCaseAssertFalse([handle isInTransaction]);
        TestCaseAssertTrue([handle beginTransaction]);
        TestCaseAssertTrue([handle isInTransaction]);
        TestCaseAssertTrue([handle commitOrRollbackTransaction]);
        TestCaseAssertFalse([handle isInTransaction]);
    }
    {
        TestCaseAssertFalse([handle isInTransaction]);
        TestCaseAssertTrue([handle beginTransaction]);
        TestCaseAssertTrue([handle isInTransaction]);
        [handle rollbackTransaction];
        TestCaseAssertFalse([handle isInTransaction]);
    }
}

#pragma mark - Interspersed
- (void)test_commit_and_commit_nested
{
    [self doTestObjects:@[]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT WCDBSavepoint_1", @"DELETE FROM main.testTable WHERE identifier == 2", @"RELEASE WCDBSavepoint_1", @"COMMIT" ]
      afterModification:^BOOL {
          return [self.database runTransaction:^BOOL(WCTHandle* handle) {
              return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                     && [handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                            return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2];
                        }]; // commit, commit nested
          }];
      }];
}

- (void)test_commit_and_rollback_nested
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT WCDBSavepoint_1", @"DELETE FROM main.testTable WHERE identifier == 2", @"ROLLBACK TO WCDBSavepoint_1", @"COMMIT" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          if (![self.database runTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      unexpected = YES;
                  }
                  if ([handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                      tested = YES;
                          if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2]) {
                              TestCaseFailure();
                              unexpected = YES;
                          }
                          return NO; // rollback nested
                      }]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return YES; // commit
              }]) {
              TestCaseFailure();
              unexpected = YES;
          }
          return !unexpected && tested;
      }];
}

- (void)test_rollback_and_commit_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT WCDBSavepoint_1", @"DELETE FROM main.testTable WHERE identifier == 2", @"RELEASE WCDBSavepoint_1", @"ROLLBACK" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  if (![handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                      tested = YES;
                          return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2]; // commit nested
                      }]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return NO; // rollback
              }]) {
              TestCaseFailure();
              unexpected = YES;
          }
          return !unexpected && tested;
      }];
}

- (void)test_rollback_and_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT WCDBSavepoint_1", @"DELETE FROM main.testTable WHERE identifier == 2", @"ROLLBACK TO WCDBSavepoint_1", @"ROLLBACK" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  if ([handle runNestedTransaction:^BOOL(WCTHandle* handle) {
                      tested = YES;
                          if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2]) {
                              TestCaseFailure();
                              unexpected = YES;
                          }
                          return NO; // rollback nested
                      }]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  return NO; // rollback
              }]) {
              TestCaseFailure();
              unexpected = YES;
          }
          return !unexpected && tested;
      }];
}

@end
