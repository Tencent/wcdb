//
// Created by qiuwenchen on 2022/8/13.
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

#import "CPPDatabaseTestCase.h"
#import "CPPTestCaseObject.h"

@interface CPPTableTestCase : CPPDatabaseTestCase

@property (nonatomic, retain) NSString *tableName;
- (BOOL)dropTable;

#pragma mark - Object
@property (nonatomic, readonly) WCDB::Table<CPPTestCaseObject> &table;
- (BOOL)createObjectTable;

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject> &)objects
               andSQL:(NSString *)sql
    afterModification:(BOOL (^)())block;

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject> &)objects
              andSQLs:(NSArray<NSString *> *)sqls
    afterModification:(BOOL (^)())block;

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject> &)objects
            andNumber:(int)numberOfInsertSQLs
         ofInsertSQLs:(NSString *)insertSQL
       afterInsertion:(BOOL (^)())block;

- (void)doTestObject:(const CPPTestCaseObject &)object
              andSQL:(NSString *)sql
         bySelecting:(WCDB::Optional<CPPTestCaseObject> (^)())block;

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject> &)objects
               andSQL:(NSString *)sql
          bySelecting:(WCDB::OptionalValueArray<CPPTestCaseObject> (^)())block;

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject> &)expectedObjects
              andSQLs:(NSArray<NSString *> *)expectedSQLs
          bySelecting:(WCDB::OptionalValueArray<CPPTestCaseObject> (^)())block;

- (WCDB::ValueArray<CPPTestCaseObject>)getAllObjects;

#pragma mark - Value
- (BOOL)createValueTable;
- (WCDB::Columns)columns;
- (WCDB::ResultColumns)resultColumns;

- (void)doTestRow:(const WCDB::OneRowValue &)row
           andSQL:(NSString *)sql
      bySelecting:(WCDB::OptionalOneRow (^)())block;

- (void)doTestColumn:(const WCDB::OneColumnValue &)column
              andSQL:(NSString *)sql
         bySelecting:(WCDB::OptionalOneColumn (^)())block;

- (void)doTestValue:(const WCDB::Value &)value
             andSQL:(NSString *)sql
        bySelecting:(WCDB::OptionalValue (^)())block;

- (void)doTestRows:(const WCDB::MultiRowsValue &)rows
            andSQL:(NSString *)sql
       bySelecting:(WCDB::OptionalMultiRows (^)())block;

- (void)doTestRows:(const WCDB::MultiRowsValue &)rows
           andSQLs:(NSArray<NSString *> *)sqls
       bySelecting:(WCDB::OptionalMultiRows (^)())block;

- (WCDB::MultiRowsValue)getAllvalues;

@end
