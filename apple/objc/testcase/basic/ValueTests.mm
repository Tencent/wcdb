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

#define AUTO_CATEGORY_NS(name)                    \
    @interface WCDB_CONCAT (NS, name)             \
    (ValueTest)                                   \
    - (BOOL) WCDB_CONCAT(test, name);             \
    @end                                          \
    @implementation WCDB_CONCAT (NS, name)        \
    (ValueTest)                                   \
    - (BOOL) WCDB_CONCAT(test, name)              \
    {                                             \
        NSLog(@"%@", NSStringFromSelector(_cmd)); \
        return YES;                               \
    }                                             \
    @end

AUTO_CATEGORY_NS(Number)
AUTO_CATEGORY_NS(Data)
AUTO_CATEGORY_NS(String)

@interface ValueTests : DatabaseTestCase

@property (nonatomic, readonly) NSString* tableName;

@property (nonatomic, readonly) WCDB::Column column;

@property (nonatomic, readonly) NSNumber* number;
@property (nonatomic, readonly) NSString* string;
@property (nonatomic, readonly) NSData* data;

@end

@implementation ValueTests

- (BOOL)insert:(WCTValue*)value
{
    WCTHandle* handle = [self.database getHandle];
    if (handle == nil) {
        return NO;
    }
    BOOL succeed = NO;
    if ([handle prepare:WCDB::StatementInsert()
                        .insertIntoTable(self.tableName)
                        .column(self.column)
                        .value(WCDB::BindParameter(1))]) {
        [handle bindValue:value toIndex:1];
        succeed = [handle step];
        [handle finalizeStatement];
    }
    [handle invalidate];
    return succeed;
}

- (void)setUp
{
    [super setUp];
    _tableName = @"testTable";
    _column = WCDB::Column("any");
    _number = @(123);
    _string = _number.stringValue;
    _data = [_string dataUsingEncoding:NSUTF8StringEncoding];

    TestCaseAssertTrue([self.database execute:WCDB::StatementCreateTable()
                                              .createTable(self.tableName)
                                              .ifNotExists()
                                              .define(WCDB::ColumnDef(self.column))]);
}

- (void)doTestCompatible:(WCTValue*)value withRealType:(Class)cls
{
    TestCaseAssertTrue([value isKindOfClass:cls]);

    TestCaseAssertTrue([value.dataValue isKindOfClass:NSData.class]);
    TestCaseAssertTrue([value.dataValue isEqualToData:self.data]);
    TestCaseAssertTrue([(NSData*) value testData]);

    TestCaseAssertTrue([value.stringValue isKindOfClass:NSString.class]);
    TestCaseAssertTrue([value.stringValue isEqualToString:self.string]);
    TestCaseAssertTrue([(NSString*) value testString]);

    TestCaseAssertTrue([value.numberValue isKindOfClass:NSNumber.class]);
    TestCaseAssertTrue([value.numberValue isEqualToNumber:self.number]);
    TestCaseAssertTrue([(NSNumber*) value testNumber]);
}

- (void)test_compatible_safe_number
{
    NSNumber* data = self.number;
    TestCaseAssertTrue([self insert:data]);

    WCTValue* value = [self.database getValueOnResultColumn:self.column fromTable:self.tableName];
    [self doTestCompatible:value withRealType:NSNumber.class];
}

- (void)test_compatible_safe_string
{
    NSString* data = self.string;
    TestCaseAssertTrue([self insert:data]);

    WCTValue* value = [self.database getValueOnResultColumn:self.column fromTable:self.tableName];
    [self doTestCompatible:value withRealType:NSString.class];
}

- (void)test_compatible_safe_data
{
    NSData* data = self.data;
    TestCaseAssertTrue([self insert:data]);

    WCTValue* value = [self.database getValueOnResultColumn:self.column fromTable:self.tableName];
    [self doTestCompatible:value withRealType:NSData.class];
}

- (void)test_compatible_safe_null
{
    TestCaseAssertTrue([self insert:nil]);

    WCTValue* value = [self.database getColumnOnResultColumn:self.column fromTable:self.tableName].firstObject;

    TestCaseAssertTrue([value isKindOfClass:NSNull.class]);

    TestCaseAssertEqual(value.dataValue, nil);
    TestCaseAssertTrue([(NSData*) value testData]);

    TestCaseAssertEqual(value.stringValue, nil);
    TestCaseAssertTrue([(NSString*) value testString]);

    TestCaseAssertEqual(value.numberValue, nil);
    TestCaseAssertTrue([(NSNumber*) value testNumber]);
}

@end
