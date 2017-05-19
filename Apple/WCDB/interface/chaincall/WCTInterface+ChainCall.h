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
#import <WCDB/WCTInterface.h>
#import <WCDB/WCTDeclare.h>

@interface WCTInterface(ChainCall)

- (WCTInsert*)prepareInsertObjectsOfClass:(Class)cls into:(NSString*)tableName;
- (WCTInsert*)prepareInsertOrReplaceObjectsOfClass:(Class)cls into:(NSString*)tableName;
- (WCTInsert*)prepareInsertObjectsOnProperties:(const WCTPropertyList&)propertyList into:(NSString*)tableName;
- (WCTInsert*)prepareInsertOrReplaceObjectsOnProperties:(const WCTPropertyList&)propertyList into:(NSString*)tableName;

- (WCTDelete*)prepareDeleteFromTable:(NSString*)tableName;

- (WCTUpdate*)prepareUpdateTable:(NSString*)tableName onProperties:(const WCTPropertyList&)propertyList;

- (WCTSelect*)prepareSelectObjectsOfClass:(Class)cls fromTable:(NSString*)tableName;
- (WCTSelect*)prepareSelectObjectsOnResults:(const WCTResultList&)resultList fromTable:(NSString*)tableName;
- (WCTRowSelect*)prepareSelectRowsOnResults:(const WCTResultList&)resultList fromTable:(NSString*)tableName;
- (WCTRowSelect*)prepareSelectRowsOnResults:(const WCTResultList&)resultList fromTables:(NSArray<NSString*>*)tableNames;
- (WCTMultiSelect*)prepareSelectMultiObjectsOnResults:(const WCTResultList&)resultList fromTables:(NSArray<NSString*>*)tableNames;

@end
