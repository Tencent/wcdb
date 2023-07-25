//
// Created by 陈秋文 on 2023/7/23.
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

#import "MigrationTestCase.h"
#import "AutoIncrementMigrationObject.h"
#import "AutoIncrementMigrationSourceObject.h"
#import "IntegerPrimaryKeyMigrationObject.h"
#import "IntegerPrimaryKeyMigrationSourceObject.h"
#import "NormalMigrationObject.h"
#import "NormalMigrationSourceObject.h"
#import "Random+MigrationTestObject.h"
#import <Foundation/Foundation.h>

@interface MigrationTestCase () {
    NSString* _sourcePath;
    NSString* _sourceTableName;
    WCTDatabase* _sourceDatabase;
    WCTTable* _sourceTable;
    NSString* _schemaName;

    NSArray<NSObject<MigrationTestSourceObject>*>* _objects;
    NSArray<NSObject<MigrationTestObject>*>* _filterObjects;
    NSData* _sourceCipher;
    NSData* _targetCipher;
}
@end

@implementation MigrationTestCase

+ (NSArray<Class<MigrationTestObject>>*)allClasses
{
    static NSArray* g_allClasses = @[
        NormalMigrationObject.class,
        IntegerPrimaryKeyMigrationObject.class,
        AutoIncrementMigrationObject.class,
    ];
    return g_allClasses;
}

+ (NSArray<Class<MigrationTestSourceObject>>*)allSourceClasses
{
    static NSArray* g_allSourceClasses = @[
        NormalMigrationSourceObject.class,
        IntegerPrimaryKeyMigrationSourceObject.class,
        AutoIncrementMigrationSourceObject.class,
    ];
    return g_allSourceClasses;
}

- (NSString*)sourcePath
{
    @synchronized(self) {
        if (_sourcePath == nil) {
            if (self.isCrossDatabaseMigration) {
                _sourcePath = [self.path stringByAppendingString:@"_source"];
            } else {
                _sourcePath = self.path;
            }
        }
        return _sourcePath;
    }
}

- (NSString*)sourceTableName
{
    @synchronized(self) {
        if (_sourceTableName == nil) {
            if (self.isCrossDatabaseMigration) {
                _sourceTableName = self.tableName;
            } else {
                _sourceTableName = @"testSourceTable";
            }
        }
        return _sourceTableName;
    }
}

- (WCTTable*)sourceTable
{
    @synchronized(self) {
        if (_sourceTable == nil) {
            _sourceTable = [self.sourceDatabase getTable:self.sourceTableName
                                               withClass:self.sourceClass];
        }
        return _sourceTable;
    }
}

- (WCTDatabase*)sourceDatabase
{
    @synchronized(self) {
        if (_sourceDatabase == nil) {
            if (_isCrossDatabaseMigration) {
                _sourceDatabase = [[WCTDatabase alloc] initWithPath:self.sourcePath];
            } else {
                _sourceDatabase = self.database;
            }
        }
        return _sourceDatabase;
    }
}

- (NSString*)schemaName
{
    @synchronized(self) {
        if (_schemaName == nil) {
            if (self.isCrossDatabaseMigration) {
                _schemaName = [NSString stringWithFormat:@"wcdb_migration_%u.", WCDB::UnsafeStringView(self.sourcePath).hash()];
            } else {
                _schemaName = @"main.";
            }
        }
        return _schemaName;
    }
}

- (NSArray<id<MigrationTestSourceObject>>*)objects
{
    @synchronized(self) {
        if (_objects == nil) {
            _objects = (NSArray<NSObject<MigrationTestSourceObject>*>*)
            [[Random shared] migrationObjectsWithClass:self.sourceClass
                                              andCount:2000
                                startingFromIdentifier:1
                                        withoutContent:self.missColumnCount > 0];
        }
        return _objects;
    }
}

- (NSArray<id<MigrationTestObject>>*)filterObjects
{
    @synchronized(self) {
        if (_filterObjects == nil) {
            NSMutableArray* filterObjects = [[NSMutableArray alloc] init];
            for (NSObject<MigrationTestSourceObject>* sourceObject in self.objects) {
                if (!_needFilter || sourceObject.classification == MigrationClassificationB) {
                    id<MigrationTestObject> object = [[(Class) self.targetClass alloc] init];
                    object.identifier = sourceObject.identifier;
                    object.content = sourceObject.content;
                    [filterObjects addObject:object];
                }
            }
            _filterObjects = filterObjects;
        }
        return _filterObjects;
    }
}

