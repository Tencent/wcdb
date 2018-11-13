//
//  HandleTests.m
//  Advance tests
//
//  Created by sanhuazhang(张三华) on 2018/11/13.
//  Copyright © 2018 sanhuazhang. All rights reserved.
//

#import "CRUDTestCase.h"
#import <sqlcipher/sqlite3.h>

@interface HandleTests : CRUDTestCase

@property (nonatomic, readonly) WCTHandle* handle;

@property (nonatomic, readonly) WCDB::StatementInsert statementInsert;

@property (nonatomic, readonly) WCDB::StatementSelect statementSelect;

@end

@implementation HandleTests

- (void)setUp
{
    [super setUp];
    _handle = [self.database getHandle];

    _statementInsert = WCDB::StatementInsert()
                       .insertIntoTable(self.tableName)
                       .columns({ TestCaseObject.identifier, TestCaseObject.content })
                       .values({ 3, WCDB::BindParameter(1) });

    _statementSelect = WCDB::StatementSelect()
                       .select(TestCaseObject.content)
                       .from(self.tableName)
                       .where(TestCaseObject.identifier == 3);
}

- (void)tearDown
{
    [_handle invalidate];
    _handle = nil;
    [super tearDown];
}

#pragma mark - Handle
- (void)test_tag
{
    TestCaseAssertEqual(self.handle.database.tag, self.database.tag);
}

- (void)test_raw_handle
{
    sqlite3** rawHandle = (sqlite3**) self.handle.rawHandle;
    sqlite3* sqliteHandle = *rawHandle;
    const char* filename = sqlite3_db_filename(sqliteHandle, "main");
    TestCaseAssertTrue([self.database.path isEqualToString:@(filename)]);
}

#pragma mark - Execute
- (void)test_execute
{
    BOOL result = [self checkAllSQLs:@[ @"PRAGMA main.user_version = 123" ]
               asExpectedByOperation:^BOOL {
                   return [self.handle execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(123)];
               }];
    TestCaseAssertTrue(result);
}

#pragma mark - Prepare
- (void)test_prepare
{
    WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(123);
    TestCaseAssertFalse([self.handle isPrepared]);
    TestCaseAssertTrue([self.handle prepare:statement]);
    TestCaseAssertTrue([self.handle isPrepared]);
    [self.handle finalizeStatement];
    TestCaseAssertFalse([self.handle isPrepared]);
}

#pragma mark - Step
- (void)test_step
{
    BOOL result = [self checkAllSQLs:@[ @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)", @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)" ]
               asExpectedByOperation:^BOOL {
                   WCDB::StatementInsert statement = WCDB::StatementInsert().insertIntoTable(self.tableName).column(WCDB::Column(@"identifier")).column(WCDB::Column(@"content")).values(WCDB::BindParameter::bindParameters(2));
                   if (![self.handle prepare:statement]) {
                       return NO;
                   }

                   [self.handle bindInteger32:3 toIndex:1];
                   [self.handle bindString:[NSString randomString] toIndex:2];
                   if (![self.handle step]) {
                       return NO;
                   }

                   [self.handle reset];
                   [self.handle bindInteger32:4 toIndex:1];
                   [self.handle bindString:[NSString randomString] toIndex:2];
                   BOOL done;
                   BOOL result = [self.handle step:done];
                   [self.handle finalizeStatement];
                   return result && done;
               }];
    TestCaseAssertTrue(result);
}

#pragma mark - State
- (void)test_readonly
{
    {
        WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion());
        TestCaseAssertTrue([self.handle prepare:statement]);
        TestCaseAssertTrue([self.handle isStatementReadonly]);
        [self.handle finalizeStatement];
    }
    {
        WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(123);
        TestCaseAssertTrue([self.handle prepare:statement]);
        TestCaseAssertFalse([self.handle isStatementReadonly]);
        [self.handle finalizeStatement];
    }
}

- (void)test_changes
{
    TestCaseAssertTrue([self.handle execute:WCDB::StatementDelete().deleteFrom(self.tableName)]);
    TestCaseAssertEqual([self.handle getChanges], self.objects.count);
}

- (void)test_get_last_inserted
{
    WCDB::StatementInsert statement = WCDB::StatementInsert().insertIntoTable(self.tableName).column(WCDB::Column(@"identifier")).column(WCDB::Column(@"content")).value(nullptr).value([NSString randomString]);
    TestCaseAssertTrue([self.handle execute:statement]);
    TestCaseAssertEqual([self.handle getLastInsertedRowID], 3);
}

