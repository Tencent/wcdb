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

#import "WCTCommon.h"

NS_ASSUME_NONNULL_BEGIN

WCDB_API @interface WCTMigrationBaseInfo : NSObject

- (instancetype)init UNAVAILABLE_ATTRIBUTE;

// Target table of migration
@property (nonatomic, readonly) NSString* table;
// Source table of migration
@property (nonatomic, readonly) NSString* sourceTable;
// Filter condition of source table
@property (nonatomic, readonly) WCDB::Expression filterCondition;

@end

WCDB_API @interface WCTMigrationUserInfo : WCTMigrationBaseInfo

// Set new value to sourceTable in WCTMigrationBaseInfo.
- (void)setSourceTable:(NSString*)table;
// Set new value to filterCondition in WCTMigrationBaseInfo.
- (void)setFilterCondition:(const WCDB::Expression&)condition;

@end

NS_ASSUME_NONNULL_END
