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

#import "FTS3Object+WCTTableCoding.h"
#import "FTS3Object.h"
#import "TestCase.h"

@interface FTS3Tests : TableTestCase

@end

@implementation FTS3Tests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.tableClass = FTS3Object.class;
    self.isVirtualTable = YES;
    [self.database addTokenizer:WCTTokenizerOneOrBinary];
    [WCTDatabase configTraditionalChineseDict:@{
        @"們" : @"们",
        @"員" : @"员",
    }];
    TestCaseAssertTrue([self createTable]);

    FTS3Object *dummy = [[FTS3Object alloc] init];
    dummy.content = @"dummy content";
    dummy.extension = @"dummy extension";
    TestCaseAssertTrue([self.table insertObject:dummy]);
}

- (void)test_chinese
{
    FTS3Object *object = [[FTS3Object alloc] init];
    object.content = @"苹果树";
    TestCaseAssertTrue([self.table insertObject:object]);

    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '苹' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("苹")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '苹果' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("苹果")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '苹果树' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("苹果树")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '果树' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("果树")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '树' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("树")];
           }];
}

- (void)test_traditional_chinese
{
    FTS3Object *object = [[FTS3Object alloc] init];
    object.content = @"我們是程序員";
    TestCaseAssertTrue([self.table insertObject:object]);
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '我們是程序員' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("我們是程序員")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '我们是程序员' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("我们是程序员")];
           }];
}

- (void)test_english
{
    FTS3Object *object = [[FTS3Object alloc] init];
    object.content = @"WCDB is a cross-platform database framework developed by WeChat.";
    TestCaseAssertTrue([self.table insertObject:object]);

    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'WCDB' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("WCDB")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'WeChat' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("WeChat")];
           }];

    // case insensive
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'DATABASE' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("DATABASE")];
           }];

    // stemming
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'developer' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("developer")];
           }];

    // prefix
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'plat*' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("plat*")];
           }];

    // phrase
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'dat* fram*' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("dat* fram*")];
           }];

    // bad case
    [self doTestObjects:@[]
                 andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'Hello' ORDER BY rowid ASC"
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return [self.table getObjectsWhere:FTS3Object.content.match("Hello")];
            }];
}

- (void)test_number
{
    FTS3Object *object = [[FTS3Object alloc] init];
    object.content = @"123 456 789";
    TestCaseAssertTrue([self.table insertObject:object]);

    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '123' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("123")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '456' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("456")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '789' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("789")];
           }];

    // bad case
    [self doTestObjects:@[]
                 andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '567' ORDER BY rowid ASC"
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return [self.table getObjectsWhere:FTS3Object.content.match("567")];
            }];
}

- (void)test_mixed
{
    FTS3Object *object = [[FTS3Object alloc] init];
    object.content = @"中文English日本語123";
    TestCaseAssertTrue([self.table insertObject:object]);

    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '中' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("中")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'English' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("English")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '語' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("語")];
           }];
    [self doTestObject:object
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '123' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("123")];
           }];
}

