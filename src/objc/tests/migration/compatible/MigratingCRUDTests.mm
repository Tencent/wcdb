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

#import "AutoIncrementMigrationObject.h"
#import "AutoIncrementMigrationSourceObject.h"
#import "IntegerPrimaryKeyMigrationObject.h"
#import "IntegerPrimaryKeyMigrationSourceObject.h"
#import "MigrationTestCase.h"
#import "NormalMigrationObject.h"
#import "NormalMigrationSourceObject.h"
#import "Random+MigrationTestObject.h"
#import <Foundation/Foundation.h>

@interface MigratingCRUDTests : MigrationTestCase

@end

@implementation MigratingCRUDTests

- (void)setUp
{
    [super setUp];
    self.skipDebugLog = YES;
}

- (void)doTestMigration:(void (^)(void))operation
{
    [self doTestMigrationWithSourceClassFilter:nil targetClassFIlter:nil andOperation:operation];
}

- (void)doTestMigrationWithSourceClassFilter:(BOOL (^)(Class<MigrationTestSourceObject>))sourceFilter
                           targetClassFIlter:(BOOL (^)(Class<MigrationTestObject>))targetFilter
                                andOperation:(void (^)(void))operation
{
    NSArray<Class<MigrationTestSourceObject>>* sourceClasses = [MigrationTestCase allSourceClasses];
    NSArray<Class<MigrationTestObject>>* targetClasses = [MigrationTestCase allClasses];
    if (sourceFilter != nil) {
        sourceClasses = [sourceClasses filteredArrayUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(Class<MigrationTestSourceObject> sourceClass, NSDictionary<NSString*, id>*) {
                                           return sourceFilter(sourceClass);
                                       }]];
    }
    if (targetFilter != nil) {
        targetClasses = [targetClasses filteredArrayUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(Class<MigrationTestObject> targetClass, NSDictionary<NSString*, id>*) {
                                           return targetFilter(targetClass);
                                       }]];
    }
    for (Class<MigrationTestSourceObject> sourceClass in sourceClasses) {
        for (Class<MigrationTestObject> targetClass in targetClasses) {
            self.sourceClass = sourceClass;
            self.targetClass = targetClass;

            for (int i = 0; i < 8; i++) {
                self.startMigration = (i & 1) > 0;
                self.isCrossDatabaseMigration = (i & 2) > 0;
                self.needFilter = (i & 4) > 0;
                for (int j = 0; j < 3; j++) {
                    if (j == 0) {
                        self.expectMode = DatabaseTestCaseExpectAllSQLs;
                    } else {
                        self.expectMode = DatabaseTestCaseExpectSomeSQLs;
                    }
                    self.missColumnCount = j;
                    TestCaseLog(@"Test migration: sourceClass %@, targetClass %@, startMigration %d, isCrossDatabase %d, needFilter %d, missColumnCount %d",
                                self.sourceClass,
                                self.targetClass,
                                self.startMigration,
                                self.isCrossDatabaseMigration,
                                self.needFilter,
                                self.missColumnCount);
                    [self configMigration];
                    operation();
                }
            }
        }
    }
}