- (void)tearDown
{
    [self clearData];
    [super tearDown];
}

- (void)configMigration
{
    [self clearData];

    NSData* sourceCipher = nil;
    NSData* targetCipher = nil;
    if (_needCipher) {
        if (_sourceCipher == nil) {
            _sourceCipher = [Random.shared data];
        }
        if (_targetCipher == nil) {
            _targetCipher = [Random.shared data];
        }
        sourceCipher = _sourceCipher;
        targetCipher = _targetCipher;
    }
    [self.database setCipherKey:targetCipher];
    [self.sourceDatabase setCipherKey:sourceCipher];

    [self createTables];

    TestCaseAssertTrue([self.sourceTable insertObjects:self.objects.reversedArray]);
    [self.sourceDatabase close:^() {
        TestCaseAssertTrue([self.sourceDatabase truncateCheckpoint]);
    }];
    self.table = [self.database getTable:self.tableName withClass:self.targetClass];

    [self.database addMigration:self.sourcePath
               withSourceCipher:sourceCipher
                     withFilter:^(WCTMigrationUserInfo* info) {
                         if ([info.table isEqualToString:self.tableName]) {
                             [info setSourceTable:self.sourceTableName];
                             if (self.needFilter) {
                                 [info setFilterCondition:[self.sourceClass classification] == MigrationClassificationB];
                             }
                         }
                     }];
    TestCaseAssertTrue([self.database canOpen]);

    if (_startMigration) {
        TestCaseAssertTrue([self.database stepMigration]);
    }
    TestCaseAssertFalse([self.database isMigrated]);

    NSArray* filterObjects = self.filterObjects;
    NSArray* dbObjects = [self.table getObjectsOrders:[self.targetClass identifier].asOrder(WCTOrderedAscending)];
    TestCaseAssertTrue([dbObjects isEqualToArray:filterObjects]);
}

- (void)clearData
{
    TestCaseAssertTrue([self.database removeFiles]);
    TestCaseAssertTrue([self.sourceDatabase removeFiles]);
    [self.database addMigration:self.sourcePath withFilter:nil];
    _sourceTable = nil;
    _sourceTableName = nil;
    _sourcePath = nil;
    _sourceDatabase = nil;
    _objects = nil;
    _filterObjects = nil;
    _schemaName = nil;
    self.table = nil;
}

- (void)createTables
{
    if (_missColumnCount == 0) {
        TestCaseAssertTrue([self.sourceDatabase createTable:self.sourceTableName
                                                  withClass:self.sourceClass]);
    } else {
        WCDB::StatementCreateTable createTable = [self.sourceClass objectRelationalMapping].generateCreateTableStatement(self.sourceTableName);
        auto iterater = createTable.syntax().columnDefs.begin();
        while (iterater != createTable.syntax().columnDefs.end()) {
            if (iterater->column.getOrCreate().name == [self.sourceClass content].getDescription()) {
                createTable.syntax().columnDefs.erase(iterater);
                break;
            }
            iterater++;
        }
        TestCaseAssertTrue([self.sourceDatabase execute:createTable]);
        WCDB::StatementCreateIndex createIndex = WCDB::StatementCreateIndex().createIndex("classification_index").ifNotExists().indexed([self.sourceClass classification]).table(self.sourceTableName);
        TestCaseAssertTrue([self.sourceDatabase execute:createIndex]);
    }
    if (_missColumnCount < 2) {
        TestCaseAssertTrue([self.database createTable:self.tableName
                                            withClass:self.targetClass]);
    } else {
        WCDB::StatementCreateTable createTable = [self.targetClass objectRelationalMapping].generateCreateTableStatement(self.tableName);
        auto iterater = createTable.syntax().columnDefs.begin();
        while (iterater != createTable.syntax().columnDefs.end()) {
            if (iterater->column.getOrCreate().name == [self.targetClass content].getDescription()) {
                createTable.syntax().columnDefs.erase(iterater);
                break;
            }
            iterater++;
        }
        TestCaseAssertTrue([self.database execute:createTable]);
    }
}

- (NSArray<NSObject<WCTTableCoding>*>*)getAllObjects
{
    return [self.table getObjectsOrders:[self.targetClass identifier].asOrder(WCTOrderedAscending)];
}

@end
