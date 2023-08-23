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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class WCTDatabase;
@class WCTError;

WCDB_API @interface WCTTable<ObjectType> : NSObject

/**
 You should use `-[WCTDatabase getTable:withClass:]` or `-[WCTHandle getTable:withClass:]` to obtain `WCTTable`.
 */
- (instancetype)init UNAVAILABLE_ATTRIBUTE;

/**
 The name of the table.
 */
@property (nonatomic, readonly) NSString *tableName;

/**
 The class binding to this table.
 */
@property (nonatomic, readonly) Class tableClass;

/**
 The database that generate current table.
 */
@property (nonatomic, readonly) WCTDatabase *database;

/**
 @brief Get the most recent error for the database of current table in the current thread.
        Since it is too cumbersome to get the error after every database operation, it‘s better to use monitoring interfaces to obtain database errors and print them to the log.
 @see   `[WCTDatabase globalTraceError:]`
 @see   `[WCTDatabase traceError:]`
 
 @return WCTError
 */
- (WCTError *)error;

@end

NS_ASSUME_NONNULL_END
