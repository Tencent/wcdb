//
// Created by qiuwenchen(陈秋文) on 2020/10/29.
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

#import "FTS5Object.h"
#import "TestCase.h"
#import <Foundation/Foundation.h>

@interface FTS5Tests : TableTestCase

@end

@implementation FTS5Tests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.tableClass = FTS5Object.class;
    self.isVirtualTable = YES;
    [self.database addTokenizer:WCTTokenizerVerbatim];
    [WCTDatabase configTraditionalChineseDict:@{
        @"們" : @"们",
        @"員" : @"员",
    }];
    TestCaseAssertTrue([self createTable]);

    FTS5Object *dummy = [[FTS5Object alloc] init];
    dummy.content = @"dummy content";
    dummy.extension = @"dummy extension";
    TestCaseAssertTrue([self.table insertObject:dummy]);
}

- (void)test_chinese
{
    FTS5Object *object = [[FTS5Object alloc] init];
    object.content = @"苹果树";
    TestCaseAssertTrue([self.table insertObject:object]);

    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '苹' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("苹")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '苹果' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("苹果")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '苹果树' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("苹果树")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '果树' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("果树")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '树' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("树")];
           }];
}

- (void)test_traditional_chinese
{
    FTS5Object *object = [[FTS5Object alloc] init];
    object.content = @"我們是程序員";
    TestCaseAssertTrue([self.table insertObject:object]);
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '我们是程序员' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("我们是程序员")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '我們是程序員' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("我們是程序員")];
           }];
}

- (void)test_english
{
    FTS5Object *object = [[FTS5Object alloc] init];
    object.content = @"WCDB is a cross-platform database framework developed by WeChat.";
    TestCaseAssertTrue([self.table insertObject:object]);

    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'WCDB' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("WCDB")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'WeChat' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("WeChat")];
           }];

    // case insensive
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'DATABASE' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("DATABASE")];
           }];

    // stemming
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'developer' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("developer")];
           }];

    // prefix
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'plat*' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("plat*")];
           }];

    // phrase
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'dat* fram*' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("dat* fram*")];
           }];

    // bad case
    [self doTestObjects:@[]
                 andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'Hello' ORDER BY rowid ASC"
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return [self.table getObjectsWhere:FTS5Object.content.match("Hello")];
            }];
}

- (void)test_number
{
    FTS5Object *object = [[FTS5Object alloc] init];
    object.content = @"123 456 789";
    TestCaseAssertTrue([self.table insertObject:object]);

    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '123' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("123")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '456' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("456")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '789' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("789")];
           }];

    // bad case
    [self doTestObjects:@[]
                 andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '567' ORDER BY rowid ASC"
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return [self.table getObjectsWhere:FTS5Object.content.match("567")];
            }];
}

- (void)test_mixed
{
    FTS5Object *object = [[FTS5Object alloc] init];
    object.content = @"中文English日本語123";
    TestCaseAssertTrue([self.table insertObject:object]);

    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '中' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("中")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'English' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("English")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '語' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("語")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '123' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("123")];
           }];
}

- (void)test_column_match
{
    FTS5Object *content = [[FTS5Object alloc] init];
    content.content = @"error_prone";
    TestCaseAssertTrue([self.table insertObject:content]);

    FTS5Object *extension = [[FTS5Object alloc] init];
    extension.extension = @"error_prone";
    TestCaseAssertTrue([self.table insertObject:extension]);

    [self doTestObject:content
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'error_prone' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.content.match("error_prone")];
           }];
    [self doTestObject:extension
                andSQL:@"SELECT content, extension FROM main.testTable WHERE extension MATCH 'error_prone' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS5Object.extension.match("error_prone")];
           }];
    [self doTestObjects:@[ content, extension ]
                 andSQL:@"SELECT content, extension FROM main.testTable WHERE testTable MATCH 'error_prone' ORDER BY rowid ASC"
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return [self.table getObjectsWhere:WCDB::Column(self.tableName).match("error_prone")];
            }];
    [self doTestObject:content
                andSQL:@"SELECT content, extension FROM main.testTable WHERE testTable MATCH 'content: error_prone' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:WCDB::Column(self.tableName).match("content: error_prone")];
           }];
    [self doTestObject:extension
                andSQL:@"SELECT content, extension FROM main.testTable WHERE testTable MATCH 'extension: error_prone' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:WCDB::Column(self.tableName).match("extension: error_prone")];
           }];
}

