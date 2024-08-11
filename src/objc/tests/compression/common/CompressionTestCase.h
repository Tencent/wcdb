//
// Created by qiuwenchen on 2023/12/3.
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

#import "CompressionTestObject.h"
#import "TestCase.h"

typedef enum : NSUInteger {
    CompressionMode_Normal = 0,
    CompressionMode_Dict,
    CompressionMode_VariousDict,
} CompressionMode;

typedef enum : NSUInteger {
    CompressionStatus_uncompressed = 0,
    CompressionStatus_startCompressed,
    CompressionStatus_finishCompressed,
} CompressionStatus;

typedef enum : NSUInteger {
    MigrationStatus_noMigration = 0,
    MigrationStatus_unmigrated,
    MigrationStatus_startMigrate,
} MigrationStatus;

@interface CompressionTestCase : TableTestCase

@property (nonatomic, assign) CompressionMode mode;
@property (nonatomic, assign) BOOL compressTwoColumn;
@property (nonatomic, assign) CompressionStatus compressionStatus;
@property (nonatomic, assign) MigrationStatus migrationStatus;
@property (nonatomic, assign) BOOL needCipher;

@property (nonatomic, strong) NSString* anotherTable;

- (NSString*)sourceSchema;
- (NSString*)uncompressTableName;
- (WCTTable*)uncompressTable;
- (NSDictionary<NSNumber*, NSNumber*>*)matchDictForText;
- (NSDictionary<NSNumber*, NSNumber*>*)matchDictForBLOB;
- (NSArray<CompressionTestObject*>*)originObjects;

- (void)configCompression;
- (void)configCompressFilter;
- (void)reconfigCompressionFilter;
- (void)clearData;
- (void)doTestCompression:(void (^)(void))operation;
- (void)executeModify:(BOOL (^)(WCTTable*))modify andSQLs:(NSArray<NSString*>*)sqls;
- (void)executeSelect:(NSArray* (^)(WCTTable*) )select andSQLs:(NSArray<NSString*>*)sqls;

@end
