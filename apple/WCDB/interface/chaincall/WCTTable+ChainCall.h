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
#import <WCDB/WCTTable.h>

/**
 Thread-safe
 */
@interface WCTTable (ChainCall)

/**
 Generate a WCTInsert to do an insertion on this table.

 @return WCTInsert
 */
- (WCTInsert *)prepareInsertObjects;

/**
 Generate a WCTInsert to do an insertion or replacement on this table.

 @return WCTInsert
 */
- (WCTInsert *)prepareInsertOrReplaceObjects;

/**
 Generate a WCTInsert to do a partial insertion on this table.

 @param propertyList The specific properties to be inserted.
 @return WCTInsert
 */
- (WCTInsert *)prepareInsertObjectsOnProperties:(const WCTPropertyList &)propertyList;

/**
 Generate a WCTInsert to do a partial insertion or replacement on this table.

 @param propertyList The specific properties to be inserted.
 @return WCTInsert
 */
- (WCTInsert *)prepareInsertOrReplaceObjectsOnProperties:(const WCTPropertyList &)propertyList;

/**
 Generate a WCTDelete to do a deletion.

 @return WCTDelete
 */
- (WCTDelete *)prepareDelete;

/**
 Generate a WCTUpdate to do an update.

 @param propertyList The specific properties to be updated.
 @return WCTUpdate
 */
- (WCTUpdate *)prepareUpdateOnProperties:(const WCTPropertyList &)propertyList;

/**
 Generate a WCTSelect to do an object selection.

 @return WCTSelect
 */
- (WCTSelect *)prepareSelectObjects;

/**
 Generate a WCTSelect to do an object selection on the specific results.

 @param resultList The column results to be selected.
 @return WCTSelect
 */
- (WCTSelect *)prepareSelectObjectsOnResults:(const WCTResultList &)resultList;

/**
 Generate a WCTRowSelect to do a row selection on the specific results.

 @param resultList The column results to be selected.
 @return WCTRowSelect
 */
- (WCTRowSelect *)prepareSelectRowsOnResults:(const WCTResultList &)resultList;

@end
