//
// Created by sanhuazhang on 2019/05/02
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

#import "TestCase.h"

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

- (void)test_database_auto_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          [WCTDatabase simulateIOError:WCTSimulateWriteIOError];
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
          [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
          return !unexpected && tested;
      }];
}

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

- (void)test_database_seperated_auto_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          BOOL unexpected = NO;
          [WCTDatabase simulateIOError:WCTSimulateWriteIOError];
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
          [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
          return !unexpected;
      }];
}

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

- (void)test_database_auto_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          [WCTDatabase simulateIOError:WCTSimulateWriteIOError];
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
          [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
          return !unexpected;
      }];
}

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

- (void)test_database_seperated_auto_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          BOOL unexpected = NO;
          [WCTDatabase simulateIOError:WCTSimulateWriteIOError];
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
          [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
          return !unexpected;
      }];
}

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
          return [[self.database getHandle] runTransaction:^BOOL(WCTHandle* handle) {
              return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
          }];
      }];
}

- (void)test_handle_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          if ([[self.database getHandle] runTransaction:^BOOL(WCTHandle* handle) {
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

- (void)test_handle_auto_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          [WCTDatabase simulateIOError:WCTSimulateWriteIOError];
          WCTHandle* handle = [self.database getHandle];
          if ([handle runTransaction:^BOOL(WCTHandle* transactionedHandle) {
                  tested = YES;
                  if (![transactionedHandle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
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
          [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
          return !unexpected && tested;
      }];
}

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

- (void)test_handle_seperated_auto_rollback
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          BOOL unexpected = NO;
          [WCTDatabase simulateIOError:WCTSimulateWriteIOError];
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
          [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
          return !unexpected;
      }];
}

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
          return [[self.database getHandle] runNestedTransaction:^BOOL(WCTHandle* handle) {
              return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1];
          }];
      }];
}

