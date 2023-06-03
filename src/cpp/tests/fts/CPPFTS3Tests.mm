//
// Created by qiuwenchen on 2022/10/29.
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

#import "CPPFTS3Object.h"
#import "CPPTestCase.h"
#import <Foundation/Foundation.h>

@interface CPPFTS3Tests : CPPTableTestCase {
    std::shared_ptr<WCDB::Table<CPPFTS3Object>> m_ftsTable;
}

@property (nonatomic, readonly) WCDB::Table<CPPFTS3Object> &ftsTable;

@end

@implementation CPPFTS3Tests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.database->addTokenizer(WCDB::BuiltinTokenizer::OneOrBinary);
    WCDB::Database::configSymbolDetector([](WCDB::Database::UnicodeChar theChar) {
        if (theChar < 0xC0) {
            if (!(theChar >= 0x30 && theChar <= 0x39) && !((theChar >= 0x41 && theChar <= 0x5a) || (theChar >= 0x61 && theChar <= 0x7a))) {
                return true;
            }
        }
        return false;
    });
    WCDB::Database::configTraditionalChineseConverter([](const WCDB::UnsafeStringView &token) {
        if (token.compare("們") == 0) {
            return WCDB::StringView("们");
        } else if (token.compare("員") == 0) {
            return WCDB::StringView("员");
        }
        return WCDB::StringView(token);
    });
    TestCaseAssertTrue(self.database->createVirtualTable<CPPFTS3Object>(self.tableName.UTF8String));
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS3Object>(CPPFTS3Object("dummy content", "dummy extension"), self.tableName.UTF8String));
}

- (WCDB::Table<CPPFTS3Object> &)ftsTable
{
    if (!m_ftsTable) {
        m_ftsTable = std::make_shared<WCDB::Table<CPPFTS3Object>>(self.database->getTable<CPPFTS3Object>(self.tableName.UTF8String));
    }
    return *m_ftsTable.get();
}

- (void)test_chinese
{
    CPPFTS3Object object("苹果树", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '苹' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("苹")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '苹果' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("苹果")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '苹果树' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("苹果树")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '果树' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("果树")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '树' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("树")).value());
         }];
}

- (void)test_traditional_chinese
{
    CPPFTS3Object object("我們是程序員", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '我們是程序員' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("我們是程序員")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '我们是程序员' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("我们是程序员")).value());
         }];
}

- (void)test_english
{
    CPPFTS3Object object("WCDB is a cross-platform database framework developed by WeChat.", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));

    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'WCDB' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("WCDB")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'WeChat' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("WeChat")).value());
         }];

    // case insensitive
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'DATABASE' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("DATABASE")).value());
         }];

    // stemming
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'developer' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("developer")).value());
         }];

    // prefix
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'plat*' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("plat*")).value());
         }];

    // phrase
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'dat* fram*' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("dat* fram*")).value());
         }];

    // bad case
    [self doTestRows:{}
    andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'Hello' ORDER BY rowid ASC"
    bySelecting:^WCDB::OptionalMultiRows {
        return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("Hello")).value());
    }];
}

- (void)test_number
{
    CPPFTS3Object object("123 456 789", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));

    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '123' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("123")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '456' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("456")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '789' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("789")).value());
         }];

    // bad case
    [self doTestRows:{}
    andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '567' ORDER BY rowid ASC"
    bySelecting:^WCDB::OptionalMultiRows {
        return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("567")).value());
    }];
}

- (void)test_mixed
{
    CPPFTS3Object object("中文English日本語123", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));

    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '中' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("中")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'English' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("English")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '語' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("語")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH '123' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("123")).value());
         }];
}

- (void)test_column_match
{
    CPPFTS3Object content("error_prone", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(content));

    CPPFTS3Object extension("", "error_prone");
    TestCaseAssertTrue(self.ftsTable.insertObjects(extension));

    [self doTestRows:{ CPPOneRowValueExtract(content) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("error_prone")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(extension) }
              andSQL:@"SELECT content, extension FROM testTable WHERE extension MATCH 'error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::extension).match("error_prone")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(content), CPPOneRowValueExtract(extension) }
              andSQL:@"SELECT content, extension FROM testTable WHERE testTable MATCH 'error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB::Column(self.tableName.UTF8String).match("error_prone")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(content) }
              andSQL:@"SELECT content, extension FROM testTable WHERE testTable MATCH 'content: error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB::Column(self.tableName.UTF8String).match("content: error_prone")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(extension) }
              andSQL:@"SELECT content, extension FROM testTable WHERE testTable MATCH 'extension: error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB::Column(self.tableName.UTF8String).match("extension: error_prone")).value());
         }];
}

