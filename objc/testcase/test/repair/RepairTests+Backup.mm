//
//  RepairTests+Backup.m
//  testcase
//
//  Created by sanhuazhang on 2018/6/15.
//  Copyright © 2018 tencent. All rights reserved.
//

#import "RepairTestCase.h"

@interface RepairTests_Backup : RepairTestCase

@end

@implementation RepairTests_Backup

- (void)test_dual_backup
{
    NSString *firstBackup = [NSString stringWithFormat:@"%@-first.material", _database.path];
    NSString *lastBackup = [NSString stringWithFormat:@"%@-last", _database.path];
    NSFileManager *fileManager = [NSFileManager defaultManager];

    XCTAssertFalse([fileManager fileExistsAtPath:firstBackup]);
    XCTAssertFalse([fileManager fileExistsAtPath:lastBackup]);

    XCTAssertTrue([_database backup]);
    XCTAssertTrue([fileManager fileExistsAtPath:firstBackup]);
    XCTAssertFalse([fileManager fileExistsAtPath:lastBackup]);

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
