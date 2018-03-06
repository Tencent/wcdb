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

#import <WCDB/WCTDatabase.h>

@interface WCTDatabase (Table)

/**
 @brief Create table and indexes from ORM if not exists.
        Note that it will add newly defined columns automatically.
        Note that it will run embedded transaction.
        The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
 @param tableName This would be the name of the table and the prefix of the index names.
 @param cls class
 @return YES while creating table or adding newly defined columns succeeds. It will not fail if error occurs during creating indexes.  
 */
- (BOOL)createTableAndIndexesOfName:(NSString *)tableName
                          withClass:(Class<WCTTableCoding>)cls;

/**
 @brief Get a wrapper from an existing table.
 @param tableName The name of the table.
 @param cls a class implement WCTTableCoding.
 @return WCTTable nil for a non-existent table.
 */
- (WCTTable *)getTableOfName:(NSString *)tableName
                   withClass:(Class<WCTTableCoding>)cls;

/**
 @brief Create virtual table from ORM if not exists.
 @see http://www.sqlite.org/vtab.html
 @param tableName The name of the virtual table to be created.
 @param cls a class implement WCTTableCoding.
 @return YES only if no error occurs.
 */
- (BOOL)createVirtualTableOfName:(NSString *)tableName
                       withClass:(Class<WCTTableCoding>)cls;

/**
 @brief Drop table if exists.
 @param tableName The name of the table to be dropped.
 @return YES only if no error occurs.
 */
- (BOOL)dropTableOfName:(NSString *)tableName;

/**
 @brief Drop index if not exists.
 @param indexName The name of the index to be dropped.
 @return YES only if no error occurs.
 */
- (BOOL)dropIndexOfName:(NSString *)indexName;

@end