- (void)test_auto_merge
{
    [self.database enableAutoMergeFTS5Index:YES];
    for (int i = 0; i < 14; i++) {
        FTS5Object *object = [[FTS5Object alloc] init];
        object.content = Random.shared.chineseString;
        TestCaseAssertTrue([self.table insertObject:object]);
        WCTValue *count = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("segid").count().distinct()).from([NSString stringWithFormat:@"%@_idx", self.tableName])];
        TestCaseAssertTrue(count.numberValue.intValue == i + 2);
    }

    [NSThread sleepForTimeInterval:2.5];

    WCTValue *count = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("segid").count().distinct()).from([NSString stringWithFormat:@"%@_idx", self.tableName])];
    TestCaseAssertTrue(count.numberValue.intValue == 1);
}

- (void)test_thread_conflict
{
    [self.database enableAutoMergeFTS5Index:YES];
    for (int i = 0; i < 1000; i++) {
        FTS5Object *object = [[FTS5Object alloc] init];
        object.content = Random.shared.chineseString;
        TestCaseAssertTrue([self.table insertObject:object]);
    }
    [NSThread sleepForTimeInterval:1.8];
    int totalCount = 10000;
    __block double maxCost = 0;
    for (int i = 0; i < totalCount; i++) {
        FTS5Object *object = [[FTS5Object alloc] init];
        object.content = Random.shared.chineseString;
        if (i > 0 && i % (totalCount / 10) == 0) {
            TestCaseLog(@"insert progress %d%%", i * 100 / totalCount);
        }
        NSDate *start = [NSDate date];
        [self.database runTransaction:^BOOL(WCTHandle *_Nonnull) {
            maxCost = MAX([[NSDate date] timeIntervalSinceDate:start], maxCost);
            TestCaseAssertTrue([self.table insertObject:object]);
            return YES;
        }];
        [NSThread sleepForTimeInterval:0.0001];
    }
    [NSThread sleepForTimeInterval:10];
    WCTValue *count = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("segid").count().distinct()).from([NSString stringWithFormat:@"%@_idx", self.tableName])];
    TestCaseAssertTrue(count.numberValue.intValue < 8);
    TestCaseAssertTrue(maxCost < 0.02);
}

@end

@interface FTS5PinyinTests : TableTestCase

@end

@implementation FTS5PinyinTests

- (void)setUp
{
    [WCTDatabase configPinYinDict:@{
        @"单" : @[ @"shan", @"dan", @"chan" ],
        @"于" : @[ @"yu" ],
        @"骑" : @[ @"qi" ],
        @"模" : @[ @"mo", @"mu" ],
        @"具" : @[ @"ju" ],
        @"车" : @[ @"che" ],
    }];
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.tableClass = FTS5PinyinObject.class;
    self.isVirtualTable = YES;
    [self.database addTokenizer:WCTTokenizerPinyin];
    TestCaseAssertTrue([self createTable]);

    FTS5PinyinObject *dummy = [[FTS5PinyinObject alloc] init];
    dummy.content = @"测试内容";
    TestCaseAssertTrue([self.table insertObject:dummy]);
}

