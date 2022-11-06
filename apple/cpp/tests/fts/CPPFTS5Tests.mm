//
// Created by 陈秋文 on 2022/10/30.
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

#import "CPPFTS5Object.h"
#import "CPPTestCase.h"
#import <Foundation/Foundation.h>

@interface CPPFTS5Tests : CPPTableTestCase {
    std::shared_ptr<WCDB::Table<CPPFTS5Object>> m_ftsTable;
}

@property (nonatomic, readonly) WCDB::Table<CPPFTS5Object> &ftsTable;

@end

@implementation CPPFTS5Tests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.database->addTokenizer(WCDB::TokenizerVerbatim);
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
    TestCaseAssertTrue(self.database->createVirtualTable<CPPFTS5Object>(self.tableName.UTF8String));
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS5Object>(CPPFTS5Object("dummy content", "dummy extension"), self.tableName.UTF8String));
}

- (WCDB::Table<CPPFTS5Object> &)ftsTable
{
    if (!m_ftsTable) {
        m_ftsTable = std::make_shared<WCDB::Table<CPPFTS5Object>>(self.database->getTable<CPPFTS5Object>(self.tableName.UTF8String));
    }
    return *m_ftsTable.get();
}

- (void)test_chinese
{
    CPPFTS5Object object("苹果树", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '苹' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("苹")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '苹果' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("苹果")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '苹果树' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("苹果树")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '果树' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("果树")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '树' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("树")).value());
         }];
}

- (void)test_traditional_chinese
{
    CPPFTS5Object object("我們是程序員", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '我們是程序員' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("我們是程序員")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '我们是程序员' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("我们是程序员")).value());
         }];
}

- (void)test_english
{
    CPPFTS5Object object("WCDB is a cross-platform database framework developed by WeChat.", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));

    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'WCDB' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("WCDB")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'WeChat' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("WeChat")).value());
         }];

    // case insensive
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'DATABASE' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("DATABASE")).value());
         }];

    // stemming
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'developer' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("developer")).value());
         }];

    // prefix
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'plat*' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("plat*")).value());
         }];

    // phrase
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'dat* fram*' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("dat* fram*")).value());
         }];

    // bad case
    [self doTestRows:{}
    andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'Hello' ORDER BY rowid ASC"
    bySelecting:^WCDB::OptionalMultiRows {
        return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("Hello")).value());
    }];
}

- (void)test_number
{
    CPPFTS5Object object("123 456 789", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));

    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '123' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("123")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '456' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("456")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '789' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("789")).value());
         }];

    // bad case
    [self doTestRows:{}
    andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '567' ORDER BY rowid ASC"
    bySelecting:^WCDB::OptionalMultiRows {
        return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("567")).value());
    }];
}

- (void)test_mixed
{
    CPPFTS5Object object("中文English日本語123", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(object));

    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '中' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("中")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'English' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("English")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '語' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("語")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(object) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH '123' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("123")).value());
         }];
}

- (void)test_column_match
{
    CPPFTS5Object content("error_prone", "");
    TestCaseAssertTrue(self.ftsTable.insertObjects(content));

    CPPFTS5Object extension("", "error_prone");
    TestCaseAssertTrue(self.ftsTable.insertObjects(extension));

    [self doTestRows:{ CPPOneRowValueExtract(content) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE content MATCH 'error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::content).match("error_prone")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(extension) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE extension MATCH 'error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB_FIELD(CPPFTS5Object::extension).match("error_prone")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(content), CPPOneRowValueExtract(extension) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE testTable MATCH 'error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB::Column(self.tableName.UTF8String).match("error_prone")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(content) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE testTable MATCH 'content: error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB::Column(self.tableName.UTF8String).match("content: error_prone")).value());
         }];
    [self doTestRows:{ CPPOneRowValueExtract(extension) }
              andSQL:@"SELECT content, extension FROM main.testTable WHERE testTable MATCH 'extension: error_prone' ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return CPPMultiRowValueExtract(self.ftsTable.getAllObjects(WCDB::Column(self.tableName.UTF8String).match("extension: error_prone")).value());
         }];
}

