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

#import "FileBomber.h"
#import "RepairTestCase.h"

@interface RepairTests_CorruptionReaction : RepairTestCase

@end

@implementation RepairTests_CorruptionReaction

- (BOOL)corrupt
{
    [_database close];
    DatabaseBomber *bomber = [[DatabaseBomber alloc] initWithPath:_database.path];
    return [bomber attackRootPage];
}

- (void)triggerCorruptionNotifier
{
    [_database getValueOnResult:1 fromTable:WCTMaster.tableName];
}

- (void)test_reaction_custom
{
    NSString *tableName = self.className;
    int count = 100;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    __block BOOL hit = NO;
    _database.reactionWhenCorrupted = WCTCorruptionReactionCustom;
    NSLock *locker = [[NSLock alloc] init];
    [_database setExtraReactionWhenCorrupted:^(WCTDatabase *database) {
      [locker lock];
      hit = YES;
      [locker unlock];
    }];

    XCTAssertTrue([self corrupt]);
    [self triggerCorruptionNotifier];

    [NSThread sleepForTimeInterval:3];

    [locker lock];
    XCTAssertTrue(hit);
    [locker unlock];
}

- (void)test_reaction_remove
{
    NSString *tableName = self.className;
    int count = 100;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    _database.reactionWhenCorrupted = WCTCorruptionReactionRemove;

    XCTAssertTrue([self corrupt]);
    [self triggerCorruptionNotifier];

    [NSThread sleepForTimeInterval:3];

    XCTAssertFalse([[NSFileManager defaultManager] fileExistsAtPath:_database.path]);
}

- (void)test_reaction_deposit
{
    NSString *tableName = self.className;
    int count = 100;
    XCTAssertEqual([self insertObjectsOfCount:count intoTable:tableName].count, count);

    _database.reactionWhenCorrupted = WCTCorruptionReactionDeposit;

    XCTAssertTrue([_database backup]);
    XCTAssertTrue([self corrupt]);
    [self triggerCorruptionNotifier];

    [NSThread sleepForTimeInterval:3];

    XCTAssertTrue([_database tableExists:tableName]);
    XCTAssertEqual([_database getObjectsOfClass:TestCaseObject.class fromTable:tableName].count, 0);
    XCTAssertEqual([_database getValueOnResult:WCTSequence.seq fromTable:WCTSequence.tableName where:WCTSequence.name == tableName].numberValue.intValue, count - 1);
}

@end
