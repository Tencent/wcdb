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

#import <WCDB/WCTCommon.h>

NS_ASSUME_NONNULL_BEGIN

@protocol WCTConvenient
#pragma mark - Get Object
- (WCTOptionalObject /* ObjectType */)getObjectOfClass:(Class)cls
                                             fromTable:(NSString *)tableName;

- (WCTOptionalObject /* ObjectType */)getObjectOfClass:(Class)cls
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition;

- (WCTOptionalObject /* ObjectType */)getObjectOfClass:(Class)cls
                                             fromTable:(NSString *)tableName
                                                orders:(const WCDB::OrderingTerms &)orders;

- (WCTOptionalObject /* ObjectType */)getObjectOfClass:(Class)cls
                                             fromTable:(NSString *)tableName
                                                offset:(const WCDB::Expression &)offset;

- (WCTOptionalObject /* ObjectType */)getObjectOfClass:(Class)cls
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                orders:(const WCDB::OrderingTerms &)orders;

- (WCTOptionalObject /* ObjectType */)getObjectOfClass:(Class)cls
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                offset:(const WCDB::Expression &)offset;

- (WCTOptionalObject /* ObjectType */)getObjectOfClass:(Class)cls
                                             fromTable:(NSString *)tableName
                                                orders:(const WCDB::OrderingTerms &)orders
                                                offset:(const WCDB::Expression &)offset;

- (WCTOptionalObject /* ObjectType */)getObjectOfClass:(Class)cls
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                orders:(const WCDB::OrderingTerms &)orders
                                                offset:(const WCDB::Expression &)offset;

#pragma mark - Get Objects
- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                             orders:(const WCDB::OrderingTerms &)orders;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              limit:(const WCDB::Expression &)limit;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                             orders:(const WCDB::OrderingTerms &)orders;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                              limit:(const WCDB::Expression &)limit;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                             orders:(const WCDB::OrderingTerms &)orders
                                                              limit:(const WCDB::Expression &)limit;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              limit:(const WCDB::Expression &)limit
                                                             offset:(const WCDB::Expression &)offset;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                             orders:(const WCDB::OrderingTerms &)orders
                                                              limit:(const WCDB::Expression &)limit;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                              limit:(const WCDB::Expression &)limit
                                                             offset:(const WCDB::Expression &)offset;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                             orders:(const WCDB::OrderingTerms &)orders
                                                              limit:(const WCDB::Expression &)limit
                                                             offset:(const WCDB::Expression &)offset;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                             orders:(const WCDB::OrderingTerms &)orders
                                                              limit:(const WCDB::Expression &)limit
                                                             offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Object