- (void)test_pinyin
{
    FTS5PinyinObject *content = [[FTS5PinyinObject alloc] init];
    content.content = @"单于骑模具单车";
    TestCaseAssertTrue([self.table insertObject:content]);

    NSArray *querys = @[
        @"\"shan yu qi mu ju dan che\"",
        @"\"chan yu qi mo ju shan che\"",
        @"\"dan yu qi mo ju chan che\"",
        @"\"dan yu qi mu ju ch\"*",
        @"\"dan yu qi mo ju d\"*",
        @"\"s y q m j d c\"",
        @"\"c y q m j s c\"",
        @"\"c y q m j\"",
    ];
    for (NSString *query in querys) {
        [self doTestObject:content
                    andSQL:[NSString stringWithFormat:@"SELECT content FROM main.testTable WHERE content MATCH '%@' ORDER BY rowid ASC", query]
               bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                   return [self.table getObjectsWhere:FTS5PinyinObject.content.match(query)];
               }];
    }
}

@end

@interface FTS5SymbolTests : TableTestCase

@end

@implementation FTS5SymbolTests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.tableClass = FTS5SymbolObject.class;
    self.isVirtualTable = YES;
    [self.database addTokenizer:WCTTokenizerVerbatim];
    [self.database addAuxiliaryFunction:WCTAuxiliaryFunction_SubstringMatchInfo];
    TestCaseAssertTrue([self createTable]);

    FTS5SymbolObject *dummy = [[FTS5SymbolObject alloc] init];
    dummy.content = @"!@#$%^&*(()))_+";
    TestCaseAssertTrue([self.table insertObject:dummy]);
}

- (void)test_symbol
{
    FTS5SymbolObject *content = [[FTS5SymbolObject alloc] init];
    content.content = @"\"#￥%…'  '！@…&*";
    TestCaseAssertTrue([self.table insertObject:content]);
    content = [[FTS5SymbolObject alloc] init];
    content.content = @"！@…&*'  '\"#￥%…";
    TestCaseAssertTrue([self.table insertObject:content]);
    NSArray *querys = @[ @"\"！@…&*'  '\"\"#￥%…\"", @"\"！@…&*'  '\"\"\"*" ];
    for (NSString *query in querys) {
        [self doTestObject:content
                    andSQL:[NSString stringWithFormat:@"SELECT content FROM main.testTable WHERE content MATCH '%@' ORDER BY rowid ASC", [query stringByReplacingOccurrencesOfString:@"'" withString:@"''"]]
               bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                   return [self.table getObjectsWhere:FTS5SymbolObject.content.match(query)];
               }];
    }

    content = [[FTS5SymbolObject alloc] init];
    content.content = @"abc*123_def@567";
    TestCaseAssertTrue([self.table insertObject:content]);
    content = [[FTS5SymbolObject alloc] init];
    content.content = @"abc@123*def_567";
    TestCaseAssertTrue([self.table insertObject:content]);
    querys = @[ @"\"abc@123*def_567\"", @"\"abc@123*def\"*" ];
    for (NSString *query in querys) {
        [self doTestObject:content
                    andSQL:[NSString stringWithFormat:@"SELECT content FROM main.testTable WHERE content MATCH '%@' ORDER BY rowid ASC", query]
               bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                   return [self.table getObjectsWhere:FTS5SymbolObject.content.match(query)];
               }];
    }
}

- (void)test_multi_level_highlight
{
    FTS5SymbolObject *content = [[FTS5SymbolObject alloc] init];
    content.content = @"多级,分隔符,串联子串1;多级,分隔符,串联子串2;多级,分隔符,串联子串3";
    TestCaseAssertTrue([self.table insertObject:content]);
    [self doTestValue:@"1,2;串联;子串2;6;"
               andSQL:@"SELECT substring_match_info(testTable, 0, ';,') FROM main.testTable WHERE content MATCH '子串2'"
          bySelecting:^WCTValue * {
              return [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Expression::function(WCTAuxiliaryFunction_SubstringMatchInfo).invoke().arguments({ WCDB::Column(self.tableName), 0, ";," })).from(self.tableName).where(FTS5SymbolObject.content.match(@"子串2"))];
          }];
}

@end