- (void)test_insert
{
    TestCaseLog(@"Start test insert");
    [self doTestMigration:^{
        NSObject<MigrationTestObject>* newObject = [Random.shared migrationObjectWithClass:self.targetClass
                                                                             andIdentifier:self.objects.lastObject.identifier + 1];
        NSMutableArray<NSObject<MigrationTestObject>*>* expectedObjects = [NSMutableArray arrayWithArray:self.filterObjects];
        [expectedObjects addObject:newObject];

        NSMutableArray<NSString*>* sqls = [NSMutableArray array];
        [sqls addObject:@"BEGIN IMMEDIATE"];
        [sqls addObject:[NSString stringWithFormat:@"INSERT INTO %@%@(identifier, content) VALUES(?1, ?2)", self.schemaName, self.sourceTableName]];
        [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@%@ WHERE rowid == ?1", self.schemaName, self.sourceTableName]];
        if ([self.targetClass isAutoIncrement]) {
            [sqls addObject:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"];
        } else if ([self.targetClass hasIntegerPrimaryKey]) {
            [sqls addObject:[NSString stringWithFormat:@"INSERT INTO main.testTable(identifier, content, rowid) VALUES(?1, ?2, ?%d)", SQLITE_MAX_VARIABLE_NUMBER]];
        } else {
            [sqls addObject:@"SELECT max(rowid) + 1 FROM main.testTable"];
            [sqls addObject:[NSString stringWithFormat:@"INSERT INTO main.testTable(identifier, content, rowid) VALUES(?1, ?2, ?%d)", SQLITE_MAX_VARIABLE_NUMBER]];
        }
        [sqls addObject:@"COMMIT"];

        [self doTestObjects:expectedObjects
                    andSQLs:sqls
          afterModification:^BOOL {
              return [self.table insertObject:newObject];
          }];
    }];
}

- (void)test_insert_autoIncrement
{
    TestCaseLog(@"Start test insert autoincrement");

    [self doTestMigrationWithSourceClassFilter:nil
    targetClassFIlter:^BOOL(Class<MigrationTestObject> cls) {
        return [cls isAutoIncrement];
    }
    andOperation:^{
        NSObject<MigrationTestObject>* newObject = [Random.shared autoIncrementMigrationObjectWithClass:self.targetClass];
        NSMutableArray<NSObject<MigrationTestObject>*>* expectedObjects = [NSMutableArray arrayWithArray:self.filterObjects];
        NSObject<MigrationTestObject>* expectedNewObject = [[(Class) self.targetClass alloc] init];
        expectedNewObject.identifier = self.objects.lastObject.identifier + 1;
        expectedNewObject.content = newObject.content;
        [expectedObjects addObject:expectedNewObject];

        NSMutableArray<NSString*>* sqls = [NSMutableArray array];
        [sqls addObject:@"BEGIN IMMEDIATE"];
        [sqls addObject:[NSString stringWithFormat:@"INSERT INTO %@%@(identifier, content) VALUES(?1, ?2)", self.schemaName, self.sourceTableName]];
        [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@%@ WHERE rowid == ?1", self.schemaName, self.sourceTableName]];
        if ([self.targetClass isAutoIncrement]) {
            [sqls addObject:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"];
        } else if ([self.targetClass hasIntegerPrimaryKey]) {
            [sqls addObject:[NSString stringWithFormat:@"INSERT INTO main.testTable(identifier, content, rowid) VALUES(?1, ?2, ?%d)", SQLITE_MAX_VARIABLE_NUMBER]];
        } else {
            [sqls addObject:@"SELECT max(rowid) + 1 FROM main.testTable"];
            [sqls addObject:[NSString stringWithFormat:@"INSERT INTO main.testTable(identifier, content, rowid) VALUES(?1, ?2, ?%d)", SQLITE_MAX_VARIABLE_NUMBER]];
        }
        [sqls addObject:@"COMMIT"];

        [self doTestObjects:expectedObjects
                    andSQLs:sqls
          afterModification:^BOOL {
              return [self.table insertObject:newObject];
          }];
    }];
}

- (void)test_insert_or_replace
{
    TestCaseLog(@"Start test insert or replace");

    auto filter = ^BOOL(Class<MigrationTestObject> cls) {
        return [cls hasIntegerPrimaryKey];
    };

    [self doTestMigrationWithSourceClassFilter:filter
                             targetClassFIlter:filter
                                  andOperation:^{
                                      NSObject<MigrationTestObject>* newObject = [Random.shared migrationObjectWithClass:self.targetClass
                                                                                                           andIdentifier:self.filterObjects.lastObject.identifier];
                                      NSMutableArray<NSObject<MigrationTestObject>*>* expectedObjects = [NSMutableArray arrayWithArray:self.filterObjects];
                                      [expectedObjects removeLastObject];
                                      [expectedObjects addObject:newObject];

                                      NSMutableArray<NSString*>* sqls = [NSMutableArray array];
                                      [sqls addObject:@"BEGIN IMMEDIATE"];
                                      [sqls addObject:[NSString stringWithFormat:@"INSERT OR REPLACE INTO %@%@(identifier, content) VALUES(?1, ?2)", self.schemaName, self.sourceTableName]];
                                      [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@%@ WHERE rowid == ?1", self.schemaName, self.sourceTableName]];
                                      if ([self.targetClass isAutoIncrement]) {
                                          [sqls addObject:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"];
                                      } else if ([self.targetClass hasIntegerPrimaryKey]) {
                                          [sqls addObject:[NSString stringWithFormat:@"INSERT OR REPLACE INTO main.testTable(identifier, content, rowid) VALUES(?1, ?2, ?%d)", SQLITE_MAX_VARIABLE_NUMBER]];
                                      } else {
                                          [sqls addObject:@"SELECT max(rowid) + 1 FROM main.testTable"];
                                          [sqls addObject:[NSString stringWithFormat:@"INSERT OR REPLACE INTO main.testTable(identifier, content, rowid) VALUES(?1, ?2, ?%d)", SQLITE_MAX_VARIABLE_NUMBER]];
                                      }
                                      [sqls addObject:@"COMMIT"];

                                      [self doTestObjects:expectedObjects
                                                  andSQLs:sqls
                                        afterModification:^BOOL {
                                            return [self.table insertOrReplaceObject:newObject];
                                        }];
                                  }];
}

- (void)test_insert_or_ignore
{
    TestCaseLog(@"Start test insert or ignore");

    auto filter = ^BOOL(Class<MigrationTestObject> cls) {
        return [cls hasIntegerPrimaryKey];
    };

    [self doTestMigrationWithSourceClassFilter:filter
                             targetClassFIlter:filter
                                  andOperation:^{
                                      NSObject<MigrationTestObject>* newObject = [Random.shared migrationObjectWithClass:self.targetClass
                                                                                                           andIdentifier:self.filterObjects.lastObject.identifier];
                                      NSMutableArray<NSObject<MigrationTestObject>*>* expectedObjects = [NSMutableArray arrayWithArray:self.filterObjects];

                                      NSMutableArray<NSString*>* sqls = [NSMutableArray array];
                                      [sqls addObject:@"BEGIN IMMEDIATE"];
                                      [sqls addObject:[NSString stringWithFormat:@"INSERT OR IGNORE INTO %@%@(identifier, content) VALUES(?1, ?2)", self.schemaName, self.sourceTableName]];
                                      if (self.startMigration) {
                                          [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@%@ WHERE rowid == ?1", self.schemaName, self.sourceTableName]];
                                          if ([self.targetClass isAutoIncrement]) {
                                              [sqls addObject:@"INSERT OR IGNORE INTO main.testTable(identifier, content) VALUES(?1, ?2)"];
                                          } else if ([self.targetClass hasIntegerPrimaryKey]) {
                                              [sqls addObject:[NSString stringWithFormat:@"INSERT OR IGNORE INTO main.testTable(identifier, content, rowid) VALUES(?1, ?2, ?%d)", SQLITE_MAX_VARIABLE_NUMBER]];
                                          } else {
                                              [sqls addObject:@"SELECT max(rowid) + 1 FROM main.testTable"];
                                              [sqls addObject:[NSString stringWithFormat:@"INSERT OR IGNORE INTO main.testTable(identifier, content, rowid) VALUES(?1, ?2, ?%d)", SQLITE_MAX_VARIABLE_NUMBER]];
                                          }
                                      }
                                      [sqls addObject:@"COMMIT"];

                                      [self doTestObjects:expectedObjects
                                                  andSQLs:sqls
                                        afterModification:^BOOL {
                                            return [self.table insertOrIgnoreObject:newObject];
                                        }];
                                  }];
}

- (void)test_insert_fail
{
    TestCaseLog(@"Start test insert fail");

    auto filter = ^BOOL(Class<MigrationTestObject> cls) {
        return [cls hasIntegerPrimaryKey];
    };

    [self doTestMigrationWithSourceClassFilter:filter
                             targetClassFIlter:filter
                                  andOperation:^{
                                      NSObject<MigrationTestObject>* newObject = [Random.shared migrationObjectWithClass:self.targetClass
                                                                                                           andIdentifier:self.filterObjects.lastObject.identifier];
                                      TestCaseAssertFalse([self.table insertObject:newObject]);
                                  }];
}

- (void)test_delete_with_limit
{
    TestCaseLog(@"Start test delelte with limit");

    [self doTestMigration:^{
        NSMutableArray<NSObject<MigrationTestObject>*>* expectedObjects = [NSMutableArray arrayWithArray:self.filterObjects];
        [expectedObjects removeObjectAtIndex:2];

        NSMutableArray<NSString*>* sqls = [NSMutableArray array];
        [sqls addObject:@"BEGIN IMMEDIATE"];
        int minId = expectedObjects.firstObject.identifier;
        if ([self.targetClass hasIntegerPrimaryKey]) {
            [sqls addObject:[NSString stringWithFormat:@"SELECT identifier FROM temp.wcdb_union_testTable WHERE identifier > %d ORDER BY identifier ASC LIMIT 1 OFFSET 1", minId]];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@%@ WHERE identifier == ?%d", self.schemaName, self.sourceTableName, SQLITE_MAX_VARIABLE_NUMBER]];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM main.testTable WHERE identifier == ?%d", SQLITE_MAX_VARIABLE_NUMBER]];
        } else {
            [sqls addObject:[NSString stringWithFormat:@"SELECT rowid FROM temp.wcdb_union_testTable WHERE identifier > %d ORDER BY identifier ASC LIMIT 1 OFFSET 1", minId]];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@%@ WHERE rowid == ?%d", self.schemaName, self.sourceTableName, SQLITE_MAX_VARIABLE_NUMBER]];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM main.testTable WHERE rowid == ?%d", SQLITE_MAX_VARIABLE_NUMBER]];
        }
        [sqls addObject:@"COMMIT"];

        [self doTestObjects:expectedObjects
                    andSQLs:sqls
          afterModification:^BOOL {
              return [self.table deleteObjectsWhere:[self.targetClass identifier] > minId
                                             orders:[self.targetClass identifier].asOrder(WCTOrderedAscending)
                                              limit:1
                                             offset:1];
          }];
    }];
}

- (void)test_update_with_limit
{
    TestCaseLog(@"Start test update with limit");

    [self doTestMigration:^{
        NSMutableArray<NSObject<MigrationTestObject>*>* expectedObjects = [NSMutableArray arrayWithArray:self.filterObjects];
        NSObject<MigrationTestObject>* secondObject = [expectedObjects objectAtIndex:2];
        NSObject<MigrationTestObject>* newSecondObject = [Random.shared migrationObjectWithClass:self.targetClass andIdentifier:secondObject.identifier];
        [expectedObjects setObject:newSecondObject atIndexedSubscript:2];

        NSMutableArray<NSString*>* sqls = [NSMutableArray array];
        [sqls addObject:@"BEGIN IMMEDIATE"];
        int minId = expectedObjects.firstObject.identifier;
        if ([self.targetClass hasIntegerPrimaryKey]) {
            [sqls addObject:[NSString stringWithFormat:@"SELECT identifier FROM temp.wcdb_union_testTable WHERE identifier > %d ORDER BY identifier ASC LIMIT 1 OFFSET 1", minId]];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE %@%@ SET content = ?1 WHERE identifier == ?%d", self.schemaName, self.sourceTableName, SQLITE_MAX_VARIABLE_NUMBER]];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE main.testTable SET content = ?1 WHERE identifier == ?%d", SQLITE_MAX_VARIABLE_NUMBER]];
        } else {
            [sqls addObject:[NSString stringWithFormat:@"SELECT rowid FROM temp.wcdb_union_testTable WHERE identifier > %d ORDER BY identifier ASC LIMIT 1 OFFSET 1", minId]];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE %@%@ SET content = ?1 WHERE rowid == ?%d", self.schemaName, self.sourceTableName, SQLITE_MAX_VARIABLE_NUMBER]];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE main.testTable SET content = ?1 WHERE rowid == ?%d", SQLITE_MAX_VARIABLE_NUMBER]];
        }
        [sqls addObject:@"COMMIT"];

        [self doTestObjects:expectedObjects
                    andSQLs:sqls
          afterModification:^BOOL {
              return [self.table updateProperties:[self.targetClass content]
                                         toObject:newSecondObject
                                            where:[self.targetClass identifier] > minId
                                           orders:[self.targetClass identifier].asOrder(WCTOrderedAscending)
                                            limit:1
                                           offset:1];
          }];
    }];
}

