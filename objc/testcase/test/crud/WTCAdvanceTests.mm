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
#import "WTCFTSObject+WCTTableCoding.h"
#import "WTCFTSObject.h"

@interface WTCAdvanceTests : WTCCRUDTestCase
@property(nonatomic, readonly) WCTCoreStatement *coreStatement;
@end

@implementation WTCAdvanceTests

- (void)tearDown
{
    if (_coreStatement) {
        XCTAssertTrue([_coreStatement finalize]);
        _coreStatement = nil;
    }
    [super tearDown];
}

- (void)testCoreStatement
{
    WCDB::StatementSelect statement = WCDB::StatementSelect().select(WCDB::Column::All).from(WCTMaster.TableName.UTF8String);
    _coreStatement = [self.database prepare:statement];
    XCTAssertNotNil(_coreStatement);
    XCTAssertEqual(self.coreStatement.tag, self.database.tag);
    XCTAssertTrue([self.coreStatement.path isEqualToString:self.database.path]);
}

- (void)testCoreStatementRead
{
    //Give
    WCDB::StatementSelect statement = WCDB::StatementSelect().select(WCDB::Column::All).from(WTCCRUDObject.Name.UTF8String);
    _coreStatement = [self.database prepare:statement];
    XCTAssertNotNil(_coreStatement);

    int expectedVariable1 = 1;
    NSString *expectedVariable2 = @"object1";
    //When
    XCTAssertTrue([self.coreStatement step]);
    //Then
    XCTAssertEqual(self.coreStatement.columnCount, 2);

    XCTAssertTrue([[self.coreStatement columnNameAtIndex:0] isEqualToString:@"variable1"]);
    XCTAssertTrue([[self.coreStatement columnNameAtIndex:1] isEqualToString:@"variable2"]);

    XCTAssertTrue([[self.coreStatement tableNameAtIndex:0] isEqualToString:WTCCRUDObject.Name]);
    XCTAssertTrue([[self.coreStatement tableNameAtIndex:1] isEqualToString:WTCCRUDObject.Name]);

    XCTAssertEqual([self.coreStatement indexByColumnName:@"variable1"], 0);
    XCTAssertEqual([self.coreStatement indexByColumnName:@"variable2"], 1);

    XCTAssertEqual([self.coreStatement indexByColumnName:@"nonexistent"], INT_MAX);

    XCTAssertEqual([self.coreStatement columnTypeAtIndex:0], WCTColumnTypeInteger64);
    XCTAssertEqual([self.coreStatement columnTypeAtIndex:1], WCTColumnTypeString);

    XCTAssertEqual([self.coreStatement columnTypeByName:@"variable1"], WCTColumnTypeInteger64);
    XCTAssertEqual([self.coreStatement columnTypeByName:@"variable2"], WCTColumnTypeString);
    XCTAssertEqual([self.coreStatement columnTypeByName:@"nonexistent"], WCTColumnTypeNull);

    {
        NSNumber *variable1 = (NSNumber *) [self.coreStatement valueAtIndex:0];
        XCTAssertNotNil(variable1);
        XCTAssertEqual(variable1.intValue, expectedVariable1);

        NSString *variable2 = (NSString *) [self.coreStatement valueAtIndex:1];
        XCTAssertNotNil(variable2);
        XCTAssertTrue([variable2 isEqualToString:expectedVariable2]);
    }

    {
        NSNumber *variable1 = (NSNumber *) [self.coreStatement valueByColumnName:@"variable1"];
        XCTAssertNotNil(variable1);
        XCTAssertEqual(variable1.intValue, expectedVariable1);

        NSString *variable2 = (NSString *) [self.coreStatement valueByColumnName:@"variable2"];
        XCTAssertNotNil(variable2);
        XCTAssertTrue([variable2 isEqualToString:expectedVariable2]);
    }

    XCTAssertTrue([self.coreStatement finalize]);
}

