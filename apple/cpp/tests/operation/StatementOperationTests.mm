//
// Created by 陈秋文 on 2022/8/14.
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

#import "CPPTestCase.h"

@interface StatementOperationTests : CPPCRUDTestCase

@property (nonatomic, assign) std::shared_ptr<WCDB::Handle> handle;
@property (nonatomic, assign) WCDB::StatementInsert statementInsert;
@property (nonatomic, assign) WCDB::StatementSelect statementSelect;

@end

@implementation StatementOperationTests

- (void)setUp
{
    [super setUp];

    _statementInsert = WCDB::StatementInsert()
                       .insertIntoTable(self.tableName.UTF8String)
                       .columns(self.columns)
                       .values({ 3, WCDB::BindParameter(1) });

    _statementSelect = WCDB::StatementSelect()
                       .select(WCDB::Column("content"))
                       .from(self.tableName.UTF8String)
                       .where(WCDB::Column("identifier") == 3);

    self.handle = std::make_shared<WCDB::Handle>(self.database->getHandle());
    [self insertPresetRows];
}

- (void)tearDown
{
    if (_handle->isPrepared()) {
        _handle->finalize();
    }
    _handle = nil;
    [super tearDown];
}

#pragma mark - Execute
- (void)test_execute
{
    [self doTestSQLs:@[ @"PRAGMA user_version = 123" ]
         inOperation:^BOOL {
             return self.handle->execute(WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(123));
         }];
}

#pragma mark - Prepare
- (void)test_prepare
{
    WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(123);
    TestCaseAssertFalse(self.handle->isPrepared());
    TestCaseAssertTrue(self.handle->prepare(statement));
    TestCaseAssertTrue(self.handle->isPrepared());
    self.handle->finalize();
    TestCaseAssertFalse(self.handle->isPrepared());
}

#pragma mark - Step
- (void)test_step
{
    [self doTestSQLs:@[ @"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)", @"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)" ]
         inOperation:^BOOL {
             WCDB::StatementInsert statement = WCDB::StatementInsert().insertIntoTable(self.tableName.UTF8String).column(WCDB::Column("identifier")).column(WCDB::Column("content")).values(WCDB::BindParameter::bindParameters(2));
             if (!self.handle->prepare(statement)) {
                 return NO;
             }

             self.handle->bindValue(3, 1);
             self.handle->bindValue(Random.shared.string.UTF8String, 2);
             if (!self.handle->step()) {
                 return NO;
             }

             self.handle->reset();
             self.handle->bindRow({ 4, Random.shared.string.UTF8String });
             BOOL succeed = self.handle->step() && self.handle->done();
             self.handle->finalize();
             return succeed;
         }];
}

#pragma mark - State
- (void)test_readonly
{
    {
        WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion());
        TestCaseAssertTrue(self.handle->prepare(statement));
        TestCaseAssertTrue(self.handle->isReadOnly());
        self.handle->finalize();
    }
    {
        WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(123);
        TestCaseAssertTrue(self.handle->prepare(statement));
        TestCaseAssertFalse(self.handle->isReadOnly());
        self.handle->finalize();
    }
}

- (void)test_changes
{
    TestCaseAssertTrue(self.handle->execute(WCDB::StatementDelete().deleteFrom(self.tableName.UTF8String)));
    TestCaseAssertEqual(self.handle->getChanges(), self.rows.size());
}

- (void)test_get_last_inserted
{
    WCDB::StatementInsert statement = WCDB::StatementInsert().insertIntoTable(self.tableName.UTF8String).column("identifier").column("content").value(nullptr).value(Random.shared.string.UTF8String);
    TestCaseAssertTrue(self.handle->execute(statement));
    TestCaseAssertEqual(self.handle->getLastInsertedRowID(), 3);
}

#pragma mark - Bind && Get
- (void)test_integer
{
    int64_t value = Random.shared.int64;
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindInteger(value);

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        TestCaseAssertEqual(self.handle->getInteger(), value);
        self.handle->finalize();
    }
}

- (void)test_integer_value
{
    int64_t value = Random.shared.int64;
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindValue(value);

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        TestCaseAssertEqual(self.handle->getValue().intValue(), value);
        self.handle->finalize();
    }
}

- (void)test_double
{
    double value = 1.2;
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindDouble(value);

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        TestCaseAssertEqual(self.handle->getDouble(), value);
        self.handle->finalize();
    }
}

- (void)test_double_value
{
    double value = 1.2;
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindValue(value);

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        TestCaseAssertEqual(self.handle->getValue().floatValue(), value);
        self.handle->finalize();
    }
}

- (void)test_null
{
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindNull();

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        TestCaseAssertEqual(self.handle->getType(), WCDB::ColumnType::Null);
        self.handle->finalize();
    }
}

- (void)test_null_value
{
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindValue(nullptr);

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        TestCaseAssertEqual(self.handle->getValue(), nullptr);
        self.handle->finalize();
    }
}

- (void)test_string
{
    NSString* value = Random.shared.string;
    TestCaseAssertTrue(value != nil);
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindText(value.UTF8String);

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        TestCaseAssertCPPStringEqual(self.handle->getText().data(), value.UTF8String);
        self.handle->finalize();
    }
}

- (void)test_string_value
{
    NSString* value = Random.shared.string;
    TestCaseAssertTrue(value != nil);
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindValue(value.UTF8String);

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        TestCaseAssertCPPStringEqual(self.handle->getValue().textValue().data(), value.UTF8String);
        self.handle->finalize();
    }
}

- (void)test_data
{
    NSData* value = Random.shared.data;
    TestCaseAssertTrue(value != nil);
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindBLOB(WCDB::UnsafeData((unsigned char*) value.bytes, value.length));

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        WCDB::UnsafeData result = self.handle->getBLOB();
        TestCaseAssertTrue(result.size() == value.length && memcmp(result.buffer(), value.bytes, result.size()) == 0);
        self.handle->finalize();
    }
}

- (void)test_data_value
{
    NSData* value = Random.shared.data;
    TestCaseAssertTrue(value != nil);
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementInsert));
        self.handle->bindValue(WCDB::UnsafeData((unsigned char*) value.bytes, value.length));

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        WCDB::Data result = self.handle->getValue();
        TestCaseAssertTrue(result.size() == value.length && memcmp(result.buffer(), value.bytes, result.size()) == 0);
        self.handle->finalize();
    }
}

- (void)test_bind_index
{
    NSString* value = Random.shared.string;
    TestCaseAssertTrue(value != nil);
    {
        WCDB::BindParameter parameter = WCDB::BindParameter::colon("data");
        WCDB::Statement insert = WCDB::StatementInsert()
                                 .insertIntoTable(self.tableName.UTF8String)
                                 .columns({ "identifier", "content" })
                                 .values({ 3, parameter });
        TestCaseAssertTrue(self.handle->prepare(insert));
        self.handle->bindText(value.UTF8String, self.handle->bindParameterIndex(parameter.getDescription()));

        TestCaseAssertTrue(self.handle->step());
        self.handle->finalize();
    }
    {
        TestCaseAssertTrue(self.handle->prepare(self.statementSelect));
        TestCaseAssertTrue(self.handle->step());
        TestCaseAssertCPPStringEqual(self.handle->getText().data(), value.UTF8String);
        self.handle->finalize();
    }
}

@end
