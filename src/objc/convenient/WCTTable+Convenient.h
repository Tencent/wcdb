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

#import "WCTTable.h"

NS_ASSUME_NONNULL_BEGIN

WCDB_API @interface WCTTable<ObjectType>(Convenient)

#pragma mark - Get Object
- (nullable ObjectType)getObject;

- (nullable ObjectType)getObjectWhere:(const WCDB::Expression &)condition;

- (nullable ObjectType)getObjectOrders:(const WCDB::OrderingTerms &)orders;

- (nullable ObjectType)getObjectOffset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectWhere:(const WCDB::Expression &)condition
                               orders:(const WCDB::OrderingTerms &)orders;

- (nullable ObjectType)getObjectWhere:(const WCDB::Expression &)condition
                               offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectOrders:(const WCDB::OrderingTerms &)orders
                                offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectWhere:(const WCDB::Expression &)condition
                               orders:(const WCDB::OrderingTerms &)orders
                               offset:(const WCDB::Expression &)offset;

#pragma mark - Get Objects
- (nullable NSArray<ObjectType> *)getObjects;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition;

- (nullable NSArray<ObjectType> *)getObjectsOrders:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray<ObjectType> *)getObjectsLimit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOrders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOffset:(const WCDB::Expression &)offset
                                           limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsOrders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit
                                            offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Object
- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          where:(const WCDB::Expression &)condition;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                         orders:(const WCDB::OrderingTerms &)orders;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                         offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          where:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          where:(const WCDB::Expression &)condition
                                         offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                         orders:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          where:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Objects
- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     orders:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                     orders:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                      limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     orders:(const WCDB::OrderingTerms &)orders
                                                      limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      limit:(const WCDB::Expression &)limit
                                                     offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                     orders:(const WCDB::OrderingTerms &)orders
                                                      limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                      limit:(const WCDB::Expression &)limit
                                                     offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     orders:(const WCDB::OrderingTerms &)orders
                                                      limit:(const WCDB::Expression &)limit
                                                     offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                     orders:(const WCDB::OrderingTerms &)orders
                                                      limit:(const WCDB::Expression &)limit
                                                     offset:(const WCDB::Expression &)offset;

#pragma mark - Insert
- (BOOL)insertObject:(ObjectType)object;

- (BOOL)insertObjects:(NSArray<ObjectType> *)objects;

- (BOOL)insertOrReplaceObject:(ObjectType)object;

- (BOOL)insertOrReplaceObjects:(NSArray<ObjectType> *)objects;

- (BOOL)insertOrIgnoreObject:(ObjectType)object;

- (BOOL)insertOrIgnoreObjects:(NSArray<ObjectType> *)objects;

- (BOOL)insertObject:(ObjectType)object onProperties:(const WCTProperties &)properties;

- (BOOL)insertObjects:(NSArray<ObjectType> *)objects onProperties:(const WCTProperties &)properties;

- (BOOL)insertOrReplaceObject:(ObjectType)object onProperties:(const WCTProperties &)properties;

- (BOOL)insertOrReplaceObjects:(NSArray<ObjectType> *)objects onProperties:(const WCTProperties &)properties;

- (BOOL)insertOrIgnoreObject:(ObjectType)object onProperties:(const WCTProperties &)properties;

- (BOOL)insertOrIgnoreObjects:(NSArray<ObjectType> *)objects onProperties:(const WCTProperties &)properties;

#pragma mark - Update Properties To Object
- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object;

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object
                   where:(const WCDB::Expression &)condition;

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object
                   limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset;

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset;

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset;

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(ObjectType)object
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset;

#pragma mark - Update Properties To Row
- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row;

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition;

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset;

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset;

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset;

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property To Value
- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value;

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition;

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                 limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset;

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit;

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset;

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset;

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset;

#pragma mark - Delete
- (BOOL)deleteObjects;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition;

- (BOOL)deleteObjectsOrders:(const WCDB::OrderingTerms &)orders;

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsOffset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsOrders:(const WCDB::OrderingTerms &)orders
                      limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                    orders:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsOrders:(const WCDB::OrderingTerms &)orders
                      limit:(const WCDB::Expression &)limit
                     offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                    orders:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

#pragma mark - Get Value
- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                        where:(const WCDB::Expression &)condition;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                       orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                        where:(const WCDB::Expression &)condition
                                       orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                        where:(const WCDB::Expression &)condition
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                       orders:(const WCDB::OrderingTerms &)orders
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                        where:(const WCDB::Expression &)condition
                                       orders:(const WCDB::OrderingTerms &)orders
                                       offset:(const WCDB::Expression &)offset;

#pragma mark - Get Row
- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        where:(const WCDB::Expression &)condition;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                       orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        where:(const WCDB::Expression &)condition
                                       orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        where:(const WCDB::Expression &)condition
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                       orders:(const WCDB::OrderingTerms &)orders
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        where:(const WCDB::Expression &)condition
                                       orders:(const WCDB::OrderingTerms &)orders
                                       offset:(const WCDB::Expression &)offset;

#pragma mark - Get Column
- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                             where:(const WCDB::Expression &)condition;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                            orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                             limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                             where:(const WCDB::Expression &)condition
                                            orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                             where:(const WCDB::Expression &)condition
                                             limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                            orders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                             limit:(const WCDB::Expression &)limit
                                            offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                             where:(const WCDB::Expression &)condition
                                            orders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                             where:(const WCDB::Expression &)condition
                                             limit:(const WCDB::Expression &)limit
                                            offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                            orders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit
                                            offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                             where:(const WCDB::Expression &)condition
                                            orders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit
                                            offset:(const WCDB::Expression &)offset;

#pragma mark - Get Rows
- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               where:(const WCDB::Expression &)condition;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                              orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               where:(const WCDB::Expression &)condition
                                              orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               where:(const WCDB::Expression &)condition
                                               limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                              orders:(const WCDB::OrderingTerms &)orders
                                               limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               limit:(const WCDB::Expression &)limit
                                              offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               where:(const WCDB::Expression &)condition
                                              orders:(const WCDB::OrderingTerms &)orders
                                               limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               where:(const WCDB::Expression &)condition
                                               limit:(const WCDB::Expression &)limit
                                              offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                              orders:(const WCDB::OrderingTerms &)orders
                                               limit:(const WCDB::Expression &)limit
                                              offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               where:(const WCDB::Expression &)condition
                                              orders:(const WCDB::OrderingTerms &)orders
                                               limit:(const WCDB::Expression &)limit
                                              offset:(const WCDB::Expression &)offset;

@end

NS_ASSUME_NONNULL_END
