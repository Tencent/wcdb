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
#import "RepairTestCaseObject+WCTTableCoding.h"
#import "RepairTestCaseObject.h"
#import "TestCaseCommon.h"

@interface RepairTests_Robust : RepairTestCase

@end

@implementation RepairTests_Robust

- (NSDictionary<NSString *, NSArray<RepairTestCaseObject *> *> *)insertRandomObjectsWithRandomTables
{
    NSMutableDictionary<NSString *, NSArray<RepairTestCaseObject *> *> *tableObjects = [[NSMutableDictionary<NSString *, NSArray<RepairTestCaseObject *> *> alloc] init];
    int tableCount = [NSNumber randomUInt8];
    for (int i = 0; i < tableCount; ++i) {
        if (![_database runTransaction:^BOOL(WCTHandle *_Nonnull) {
                NSArray<RepairTestCaseObject *> *objects = [RepairTestCaseObject randomObjects];
                NSString *tableName;
                do {
                    tableName = [NSString stringWithFormat:@"t_%@", [NSString randomString]];
                } while ([tableObjects objectForKey:tableName] != nil);
                [tableObjects setObject:objects forKey:tableName];
                return [_database createTableAndIndexes:tableName withClass:RepairTestCaseObject.class] && [_database insertObjects:objects intoTable:tableName];
            }]) {
            return nil;
        }
    }
    return tableObjects;
}

- (void)test_all_types_without_backup
{
    NSDictionary<NSString *, NSArray<RepairTestCaseObject *> *> *tableObjects = [self insertRandomObjectsWithRandomTables];
    XCTAssertGreaterThan(tableObjects.count, 0);

    XCTAssertEqual([_database retrieve:nil], 1.0);

    for (NSString *tableName in tableObjects.allKeys) {
        NSArray<RepairTestCaseObject *> *objects = tableObjects[tableName];

        NSArray<RepairTestCaseObject *> *retrieved = [_database getObjectsOfClass:RepairTestCaseObject.class fromTable:tableName orderBy:RepairTestCaseObject.int64Value];
        NSArray<RepairTestCaseObject *> *expected = [objects sortedArrayUsingComparator:^NSComparisonResult(RepairTestCaseObject *obj1, RepairTestCaseObject *obj2) {
            if (obj1.int64Value > obj2.int64Value) {
                return NSOrderedDescending;
            } else if (obj1.int64Value < obj2.int64Value) {
                return NSOrderedAscending;
            } else {
                return NSOrderedSame;
            }
        }];
        XCTAssertTrue([retrieved isEqualToObjects:expected
                                   withComparator:^BOOL(RepairTestCaseObject *lhs, RepairTestCaseObject *rhs) {
                                       return [lhs isEqualToRepairTestCaseObject:rhs];
                                   }]);
    }
}

- (void)test_all_types_with_backup
{
    NSDictionary<NSString *, NSArray<RepairTestCaseObject *> *> *tableObjects = [self insertRandomObjectsWithRandomTables];
    XCTAssertGreaterThan(tableObjects.count, 0);

    XCTAssertTrue([_database backup]);
    XCTAssertTrue([self corruptWithCheckpoint:YES]);
    XCTAssertEqual([_database retrieve:nil], 1.0);

    for (NSString *tableName in tableObjects.allKeys) {
        NSArray<RepairTestCaseObject *> *objects = tableObjects[tableName];

        NSArray<RepairTestCaseObject *> *retrieved = [_database getObjectsOfClass:RepairTestCaseObject.class fromTable:tableName orderBy:RepairTestCaseObject.int64Value];
        NSArray<RepairTestCaseObject *> *expected = [objects sortedArrayUsingComparator:^NSComparisonResult(RepairTestCaseObject *obj1, RepairTestCaseObject *obj2) {
            if (obj1.int64Value > obj2.int64Value) {
                return NSOrderedDescending;
            } else if (obj1.int64Value < obj2.int64Value) {
                return NSOrderedAscending;
            } else {
                return NSOrderedSame;
            }
        }];
        XCTAssertTrue([retrieved isEqualToObjects:expected
                                   withComparator:^BOOL(RepairTestCaseObject *lhs, RepairTestCaseObject *rhs) {
                                       return [lhs isEqualToRepairTestCaseObject:rhs];
                                   }]);
    }
}

