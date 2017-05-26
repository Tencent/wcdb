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
#import <WCDB/WCTCore.h>

/**
 Not Thread-safe
 
 This class is a wrapper for sqlite3_stmt.
 */
@interface WCTStatement : WCTCore

/**
 @brief More detailed error messages.
 @return nil or error.isOK==YES if no error occurs.
 */
- (WCTError*)getError;

/**
 @brief The wrapper of sqlite3_bind_*.
 @param value The WCTValue can be NSString, NSNumber, NSData, NSNull and nil.
 @param index Begin with 1.
 @return NO only if you pass a incorrect type of value.
 */
- (BOOL)bindValue:(WCTValue*)value toIndex:(int)index;

/**
 @brief The wrapper of sqlite3_column_*.
 @param index Begin with 0.
 @return The real type of WCTValue depends on the value in database.
 */
- (WCTValue*)getValueAtIndex:(int)index;

/**
 @brief The wrapper of sqlite3_step.
 @return YES means you can continue stepping.
         NO means the stepping has been completed or an error occurs. 
 */
- (BOOL)step;

/**
 @brief The wrapper of sqlite3_reset.
 */
- (void)resetBinding;

/**
 @brief The wrapper of sqlite3_column_type.
 @param index Begin with 0.
 @return Column type.
 */
- (WCTColumnType)getTypeAtIndex:(int)index;

/**
 @brief The wrapper of sqlite3_column_count.
 @return Count of column result.
 */
- (int)getCount;

/**
 @brief The wrapper of sqlite3_column_name.
 @param index Begin with 0.
 @return The description of column result.
 */
- (NSString*)getNameAtIndex:(int)index;

/**
 @brief The wrapper of sqlite3_column_table_name.
 @param index Begin with 0
 @return The name of table
 */
- (NSString*)getTableNameAtIndex:(int)index;

/**
 @brief The wrapper of [sqlite3_finalize].
 */
- (void)finalize;

@end
