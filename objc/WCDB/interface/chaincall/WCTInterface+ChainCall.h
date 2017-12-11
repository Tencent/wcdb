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

@interface WCTInterface (ChainCall)

/**
 @brief Generate a WCTInsert to do an insertion.
 @param cls The class of the object to be inserted.
 @param tableName The name of the table to be used to do an insertion.
 @return WCTInsert
 */
- (WCTInsert *)prepareInsertObjectsOfClass:(Class)cls into:(NSString *)tableName;

/**
 @brief Generate a WCTInsert to do an insertion or replacement.
 @param cls The class of the object to be inserted.
 @param tableName The name of the table to be used to do an insertion.
 @return WCTInsert
 */
- (WCTInsert *)prepareInsertOrReplaceObjectsOfClass:(Class)cls into:(NSString *)tableName;

/**
 @brief Generate a WCTInsert to do a partial insertion.
 @param propertyList The specific properties to be inserted.
 @param tableName The name of the table to be used to do an insertion.
 @return WCTInsert
 */
- (WCTInsert *)prepareInsertObjectsOnProperties:(const WCTPropertyList &)propertyList into:(NSString *)tableName;

/**
 @brief Generate a WCTInsert to do a partial insertion or replacement.
 @param propertyList The specific properties to be inserted.
 @param tableName The name of the table to be used to do an insertion.
 @return WCTInsert
 */
- (WCTInsert *)prepareInsertOrReplaceObjectsOnProperties:(const WCTPropertyList &)propertyList into:(NSString *)tableName;

/**
 @brief Generate a WCTDelete to do a deletion.
 @param tableName The name of the table to be used to do a deletion.
 @return WCTDelete
 */
- (WCTDelete *)prepareDeleteFromTable:(NSString *)tableName;

/**
 @brief Generate a WCTUpdate to do an update.
 @param tableName The name of the table to be used to do an update.
 @param propertyList The specific properties to be updated.
 @return WCTUpdate
 */
- (WCTUpdate *)prepareUpdateTable:(NSString *)tableName onProperties:(const WCTPropertyList &)propertyList;

/**
 @brief Generate a WCTSelect to do an object selection.
 @param cls The class of the object to be selected.
 @param tableName The name of the table to be used to do a selection.
 @return WCTSelect
 */
- (WCTSelect *)prepareSelectObjectsOfClass:(Class)cls fromTable:(NSString *)tableName;

/**
 @brief Generate a WCTSelect to do an object selection on the specific results.
 @param resultList The column results to be selected.
 @param tableName The name of the table to be used to do a selection.
 @return WCTSelect
 */
- (WCTSelect *)prepareSelectObjectsOnResults:(const WCTResultList &)resultList fromTable:(NSString *)tableName;

/**
 @brief Generate a WCTRowSelect to do a row selection on the specific results.
 @param resultList The column results to be selected.
 @param tableName The name of the table to be used to do a selection.
 @return WCTRowSelect
 */
- (WCTRowSelect *)prepareSelectRowsOnResults:(const WCTResultList &)resultList fromTable:(NSString *)tableName;

/**
 @brief Generate a WCTRowSelect to do a row selection on the specific results.
 @param resultList The column results to be selected.
 @param tableNames The name of the table to be used to do a selection.
 @return WCTRowSelect
 */
- (WCTRowSelect *)prepareSelectRowsOnResults:(const WCTResultList &)resultList fromTables:(NSArray<NSString *> *)tableNames;

/**
 @brief Generate a WCTMultiSelect to do a cross table selection on the specific results.
 @param resultList The column results to be selected.
 @param tableNames The name of the table to be used to do a selection.
 @return WCTMultiSelect
 */
- (WCTMultiSelect *)prepareSelectMultiObjectsOnResults:(const WCTResultList &)resultList fromTables:(NSArray<NSString *> *)tableNames;

@end