- (void)test_handle_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"ROLLBACK" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          if ([[self.database getHandle] runNestedTransaction:^BOOL(WCTHandle* handle) {
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

- (void)test_handle_auto_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          [WCTDatabase simulateIOError:WCTSimulateWriteIOError];
          WCTHandle* handle = [self.database getHandle];
          if ([handle runNestedTransaction:^BOOL(WCTHandle* transactionedHandle) {
                  if (![transactionedHandle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
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
          [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
          return !unexpected;
      }];
}

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

- (void)test_handle_seperated_auto_rollback_nested
{
    [self doTestObjects:self.objects
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"COMMIT" ]
      afterModification:^BOOL {
          WCTHandle* handle = [self.database getHandle];
          BOOL unexpected = NO;
          [WCTDatabase simulateIOError:WCTSimulateWriteIOError];
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
          [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
          return !unexpected;
      }];
}

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
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT wcdb_lv_2", @"DELETE FROM main.testTable WHERE identifier == 2", @"RELEASE wcdb_lv_2", @"COMMIT" ]
      afterModification:^BOOL {
          return [self.database runTransaction:^BOOL(WCTHandle* handle) {
              return [handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]
                     && [handle runNestedTransaction:^BOOL(WCTHandle* nestedHandle) {
                            return [nestedHandle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2];
                        }]; // commit, commit nested
          }];
      }];
}

- (void)test_commit_and_rollback_nested
{
    [self doTestObjects:@[ self.object2 ]
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT wcdb_lv_2", @"DELETE FROM main.testTable WHERE identifier == 2", @"ROLLBACK TO wcdb_lv_2", @"COMMIT" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          if (![self.database runTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      unexpected = YES;
                  }
                  if ([handle runNestedTransaction:^BOOL(WCTHandle* nestedHandle) {
                          tested = YES;
                          if (![nestedHandle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2]) {
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
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT wcdb_lv_2", @"DELETE FROM main.testTable WHERE identifier == 2", @"RELEASE wcdb_lv_2", @"ROLLBACK" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  if (![handle runNestedTransaction:^BOOL(WCTHandle* nestedHandle) {
                          tested = YES;
                          return [nestedHandle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2]; // commit nested
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
                andSQLs:@[ @"BEGIN IMMEDIATE", @"DELETE FROM main.testTable WHERE identifier == 1", @"SAVEPOINT wcdb_lv_2", @"DELETE FROM main.testTable WHERE identifier == 2", @"ROLLBACK TO wcdb_lv_2", @"ROLLBACK" ]
      afterModification:^BOOL {
          __block BOOL unexpected = NO;
          __block BOOL tested = NO;
          if ([self.database runTransaction:^BOOL(WCTHandle* handle) {
                  if (![handle deleteFromTable:self.tableName where:TestCaseObject.identifier == 1]) {
                      TestCaseFailure();
                      unexpected = YES;
                  }
                  if ([handle runNestedTransaction:^BOOL(WCTHandle* nestedHandle) {
                          tested = YES;
                          if (![nestedHandle deleteFromTable:self.tableName where:TestCaseObject.identifier == 2]) {
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

- (void)test_transaction_handle
{
    BOOL succeed = [self.database runTransaction:^BOOL(WCTHandle* _) {
        WCDB_UNUSED(_);
        WCTHandle* handle = [self.database getHandle];
        TestCaseAssertTrue([handle isInTransaction]);
        return YES;
    }];
    TestCaseAssertTrue(succeed);
}

- (void)test_feature_error_not_allowed_in_transaction
{
    BOOL succeed = [[self.database getHandle] runTransaction:^BOOL(WCTHandle* handle) {
        [handle markErrorNotAllowedWithinTransaction];
        [handle getObjectOfClass:TestCaseObject.class fromTable:@"non-existent"];
        return YES;
    }];
    TestCaseAssertFalse(succeed);
}

- (void)test_pauseable_transaction
{
    TestCaseAssertTrue([self dropTable]);
    TestCaseAssertTrue([self createTable]);
    __block int identifier = 0;
    __block int pauseTimes = -1;
    [self.dispatch async:^{
        __block int i = 0;
        __block NSDate* write1Begin = [NSDate date];
        BOOL ret = [self.database runPauseableTransactionWithOneLoop:^BOOL(WCTHandle * _Nonnull handle, BOOL * _Nonnull stop, BOOL isNewTransaction) {
            TestCaseAssertTrue(handle.isInTransaction);
            if(isNewTransaction)pauseTimes++;
            
            double beginInterval = [[NSDate date] timeIntervalSinceDate:write1Begin];
            TestCaseAssertTrue(beginInterval < 1);
            
            WCTHandleStatement* handleStament = [handle getOrCreateHandleStatementByTag:@"test"];
            WCTProperties properties = [self.tableClass allProperties];
            if(![handleStament isPrepared]){
                [self.tableClass allProperties];
                TestCaseAssertTrue([handleStament prepare: WCDB::StatementInsert().insertIntoTable(self.tableName).columns(properties).values(WCDB::BindParameter::bindParameters(properties.size()))]);
            }
            [handleStament reset];
            [handleStament bindProperties:properties ofObject:[Random.shared testCaseObjectWithIdentifier:identifier++]];
            TestCaseAssertTrue([handleStament step]);
            
            [NSThread sleepForTimeInterval:0.1];
            *stop = ++i > 30;
            write1Begin = [NSDate date];
            return YES;
        }];
        TestCaseAssertTrue(ret);
    }];
    [NSThread sleepForTimeInterval:0.5];
    int i = 0;
    NSDate* write2Begin;
    do{
        write2Begin = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginImmediate()]);
        TestCaseAssertTrue(self.database.isInTransaction);
        double beginInterval = [[NSDate date] timeIntervalSinceDate:write2Begin];
        TestCaseAssertTrue(beginInterval < 1);
        [self.table insertObject:[Random.shared testCaseObjectWithIdentifier:identifier++]];
        write2Begin = [NSDate date];
        [NSThread sleepForTimeInterval:0.1];
        TestCaseAssertTrue([self.database commitOrRollbackTransaction]);
    }while(++i<5);
    [self.dispatch waitUntilDone];
    int count = [self.table getValueOnResultColumn:TestCaseObject.allProperties.count()].numberValue.intValue;
    TestCaseAssertTrue(count == identifier);
    TestCaseAssertTrue(pauseTimes == 5);
}

- (void)test_pauseable_transaction2
{
    TestCaseAssertTrue([self dropTable]);
    TestCaseAssertTrue([self createTable]);
    __block int identifier = 0;
    __block int pauseTime = -1;
    [self.dispatch async:^{
        __block int i = 0;
        __block NSDate* write1Begin = [NSDate date];
        WCTHandle* handle = self.database.getHandle;
        [handle runPauseableTransactionWithOneLoop:^BOOL(WCTHandle * _Nonnull, BOOL * _Nonnull stop, BOOL isNewTransaction) {
            TestCaseAssertTrue(handle.isInTransaction);
            if(isNewTransaction)pauseTime++;
            double beginInterval = [[NSDate date] timeIntervalSinceDate:write1Begin];
            TestCaseAssertTrue(beginInterval < 1);
            
            WCTHandleStatement* handleStament = [handle getOrCreateHandleStatementByTag:@"test"];
            WCTProperties properties = [self.tableClass allProperties];
            if(![handleStament isPrepared]){
                [self.tableClass allProperties];
                TestCaseAssertTrue([handleStament prepare: WCDB::StatementInsert().insertIntoTable(self.tableName).columns(properties).values(WCDB::BindParameter::bindParameters(properties.size()))]);
            }
            [handleStament reset];
            [handleStament bindProperties:properties ofObject:[Random.shared testCaseObjectWithIdentifier:identifier++]];
            TestCaseAssertTrue([handleStament step]);
            
            [NSThread sleepForTimeInterval:0.1];
            *stop = ++i > 30;
            write1Begin = [NSDate date];
            return YES;
        }];
        [handle invalidate];
    }];
    [NSThread sleepForTimeInterval:0.5];
    int i = 0;
    NSDate* write2Begin;
    do{
        write2Begin = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginImmediate()]);
        TestCaseAssertTrue(self.database.isInTransaction);
        double beginInterval = [[NSDate date] timeIntervalSinceDate:write2Begin];
        TestCaseAssertTrue(beginInterval < 1);
        [self.table insertObject:[Random.shared testCaseObjectWithIdentifier:identifier++]];
        write2Begin = [NSDate date];
        [NSThread sleepForTimeInterval:0.1];
        TestCaseAssertTrue([self.database commitOrRollbackTransaction]);
    }while(++i<5);
    [self.dispatch waitUntilDone];
    int count = [self.table getValueOnResultColumn:TestCaseObject.allProperties.count()].numberValue.intValue;
    TestCaseAssertTrue(count == identifier);
    TestCaseAssertTrue(pauseTime == 5);
}

- (void)test_pauseable_transaction3
{
    TestCaseAssertTrue([self dropTable]);
    TestCaseAssertTrue([self createTable]);
    __block int identifier = 0;
    for(int loopCount = 0; loopCount < 5; loopCount++){
        [self.dispatch async:^{
            __block int i = 0;
            BOOL ret = [self.database runPauseableTransactionWithOneLoop:^BOOL(WCTHandle * _Nonnull handle, BOOL * _Nonnull stop, BOOL) {
                TestCaseAssertTrue(handle.isInTransaction);
                WCTHandleStatement* handleStament = [handle getOrCreateHandleStatementByTag:@"test"];
                WCTProperties properties = [self.tableClass allProperties];
                if(![handleStament isPrepared]){
                    [self.tableClass allProperties];
                    TestCaseAssertTrue([handleStament prepare: WCDB::StatementInsert().insertIntoTable(self.tableName).columns(properties).values(WCDB::BindParameter::bindParameters(properties.size()))]);
                }
                [handleStament reset];
                [handleStament bindProperties:properties ofObject:[Random.shared testCaseObjectWithIdentifier:identifier++]];
                TestCaseAssertTrue([handleStament step]);
                
                [NSThread sleepForTimeInterval:0.1];
                *stop = ++i > 5;
                return YES;
            }];
            TestCaseAssertTrue(ret);
        }];
    }
    [NSThread sleepForTimeInterval:0.4];
    int i = 0;
    int lastIdentifier = 0;
    do{
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginImmediate()]);
        TestCaseAssertTrue(self.database.isInTransaction);
        TestCaseAssertTrue(identifier < 25);
        TestCaseAssertTrue((lastIdentifier ==0 && identifier< 10) || identifier - lastIdentifier < 10);
        lastIdentifier = identifier;
        [self.table insertObject:[Random.shared testCaseObjectWithIdentifier:identifier++]];
        [NSThread sleepForTimeInterval:0.5];
        TestCaseAssertTrue([self.database commitOrRollbackTransaction]);
    }while(++i<5);
    [self.dispatch waitUntilDone];
    int count = [self.table getValueOnResultColumn:TestCaseObject.allProperties.count()].numberValue.intValue;
    TestCaseAssertTrue(count == identifier);
}

@end
