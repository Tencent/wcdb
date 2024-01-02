//
// Created by sanhuazhang on 2019/05/02
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

#import "WCTChainCall.h"

NS_ASSUME_NONNULL_BEGIN

// TODO: implement insertOrUpdate through upsert feature
/**
 Not Thread-safe
 */
WCDB_API @interface WCTInsert<ObjectType> : WCTChainCall

/**
 @brief The statement that `WCTInsert` will execute.
 You can customize this statement directly to implement the capabilities not provided by the following methods.
 */
- (WCDB::StatementInsert &)statement;

/**
 @brief WINQ interface for SQL.
 @return self
 */
- (instancetype)orReplace;

/**
 @brief WINQ interface for SQL.
 @return self
 */
- (instancetype)orIgnore;

/**
 @brief WINQ interface for SQL.
 @param tableName The name of the table to insert objects to.
 @return self
 */
- (instancetype)intoTable:(NSString *)tableName;

/**
 @brief WINQ interface for SQL.
 @param properties Do a partial insertion with the specific properties.
 @return self
 */
- (instancetype)onProperties:(const WCTProperties &)properties;

/**
 @brief Insert an array of objects.
 @param objects Objects to be inserted into table.
 @return self.
 */
- (instancetype)values:(NSArray<ObjectType> *)objects;

/**
 @brief Insert one object.
 @param object Object to be inserted into table.
 @return self.
 */
- (instancetype)value:(ObjectType)object;

/**
 @brief Execute the insert statement.
        Note that it will run embedded transaction while objects.count>1 .
        The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
 @return YES if no error occurs.
 */
- (BOOL)execute;

@end

NS_ASSUME_NONNULL_END