- (void)test_select
{
    TestCaseLog(@"Start test select");
    [self doTestMigration:^{
        NSObject<MigrationTestObject>* expectedObject = self.filterObjects.lastObject;
        NSString* sql = [NSString stringWithFormat:@"SELECT identifier, content FROM temp.wcdb_union_testTable WHERE identifier == %d ORDER BY rowid ASC", expectedObject.identifier];

        [self doTestObjects:@[ expectedObject ]
                     andSQL:sql
                bySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                    return [self.table getObjectsWhere:[self.targetClass identifier] == expectedObject.identifier];
                }];

        sql = @"SELECT identifier, content FROM temp.wcdb_union_testTable ORDER BY identifier DESC LIMIT 1";

        [self doTestObjects:@[ expectedObject ]
                     andSQL:sql
                bySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                    return [self.table getObjectsOrders:[self.targetClass identifier].asOrder(WCTOrderedDescending) limit:1];
                }];
    }];
}

- (void)test_drop_table
{
    TestCaseLog(@"Start test drop table");
    [self doTestMigration:^{
        NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                      @"DROP TABLE IF EXISTS main.testTable",
                                      [NSString stringWithFormat:@"DELETE FROM %@%@", self.schemaName, self.sourceTableName],
                                      @"COMMIT" ];

        [self doTestSQLs:sqls
             inOperation:^BOOL {
                 return [self.database dropTable:self.tableName];
             }];
    }];
}

