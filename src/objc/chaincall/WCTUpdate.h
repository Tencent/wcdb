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

/**
 Not Thread-safe
 */
WCDB_API @interface WCTUpdate<ObjectType> : WCTChainCall

/**
 @brief The statement that `WCTUpdate` will execute.
 You can customize this statement directly to implement the capabilities not provided by the following methods.
 */
- (WCDB::StatementUpdate &)statement;

/**
 @brief WINQ interface for SQL.
 @param tableName The name of the table to update content.
 @return self
 */
- (instancetype)table:(NSString *)tableName;

/**
 @brief WINQ interface for SQL.
 @param properties The properties used to specify the columns where data is updated.
 @return self
 */
- (instancetype)set:(const WCTProperties &)properties;

/**
 @brief WINQ interface for SQL.
 @param condition condition
 @return self
 */
- (instancetype)where:(const WCDB::Expression &)condition;

/**
 @brief WINQ interface for SQL.
 @param orders order list
 @return self
 */
- (instancetype)orders:(const WCDB::OrderingTerms &)orders;

/**
 @brief WINQ interface for SQL.
 @param limit limit
 @return self
 */
- (instancetype)limit:(const WCDB::Expression &)limit;

/**
 @brief WINQ interface for SQL.
 @param offset offset
 @return self
 */
- (instancetype)offset:(const WCDB::Expression &)offset;

/**
 @brief Execute the update statement with one object.
 @param object Object to be updated into table.
 @return self.
 */
- (instancetype)toObject:(ObjectType)object;

/**
 @brief Execute the update statement with one value.
 @param value Value to be updated into table.
 @return self.
 */
- (instancetype)toValue:(WCTColumnCodingValue *)value;

/**
 @brief Execute the update statement with one row of value.
 @param row The row to be updated into table.
 @return self.
 */
- (instancetype)toRow:(WCTColumnCodingRow *)row;

/**
 @brief Execute the update statement.
 @return YES if no error occurs.
 */
- (BOOL)execute;

@end

NS_ASSUME_NONNULL_END
