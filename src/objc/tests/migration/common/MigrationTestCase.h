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

#import "MigrationTestObject.h"
#import "MigrationTestSourceObject.h"
#import "TestCase.h"

@interface MigrationTestCase : TableTestCase

@property (nonatomic, assign) BOOL startMigration;
@property (nonatomic, assign) BOOL isCrossDatabaseMigration;
@property (nonatomic, assign) BOOL needCipher;
@property (nonatomic, assign) BOOL needFilter;
@property (nonatomic, assign) int missColumnCount;

@property (nonatomic, strong) Class<MigrationTestSourceObject> sourceClass;
@property (nonatomic, strong) Class<MigrationTestObject> targetClass;

@property (nonatomic, readonly) NSString* schemaName;
@property (nonatomic, readonly) NSString* sourceTableName;
@property (nonatomic, readonly) NSString* sourcePath;
@property (nonatomic, readonly) WCTDatabase* sourceDatabase;
@property (nonatomic, readonly) WCTTable* sourceTable;

@property (readonly, nonatomic) NSArray<NSObject<MigrationTestSourceObject>*>* objects;
@property (readonly, nonatomic) NSArray<NSObject<MigrationTestObject>*>* filterObjects;

+ (NSArray<Class<MigrationTestObject>>*)allClasses;
+ (NSArray<Class<MigrationTestSourceObject>>*)allSourceClasses;

- (void)createTables;
- (void)configMigration;
- (void)clearData;

@end