- (void)test_subquery_within_update
{
    TestCaseLog(@"Start test subquery within update");

    [self doTestMigration:^{
        NSMutableArray<NSObject<MigrationTestObject>*>* expectedObjects = [NSMutableArray arrayWithArray:self.filterObjects];
        NSObject<MigrationTestObject>* newObject = [Random.shared migrationObjectWithClass:self.targetClass andIdentifier:expectedObjects.lastObject.identifier];
        [expectedObjects removeLastObject];
        [expectedObjects addObject:newObject];

        NSMutableArray<NSString*>* sqls = [NSMutableArray array];
        [sqls addObject:@"BEGIN IMMEDIATE"];
        if ([self.targetClass hasIntegerPrimaryKey]) {
            [sqls addObject:@"SELECT identifier FROM temp.wcdb_union_testTable WHERE temp.wcdb_union_testTable.identifier IN(SELECT max(identifier) FROM temp.wcdb_union_testTable)"];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE %@%@ SET content = ?1 WHERE identifier == ?%d", self.schemaName, self.sourceTableName, SQLITE_MAX_VARIABLE_NUMBER]];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE main.testTable SET content = ?1 WHERE identifier == ?%d", SQLITE_MAX_VARIABLE_NUMBER]];
        } else {
            [sqls addObject:@"SELECT rowid FROM temp.wcdb_union_testTable WHERE temp.wcdb_union_testTable.identifier IN(SELECT max(identifier) FROM temp.wcdb_union_testTable)"];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE %@%@ SET content = ?1 WHERE rowid == ?%d", self.schemaName, self.sourceTableName, SQLITE_MAX_VARIABLE_NUMBER]];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE main.testTable SET content = ?1 WHERE rowid == ?%d", SQLITE_MAX_VARIABLE_NUMBER]];
        }
        [sqls addObject:@"COMMIT"];

        [self doTestObjects:expectedObjects
                    andSQLs:sqls
          afterModification:^BOOL {
              return [self.table updateProperties:[self.targetClass content]
                                         toObject:newObject
                                            where:[self.targetClass identifier].table(self.tableName).in(WCDB::StatementSelect().select([self.targetClass identifier].max()).from(self.tableName))];
          }];
    }];
}

