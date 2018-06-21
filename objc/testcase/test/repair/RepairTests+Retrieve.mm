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

#import "RepairTestCase.h"

@interface RepairTests_Retrieve : RepairTestCase

@end

@implementation RepairTests_Retrieve

- (void)test_retrieve
{
    NSString *tableName = self.className;
    int count = 100;
    NSArray<TestCaseObject *> *objects = [self insertObjectsOfCount:count intoTable:tableName];
    XCTAssertEqual(objects.count, count);
    NSArray<WCTSequence *> *sequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName];
    XCTAssertEqual(sequences.count, 1);
    XCTAssertEqual(sequences[0].seq, count - 1);

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *walPath = [NSString stringWithFormat:@"%@-wal", _database.path];
    XCTAssertGreaterThan([[fileManager attributesOfItemAtPath:walPath error:nil] fileSize], 0);

    XCTAssertTrue([_database deposit]);

    XCTAssertEqual([_database retrieve:nil], 1.0);

    NSArray<TestCaseObject *> *retrieved = [_database getObjectsOfClass:TestCaseObject.class fromTable:tableName orderBy:TestCaseObject.variable1];
    XCTAssertTrue([retrieved isEqualToTestCaseObjects:objects]);

    NSArray<WCTSequence *> *retrievedSequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName];
    XCTAssertEqual(retrievedSequences.count, 1);
    XCTAssertEqual(retrievedSequences[0].seq, count - 1);
}

- (void)test_retrieve_without_deposit
{
    NSString *tableName = self.className;
    int count = 100;
    NSArray<TestCaseObject *> *objects = [self insertObjectsOfCount:count intoTable:tableName];
    XCTAssertEqual(objects.count, count);
    NSArray<WCTSequence *> *sequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName];
    XCTAssertEqual(sequences.count, 1);
    XCTAssertEqual(sequences[0].seq, count - 1);

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *walPath = [NSString stringWithFormat:@"%@-wal", _database.path];
    XCTAssertGreaterThan([[fileManager attributesOfItemAtPath:walPath error:nil] fileSize], 0);

    XCTAssertEqual([_database retrieve:nil], 1.0);

    NSArray<TestCaseObject *> *retrieved = [_database getObjectsOfClass:TestCaseObject.class fromTable:tableName orderBy:TestCaseObject.variable1];
    XCTAssertTrue([retrieved isEqualToTestCaseObjects:objects]);

    NSArray<WCTSequence *> *retrievedSequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName];
    XCTAssertEqual(retrievedSequences.count, 1);
    XCTAssertEqual(retrievedSequences[0].seq, count - 1);
}

- (void)test_retrieve_without_wal
{
    NSString *tableName = self.className;
    int count = 100;
    NSArray<TestCaseObject *> *objects = [self insertObjectsOfCount:count intoTable:tableName];
    XCTAssertEqual(objects.count, count);
    NSArray<WCTSequence *> *sequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName];
    XCTAssertEqual(sequences.count, 1);
    XCTAssertEqual(sequences[0].seq, count - 1);

    XCTAssertTrue([_database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *walPath = [NSString stringWithFormat:@"%@-wal", _database.path];
    NSError *error;
    XCTAssertEqual([[fileManager attributesOfItemAtPath:walPath error:&error] fileSize], 0);
    XCTAssertNil(error);

    XCTAssertTrue([_database deposit]);

    XCTAssertEqual([_database retrieve:nil], 1.0);

    NSArray<TestCaseObject *> *retrieved = [_database getObjectsOfClass:TestCaseObject.class fromTable:tableName orderBy:TestCaseObject.variable1];
    XCTAssertTrue([retrieved isEqualToTestCaseObjects:objects]);

    NSArray<WCTSequence *> *retrievedSequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName];
    XCTAssertEqual(retrievedSequences.count, 1);
    XCTAssertEqual(retrievedSequences[0].seq, count - 1);
}

- (void)test_retrieve_corrupted
{
}

- (void)test_retrieve_corrupted_with_backup
{
}

- (void)test_retrieve_corrupted_with_corrupted_backup
{
}

@end