- (double)carpetBombingPage:(double)ratio
{
    if (![_database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]) {
        return 0;
    }
    DatabaseBomber *bomber = [[DatabaseBomber alloc] initWithPath:_database.path];
    int attackTimes = 0;
    double attackRatio = 0;
    do {
        if ([NSNumber randomBool]) {
            bomber.attackType = FileBomberAttackTypeZero;
        } else {
            bomber.attackType = FileBomberAttackTypeRandom;
        }
        if (![bomber randomAttackPage]) {
            return 0;
        }
        attackRatio = (double) ++attackTimes / bomber.pageCount;
    } while (attackRatio < ratio);
    return attackRatio;
}

- (void)test_robust_with_page_carpet_bombing
{
    NSDictionary<NSString *, NSArray<RepairTestCaseObject *> *> *tableObjects = [self insertRandomObjectsWithRandomTables];
    XCTAssertGreaterThan(tableObjects.count, 0);

    XCTAssertTrue([_database backup]);

    double ratio = 0.1;
    double attackRatio = [self carpetBombingPage:ratio];
    XCTAssertGreaterThanOrEqual(attackRatio, ratio);

    double score = [_database retrieve:nil];
    XCTAssertGreaterThanOrEqual(score, 0);

    int retrieved = 0;
    int total = 0;
    for (NSString *tableName in tableObjects.allKeys) {
        NSArray<RepairTestCaseObject *> *objects = tableObjects[tableName];
        total += objects.count;

        NSMutableDictionary<NSNumber *, RepairTestCaseObject *> *expected = [[NSMutableDictionary<NSNumber *, RepairTestCaseObject *> alloc] initWithCapacity:objects.count];
        for (RepairTestCaseObject *object in objects) {
            expected[@(object.int64Value)] = object;
        }

        NSArray<RepairTestCaseObject *> *retrievedObjects = [_database getObjectsOfClass:RepairTestCaseObject.class fromTable:tableName orderBy:RepairTestCaseObject.int64Value];
        for (RepairTestCaseObject *retrievedObject in retrievedObjects) {
            RepairTestCaseObject *expectedObject = expected[@(retrievedObject.int64Value)];
            if (expectedObject && [expectedObject isEqualToRepairTestCaseObject:retrievedObject]) {
                ++retrieved;
            }
        }
    }

    double retrievedScore = (double) retrieved / total;
    XCTAssertGreaterThanOrEqual(score, 1.0 - ratio * 2);
    XCTAssertLessThanOrEqual(score, 1.0);
    XCTAssertGreaterThanOrEqual(retrievedScore, 1.0 - ratio * 2);
    XCTAssertLessThanOrEqual(retrievedScore, 1.0);

    NSLog(@"Carpet Bombing Retrieving Result: Attack Ratio %f, Retrieve Score %f, retrieved %f", attackRatio, score, retrievedScore);
}

- (BOOL)carpetBombing
{
    if (![_database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]) {
        return NO;
    }
    FileBomber *bomber = [[FileBomber alloc] initWithPath:_database.path];
    int attackTimes = [NSNumber randomUInt8];
    for (int i = 0; i < attackTimes; ++i) {
        if (![bomber randomAttack]) {
            return NO;
        }
    }
    return YES;
}

//This test is for the robusty with messy data
- (void)test_robust_with_carpet_bombing
{
    NSDictionary<NSString *, NSArray<RepairTestCaseObject *> *> *tableObjects = [self insertRandomObjectsWithRandomTables];
    XCTAssertGreaterThan(tableObjects.count, 0);

    XCTAssertTrue([_database backup]);

    XCTAssertTrue([self carpetBombing]);

    double score = [_database retrieve:nil];
    XCTAssertGreaterThanOrEqual(score, 0);
}

@end
