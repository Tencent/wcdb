//
// Created by qiuwenchen on 2023/7/25.
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

#import "IntegerPrimaryKeyMigrationObject.h"
#import "IntegerPrimaryKeyMigrationSourceObject.h"
#import "MigrationTestCase.h"
#import <Foundation/Foundation.h>

@interface CipherMigrationTests : MigrationTestCase
@end

@implementation CipherMigrationTests

- (void)setUp
{
    [super setUp];
    self.needCipher = YES;
    self.sourceClass = IntegerPrimaryKeyMigrationSourceObject.class;
    self.targetClass = IntegerPrimaryKeyMigrationObject.class;
    self.isCrossDatabaseMigration = YES;
    self.startMigration = YES;
    [self configMigration];
}

- (void)test_cipher
{
    NSObject<MigrationTestObject>* expectedObject = self.filterObjects.lastObject;
    NSString* sql = [NSString stringWithFormat:@"SELECT identifier, content FROM temp.wcdb_union_testTable WHERE identifier == %d ORDER BY rowid ASC", expectedObject.identifier];

    [self doTestObjects:@[ expectedObject ]
                 andSQL:sql
            bySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                return [self.table getObjectsWhere:[self.targetClass identifier] == expectedObject.identifier];
            }];
}

- (void)test_cipher_with_multi_handle
{
    NSObject<MigrationTestObject>* expectedObject = self.filterObjects.lastObject;
    WCTHandle* handle = [self.database getHandle];
    NSArray* dbObjects = [handle getObjectsOfClass:self.targetClass
                                         fromTable:self.tableName
                                             where:[self.targetClass identifier] == expectedObject.identifier];
    TestCaseAssertTrue([dbObjects isEqualToArray:@[ expectedObject ]]);
    [self.dispatch async:^{
        WCTHandle* newHandle = [self.database getHandle];
        NSArray* dbObjects2 = [newHandle getObjectsOfClass:self.targetClass
                                                 fromTable:self.tableName
                                                     where:[self.targetClass identifier] == expectedObject.identifier];
        TestCaseAssertTrue([dbObjects2 isEqualToArray:@[ expectedObject ]]);
    }];
    sleep(1);
    [self.dispatch waitUntilDone];
}

@end