- (void)test_near_query
{
    CPPFTS3Object objectWithOneGaps("Cowherd one WeaverGirl", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(objectWithOneGaps));

    CPPFTS3Object objectWithTenGaps("Cowherd one two three four five six seven eight nine ten WeaverGirl", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(objectWithTenGaps));

    CPPFTS3Object objectWithEleventGaps("Cowherd one two three four five six seven eight nine ten eleven WeaverGirl", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(objectWithEleventGaps));

    [self doTestRows:{ CPPOneRowValueExtract(objectWithOneGaps) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'Cowherd NEAR/1 WeaverGirl' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("Cowherd NEAR/1 WeaverGirl")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(objectWithOneGaps), CPPOneRowValueExtract(objectWithTenGaps) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'Cowherd NEAR/10 WeaverGirl' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("Cowherd NEAR/10 WeaverGirl")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(objectWithOneGaps), CPPOneRowValueExtract(objectWithTenGaps), CPPOneRowValueExtract(objectWithEleventGaps) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'Cowherd NEAR/11 WeaverGirl' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("Cowherd NEAR/11 WeaverGirl")).value());
         }];

    // default behaviours
    [self doTestRows:{ CPPOneRowValueExtract(objectWithOneGaps), CPPOneRowValueExtract(objectWithTenGaps) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'Cowherd NEAR WeaverGirl' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("Cowherd NEAR WeaverGirl")).value());
         }];
}

- (void)test_operator
{
    CPPFTS3Object ab("a b", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(ab));

    CPPFTS3Object bc("b c", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(bc));

    CPPFTS3Object cd("c d", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(cd));

    [self doTestRows:{ CPPOneRowValueExtract(bc) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'b AND c' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("b AND c")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(ab), CPPOneRowValueExtract(bc), CPPOneRowValueExtract(cd) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'b OR c' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("b OR c")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(ab) }
              andSQL:@"SELECT content, extension FROM testTable WHERE content MATCH 'b NOT c' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS3Object::content).match("b NOT c")).value());
         }];
}

- (void)test_offsets
{
    CPPFTS3Object object("one two three four", "five six seven eight nine ten");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));

    // The return value of [offsets] is grouped by every four numbers.
    // 1. The serial number of matched column in the table
    // 2. The serial number of matching term in the query
    // 3. The byte offset of matched term
    // 4. The byte length of matched term
    // In conclusion, every group show that the word at offset [3.] length [4.] in the column [1.] is matching the query [2.].

    // Note that offsets interface is not recommended since the column order may be undefined due to ORM.

    [self doTestValue:
          "0 1 4 3 "  // The word at offset 4 length 3 in the column [content] is matching the query [t*], which is "two".
          "0 1 8 5 "  // The word at offset 8 length 5 in the column [content] is matching the query [t*], which is "three".
          "0 0 14 4 " // The word at offset 14 length 4 in the column [content] is matching the query [f*], which is "four".
          "1 0 0 4 "  // The word at offset 0 length 4 in the column [extension] is matching the query [f*], which is "five".
          "1 1 26 3"  // The word at offset 26 length 3 in the column [extension] is matching the query [t*], which is "ten".
               andSQL:@"SELECT offsets(testTable) FROM testTable WHERE testTable MATCH 'f* t*' ORDER BY rowid ASC"
          bySelecting:^WCDB::OptionalValue {
              WCDB::Column tableColumn(self.tableName.UTF8String);
              return self.database->getValueFromStatement(WCDB::StatementSelect().select(tableColumn.offsets()).from(self.tableName.UTF8String).where(tableColumn.match("f* t*")).order(WCDB::OrderingTerm::ascendingRowid()));
          }];
}