- (void)test_auto_merge
{
    self.database->enableAutoMergeFTS5Index(true);
    for (int i = 0; i < 14; i++) {
        CPPFTS5Object object(Random.shared.chineseString.UTF8String, "");
        TestCaseAssertTrue(self.ftsTable.insertObjects(object));
        WCDB::OptionalValue count = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Column("segid").count().distinct()).from(WCDB::StringView::formatted("%s_idx", self.tableName.UTF8String)));
        TestCaseAssertTrue(count.has_value());
        TestCaseAssertTrue(count.value() == i + 2);
    }

    [NSThread sleepForTimeInterval:2.5];

    WCDB::OptionalValue count = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Column("segid").count().distinct()).from(WCDB::StringView::formatted("%s_idx", self.tableName.UTF8String)));
    TestCaseAssertTrue(count.has_value());
    TestCaseAssertTrue(count.value() == 1);
}

- (void)test_thread_conflict
{
    self.database->enableAutoMergeFTS5Index(true);
    for (int i = 0; i < 1000; i++) {
        CPPFTS5Object object(Random.shared.chineseString.UTF8String, "");
        TestCaseAssertTrue(self.ftsTable.insertObjects(object));
    }
    [NSThread sleepForTimeInterval:1.8];
    int totalCount = 10000;
    double maxCost = 0;
    for (int i = 0; i < totalCount; i++) {
        if (i > 0 && i % (totalCount / 10) == 0) {
            TestCaseLog(@"insert progress %d%%", i * 100 / totalCount);
        }
        NSDate *start = [NSDate date];
        self.database->runTransaction([&](WCDB::Handle &) {
            maxCost = MAX([[NSDate date] timeIntervalSinceDate:start], maxCost);
            CPPFTS5Object object(Random.shared.chineseString.UTF8String, "");
            TestCaseAssertTrue(self.ftsTable.insertObjects(object));
            return true;
        });
        [NSThread sleepForTimeInterval:0.0001];
    }
    [NSThread sleepForTimeInterval:10];
    WCDB::OptionalValue count = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Column("segid").count().distinct()).from(WCDB::StringView::formatted("%s_idx", self.tableName.UTF8String)));
    TestCaseAssertTrue(count.has_value());
    TestCaseAssertTrue(count.value().intValue() < 8);
    TestCaseAssertTrue(maxCost < 0.1);
}

@end

@interface CPPFTS5PinyinTests : CPPTableTestCase

@end

@implementation CPPFTS5PinyinTests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    std::map<WCDB::UnsafeStringView, std::vector<WCDB::UnsafeStringView>> pinyinDict = {
        { "单", { "shan", "dan", "chan" } },
        { "于", { "yu" } },
        { "骑", { "qi" } },
        { "模", { "mo", "mu" } },
        { "具", { "ju" } },
        { "车", { "che" } }
    };
    WCDB::Database::configPinyinConverter([=](const WCDB::UnsafeStringView &token) {
        if (pinyinDict.find(token) == pinyinDict.end()) {
            return std::vector<WCDB::StringView>();
        }
        auto pinyins = pinyinDict.at(token);
        std::vector<WCDB::StringView> result;
        for (auto pinyin : pinyins) {
            result.emplace_back(pinyin);
        }
        return result;
    });
    WCDB::Database::configSymbolDetector([](WCDB::Database::UnicodeChar theChar) {
        if (theChar < 0xC0) {
            if (!(theChar >= 0x30 && theChar <= 0x39) && !((theChar >= 0x41 && theChar <= 0x5a) || (theChar >= 0x61 && theChar <= 0x7a))) {
                return true;
            }
        }
        return false;
    });
    self.database->addTokenizer(WCDB::TokenizerPinyin);

    TestCaseAssertTrue(self.database->createVirtualTable<CPPFTS5PinyinObject>(self.tableName.UTF8String));
    CPPFTS5PinyinObject object;
    object.content = "测试内容";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS5PinyinObject>(object, self.tableName.UTF8String));
}

- (void)test_pinyin
{
    CPPFTS5PinyinObject content;
    content.content = "单于骑模具单车";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS5PinyinObject>(content, self.tableName.UTF8String));

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
        [self doTestRows:{ CPPOneRowValueExtract(content) }
                  andSQL:[NSString stringWithFormat:@"SELECT content FROM main.testTable WHERE content MATCH '%@' ORDER BY rowid ASC", query]
             bySelecting:^WCDB::OptionalMultiRows {
                 return CPPMultiRowValueExtract(self.database->getAllObjects<CPPFTS5PinyinObject>(self.tableName.UTF8String, WCDB_FIELD(CPPFTS5PinyinObject::content).match(query.UTF8String)).value());
             }];
    }
}

