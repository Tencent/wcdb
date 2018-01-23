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

#import "WTCCRUDTestCase.h"

@interface WTCTransactionTests : WTCCRUDTestCase
@property(nonatomic, readonly) WCTTransaction* transaction;
@property(nonatomic, readonly) WCTCoreStatement* coreStatement;
@end

@implementation WTCTransactionTests

- (void)tearDown
{
    _transaction = nil;
    if (_coreStatement) {
        XCTAssertTrue([_coreStatement finalize]);
        _coreStatement = nil;
    }
    [super tearDown];
}

- (void)testBase {
    //When
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //Then
    XCTAssertEqual(self.transaction.tag, self.database.tag);
    XCTAssertTrue([self.transaction.path isEqualToString:self.database.path]);
}

- (void)testCommitTransactionWithDatabase {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.database begin]);
    XCTAssertTrue([self.database insertObject:object into:WTCCRUDObject.Name]);
    XCTAssertTrue([self.database commit]);
    //Then
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
}

- (void)testRollbackTransactionWithDatabase {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.database begin]);
    XCTAssertTrue([self.database insertObject:object into:WTCCRUDObject.Name]);
    XCTAssertTrue([self.database rollback]);
    //Then
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)testControllableTransactionFailedWithDatabase {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    //Then
    XCTAssertFalse([self.database runControllableTransaction:^BOOL{
        return [self.database insertObject:object into:@"nonexistentTable"];
    }]);
}

- (void)testRunTransactionWithDatabase {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    //Then
    XCTAssertTrue([self.database runTransaction:^{
        XCTAssertTrue([self.database insertObject:object into:WTCCRUDObject.Name]);
    }]);
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
}

- (void)testRunTransactionFailedWithDatabase {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    //Then
    XCTAssertTrue([self.database runTransaction:^{
        XCTAssertFalse([self.database insertObject:object into:@"nonexistentTable"]);
    }]);
}

- (void)testRollbackControllableTransactionWithDatabase {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    //Then
    XCTAssertFalse([self.database runControllableTransaction:^BOOL{
        XCTAssertTrue([self.database insertObject:object into:WTCCRUDObject.Name]);
        return NO;
    }]);
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)testCommitControllableTransactionWithDatabase {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    //Then
    XCTAssertTrue([self.database runControllableTransaction:^BOOL{
        XCTAssertTrue([self.database insertObject:object into:WTCCRUDObject.Name]);
        return YES;
    }]);
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
}

- (void)testRunEmbededTransactionWithDatabase {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    //Then
    XCTAssertTrue([self.database runTransaction:^{
        XCTAssertTrue([self.database runEmbeddedTransaction:^{
            XCTAssertTrue([self.database insertObject:object into:WTCCRUDObject.Name]);
        }]);
    }]);
    
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
}

- (void)testCommitTransactionWithTransaction {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //When
    XCTAssertTrue([self.transaction begin]);
    XCTAssertTrue([self.transaction insertObject:object into:WTCCRUDObject.Name]);
    XCTAssertTrue([self.transaction commit]);
    //Then
    XCTAssertEqual(self.transaction.changes, 1);
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
}

- (void)testRollbackTransactionWithTransaction {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //When
    XCTAssertTrue([self.transaction begin]);
    XCTAssertTrue([self.transaction insertObject:object into:WTCCRUDObject.Name]);
    XCTAssertTrue([self.transaction rollback]);
    //Then
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)testControllableTransactionFailedWithTransaction {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //Then
    XCTAssertFalse([self.transaction runControllableTransaction:^BOOL{
        return [self.transaction insertObject:object into:@"nonexistentTable"];
    }]);
}

- (void)testRunTransactionWithTransaction {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //Then
    XCTAssertTrue([self.transaction runTransaction:^{
        XCTAssertTrue([self.transaction insertObject:object into:WTCCRUDObject.Name]);
    }]);
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
}

- (void)testRunTransactionFailedWithTransaction {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //Then
    XCTAssertTrue([self.transaction runTransaction:^{
        XCTAssertFalse([self.transaction insertObject:object into:@"nonexistentTable"]);
    }]);
}

- (void)testRollbackControllableTransactionWithTransaction {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //Then
    XCTAssertFalse([self.transaction runControllableTransaction:^BOOL{
        XCTAssertTrue([self.transaction insertObject:object into:WTCCRUDObject.Name]);
        return NO;
    }]);
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)testCommitControllableTransactionWithTransaction {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //Then
    XCTAssertTrue([self.transaction runControllableTransaction:^BOOL{
        XCTAssertTrue([self.transaction insertObject:object into:WTCCRUDObject.Name]);
        return YES;
    }]);
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
}

- (void)testRunEmbededTransactionWithTransaction {
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //Then
    XCTAssertTrue([self.transaction runTransaction:^{
        XCTAssertTrue([self.transaction runEmbeddedTransaction:^{
            XCTAssertTrue([self.transaction insertObject:object into:WTCCRUDObject.Name]);
        }]);
    }]);
    
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
}

- (void)testPrepareWithTransaction
{
    //Give
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //When
    _coreStatement = [self.transaction prepare:WCDB::StatementPragma().pragma(WCDB::Pragma::JournalMode)];
    XCTAssertNotNil(_coreStatement);
    XCTAssertTrue([self.coreStatement step]);
    //Then
    NSString* journalMode = (NSString*)[self.coreStatement valueAtIndex:0];
    XCTAssertNotNil(journalMode);
    XCTAssertTrue([journalMode.lowercaseString isEqualToString:@"wal"]);
}

- (void)testExecWithTransaction
{
    //Give
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //When
    [self.transaction exec:WCDB::StatementDelete().deleteFrom(WTCCRUDObject.Name.UTF8String)];
    //Then
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)testIsTableExistsWithTransaction
{
    //Give
    _transaction = [self.database getTransaction];
    XCTAssertNotNil(_transaction);
    //Then
    bool randomTableExists = [self.transaction isTableExists:@"randomTable"];
    XCTAssertFalse(randomTableExists);
    
    bool crudTableExists = [self.transaction isTableExists:WTCCRUDObject.Name];
    XCTAssertTrue(crudTableExists);
}

- (void)testAutoRollbackTransactionWithTransaction
{
    //Give
    WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
    object.variable1 = 3;
    object.variable2 = self.name;
    @autoreleasepool{
        _transaction = [self.database getTransaction];
        XCTAssertNotNil(_transaction);
        //When
        XCTAssertTrue([self.transaction begin]);
        XCTAssertTrue([self.transaction insertObject:object into:WTCCRUDObject.Name]);
        _transaction = nil;
    }
    //Then
    NSArray<WTCCRUDObject*> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 3];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

@end
