//
// Created by qiuwenchen on 2023/6/29.
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

#import "AndroidDBTest.h"
#import "AppAttach.h"
#import "ImgInfo.h"
#import "Message.h"
#import "VideoInfo.h"
#import <Foundation/Foundation.h>

@implementation AndroidDBTest

- (void)testCreateCipherTable
{
    std::string str = "how are you";
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    NSDate* start = [NSDate date];
    TestCaseLog(@"Database path: %@", self.database.path);
    NSData* cipher = [@"abc" dataUsingEncoding:NSUTF8StringEncoding];
    [self.database setCipherKey:cipher andCipherPageSize:4096 andCipherViersion:WCTCipherVersion2];
    for (int i = 0; i < 10000; i++) {
        if (i % 100 == 0 && i > 0) {
            TestCaseLog(@"CreateTableTime: %lld, %d", (UInt64) [[NSDate date] timeIntervalSinceDate:start], i);
            start = [NSDate date];
        }

        if (i % 1000 == 0 && i > 0) {
            [self.database close:^{
                [self.database truncateCheckpoint];
            }];
        }

        NSString* userName = [Random.shared stringWithLength:30];

        TestCaseAssertTrue([self.database createTable:[@"Message_" stringByAppendingString:userName] withClass:Message.class]);

        TestCaseAssertTrue([self.database createTable:[@"ImgInfo_" stringByAppendingString:userName] withClass:ImgInfo.class]);

        TestCaseAssertTrue([self.database createTable:[@"VideoInfo_" stringByAppendingString:userName] withClass:VideoInfo.class]);

        TestCaseAssertTrue([self.database createTable:[@"AppAttach_" stringByAppendingString:userName] withClass:AppAttach.class]);
    }
    [self.database close:^{
        [self.database truncateCheckpoint];
    }];
    sleep(100000);
}

- (void)testCreateTable
{
    NSDate* start = [NSDate date];
    TestCaseLog(@"Database path: %@", self.database.path);
    for (int i = 0; i < 10000; i++) {
        if (i % 100 == 0 && i > 0) {
            TestCaseLog(@"CreateTableTime: %lld, %d", (UInt64) [[NSDate date] timeIntervalSinceDate:start], i);
            start = [NSDate date];
        }

        if (i % 1000 == 0 && i > 0) {
            [self.database close:^{
                [self.database truncateCheckpoint];
            }];
        }

        NSString* userName = [Random.shared stringWithLength:30];

        TestCaseAssertTrue([self.database createTable:[@"Message_" stringByAppendingString:userName] withClass:Message.class]);

        TestCaseAssertTrue([self.database createTable:[@"ImgInfo_" stringByAppendingString:userName] withClass:ImgInfo.class]);

        TestCaseAssertTrue([self.database createTable:[@"VideoInfo_" stringByAppendingString:userName] withClass:VideoInfo.class]);

        TestCaseAssertTrue([self.database createTable:[@"AppAttach_" stringByAppendingString:userName] withClass:AppAttach.class]);
    }
    [self.database close:^{
        [self.database truncateCheckpoint];
    }];
    sleep(100000);
}

- (void)testOpenDatabase
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:@"/Users/chenqiuwen/Desktop/MessageTest/testDatabase-2000"];
    [self
    doMeasure:^{
        [database canOpen];
    }
    setUp:nil
    tearDown:^{
        [database close];
    }
    checkCorrectness:nil];
}

- (void)testMemory
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:@"/Users/chenqiuwen/Desktop/MessageTest/testDatabase-5000"];
    for (int i = 0; i < 32; i++) {
        [self.dispatch async:^{
            @autoreleasepool {
                WCTHandle* handle = [database getHandle];
                [handle tableExists:@"abc"];
                sleep(10);
            }
        }];
    }
    [self.dispatch waitUntilDone];
    TestCaseLog(@"Test finish");
}

- (void)testOneHandleMemory
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:@"/Users/chenqiuwen/Desktop/MessageTest/testDatabase-10000"];
    WCTHandle* handle = [database getHandle];
    [handle tableExists:@"abc"];
    TestCaseLog(@"Test finish");
}

- (void)testSchemaChange
{
    NSString* tableName = Random.shared.string;
    TestCaseAssertTrue([self.database createTable:tableName withClass:Message.class]);
    WCTHandle* handle = [self.database getHandle];
    WCTPreparedStatement* prepareStatement = [handle getOrCreatePreparedStatement:WCDB::StatementSelect().select(Message.allProperties).from(tableName)];
    TestCaseAssertTrue(prepareStatement != nil);

    [self.dispatch async:^{
        TestCaseAssertTrue([self.database dropTable:tableName]);
    }];
    [self.dispatch waitUntilDone];

    TestCaseAssertFalse([prepareStatement step]);
    [handle finalizeAllStatements];
}

- (void)testAddPrimaryKey
{
    NSString* tableName = Random.shared.string;

    const WCTBinding& binding = Message.objectRelationalMapping;
    WCTBinding& multableBinding = const_cast<WCTBinding&>(binding);
    WCDB::ColumnDef* msgIdDef = multableBinding.getColumnDef(Message.msgId);
    for (auto iter = msgIdDef->syntax().constraints.begin(); iter != msgIdDef->syntax().constraints.end(); iter++) {
        if (iter->isPrimaryKey()) {
            msgIdDef->syntax().constraints.erase(iter);
            break;
        }
    }

    TestCaseAssertTrue([self.database createTable:tableName withClass:Message.class]);

    Message* msg = [[Message alloc] init];
    msg.isAutoIncrement = YES;

    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::writableSchema()).to(true)]);
    TestCaseAssertTrue([self.database insertObject:msg intoTable:tableName]);
    assert(msg.lastInsertedRowID == 1);

    WCDB::ColumnConstraint columnConstraint;
    columnConstraint.primaryKey().autoIncrement();
    msgIdDef->constraint(columnConstraint);

    NSString* createTableSql = [NSString stringWithUTF8String:multableBinding.generateCreateTableStatement(tableName).getDescription().data()];
    TestCaseAssertTrue([self.database updateTable:WCTMaster.tableName setProperty:WCTMaster.sql toValue:createTableSql]);
    TestCaseAssertTrue([self.database createTable:WCTSequence.tableName withClass:WCTSequence.class]);
    WCTSequence* seq = [[WCTSequence alloc] init];
    seq.name = tableName;
    seq.seq = 100;
    TestCaseAssertTrue([self.database insertObject:seq intoTable:WCTSequence.tableName]);

    [self.database close];

    TestCaseAssertTrue([self.database insertObject:msg intoTable:tableName]);

    assert(msg.lastInsertedRowID == 101);
}

@end