- (void)test_column_match
{
    FTS3Object *content = [[FTS3Object alloc] init];
    content.content = @"error_prone";
    TestCaseAssertTrue([self.table insertObject:content]);

    FTS3Object *extension = [[FTS3Object alloc] init];
    extension.extension = @"error_prone";
    TestCaseAssertTrue([self.table insertObject:extension]);

    [self doTestObject:content
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'error_prone' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("error_prone")];
           }];
    [self doTestObject:extension
                andSQL:@"SELECT content, extension FROM main.testTable WHERE extension MATCH 'error_prone' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.extension.match("error_prone")];
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

- (void)test_near_query
{
    FTS3Object *objectWithOneGaps = [[FTS3Object alloc] init];
    objectWithOneGaps.content = @"Cowherd one WeaverGirl";
    TestCaseAssertTrue([self.table insertObject:objectWithOneGaps]);

    FTS3Object *objectWithTenGaps = [[FTS3Object alloc] init];
    objectWithTenGaps.content = @"Cowherd one two three four five six seven eight nine ten WeaverGirl";
    TestCaseAssertTrue([self.table insertObject:objectWithTenGaps]);

    FTS3Object *objectWithEleventGaps = [[FTS3Object alloc] init];
    objectWithEleventGaps.content = @"Cowherd one two three four five six seven eight nine ten eleven WeaverGirl";
    TestCaseAssertTrue([self.table insertObject:objectWithEleventGaps]);

    [self doTestObject:objectWithOneGaps
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'Cowherd NEAR/1 WeaverGirl' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("Cowherd NEAR/1 WeaverGirl")];
           }];
    [self doTestObjects:@[ objectWithOneGaps, objectWithTenGaps ]
                 andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'Cowherd NEAR/10 WeaverGirl' ORDER BY rowid ASC"
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return [self.table getObjectsWhere:FTS3Object.content.match("Cowherd NEAR/10 WeaverGirl")];
            }];
    [self doTestObjects:@[ objectWithOneGaps, objectWithTenGaps, objectWithEleventGaps ]
                 andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'Cowherd NEAR/11 WeaverGirl' ORDER BY rowid ASC"
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return [self.table getObjectsWhere:FTS3Object.content.match("Cowherd NEAR/11 WeaverGirl")];
            }];

    // default behaviours
    [self doTestObjects:@[ objectWithOneGaps, objectWithTenGaps ]
                 andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'Cowherd NEAR WeaverGirl' ORDER BY rowid ASC"
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return [self.table getObjectsWhere:FTS3Object.content.match("Cowherd NEAR WeaverGirl")];
            }];
}

- (void)test_operator
{
    FTS3Object *ab = [[FTS3Object alloc] init];
    ab.content = @"a b";
    TestCaseAssertTrue([self.table insertObject:ab]);

    FTS3Object *bc = [[FTS3Object alloc] init];
    bc.content = @"b c";
    TestCaseAssertTrue([self.table insertObject:bc]);

    FTS3Object *cd = [[FTS3Object alloc] init];
    cd.content = @"c d";
    TestCaseAssertTrue([self.table insertObject:cd]);

    [self doTestObject:bc
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'b AND c' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("b AND c")];
           }];
    [self doTestObjects:@[ ab, bc, cd ]
                 andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'b OR c' ORDER BY rowid ASC"
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return [self.table getObjectsWhere:FTS3Object.content.match("b OR c")];
            }];
    [self doTestObject:ab
                andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'b NOT c' ORDER BY rowid ASC"
           bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
               return [self.table getObjectsWhere:FTS3Object.content.match("b NOT c")];
           }];
}

- (void)test_offsets
{
    FTS3Object *object = [[FTS3Object alloc] init];
    object.content = @"one two three four";
    object.extension = @"five six seven eight nine ten";
    TestCaseAssertTrue([self.table insertObject:object]);

    // The return value of [offsets] is grouped by every four numbers.
    // 1. The serial number of matched column in the table
    // 2. The serial number of matching term in the query
    // 3. The byte offset of matched term
    // 4. The byte length of matched term
    // In conclusion, every group show that the word at offset [3.] length [4.] in the column [1.] is matching the query [2.].

    // Note that offsets interface is not recommended since the column order may be undefined due to ORM.

    [self doTestValue:
          @"0 1 4 3 "  // The word at offset 4 length 3 in the column [content] is matching the query [t*], which is "two".
           "0 1 8 5 "  // The word at offset 8 length 5 in the column [content] is matching the query [t*], which is "three".
           "0 0 14 4 " // The word at offset 14 length 4 in the column [content] is matching the query [f*], which is "four".
           "1 0 0 4 "  // The word at offset 0 length 4 in the column [extension] is matching the query [f*], which is "five".
           "1 1 26 3"  // The word at offset 26 length 3 in the column [extension] is matching the query [t*], which is "ten".
               andSQL:@"SELECT offsets(testTable) FROM main.testTable WHERE testTable MATCH 'f* t*' ORDER BY rowid ASC"
          bySelecting:^WCTValue * {
              WCDB::Column tableColumn(self.tableName);
              return [self.database getValueFromStatement:WCDB::StatementSelect().select(tableColumn.offsets()).from(self.tableName).where(tableColumn.match(@"f* t*")).order(WCDB::OrderingTerm::ascendingRowid())];
          }];
}

