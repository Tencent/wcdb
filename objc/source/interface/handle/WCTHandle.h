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

@interface WCTHandle : NSObject

#pragma mark - Handle
@property (nonatomic, readonly) void *_Nonnull *_Nonnull rawHandle;

@property (nonatomic, readonly) WCTDatabase *database;

- (void)invalidate;

#pragma mark - Execute
- (BOOL)execute:(const WCDB::Statement &)statement;

#pragma mark - Step
- (BOOL)prepare:(const WCDB::Statement &)statement;

- (BOOL)isPrepared;

- (BOOL)step:(BOOL &)done;

- (BOOL)step;

- (void)reset;

- (long long)getLastInsertedRowID;

- (int)getChanges;

- (BOOL)isStatementReadonly;

- (void)finalizeStatement;

#pragma mark - Bind
- (void)bindBool:(BOOL)value toIndex:(int)index;

- (void)bindInteger32:(const int32_t &)value toIndex:(int)index;

- (void)bindInteger64:(const int64_t &)value toIndex:(int)index;

- (void)bindDouble:(const double &)value toIndex:(int)index;

- (void)bindNullToIndex:(int)index;

- (void)bindString:(NSString *)string toIndex:(int)index;

- (void)bindBLOB:(NSData *)data toIndex:(int)index;

- (void)bindNumber:(NSNumber *)number toIndex:(int)index;

- (void)bindValue:(nullable WCTColumnCodingValue *)value
          toIndex:(int)index;

- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index;

- (void)bindProperties:(const WCTProperties &)properties
              ofObject:(WCTObject *)object;

#pragma mark - Extract
- (int32_t)getInteger32AtIndex:(int)index;

- (int64_t)getInteger64AtIndex:(int)index;

- (double)getDoubleAtIndex:(int)index;

- (const char *)getTextAtIndex:(int)index;

- (const unsigned char *)getBLOBAtIndex:(int)index;

- (size_t)getSizeAtIndex:(int)index;

- (NSString *)getStringAtIndex:(int)index;

- (NSNumber *)getNumberAtIndex:(int)index;

- (NSData *)getDataAtIndex:(int)index;

- (nullable WCTValue *)getValueAtIndex:(int)index;

- (WCTColumnType)getTypeAtIndex:(int)index;

- (int)getColumnCount;

- (NSString *)getColumnNameAtIndex:(int)index;

- (NSString *)getTableNameAtIndex:(int)index;

- (WCTOneRow *)getRow;

- (WCTObject *)getObjectOfClass:(Class)cls;

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns;

- (WCTMultiObject *)getMultiObjectOnResultColumns:(const WCTResultColumns &)resultColumns;

- (void)extractValueAtIndex:(int)index
                 toProperty:(const WCTProperty &)property
                   ofObject:(WCTObject *)object;

#pragma mark - Get All
- (nullable WCTOneColumn *)allValues;

- (nullable WCTOneColumn *)allValuesAtIndex:(int)index;

- (nullable WCTColumnsXRows *)allRows;

- (nullable NSArray /* <WCTObject*> */ *)allObjectsOnResultColumns:(const WCTResultColumns &)resultColumns;

- (nullable NSArray /* <WCTObject*> */ *)allObjectsOfClass:(Class)cls;

- (nullable NSArray<WCTMultiObject *> *)allMultiObjectsOnResultColumns:(const WCTResultColumns &)resultColumns;

#pragma mark - Error
- (WCTError *)error;

@end

NS_ASSUME_NONNULL_END