//- (void)testCoreStatementWrite {
//    //Give
//    const WCDB::Expression& bindParameter = WCDB::Expression::BindParameter;
//    WCDB::StatementInsert statementInsert = WCDB::StatementInsert().insert(WTCCRUDObject.Name.UTF8String).values({bindParameter, bindParameter});
//    _coreStatement = [self.database prepare:statementInsert];
//    XCTAssertNotNil(_coreStatement);
//
//    int expectedVairable1 = 3;
//    NSString* expectedVariable2 = @"object3";
//
//    //When
//    [self.coreStatement bindValue:nil toIndex:1];
//    [self.coreStatement bindValue:expectedVariable2 toIndex:2];
//    XCTAssertTrue([self.coreStatement step]);
//    //Then
//    XCTAssertEqual(self.coreStatement.lastInsertedRowID, 3);
//    XCTAssertEqual(self.coreStatement.changes, 1);
//    //When
//    XCTAssertTrue([self.coreStatement reset]);
//    WTCCRUDObject* expectedObject = [[WTCCRUDObject alloc] init];
//    expectedObject.variable1 = 4;
//    expectedVariable2 = @"object4";
//    //TODO Add bind property interface
//    //    XCTAssertTrue([coreStatement bind])
//    //    XCTAssertTrue([coreStatement bind])
//    XCTAssertTrue([self.coreStatement step]);
//
//    XCTAssertTrue([self.coreStatement finalize]);
//
//    //Then
//    {
//        WTCCRUDObject* result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == expectedVairable1];
//        XCTAssertNotNil(result);
//        XCTAssertTrue([result.variable2 isEqualToString:expectedVariable2]);
//    }
//    {
//        WTCCRUDObject* result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == expectedObject.variable1];
//        XCTAssertNotNil(result);
//        XCTAssertTrue([result isEqual:expectedObject]);
//    }
//}

- (void)testStepFailed
{
    //Give
    const WCDB::Expression &bindParameter = WCDB::Expression::BindParameter;
    WCDB::StatementInsert statementInsert = WCDB::StatementInsert().insert(WTCCRUDObject.Name.UTF8String).values({bindParameter, bindParameter});

    _coreStatement = [self.database prepare:statementInsert];
    XCTAssertNotNil(_coreStatement);

    NSString *expectedVariable2 = @"object3";
    //When
    [self.coreStatement bindValue:nil toIndex:1];
    [self.coreStatement bindValue:expectedVariable2 toIndex:2];
    [self.coreStatement step];
    XCTAssertNil([self.coreStatement getError]);
    [self.coreStatement step];
    XCTAssertNotNil([self.coreStatement getError]);
}

- (void)testExecFailed
{
    WCDB::StatementSelect statementSelect = WCDB::StatementSelect().select(WCDB::Column::All).from("nonexistentTable");
    XCTAssertFalse([self.database exec:statementSelect]);
}

- (void)testCipher
{
    //Give
    [self.database close:^{
      XCTAssertTrue([self.database removeFilesWithError:nil]);
    }];
    NSData *password = [@"password" dataUsingEncoding:NSASCIIStringEncoding];
    NSData *wrongPassword = [@"wrongPassword" dataUsingEncoding:NSASCIIStringEncoding];
    //When
    [self.database setCipherKey:password];
    XCTAssertTrue([self.database createTableAndIndexesOfName:WTCCRUDObject.Name withClass:WTCCRUDObject.class]);
    //Then
    [self.database close];
    [self.database setCipherKey:wrongPassword];
    XCTAssertFalse([self.database isTableExists:WTCCRUDObject.Name]);

    [self.database close];
    [self.database setCipherKey:nil];
    XCTAssertFalse([self.database isTableExists:WTCCRUDObject.Name]);

    [self.database close];
    [self.database setCipherKey:password];
    XCTAssertTrue([self.database isTableExists:WTCCRUDObject.Name]);
}

- (void)testCipherFailed
{
    //Give
    [self.database close:^{
      XCTAssertTrue([self.database removeFilesWithError:nil]);
    }];
    NSData *emptyPassword = [[NSData alloc] init];
    //When
    [self.database setCipherKey:emptyPassword];
    XCTAssertFalse([self.database canOpen]);
}

- (void)testConfig
{
    //Then
    {
        _coreStatement = [self.database prepare:WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete)];
        XCTAssertNotNil(_coreStatement);
        [self.coreStatement step];
        XCTAssertNil([self.coreStatement getError]);
        NSNumber *isSecureDelete = (NSNumber *) [self.coreStatement valueAtIndex:0];
        XCTAssertNotNil(isSecureDelete);
        XCTAssertEqual(isSecureDelete.intValue, 0);
        XCTAssertTrue([self.coreStatement finalize]);
    }
    //Give
    [self.database close];
    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) {
      return handle->exec(WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete, YES));
    }
                     forName:@"SecureDelete"];
    //When
    XCTAssertTrue([self.database canOpen]);
    //Then
    {
        _coreStatement = [self.database prepare:WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete)];
        XCTAssertNotNil(_coreStatement);
        [self.coreStatement step];
        XCTAssertNil([self.coreStatement getError]);
        NSNumber *isSecureDelete = (NSNumber *) [self.coreStatement valueAtIndex:0];
        XCTAssertNotNil(isSecureDelete);
        XCTAssertEqual(isSecureDelete.intValue, 1);
        XCTAssertTrue([self.coreStatement finalize]);
    }
}

