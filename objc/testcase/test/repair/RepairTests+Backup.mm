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

@interface RepairTests_Backup : RepairTestCase

@end

@implementation RepairTests_Backup

- (void)test_dual_backup
{
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

@end