- (void)test_snippet
{
    FTS3Object *easyToUse = [[FTS3Object alloc] init];
    easyToUse.content = @"Easy-to-use";
    easyToUse.extension = @"Through WCDB, you can get objects from database in one line code.";
    TestCaseAssertTrue([self.table insertObject:easyToUse]);

    FTS3Object *efficient = [[FTS3Object alloc] init];
    efficient.content = @"Efficient";
    efficient.extension = @"Through the framework layer and sqlcipher source optimization, WCDB have more efficient performance.";
    TestCaseAssertTrue([self.table insertObject:efficient]);

    FTS3Object *complete = [[FTS3Object alloc] init];
    complete.content = @"Complete";
    complete.extension = @"Encryption Support, Corruption recovery, Anti-injection.";
    TestCaseAssertTrue([self.table insertObject:complete]);

    [self doTestColumn:@[ @"...WCDB have more /* efficient */ performance.",
                          @"/* Encryption */ Support, Corruption recovery, Anti..." ]
                andSQL:@"SELECT snippet(testTable, '/* ', ' */', '...', -1, 5) FROM main.testTable WHERE extension MATCH 'e*' ORDER BY rowid ASC"
           bySelecting:^WCTOneColumn * {
               const char *startMatchText = "/* ";
               const char *endMatchText = " */";
               const char *ellipses = "...";
               int columnNumber = -1;
               int approximateNumberOfTokens = 5;
               return [self.database getColumnFromStatement:WCDB::StatementSelect().select(WCDB::Column(self.tableName).snippet(startMatchText, endMatchText, ellipses, columnNumber, approximateNumberOfTokens)).from(self.tableName).where(FTS3Object.extension.match("e*")).order(WCDB::OrderingTerm::ascendingRowid())];
           }];
}

@end

@interface FTS3SymbolTests : TableTestCase

@end

@implementation FTS3SymbolTests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.tableClass = FTS3SymbolObject.class;
    self.isVirtualTable = YES;
    [self.database addTokenizer:WCTTokenizerOneOrBinary];
    TestCaseAssertTrue([self createTable]);

    FTS3SymbolObject *dummy = [[FTS3SymbolObject alloc] init];
    dummy.content = @"!@#$%^&*(()))_+";
    TestCaseAssertTrue([self.table insertObject:dummy]);
}

- (void)test_symbol
{
    FTS3SymbolObject *content = [[FTS3SymbolObject alloc] init];
    content.content = @"\"#￥%…'  '！@…&*";
    TestCaseAssertTrue([self.table insertObject:content]);
    content = [[FTS3SymbolObject alloc] init];
    content.content = @"！@…&*'  '\"#￥%…";
    TestCaseAssertTrue([self.table insertObject:content]);
    NSArray *querys = @[ @"\"！@…&*'  '\"\"#￥%…\"", @"\"！@…&*'  '\"\"*\"" ];
    for (NSString *query in querys) {
        [self doTestObject:content
                    andSQL:[NSString stringWithFormat:@"SELECT content FROM main.testTable WHERE content MATCH '%@' ORDER BY rowid ASC", [query stringByReplacingOccurrencesOfString:@"'" withString:@"''"]]
               bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                   return [self.table getObjectsWhere:FTS3SymbolObject.content.match(query)];
               }];
    }

    content = [[FTS3SymbolObject alloc] init];
    content.content = @"abc*123_def@567";
    TestCaseAssertTrue([self.table insertObject:content]);
    content = [[FTS3SymbolObject alloc] init];
    content.content = @"abc@123*def_567";
    TestCaseAssertTrue([self.table insertObject:content]);
    querys = @[ @"\"abc@123*def_567\"", @"\"abc@123*def\"*" ];
    for (NSString *query in querys) {
        [self doTestObject:content
                    andSQL:[NSString stringWithFormat:@"SELECT content FROM main.testTable WHERE content MATCH '%@' ORDER BY rowid ASC", query]
               bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                   return [self.table getObjectsWhere:FTS3SymbolObject.content.match(query)];
               }];
    }
}

@end