- (WCTOptionalObject /* ObjectType */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     fromTable:(NSString *)tableName;

- (WCTOptionalObject /* ObjectType */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     fromTable:(NSString *)tableName
                                                         where:(const WCDB::Expression &)condition;

- (WCTOptionalObject /* ObjectType */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     fromTable:(NSString *)tableName
                                                        orders:(const WCDB::OrderingTerms &)orders;

- (WCTOptionalObject /* ObjectType */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     fromTable:(NSString *)tableName
                                                        offset:(const WCDB::Expression &)offset;

- (WCTOptionalObject /* ObjectType */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     fromTable:(NSString *)tableName
                                                         where:(const WCDB::Expression &)condition
                                                        orders:(const WCDB::OrderingTerms &)orders;

- (WCTOptionalObject /* ObjectType */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     fromTable:(NSString *)tableName
                                                         where:(const WCDB::Expression &)condition
                                                        offset:(const WCDB::Expression &)offset;

- (WCTOptionalObject /* ObjectType */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     fromTable:(NSString *)tableName
                                                        orders:(const WCDB::OrderingTerms &)orders
                                                        offset:(const WCDB::Expression &)offset;

- (WCTOptionalObject /* ObjectType */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                     fromTable:(NSString *)tableName
                                                         where:(const WCDB::Expression &)condition
                                                        orders:(const WCDB::OrderingTerms &)orders
                                                        offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Objects
- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                     orders:(const WCDB::OrderingTerms &)orders;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      limit:(const WCDB::Expression &)limit;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                     orders:(const WCDB::OrderingTerms &)orders;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                      limit:(const WCDB::Expression &)limit;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                     orders:(const WCDB::OrderingTerms &)orders
                                                                      limit:(const WCDB::Expression &)limit;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      limit:(const WCDB::Expression &)limit
                                                                     offset:(const WCDB::Expression &)offset;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                     orders:(const WCDB::OrderingTerms &)orders
                                                                      limit:(const WCDB::Expression &)limit;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                      limit:(const WCDB::Expression &)limit
                                                                     offset:(const WCDB::Expression &)offset;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                     orders:(const WCDB::OrderingTerms &)orders
                                                                      limit:(const WCDB::Expression &)limit
                                                                     offset:(const WCDB::Expression &)offset;

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                     orders:(const WCDB::OrderingTerms &)orders
                                                                      limit:(const WCDB::Expression &)limit
                                                                     offset:(const WCDB::Expression &)offset;

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
           intoTable:(NSString *)tableName;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
            intoTable:(NSString *)tableName;

#pragma mark - Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                    intoTable:(NSString *)tableName;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                     intoTable:(NSString *)tableName;

#pragma mark - Partial Insert
- (BOOL)insertObject:(WCTObject *)object
        onProperties:(const WCTProperties &)properties
           intoTable:(NSString *)tableName;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
         onProperties:(const WCTProperties &)properties
            intoTable:(NSString *)tableName;

#pragma mark - Partial Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                 onProperties:(const WCTProperties &)properties
                    intoTable:(NSString *)tableName;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                  onProperties:(const WCTProperties &)properties
                     intoTable:(NSString *)tableName;

#pragma mark - Update Properties To Object
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

#pragma mark - Update Properties To Row
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property To Value
- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value;

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition;

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

#pragma mark - Delete
- (BOOL)deleteFromTable:(NSString *)tableName;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition;

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

- (BOOL)deleteFromTable:(NSString *)tableName
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

#pragma mark - Get Value
- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    fromTable:(NSString *)tableName;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    fromTable:(NSString *)tableName
                                        where:(const WCDB::Expression &)condition;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    fromTable:(NSString *)tableName
                                       orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    fromTable:(NSString *)tableName
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    fromTable:(NSString *)tableName
                                        where:(const WCDB::Expression &)condition
                                       orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    fromTable:(NSString *)tableName
                                        where:(const WCDB::Expression &)condition
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    fromTable:(NSString *)tableName
                                       orders:(const WCDB::OrderingTerms &)orders
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    fromTable:(NSString *)tableName
                                        where:(const WCDB::Expression &)condition
                                       orders:(const WCDB::OrderingTerms &)orders
                                       offset:(const WCDB::Expression &)offset;

#pragma mark - Get Row
- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                    fromTable:(NSString *)tableName;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                    fromTable:(NSString *)tableName
                                        where:(const WCDB::Expression &)condition;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                    fromTable:(NSString *)tableName
                                       orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                    fromTable:(NSString *)tableName
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                    fromTable:(NSString *)tableName
                                        where:(const WCDB::Expression &)condition
                                       orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                    fromTable:(NSString *)tableName
                                        where:(const WCDB::Expression &)condition
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                    fromTable:(NSString *)tableName
                                       orders:(const WCDB::OrderingTerms &)orders
                                       offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                    fromTable:(NSString *)tableName
                                        where:(const WCDB::Expression &)condition
                                       orders:(const WCDB::OrderingTerms &)orders
                                       offset:(const WCDB::Expression &)offset;

#pragma mark - Get Column
- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                             where:(const WCDB::Expression &)condition;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                            orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                             limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                             where:(const WCDB::Expression &)condition
                                            orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                             where:(const WCDB::Expression &)condition
                                             limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                            orders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                             limit:(const WCDB::Expression &)limit
                                            offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                             where:(const WCDB::Expression &)condition
                                            orders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                             where:(const WCDB::Expression &)condition
                                             limit:(const WCDB::Expression &)limit
                                            offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                            orders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit
                                            offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                         fromTable:(NSString *)tableName
                                             where:(const WCDB::Expression &)condition
                                            orders:(const WCDB::OrderingTerms &)orders
                                             limit:(const WCDB::Expression &)limit
                                            offset:(const WCDB::Expression &)offset;

#pragma mark - Get Rows
- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                               where:(const WCDB::Expression &)condition;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                              orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                               limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                               where:(const WCDB::Expression &)condition
                                              orders:(const WCDB::OrderingTerms &)orders;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                               where:(const WCDB::Expression &)condition
                                               limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                              orders:(const WCDB::OrderingTerms &)orders
                                               limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                               limit:(const WCDB::Expression &)limit
                                              offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                               where:(const WCDB::Expression &)condition
                                              orders:(const WCDB::OrderingTerms &)orders
                                               limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                               where:(const WCDB::Expression &)condition
                                               limit:(const WCDB::Expression &)limit
                                              offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                              orders:(const WCDB::OrderingTerms &)orders
                                               limit:(const WCDB::Expression &)limit
                                              offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromTable:(NSString *)tableName
                                               where:(const WCDB::Expression &)condition
                                              orders:(const WCDB::OrderingTerms &)orders
                                               limit:(const WCDB::Expression &)limit
                                              offset:(const WCDB::Expression &)offset;

#pragma mark - Get From Statement
- (nullable WCTValue *)getValueFromStatement:(const WCDB::Statement &)statement;

- (nullable WCTOneRow *)getRowFromStatement:(const WCDB::Statement &)statement;

- (nullable WCTOneColumn *)getColumnFromStatement:(const WCDB::Statement &)statement;

- (nullable WCTColumnsXRows *)getRowsFromStatement:(const WCDB::Statement &)statement;

@end

NS_ASSUME_NONNULL_END
