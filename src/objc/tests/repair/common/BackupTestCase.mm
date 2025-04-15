//
// Created by sanhuazhang on 2019/05/02
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

#import "BackupTestCase.h"
#import "AutoIncrementRepairTestObject.h"
#import "CoreConst.h"
#import "NormalRepairTestObject.h"
#import "Random+RepairTestObject.h"
#import "WithoutRowidRepairTestObject.h"

@implementation BackupTestCase {
    NSMutableArray<NSObject<RepairTestObject>*>* m_objects;
}

+ (NSArray<Class<RepairTestObject>>*)allTestClassess
{
    static NSArray<Class<RepairTestObject>>* g_testClasses = @[
        NormalRepairTestObject.class,
        AutoIncrementRepairTestObject.class,
        WithoutRowidRepairTestObject.class,
    ];
    return g_testClasses;
}

- (void)setUp
{
    [super setUp];
    self.objectCount = 100;
}

- (NSMutableArray<NSObject<RepairTestObject>*>*)objects
{
    return m_objects;
}

- (void)setObjects:(NSMutableArray*)objects
{
    m_objects = (NSMutableArray<NSObject<RepairTestObject>*>*) objects;
}

- (void)executeTest:(void (^)())operation
{
    for (int i = 0; i < 12; i++) {
        int config = i;
        if (config % 2 == 0) {
            self.tableName = @"testTable";
        } else {
            self.tableName = @"'test@Table'";
        }
        config /= 2;
        _needCipher = (BOOL) (config % 2);
        config /= 2;
        _testClass = [BackupTestCase allTestClassess][config];
        [self clearData];
        NSData* cipher = nil;
        if (_needCipher) {
            cipher = [Random.shared data];
        }
        TestCaseLog(@"Test repair %d: tableName %@, testClass %@, needCipher %d, incrementalBackup %d, corruptHeader %d",
                    i,
                    self.tableName,
                    self.testClass,
                    self.needCipher,
                    self.incrementalBackup,
                    self.corruptHeader);
        [self.database setCipherKey:cipher];
        TestCaseAssertTrue([self.database createTable:self.tableName withClass:self.testClass]);
        self.table = [self.database getTable:self.tableName withClass:self.testClass];

        NSArray* objects = [[Random shared] repairObjectsWithClass:_testClass andCount:self.objectCount startingFromIdentifier:1];
        [m_objects addObjectsFromArray:objects];
        XCTAssertTrue([self.table insertObjects:objects]);

        operation();
    }
}

- (void)executeFullTest:(void (^)())operation withCheck:(void (^)())check
{
    [self.database enableAutoCheckpoint:NO];
    for (int i = 0; i < 48; i++) {
        int config = i;
        if (config % 2 == 0) {
            self.tableName = @"testTable";
        } else {
            self.tableName = @"'test@Table'";
        }
        config /= 2;
        _needCipher = (BOOL) (config % 2);
        config /= 2;
        _incrementalBackup = config % 2;
        config /= 2;
        _corruptHeader = config % 2;
        config /= 2;
        _testClass = [BackupTestCase allTestClassess][config];

        TestCaseLog(@"Test repair %d: tableName %@, testClass %@, needCipher %d, incrementalBackup %d, corruptHeader %d",
                    i,
                    self.tableName,
                    self.testClass,
                    self.needCipher,
                    self.incrementalBackup,
                    self.corruptHeader);

        [self configRepairDatabase];
        operation();

        if (_incrementalBackup) {
            XCTAssertTrue([self.database passiveCheckpoint]);
            usleep(10000);
            XCTAssertTrue([self.database backup]);
            XCTAssertTrue([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);
            XCTAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
            XCTAssertTrue([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

            NSArray* objects = [[Random shared] repairObjectsWithClass:_testClass andCount:self.objectCount startingFromIdentifier:1000];
            if ([self.table insertObjects:objects]) {
                [m_objects addObjectsFromArray:objects];
            }

            XCTAssertTrue([self.database passiveCheckpoint]);
            usleep(10000);
            XCTAssertTrue([self.database backup]);
            XCTAssertTrue([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);
            XCTAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
            XCTAssertTrue([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);
        }

        if (_corruptHeader) {
            [self.database corruptPage:1];
        }

        XCTAssertTrue([self.database retrieve:nil] > 0);

        if (check != nil) {
            check();
        }
    }
    [self.database enableAutoCheckpoint:YES];
}

- (void)checkObjects:(NSArray*)objects containedIn:(NSArray*)allObjects
{
    for (NSObject* object in objects) {
        BOOL found = NO;
        for (NSObject* currentObject in allObjects) {
            if ([currentObject isEqual:object]) {
                found = YES;
                break;
            }
        }
        if (!found) {
            XCTAssertTrue(found);
        }
    }
}

- (void)configRepairDatabase
{
    [self clearData];
    NSData* cipher = nil;
    if (_needCipher) {
        cipher = [Random.shared data];
    }
    [self.database setCipherKey:cipher];
    TestCaseAssertTrue([self.database createTable:self.tableName withClass:self.testClass]);
    self.table = [self.database getTable:self.tableName withClass:self.testClass];

    NSArray* objects = [[Random shared] repairObjectsWithClass:_testClass andCount:self.objectCount startingFromIdentifier:1];
    [m_objects addObjectsFromArray:objects];
    XCTAssertTrue([self.table insertObjects:objects]);
    XCTAssertTrue([self.database passiveCheckpoint]);
    usleep(10000);
    XCTAssertFalse([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);
    XCTAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    XCTAssertFalse([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

    [self.database enableAutoBackup:true];
    XCTAssertTrue([self.database backup]);

    XCTAssertTrue([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);
    XCTAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    XCTAssertFalse([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);
}

- (void)clearData
{
    TestCaseAssertTrue([self.database removeFiles]);
    m_objects = [[NSMutableArray alloc] init];
    self.table = nil;
}

@end