@end

@interface CPPFTS5SymbolTests : CPPTableTestCase

@end

@implementation CPPFTS5SymbolTests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.database->addTokenizer(WCDB::TokenizerVerbatim);
    self.database->addAuxiliaryFunction(WCDB::AuxiliaryFunction_SubstringMatchInfo);
    WCDB::Database::configSymbolDetector([](WCDB::Database::UnicodeChar theChar) {
        if (theChar < 0xC0) {
            if (!(theChar >= 0x30 && theChar <= 0x39) && !((theChar >= 0x41 && theChar <= 0x5a) || (theChar >= 0x61 && theChar <= 0x7a))) {
                return true;
            }
        }
        return false;
    });
    TestCaseAssertTrue(self.database->createVirtualTable<CPPFTS5SymbolObject>(self.tableName.UTF8String));
    CPPFTS5SymbolObject object;
    object.content = "!@#$%^&*(()))_+";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS5SymbolObject>(object, self.tableName.UTF8String));
}

- (void)test_symbol
{
    CPPFTS5SymbolObject content;
    content.content = "#￥%…'  '！@…&*";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS5SymbolObject>(content, self.tableName.UTF8String));

    content.content = "！@…&*'  '\"#￥%…";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS5SymbolObject>(content, self.tableName.UTF8String));

    NSArray *querys = @[ @"\"！@…&*'  '\"\"#￥%…\"", @"\"！@…&*'  '\"\"\"*" ];
    for (NSString *query in querys) {
        [self doTestRows:{ CPPOneRowValueExtract(content) }
                  andSQL:[NSString stringWithFormat:@"SELECT content FROM main.testTable WHERE content MATCH '%@' ORDER BY rowid ASC", [query stringByReplacingOccurrencesOfString:@"'" withString:@"''"]]
             bySelecting:^WCDB::OptionalMultiRows {
                 return CPPMultiRowValueExtract(self.database->getAllObjects<CPPFTS5SymbolObject>(self.tableName.UTF8String, WCDB_FIELD(CPPFTS5SymbolObject::content).match(query.UTF8String)).value());
             }];
    }

    content.content = "abc*123_@567";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS5SymbolObject>(content, self.tableName.UTF8String));

    content.content = "abc@123*def_567";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS5SymbolObject>(content, self.tableName.UTF8String));

    querys = @[ @"\"abc@123*def_567\"", @"\"abc@123*def\"*" ];
    for (NSString *query in querys) {
        [self doTestRows:{ CPPOneRowValueExtract(content) }
                  andSQL:[NSString stringWithFormat:@"SELECT content FROM main.testTable WHERE content MATCH '%@' ORDER BY rowid ASC", query]
             bySelecting:^WCDB::OptionalMultiRows {
                 return CPPMultiRowValueExtract(self.database->getAllObjects<CPPFTS5SymbolObject>(self.tableName.UTF8String, WCDB_FIELD(CPPFTS5SymbolObject::content).match(query.UTF8String)).value());
             }];
    }
}

- (void)test_substring_match_info
{
    CPPFTS5SymbolObject content;
    content.content = "多级,分隔符,串联子串1;多级,分隔符,串联子串2;多级,分隔符,串联子串3";
    TestCaseAssertTrue(self.database->insertObjects<CPPFTS5SymbolObject>(content, self.tableName.UTF8String));
    [self doTestValue:"1,2;串联;子串2;6;"
               andSQL:@"SELECT substring_match_info(testTable, 0, ';,') FROM main.testTable WHERE content MATCH '子串2'"
          bySelecting:^WCDB::OptionalValue {
              return self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Expression::function(WCDB::AuxiliaryFunction_SubstringMatchInfo).invoke().arguments({ WCDB::Column(self.tableName.UTF8String), 0, ";," })).from(self.tableName.UTF8String).where(WCDB_FIELD(CPPFTS5SymbolObject::content).match("子串2")));
          }];
}

@end