- (void)testOrderedConfig
{
    //Then
    {
        _coreStatement = [self.database prepare:WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete)];
        XCTAssertNotNil(_coreStatement);
        [self.coreStatement step];
        XCTAssertNil([self.coreStatement getError]);
        NSNumber *isSecureDelete = (NSNumber *) [self.coreStatement valueAtIndex:0];
        XCTAssertNotNil(isSecureDelete);
        XCTAssertFalse(isSecureDelete.boolValue);
        XCTAssertTrue([self.coreStatement finalize]);
    }
    //Give
    [self.database close];
    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) {
      return handle->exec(WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete, YES));
    }
                     forName:@"SecureDelete"
                   withOrder:-2];
    __block bool tested = false;
    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) {
      std::shared_ptr<WCDB::StatementHandle> handleStatement = handle->prepare(WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete));
      XCTAssertNotEqual(handleStatement, nullptr);
      handleStatement->step();
      XCTAssertTrue(handleStatement->isOK());
      bool isSecureDelete = handleStatement->getValue<WCDB::ColumnType::Integer32>(0);
      XCTAssertTrue(isSecureDelete);
      XCTAssertTrue(handleStatement->finalize());

      tested = true;
      return YES;
    }
                     forName:@"test"];

    //When
    XCTAssertTrue([self.database canOpen]);
    XCTAssertTrue(tested);
}

- (void)testConfigChangeOrder
{
    //Give
    [self.database close];
    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &, WCDB::Error &) {
      XCTFail("should not reach");
      return NO;
    }
                     forName:@"SecureDelete"
                   withOrder:0];
    __block bool tested = false;
    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) {
      std::shared_ptr<WCDB::StatementHandle> handleStatement = handle->prepare(WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete));
      XCTAssertNotEqual(handleStatement, nullptr);
      handleStatement->step();
      XCTAssertTrue(handleStatement->isOK());
      bool isSecureDelete = handleStatement->getValue<WCDB::ColumnType::Integer32>(0);
      XCTAssertFalse(isSecureDelete);
      XCTAssertTrue(handleStatement->finalize());

      tested = true;
      return YES;
    }
                     forName:@"test"
                   withOrder:10];

    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) {
      return handle->exec(WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete, YES));
    }
                     forName:@"SecureDelete"
                   withOrder:100];
    //When
    XCTAssertTrue([self.database canOpen]);
    XCTAssertTrue(tested);
}

- (void)testSynchronousConfig
{
    //Give
    WCDB::StatementPragma synchronousStatement = WCDB::StatementPragma().pragma(WCDB::Pragma::Synchronous);
    [self.database close];

    __block bool testBeforeSynchronousEnable = false;
    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) {
      std::shared_ptr<WCDB::StatementHandle> handleStatement = handle->prepare(WCDB::StatementPragma().pragma(WCDB::Pragma::Synchronous));
      XCTAssertNotEqual(handleStatement, nullptr);
      handleStatement->step();
      XCTAssertTrue(handleStatement->isOK());
      int synchronous = handleStatement->getValue<WCDB::ColumnType::Integer32>(0);
      XCTAssertEqual(synchronous, 1);
      XCTAssertTrue(handleStatement->finalize());

      testBeforeSynchronousEnable = true;
      return YES;
    }
                     forName:@"test"];

    XCTAssertTrue([self.database canOpen]);
    XCTAssertTrue(testBeforeSynchronousEnable);

    [self.database close];
    [self.database setSynchronousFull:YES];

    __block bool testAfterSynchronousEnable = false;
    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) {
      std::shared_ptr<WCDB::StatementHandle> handleStatement = handle->prepare(WCDB::StatementPragma().pragma(WCDB::Pragma::Synchronous));
      XCTAssertNotEqual(handleStatement, nullptr);
      handleStatement->step();
      XCTAssertTrue(handleStatement->isOK());
      int synchronous = handleStatement->getValue<WCDB::ColumnType::Integer32>(0);
      XCTAssertEqual(synchronous, 2);
      XCTAssertTrue(handleStatement->finalize());

      testAfterSynchronousEnable = true;
      return YES;
    }
                     forName:@"test"];

    XCTAssertTrue([self.database canOpen]);
    XCTAssertTrue(testAfterSynchronousEnable);

    [self.database close];
    [self.database setSynchronousFull:NO];

    __block bool testAfterSynchronousDisable = false;
    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) {
      std::shared_ptr<WCDB::StatementHandle> handleStatement = handle->prepare(WCDB::StatementPragma().pragma(WCDB::Pragma::Synchronous));
      XCTAssertNotEqual(handleStatement, nullptr);
      handleStatement->step();
      XCTAssertTrue(handleStatement->isOK());
      int synchronous = handleStatement->getValue<WCDB::ColumnType::Integer32>(0);
      XCTAssertEqual(synchronous, 1);
      XCTAssertTrue(handleStatement->finalize());

      testAfterSynchronousDisable = true;
      return YES;
    }
                     forName:@"test"];

    XCTAssertTrue([self.database canOpen]);
    XCTAssertTrue(testAfterSynchronousDisable);
}

