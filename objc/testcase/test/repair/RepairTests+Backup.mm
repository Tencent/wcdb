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

@interface RepairTests_Backup : RepairTestCase

@end

@implementation RepairTests_Backup

- (void)test_dual_backup
{
    NSString *tableName = self.className;
    int count = 1000;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    NSString *firstBackup = [NSString stringWithFormat:@"%@-first.material", _database.path];
    NSString *lastBackup = [NSString stringWithFormat:@"%@-last.material", _database.path];
    NSFileManager *fileManager = [NSFileManager defaultManager];

    XCTAssertFalse([fileManager fileExistsAtPath:firstBackup]);
    XCTAssertFalse([fileManager fileExistsAtPath:lastBackup]);

    XCTAssertTrue([_database backup]);
    XCTAssertTrue([fileManager fileExistsAtPath:firstBackup]);
    XCTAssertFalse([fileManager fileExistsAtPath:lastBackup]);

    [NSThread sleepForTimeInterval:1];
    XCTAssertTrue([_database backup]);
    XCTAssertTrue([fileManager fileExistsAtPath:firstBackup]);
    XCTAssertTrue([fileManager fileExistsAtPath:lastBackup]);

    {
        NSDate *firstBackupModifiedDate = [fileManager attributesOfItemAtPath:firstBackup error:nil][NSFileModificationDate];
        XCTAssertNotNil(firstBackupModifiedDate);
        NSDate *lastBackupModifiedDate = [fileManager attributesOfItemAtPath:lastBackup error:nil][NSFileModificationDate];
        XCTAssertNotNil(lastBackupModifiedDate);
        XCTAssertTrue([firstBackupModifiedDate compare:lastBackupModifiedDate] == NSOrderedAscending);
    }

    [NSThread sleepForTimeInterval:1];
    XCTAssertTrue([_database backup]);

    {
        NSDate *firstBackupModifiedDate = [fileManager attributesOfItemAtPath:firstBackup error:nil][NSFileModificationDate];
        XCTAssertNotNil(firstBackupModifiedDate);
        NSDate *lastBackupModifiedDate = [fileManager attributesOfItemAtPath:lastBackup error:nil][NSFileModificationDate];
        XCTAssertNotNil(lastBackupModifiedDate);
        XCTAssertTrue([firstBackupModifiedDate compare:lastBackupModifiedDate] == NSOrderedDescending);
    }
}

- (void)test_empty_backup
{
    XCTAssertFalse([_database backup]);
}

- (void)test_wal
{
    NSString *tableName = self.className;
    int count = 100;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *walPath = [NSString stringWithFormat:@"%@-wal", _database.path];
    XCTAssertTrue([[fileManager attributesOfItemAtPath:walPath error:nil] fileSize] > 0);

    XCTAssertTrue([_database backup]);
}

- (void)test_empty_wal
{
    NSString *tableName = self.className;
    int count = 100;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *walPath = [NSString stringWithFormat:@"%@-wal", _database.path];
    XCTAssertTrue([[fileManager attributesOfItemAtPath:walPath error:nil] fileSize] > 0);
    XCTAssertTrue([_database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);
    XCTAssertTrue([[fileManager attributesOfItemAtPath:walPath error:nil] fileSize] == 0);
    XCTAssertTrue([_database backup]);
}

- (void)test_filter
{
    int count = 100;

    NSString *tableName1 = [self.className stringByAppendingString:@"_1"];
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName1].count, count);

    NSString *tableName2 = [self.className stringByAppendingString:@"_2"];
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName2].count, count);

    [_database filterBackup:^BOOL(NSString *tableName) {
      if ([tableName isEqualToString:tableName1]) {
          return YES;
      }
      return NO;
    }];

    XCTAssertTrue([_database backup]);
    XCTAssertTrue([_database deposit]);

    NSArray<WCTSequence *> *sequences = [_database getObjectsOfClass:WCTSequence.class fromTable:WCTSequence.tableName orderBy:WCTSequence.name];
    XCTAssertEqual(sequences.count, 1);
    XCTAssertEqual(sequences[0].seq, count - 1);
    XCTAssertTrue([sequences[0].name isEqualToString:tableName1]);

    XCTAssertTrue([_database tableExists:tableName1]);
    WCTError *error;
    XCTAssertFalse([_database tableExists:tableName2 withError:&error]);
    XCTAssertNil(error);
}

- (void)test_filter_all
{
    int count = 100;
    NSString *tableName = self.className;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    [_database filterBackup:^BOOL(NSString *tableName) {
      return NO;
    }];

    XCTAssertFalse([_database backup]);
}

- (void)test_auto_backup
{
    _database.autoBackup = YES;

    int count = 100;
    NSString *tableName = self.className;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *backupPath = [_database.path stringByAppendingString:@"-first.material"];
    XCTAssertFalse([fileManager fileExistsAtPath:backupPath]);

    XCTAssertTrue([_database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);

    XCTAssertTrue([fileManager fileExistsAtPath:backupPath]);
}

- (void)test_backup_fail
{
    int count = 100;
    NSString *tableName = self.className;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *firstBackupPath = [_database.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [_database.path stringByAppendingString:@"-last.material"];
    XCTAssertTrue([fileManager createDirectoryAtPath:firstBackupPath withIntermediateDirectories:YES attributes:nil error:nil]);
    XCTAssertTrue([fileManager createDirectoryAtPath:lastBackupPath withIntermediateDirectories:YES attributes:nil error:nil]);

    XCTAssertFalse([_database backup]);
}

- (void)test_checkpoint_failed_with_backup
{
    _database.autoBackup = YES;

    int count = 100;
    NSString *tableName = self.className;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *firstBackupPath = [_database.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [_database.path stringByAppendingString:@"-last.material"];
    XCTAssertTrue([fileManager createDirectoryAtPath:firstBackupPath withIntermediateDirectories:YES attributes:nil error:nil]);
    XCTAssertTrue([fileManager createDirectoryAtPath:lastBackupPath withIntermediateDirectories:YES attributes:nil error:nil]);

    XCTAssertFalse([_database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);
}

- (void)test_mandatory_checkpoint_cover_backup_with_super_large_wal
{
    _database.autoBackup = YES;

    int count = 100;
    NSString *tableName = self.className;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *firstBackupPath = [_database.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [_database.path stringByAppendingString:@"-last.material"];

    XCTAssertTrue([_database backup]);
    XCTAssertTrue([fileManager fileExistsAtPath:firstBackupPath]);
    XCTAssertTrue([fileManager createDirectoryAtPath:lastBackupPath withIntermediateDirectories:YES attributes:nil error:nil]);

    for (int i = 0; i < 10001; ++i) {
        XCTAssertTrue([self insertObjectsOfCount:1 from:i intoTable:tableName]);
    }

    XCTAssertTrue([_database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);
    XCTAssertFalse([fileManager fileExistsAtPath:firstBackupPath]);
    XCTAssertFalse([fileManager fileExistsAtPath:lastBackupPath]);
}

@end
