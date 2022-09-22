//
// Created by 陈秋文 on 2022/8/13.
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
#import <WCDB/Value.hpp>
#import <WCDB/WINQ.h>

@interface CPPTableTestCase : CPPDatabaseTestCase

@property (nonatomic, retain) NSString* tableName;

- (BOOL)createTable;

- (WCDB::Columns)columns;
- (WCDB::ResultColumns)resultColumns;

- (BOOL)dropTable;

- (void)doTestRow:(const WCDB::OneRowValue&)row
           andSQL:(NSString*)sql
      bySelecting:(WCDB::OneRowValue (^)())block;

- (void)doTestColumn:(const WCDB::OneColumnValue&)column
              andSQL:(NSString*)sql
         bySelecting:(WCDB::OneColumnValue (^)())block;

- (void)doTestValue:(WCDB::Value)value
             andSQL:(NSString*)sql
        bySelecting:(WCDB::Value (^)())block;

- (void)doTestRows:(const WCDB::MultiRowsValue&)rows
            andSQL:(NSString*)sql
       bySelecting:(WCDB::MultiRowsValue (^)())block;

- (void)doTestRows:(const WCDB::MultiRowsValue&)rows
           andSQLs:(NSArray<NSString*>*)sqls
       bySelecting:(WCDB::MultiRowsValue (^)())block;

- (WCDB::MultiRowsValue)getAllvalues;

@end
