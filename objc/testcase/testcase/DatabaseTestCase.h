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

#import <TestCase/BaseTestCase.h>
#import <WCDB/WCDB.h>

@interface DatabaseTestCase : BaseTestCase

#pragma mark - Path
@property (readonly, nonatomic) NSString* path;
@property (readonly, nonatomic) NSString* walPath;
@property (nonatomic, readonly) NSString* factory;
@property (nonatomic, readonly) NSString* firstMaterial;
@property (nonatomic, readonly) NSString* lastMaterial;
@property (nonatomic, readonly) NSArray<NSString*>* paths;

#pragma mark - Database
@property (readonly, nonatomic) WCTDatabase* database;

#pragma mark - File
@property (nonatomic, readonly) int headerSize;
@property (nonatomic, readonly) int pageSize;
@property (nonatomic, readonly) int walHeaderSize;
@property (nonatomic, readonly) int walFrameHeaderSize;
@property (nonatomic, readonly) int walFrameSize;
- (int)getWalFrameCount;

#pragma mark - SQL
@property (nonatomic, assign) BOOL expectSQLsInAllThreads;
@property (nonatomic, assign) BOOL expectFirstFewSQLsOnly;

- (void)doTestSQLs:(NSArray<NSString*>*)expectedSQLs
       inOperation:(BOOL (^)())block;

+ (void)enableSQLTrace;
+ (void)disableSQLTrace;

@end
