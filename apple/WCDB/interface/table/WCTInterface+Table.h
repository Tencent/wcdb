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
#import <WCDB/WCTDeclare.h>
#import <WCDB/WCTInterface.h>
#import <WCDB/error.hpp>

@interface WCTInterface (Table)

- (BOOL)createTableAndIndexesOfName:(NSString *)tableName withClass:(Class<WCTTableCoding>)cls andError:(WCDB::Error &)error;

- (BOOL)createVirtualTableOfName:(NSString *)tableName withClass:(Class)cls andError:(WCDB::Error &)error;

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefList:(const WCTColumnDefList &)columnDefList andError:(WCDB::Error &)error;

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefList:(const WCTColumnDefList &)columnDefList andConstraintList:(const WCTTableConstraintList &)constraintList andError:(WCDB::Error &)error;

- (BOOL)addColumn:(const WCTColumnDef &)columnDef forTable:(NSString *)tableName withError:(WCDB::Error &)error;

- (BOOL)dropTableOfName:(NSString *)tableName withError:(WCDB::Error &)error;

- (BOOL)isTableExists:(NSString *)tableName withError:(WCDB::Error &)error;

- (BOOL)createIndexOfName:(NSString *)indexName withIndexList:(const WCTIndexList &)indexList forTable:(NSString *)tableName andError:(WCDB::Error &)error;

- (BOOL)dropIndexOfName:(NSString *)indexName withError:(WCDB::Error &)error;

@end
