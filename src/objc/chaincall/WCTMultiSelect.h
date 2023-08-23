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

#import "WCTSelectable.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Not Thread-safe
 */
WCDB_API @interface WCTMultiSelect : WCTSelectable

/**
 @brief WINQ interface for SQL.
 @param tableNames The names of the tables to query data from.
 @return self
 */
- (instancetype)fromTables:(NSArray<NSString *> *)tableNames;

/**
 @brief WINQ interface for SQL.
 @param resultColumns The column results to be selected.
 @return self
 */
- (instancetype)onResultColumns:(const WCTResultColumns &)resultColumns;

/**
 @brief Get first selected objects.
 
     WCTMultiObject* multiObject = [multiSelect firstMultiObject];
     Class1* object1 = [multiObject objectForKey:tableName1];
     Class2* object2 = [multiObject objectForKey:tableName2];

 @return See the example above
 */
- (nullable WCTMultiObject *)firstMultiObject;

/**
 @brief Get all selected objects. 
 @return Array with all selected objects.
 */
- (nullable NSArray<WCTMultiObject *> *)allMultiObjects;

@end

NS_ASSUME_NONNULL_END
