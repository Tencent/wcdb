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
    XCTAssertTrue([self corrupt]);
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

@end