- (void)test_subquery_within_delete
{
    TestCaseLog(@"Start test subquery within delete");

    [self doTestMigration:^{
        NSMutableArray<NSObject<MigrationTestObject>*>* expectedObjects = [NSMutableArray arrayWithArray:self.filterObjects];
        [expectedObjects removeLastObject];

        NSMutableArray<NSString*>* sqls = [NSMutableArray array];
        [sqls addObject:@"BEGIN IMMEDIATE"];
        if ([self.targetClass hasIntegerPrimaryKey]) {
            [sqls addObject:@"SELECT identifier FROM temp.wcdb_union_testTable WHERE temp.wcdb_union_testTable.identifier IN(SELECT max(identifier) FROM temp.wcdb_union_testTable)"];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@%@ WHERE identifier == ?%d", self.schemaName, self.sourceTableName, SQLITE_MAX_VARIABLE_NUMBER]];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM main.testTable WHERE identifier == ?%d", SQLITE_MAX_VARIABLE_NUMBER]];
        } else {
            [sqls addObject:@"SELECT rowid FROM temp.wcdb_union_testTable WHERE temp.wcdb_union_testTable.identifier IN(SELECT max(identifier) FROM temp.wcdb_union_testTable)"];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@%@ WHERE rowid == ?%d", self.schemaName, self.sourceTableName, SQLITE_MAX_VARIABLE_NUMBER]];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM main.testTable WHERE rowid == ?%d", SQLITE_MAX_VARIABLE_NUMBER]];
        }
        [sqls addObject:@"COMMIT"];

        [self doTestObjects:expectedObjects
                    andSQLs:sqls
          afterModification:^BOOL {
              return [self.table deleteObjectsWhere:[self.targetClass identifier].table(self.tableName).in(WCDB::StatementSelect().select([self.targetClass identifier].max()).from(self.tableName))];
          }];
    }];
}

@end
