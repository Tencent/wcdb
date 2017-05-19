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

#import <Foundation/Foundation.h>
#import <WCDB/WCTTransaction.h>

@interface WCTTransaction(Table)

- (BOOL)createTableAndIndexesOfName:(NSString*)tableName withClass:(Class<WCTTableCoding>)cls;

- (BOOL)createVirtualTableOfName:(NSString*)tableName usingModule:(NSString*)moduleName withClass:(Class<WCTTableCoding>)cls;

- (BOOL)createTableOfName:(NSString*)tableName withColumnDefList:(const WCTColumnDefList&)columnDefList;

- (BOOL)createTableOfName:(NSString*)tableName withColumnDefList:(const WCTColumnDefList&)columnDefList andConstraintList:(const WCTTableConstraintList&)constraintList;

- (BOOL)isTableExists:(NSString*)tableName;

- (BOOL)dropTableOfName:(NSString*)tableName;

- (BOOL)createIndexOfName:(NSString*)indexName withIndexList:(const WCTIndexList&)indexList forTable:(NSString*)tableName;

- (BOOL)dropIndexOfName:(NSString*)indexName;

- (BOOL)addColumn:(const WCTColumnDef&)columnDef forTable:(NSString*)tableName;

@end
