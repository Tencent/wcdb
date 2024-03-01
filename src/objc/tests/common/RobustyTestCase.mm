//
// Created by qiuwenchen on 2023/12/20.
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

#import "RobustyTestCase.h"
#import "TestCase.h"
#import <Foundation/Foundation.h>

@interface RobustyTestCase () {
    NSArray<NSString*>* m_tables;
}
@end

@implementation RobustyTestCase

- (void)setUp
{
    [super setUp];
    self.tableFactor = 1000;
}

- (NSArray<NSString*>*)allTables
{
    return m_tables;
}

- (NSString*)tablePrefix
{
    return @"t_";
}

- (BOOL)fillDatabaseUntilMeetExpectedSize
{
    NSString* currentTable = nil;
    BOOL checkpointed = NO; // leave wal exists
    NSMutableArray* tables = [[NSMutableArray alloc] init];
    while (checkpointed || [self.database getFilesSize] < self.expectedDatabaseSize) {
        @autoreleasepool {
            if (currentTable == nil || Random.shared.uint16 % self.tableFactor == 0) {
                currentTable = [Random.shared tableNameWithPrefix:self.tablePrefix];
                if (![self.database createTable:currentTable withClass:TestCaseObject.class]) {
                    TestCaseFailure();
                    return NO;
                }
                [tables addObject:currentTable];
            }

            int count = 0;
            do {
                count = Random.shared.uint8;
            } while (count == 0);
            NSArray<TestCaseObject*>* objects = [Random.shared autoIncrementTestCaseObjectsWithCount:count];
            if (![self.database insertObjects:objects intoTable:currentTable]) {
                TestCaseFailure();
                return NO;
            }
            if (Random.shared.uint8 % 100 == 0) {
                if (![self.database passiveCheckpoint]) {
                    TestCaseFailure();
                    return NO;
                }
                checkpointed = YES;
            } else {
                checkpointed = NO;
            }
        }
    }
    if (![self.fileManager fileExistsAtPath:self.database.walPath]) {
        TestCaseFailure();
        return NO;
    }
    m_tables = tables;
    return YES;
}

- (NSDictionary<NSString*, NSArray<TestCaseObject*>*>*)getTableObjects
{
    NSMutableDictionary<NSString*, NSArray<TestCaseObject*>*>* tableObjects = [NSMutableDictionary<NSString*, NSArray<TestCaseObject*>*> dictionary];
    for (NSString* tableName in m_tables) {
        @autoreleasepool {
            NSArray<TestCaseObject*>* objects = [self.database getObjectsOfClass:TestCaseObject.class fromTable:tableName];
            if (objects != nil) {
                [tableObjects setObject:objects forKey:tableName];
            }
        }
    }
    return tableObjects;
}

@end
