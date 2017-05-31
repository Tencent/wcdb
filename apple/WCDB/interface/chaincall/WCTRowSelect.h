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
#import <WCDB/WCTSelectBase.h>

/**
 Not Thread-safe

 If you only select one column result from table, you can use nextValue and allValues interface to avoid unwraping for row.
 Otherwise, you should use nextRow and allRows interface.
 */
@interface WCTRowSelect : WCTSelectBase

/**
 @brief Get next selected row. You can do an iteration using it.
 
     WCTOneRow* row = nil;
     while ((row = [rowSelect nextRow])) {
         NSString* result1 = row[0];
         NSNumber* result2 = row[1];
         NSData* result3 = row[2];
         NSNull* result4 = row[3];
         //...
     }

 @return WCTOneRow is a NSArray collection of WCTValue.
         The real type of WCTValue depends on your selection, which can be NSString, NSNumber, NSData or NSNull.
         See the example above.
 */
- (WCTOneRow *)nextRow;

/**
 @brief Get all selected row.
 
     for (WCTOneRow* row in [rowSelect allRows]) {
         NSString* result1 = row[0];
         NSNumber* result2 = row[1];
         NSData* result3 = row[2];
         NSNull* result4 = row[3];
         //...
     }

 @return WCTColumnsXRows is a NSArray collection of WCTOneRow. 
         Since WCTOneRow is a wrapper of NSArray, WCTColumnsXRows is a dual array.
         The real type of WCTValue depends on your selection, which can be NSString, NSNumber, NSData or NSNull.
         See the example above.         
 */
- (WCTColumnsXRows *)allRows;

/**
 @brief Get next selected value. You can do an iteration using it.
 
     WCTValue* value = nil;
     while ((value = [rowSelect nextValue])) {
         if ([value isKindOfClass:NSString.class]) {
             //...
         }else if ([value isKindOfClass:NSNumber.class]) {
             //...
         }else if ([value isKindOfClass:NSData.class]) {
             //...
         }else if ([value isKindOfClass:NSNull.class]||value==nil) {
             //...
         }
     }

 @return The real type of WCTValue depends on your selection, which can be NSString, NSNumber, NSData or NSNull.
         See the example above.
 */
- (WCTValue *)nextValue;

/**
 @brief Get all selected values.
 
     for (WCTValue* value in [rowSelect allValues]) {
         if ([value isKindOfClass:NSString.class]) {
             //...
         }else if ([value isKindOfClass:NSNumber.class]) {
             //...
         }else if ([value isKindOfClass:NSData.class]) {
             //...
         }else if ([value isKindOfClass:NSNull.class]||value==nil) {
             //...
         }
     }

 @return WCTOneColumn is a NSArray collection of WCTValue. 
         The real type of WCTValue depends on your selection, which can be NSString, NSNumber, NSData or NSNull.
         See the example above.         
 */
- (WCTOneColumn *)allValues;

@end
