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

#import <WCDB/WCTTable.h>

NS_ASSUME_NONNULL_BEGIN

@interface WCTTable <ObjectType>(Convenient)

#pragma mark - Get Object
- (nullable ObjectType)getObject;

- (nullable ObjectType)getObjectWhere:(const WCDB::Expression &)condition;

- (nullable ObjectType)getObjectOrderBy:(const WCDB::OrderingTerms &)orders;

- (nullable ObjectType)getObjectOffset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectWhere:(const WCDB::Expression &)condition
                                order:(const WCDB::OrderingTerms &)orders;

- (nullable ObjectType)getObjectWhere:(const WCDB::Expression &)condition
                               offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectOrderBy:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectWhere:(const WCDB::Expression &)condition
                                order:(const WCDB::OrderingTerms &)orders
                               offset:(const WCDB::Expression &)offset;

#pragma mark - Get Objects
- (nullable NSArray<ObjectType> *)getObjects;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition;

- (nullable NSArray<ObjectType> *)getObjectsOrderBy:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray<ObjectType> *)getObjectsLimit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                            order:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOrderBy:(const WCDB::OrderingTerms &)orders
                                              limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsLimit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                            order:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsOrderBy:(const WCDB::OrderingTerms &)orders
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                            order:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Object
- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          where:(const WCDB::Expression &)condition;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          order:(const WCDB::OrderingTerms &)orders;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                         offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          where:(const WCDB::Expression &)condition
                                          order:(const WCDB::OrderingTerms &)orders;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          where:(const WCDB::Expression &)condition
                                         offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          order:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset;

- (nullable ObjectType)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                          where:(const WCDB::Expression &)condition
                                          order:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Objects
- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      order:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                      order:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                      limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      order:(const WCDB::OrderingTerms &)orders
                                                      limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      limit:(const WCDB::Expression &)limit
                                                     offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                      order:(const WCDB::OrderingTerms &)orders
                                                      limit:(const WCDB::Expression &)limit;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                      limit:(const WCDB::Expression &)limit
                                                     offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      order:(const WCDB::OrderingTerms &)orders
                                                      limit:(const WCDB::Expression &)limit
                                                     offset:(const WCDB::Expression &)offset;

- (nullable NSArray<ObjectType> *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      where:(const WCDB::Expression &)condition
                                                      order:(const WCDB::OrderingTerms &)orders
                                                      limit:(const WCDB::Expression &)limit
                                                     offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Row
- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                                  order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Column
- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       where:(const WCDB::Expression &)condition;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       where:(const WCDB::Expression &)condition
                                       order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       where:(const WCDB::Expression &)condition
                                       limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       order:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       where:(const WCDB::Expression &)condition
                                       order:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       where:(const WCDB::Expression &)condition
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       order:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                       where:(const WCDB::Expression &)condition
                                       order:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Value
- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                  where:(const WCDB::Expression &)condition;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                  order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset;

#pragma mark - Get Rows
- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         where:(const WCDB::Expression &)condition;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         where:(const WCDB::Expression &)condition
                                         order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         order:(const WCDB::OrderingTerms &)orders
                                         limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         where:(const WCDB::Expression &)condition
                                         order:(const WCDB::OrderingTerms &)orders
                                         limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         order:(const WCDB::OrderingTerms &)orders
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                         where:(const WCDB::Expression &)condition
                                         order:(const WCDB::OrderingTerms &)orders
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

#pragma mark - Insert
- (BOOL)insertObject:(ObjectType)object;

- (BOOL)insertObjects:(NSArray<ObjectType> *)objects;

- (BOOL)insertOrReplaceObject:(ObjectType)object;

- (BOOL)insertOrReplaceObjects:(NSArray<ObjectType> *)objects;

- (BOOL)insertObject:(ObjectType)object onProperties:(const WCTProperties &)properties;

- (BOOL)insertObjects:(NSArray<ObjectType> *)objects onProperties:(const WCTProperties &)properties;

- (BOOL)insertOrReplaceObject:(ObjectType)object onProperties:(const WCTProperties &)properties;

- (BOOL)insertOrReplaceObjects:(NSArray<ObjectType> *)objects onProperties:(const WCTProperties &)properties;

#pragma mark - Update Properties With Object
- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Update Properties With Row
- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property With Value
- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       order:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       order:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       order:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       order:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

#pragma mark - Delete
- (BOOL)deleteObjects;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition;

- (BOOL)deleteObjectsOrderBy:(const WCDB::OrderingTerms &)orders;

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsOffset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsOrderBy:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     order:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsOrderBy:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     order:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

@end

NS_ASSUME_NONNULL_END