- (void)testConfigFailed
{
    //Give
    [self.database close];
    [self.database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error &error) {
      return handle->exec(WCDB::StatementSelect().select(WCDB::Column::All).from("nonexistentTable"));
    }
                     forName:@"testConfigFailed"];
    //When
    XCTAssertFalse([self.database canOpen]);
}

- (void)testRedirect
{
    WTCCRUDObject *object = [self.database getObjectOnProperties:WTCCRUDObject.AllColumns.count().as(WTCCRUDObject.variable1) fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(object);
    XCTAssertEqual(object.variable1, self.preInsertedObjects.count);
}

- (void)testPropertyRedirect
{
    WTCCRUDObject *object = [self.database getObjectOnProperties:WTCCRUDObject.variable1.as(WTCCRUDObject.variable1) fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(object);
    XCTAssertEqual(object.variable1, self.preInsertedObjects[0].variable1);
}

- (void)testFTS
{
    //Give
    [self.database close:^{
      XCTAssertTrue([self.database removeFilesWithError:nil]);
    }];
    [self.database setTokenizer:WCTTokenizerNameWCDB];
    XCTAssertTrue([self.database createVirtualTableOfName:WTCFTSObject.Name withClass:WTCFTSObject.class]);

    WTCFTSObject *preInsertedEnglishFTSObject = [[WTCFTSObject alloc] init];
    preInsertedEnglishFTSObject.variable1 = 1;
    preInsertedEnglishFTSObject.variable2 = @"This is English test content";

    WTCFTSObject *preInsertedChineseFTSObject = [[WTCFTSObject alloc] init];
    preInsertedChineseFTSObject.variable1 = 2;
    preInsertedChineseFTSObject.variable2 = @"这是中文测试内容";

    WTCFTSObject *preInsertedNumbericFTSObject = [[WTCFTSObject alloc] init];
    preInsertedNumbericFTSObject.variable1 = 1;
    preInsertedNumbericFTSObject.variable2 = @"123456";

    WTCFTSObject *preInsertedSymbolicFTSObject = [[WTCFTSObject alloc] init];
    preInsertedSymbolicFTSObject.variable1 = 1;
    preInsertedSymbolicFTSObject.variable2 = @"abc..def";

    NSArray *objects = @[ preInsertedEnglishFTSObject, preInsertedChineseFTSObject, preInsertedNumbericFTSObject, preInsertedSymbolicFTSObject ];
    XCTAssertTrue([self.database insertObjects:objects into:WTCFTSObject.Name]);

    //English
    {
        //When
        NSArray<WTCFTSObject *> *objects = [self.database getObjectsOfClass:WTCFTSObject.class fromTable:WTCFTSObject.Name where:WTCFTSObject.variable2.match(@"Engl*")];
        //Then
        XCTAssertEqual(objects.count, 1);
        XCTAssertTrue([objects[0] isEqual:preInsertedEnglishFTSObject]);
    }

    //Chinese
    {
        //When
        NSArray<WTCFTSObject *> *objects = [self.database getObjectsOfClass:WTCFTSObject.class fromTable:WTCFTSObject.Name where:WTCFTSObject.variable2.match(@"中文*")];
        //Then
        XCTAssertEqual(objects.count, 1);
        XCTAssertTrue([objects[0] isEqual:preInsertedChineseFTSObject]);
    }

    //Numberic
    {
        //When
        NSArray<WTCFTSObject *> *objects = [self.database getObjectsOfClass:WTCFTSObject.class fromTable:WTCFTSObject.Name where:WTCFTSObject.variable2.match(@"123*")];
        //Then
        XCTAssertEqual(objects.count, 1);
        XCTAssertTrue([objects[0] isEqual:preInsertedNumbericFTSObject]);
    }

    //Symbolic
    {
        //When
        NSArray<WTCFTSObject *> *objects = [self.database getObjectsOfClass:WTCFTSObject.class fromTable:WTCFTSObject.Name where:WTCFTSObject.variable2.match(@"def")];
        //Then
        XCTAssertEqual(objects.count, 1);
        XCTAssertTrue([objects[0] isEqual:preInsertedSymbolicFTSObject]);
    }

    //Find Chinese
    {
        //When
        NSArray<WTCFTSObject *> *objects = [self.database getObjectsOfClass:WTCFTSObject.class fromTable:WTCFTSObject.Name where:WTCFTSObject.variable2.match(@"文测")];
        //Then
        XCTAssertEqual(objects.count, 1);
        XCTAssertTrue([objects[0] isEqual:preInsertedChineseFTSObject]);
    }
}

@end
