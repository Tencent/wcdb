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
#import "TestCaseCommon.h"

@interface RepairTests_Deposit : RepairTestCase

@end

@implementation RepairTests_Deposit

- (void)test_deposit
{
    NSFileManager *fileManager = [NSFileManager defaultManager];

    NSString *tableName = self.className;
    int count = 100;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    //Before
    {
        XCTAssertTrue([_database tableExists:tableName]);

        WCTValue *value = [_database getValueOnResult:TestCaseObject.allResults.count() fromTable:tableName];
        XCTAssertNotNil(value);
        XCTAssertEqual(value.numberValue.intValue, count);

        NSArray<WCTSequence *> *sequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName orderBy:WCTSequence.name];
        XCTAssertEqual(sequences.count, 1);
        XCTAssertEqual(sequences[0].seq, count - 1);
    }

    XCTAssertTrue([_database isOpened]);
    XCTAssertTrue([fileManager fileExistsAtPath:[_database.path stringByAppendingString:@"-wal"]]);

    XCTAssertTrue([_database backup]);

    //Deposit
    XCTAssertTrue([_database deposit]);

    //After
    XCTAssertFalse([_database isOpened]);
    //Check wal still exists
    NSString *factory = [_database.path stringByAppendingString:@".factory"];
    NSArray<NSString *> *subdir = [fileManager contentsOfDirectoryAtPath:factory error:nil];
    XCTAssertEqual(subdir.count, 1);
    NSString *wal = [[[factory stringByAppendingPathComponent:subdir[0]] stringByAppendingPathComponent:_database.path.lastPathComponent] stringByAppendingString:@"-wal"];
    XCTAssertTrue([fileManager fileExistsAtPath:wal]);

    //Check if already backedup
    NSString *material = [_database.path stringByAppendingString:@"-first.material"];
    XCTAssertTrue([fileManager fileExistsAtPath:material]);

    {
        XCTAssertTrue([_database tableExists:tableName]);

        WCTValue *value = [_database getValueOnResult:TestCaseObject.allResults.count() fromTable:tableName];
        XCTAssertNotNil(value);
        XCTAssertEqual(value.numberValue.intValue, 0);

        NSArray<WCTSequence *> *sequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName orderBy:WCTSequence.name];
        XCTAssertEqual(sequences.count, 1);
        XCTAssertEqual(sequences[0].seq, count - 1);
    }
}

- (void)test_deposit_without_backup
{
    NSString *tableName = self.className;
    int count = 100;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    //Before
    {
        XCTAssertTrue([_database tableExists:tableName]);

        WCTValue *value = [_database getValueOnResult:TestCaseObject.allResults.count() fromTable:tableName];
        XCTAssertNotNil(value);
        XCTAssertEqual(value.numberValue.intValue, count);

        NSArray<WCTSequence *> *sequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName orderBy:WCTSequence.name];
        XCTAssertEqual(sequences.count, 1);
        XCTAssertEqual(sequences[0].seq, count - 1);
    }

    //Deposit
    XCTAssertTrue([_database deposit]);

    //After
    {
        WCTError *error;
        XCTAssertFalse([_database tableExists:tableName withError:&error]);
        XCTAssertNil(error);

        NSArray<WCTSequence *> *sequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName orderBy:WCTSequence.name];
        XCTAssertEqual(sequences.count, 0);
    }
}

- (void)test_deposit_multiple_times
{
    int count1 = 123;
    NSString *tableName1 = [self.className stringByAppendingString:@"_1"];
    XCTAssertEqual([self insertObjectsOfCount:count1 intoTable:tableName1].count, count1);
    XCTAssertTrue([_database backup]);
    XCTAssertTrue([_database deposit]);

    int count2 = 234;
    NSString *tableName2 = [self.className stringByAppendingString:@"_2"];
    XCTAssertEqual([self insertObjectsOfCount:count2 intoTable:tableName2].count, count2);
    XCTAssertTrue([_database backup]);
    XCTAssertTrue([_database deposit]);

    int count3 = 345;
    NSString *tableName3 = [self.className stringByAppendingString:@"_3"];
    XCTAssertEqual([self insertObjectsOfCount:count3 intoTable:tableName3].count, count3);
    XCTAssertTrue([_database backup]);
    XCTAssertTrue([_database deposit]);

    // After
    WCTValue *value;
    NSArray<WCTSequence *> *sequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName orderBy:WCTSequence.name];
    XCTAssertEqual(sequences.count, 3);

    XCTAssertTrue([_database tableExists:tableName1]);
    XCTAssertTrue([_database tableExists:tableName2]);
    XCTAssertTrue([_database tableExists:tableName3]);

    value = [_database getValueOnResult:TestCaseObject.allResults.count() fromTable:tableName1];
    XCTAssertNotNil(value);
    XCTAssertEqual(value.numberValue.intValue, 0);
    XCTAssertEqual(sequences[0].seq, count1 - 1);

    value = [_database getValueOnResult:TestCaseObject.allResults.count() fromTable:tableName2];
    XCTAssertNotNil(value);
    XCTAssertEqual(value.numberValue.intValue, 0);
    XCTAssertEqual(sequences[1].seq, count2 - 1);

    value = [_database getValueOnResult:TestCaseObject.allResults.count() fromTable:tableName3];
    XCTAssertNotNil(value);
    XCTAssertEqual(value.numberValue.intValue, 0);
    XCTAssertEqual(sequences[2].seq, count3 - 1);
}

- (void)test_remove_factory_after_remove_deposited
{
    NSString *tableName = self.className;
    int count = 100;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    XCTAssertTrue([_database isOpened]);

    //Deposit
    XCTAssertTrue([_database deposit]);

    NSString *factory = [_database.path stringByAppendingString:@".factory"];
    XCTAssertTrue([self.fileManager fileExistsAtPath:factory]);

    XCTAssertTrue([_database removeDeposit]);

    XCTAssertFalse([self.fileManager fileExistsAtPath:factory]);
}

- (void)test_remove_factory_after_renewed
{
    NSString *tableName = self.className;
    int count = 100;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    XCTAssertTrue([_database isOpened]);

    //Deposit
    XCTAssertTrue([_database deposit]);

    NSString *factory = [_database.path stringByAppendingString:@".factory"];
    XCTAssertTrue([self.fileManager fileExistsAtPath:factory]);

    NSArray<NSString *> *subpaths = [self.fileManager contentsOfDirectoryAtPath:factory error:nil];
    XCTAssertEqual(subpaths.count, 1);
    for (NSString *subpathName in subpaths) {
        NSString *subpath = [factory stringByAppendingPathComponent:subpathName];
        XCTAssertTrue([self.fileManager removeItemAtPath:subpath error:nil]);
    }

    // trigger renew
    XCTAssertTrue([_database canOpen]);

    XCTAssertFalse([self.fileManager fileExistsAtPath:factory]);
}

@end