//- (void)test_snippet
//{
//    CPPFTS3Object easyToUse("Easy-to-use", "Through WCDB, you can get objects from database in one line code.");
//    TestCaseAssertTrue(self.ftsTable.insertObjects(easyToUse));
//
//    CPPFTS3Object efficient("Efficient", "Through the framework layer and sqlcipher source optimization, WCDB have more efficient performance.");
//    TestCaseAssertTrue(self.ftsTable.insertObjects(efficient));
//
//    CPPFTS3Object complete("Complete", "Encryption Support, Corruption recovery, Anti-injection.");
//    TestCaseAssertTrue(self.ftsTable.insertObjects(complete));
//
//    [self doTestColumn:{ "dummy /* extension */"
//                         "...WCDB have more /* efficient */ performance.",
//                         "/* Encryption */ Support, Corruption recovery..." }
//                andSQL:@"SELECT snippet(testTable, '/* ', ' */', '...', -1, 5) FROM testTable WHERE extension MATCH 'e*' ORDER BY rowid ASC"
//           bySelecting:^WCDB::OptionalOneColumn {
//               const char *startMatchText = "/* ";
//               const char *endMatchText = " */";
//               const char *ellipses = "...";
//               int columnNumber = -1;
//               int approximateNumberOfTokens = 5;
//               return self.database->getOneColumnFromStatement(WCDB::StatementSelect().select(WCDB::Column(self.tableName.UTF8String).snippet(startMatchText, endMatchText, ellipses, columnNumber, approximateNumberOfTokens)).from(self.tableName.UTF8String).where(WCDB_FIELD(CPPFTS3Object::extension).match("e*")).order(WCDB::OrderingTerm::ascendingRowid()));
//           }];
//}

@end

@interface CPPFTS3SymbolTests : CPPTableTestCase

@end

@implementation CPPFTS3SymbolTests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.database->addTokenizer(WCDB::BuiltinTokenizer::OneOrBinary);
    WCDB::Database::configSymbolDetector([](WCDB::Database::UnicodeChar theChar) {
        if (theChar < 0xC0) {
            if (!(theChar >= 0x30 && theChar <= 0x39) && !((theChar >= 0x41 && theChar <= 0x5a) || (theChar >= 0x61 && theChar <= 0x7a))) {
                return true;
            }
        }
        return false;
    });
    TestCaseAssertTrue(self.database->createVirtualTable<CPPFTS3SymbolObject>(self.tableName.UTF8String));
    CPPFTS3SymbolObject object;
    object.content = "!@#$%^&*(()))_+";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS3SymbolObject>(object, self.tableName.UTF8String));
}

- (void)test_symbol
{
    CPPFTS3SymbolObject content;
    content.content = "#￥%…'  '！@…&*";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS3SymbolObject>(content, self.tableName.UTF8String));

    content.content = "！@…&*'  '\"#￥%…";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS3SymbolObject>(content, self.tableName.UTF8String));

    NSArray *querys = @[ @"！@…&*'  '\"\"#￥%…", @"！@…&*'  '\"\"" ];
    for (NSString *query in querys) {
        [self doTestRows:{ CPPOneRowValueExtract(content) }
                  andSQL:[NSString stringWithFormat:@"SELECT content FROM testTable WHERE content MATCH '%@' ORDER BY rowid ASC", [query stringByReplacingOccurrencesOfString:@"'" withString:@"''"]]
             bySelecting:^WCDB::OptionalMultiRows {
                 return CPPMultiRowValueExtract(self.database->getAllObjects<CPPFTS3SymbolObject>(self.tableName.UTF8String, WCDB_FIELD(CPPFTS3SymbolObject::content).match(query.UTF8String)).value());
             }];
    }

    content.content = "abc*123_@567";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS3SymbolObject>(content, self.tableName.UTF8String));

    content.content = "abc@123*def_567";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS3SymbolObject>(content, self.tableName.UTF8String));

    querys = @[ @"abc@123*def_567", @"abc@123*def" ];
    for (NSString *query in querys) {
        [self doTestRows:{ CPPOneRowValueExtract(content) }
                  andSQL:[NSString stringWithFormat:@"SELECT content FROM testTable WHERE content MATCH '%@' ORDER BY rowid ASC", query]
             bySelecting:^WCDB::OptionalMultiRows {
                 return CPPMultiRowValueExtract(self.database->getAllObjects<CPPFTS3SymbolObject>(self.tableName.UTF8String, WCDB_FIELD(CPPFTS3SymbolObject::content).match(query.UTF8String)).value());
             }];
    }
}

@end