#pragma mark - Bind && Get
- (void)test_integer32
{
    int32_t value = [NSNumber randomInt32];
    {
        TestCaseAssertTrue([self.handle prepare:self.statementInsert]);
        [self.handle bindInteger32:value toIndex:1];

        TestCaseAssertTrue([self.handle step]);
        [self.handle finalizeStatement];
    }
    {
        TestCaseAssertTrue([self.handle prepare:self.statementSelect]);
        TestCaseAssertTrue([self.handle step]);
        TestCaseAssertEqual([self.handle getInteger32AtIndex:0], value);
        [self.handle finalizeStatement];
    }
}

- (void)test_integer64
{
    int64_t value = [NSNumber randomInt64];
    {
        TestCaseAssertTrue([self.handle prepare:self.statementInsert]);
        [self.handle bindInteger64:value toIndex:1];

        TestCaseAssertTrue([self.handle step]);
        [self.handle finalizeStatement];
    }
    {
        TestCaseAssertTrue([self.handle prepare:self.statementSelect]);
        TestCaseAssertTrue([self.handle step]);
        TestCaseAssertEqual([self.handle getInteger64AtIndex:0], value);
        [self.handle finalizeStatement];
    }
}

- (void)test_double
{
    double value = [NSNumber randomDouble];
    {
        TestCaseAssertTrue([self.handle prepare:self.statementInsert]);
        [self.handle bindDouble:value toIndex:1];

        TestCaseAssertTrue([self.handle step]);
        [self.handle finalizeStatement];
    }
    {
        TestCaseAssertTrue([self.handle prepare:self.statementSelect]);
        TestCaseAssertTrue([self.handle step]);

        TestCaseAssertTrue(fabs([self.handle getDoubleAtIndex:0] - value) < std::numeric_limits<double>::epsilon());
        [self.handle finalizeStatement];
    }
}

- (void)test_null
{
    {
        TestCaseAssertTrue([self.handle prepare:self.statementInsert]);
        [self.handle bindNullToIndex:1];

        TestCaseAssertTrue([self.handle step]);
        [self.handle finalizeStatement];
    }
    {
        TestCaseAssertTrue([self.handle prepare:self.statementSelect]);
        TestCaseAssertTrue([self.handle step]);
        TestCaseAssertEqual([self.handle getTypeAtIndex:0], WCTColumnTypeNil);
        [self.handle finalizeStatement];
    }
}

- (void)test_string
{
    NSString* value = [NSString randomString];
    {
        TestCaseAssertTrue([self.handle prepare:self.statementInsert]);
        [self.handle bindString:value toIndex:1];

        TestCaseAssertTrue([self.handle step]);
        [self.handle finalizeStatement];
    }
    {
        TestCaseAssertTrue([self.handle prepare:self.statementSelect]);
        TestCaseAssertTrue([self.handle step]);
        TestCaseAssertTrue([[self.handle getStringAtIndex:0] isEqualToString:value]);
        [self.handle finalizeStatement];
    }
}

- (void)test_number
{
    NSNumber* value = [NSNumber randomNumber];
    {
        TestCaseAssertTrue([self.handle prepare:self.statementInsert]);
        [self.handle bindNumber:value toIndex:1];

        TestCaseAssertTrue([self.handle step]);
        [self.handle finalizeStatement];
    }
    {
        TestCaseAssertTrue([self.handle prepare:self.statementSelect]);
        TestCaseAssertTrue([self.handle step]);
        TestCaseAssertTrue(fabs([self.handle getNumberAtIndex:0].doubleValue - value.doubleValue) < std::numeric_limits<double>::epsilon());
        [self.handle finalizeStatement];
    }
}

- (void)test_data
{
    NSData* value = [NSData randomData];
    {
        TestCaseAssertTrue([self.handle prepare:self.statementInsert]);
        [self.handle bindData:value toIndex:1];

        TestCaseAssertTrue([self.handle step]);
        [self.handle finalizeStatement];
    }
    {
        TestCaseAssertTrue([self.handle prepare:self.statementSelect]);
        TestCaseAssertTrue([self.handle step]);
        TestCaseAssertTrue([[self.handle getDataAtIndex:0] isEqualToData:value]);
        [self.handle finalizeStatement];
    }
}

- (void)test_meta
{
    NSData* value = [NSData randomData];
    {
        TestCaseAssertTrue([self.handle prepare:self.statementInsert]);
        [self.handle bindData:value toIndex:1];

        TestCaseAssertTrue([self.handle step]);
        [self.handle finalizeStatement];
    }
    {
        TestCaseAssertTrue([self.handle prepare:self.statementSelect]);
        TestCaseAssertTrue([self.handle step]);
        TestCaseAssertEqual([self.handle getColumnCount], 1);
        TestCaseAssertTrue([[self.handle getColumnNameAtIndex:0] isEqualToString:@"content"]);
        TestCaseAssertTrue([[self.handle getTableNameAtIndex:0] isEqualToString:self.tableName]);
        [self.handle finalizeStatement];
    }
}

@end
