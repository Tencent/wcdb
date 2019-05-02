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

#import "BaseTestCase.h"
#import "ReusableFactory.h"
#import <WCDB/WCDB.h>

typedef NS_ENUM(NSUInteger, DatabaseTestCaseExpect) {
    DatabaseTestCaseExpectAllSQLs = 0,
    DatabaseTestCaseExpectFirstFewSQLs,
    DatabaseTestCaseExpectSomeSQLs,
};

@interface DatabaseTestCase : BaseTestCase <ReusableFactoryPreparation>

#pragma mark - Path
@property (nonatomic, retain) NSString* path;

#pragma mark - Database
@property (readonly, nonatomic) WCTDatabase* database;

#pragma mark - Factory
@property (nonatomic, readonly) ReusableFactory* factory;

#pragma mark - SQL
+ (void)enableSQLTrace;
+ (void)disableSQLTrace;

#pragma mark - Test
@property (nonatomic, assign) BOOL expectSQLsInAllThreads;
@property (nonatomic, assign) DatabaseTestCaseExpect expectMode;

- (void)doTestSQLs:(NSArray<NSString*>*)expectedSQLs
       inOperation:(BOOL (^)())block;

- (BOOL)tryToMakeHeaderCorrupted;

@end
