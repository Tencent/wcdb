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

#ifndef WCDB_OMIT_DEPRECATED

#import <Foundation/Foundation.h>
#import <WCDB/WCTStatement.h>

@interface WCTStatement (Compatible)

/**
 @brief The wrapper of sqlite3_column_count.
 @return Count of column result.
 */
- (int)getCount DEPRECATED_MSG_ATTRIBUTE("-getCount is deprecated since v1.0.4. Use -getColumnCount instead");

/**
 @brief The wrapper of sqlite3_column_name. Use -getColumNameAtIndex: instead.
 @param index Begin with 0.
 @return The description of column result.
 */
- (NSString *)getNameAtIndex:(int)index DEPRECATED_MSG_ATTRIBUTE("-getNameAtIndex: is deprecated since v1.0.4. Use -getColumNameAtIndex: instead");

/**
 @brief The wrapper of sqlite3_reset.
 */
- (void)resetBinding DEPRECATED_MSG_ATTRIBUTE("-resetBinding is deprecated since v1.0.4. Use -reset instead");

@end

#endif //WCDB_OMIT_DEPRECATED
