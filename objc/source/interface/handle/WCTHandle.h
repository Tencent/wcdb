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
// Developers can call sqlite interfaces those WCDB does not provided currently by using this raw handle.
// Note that this is not tested, which means that it may result in an unpredictable behavior.
// Usage:
//  e.g. 1. sqlite3** rawHandle = (sqlite3**)getRawHandle()
//  e.g. 2. sqlite3_open(rawHandle, ...)
//  e.g. 3. sqlite3_exec(*rawHandle, ...)
@property (nonatomic, readonly) void *_Nonnull *_Nonnull rawHandle;

@property (nonatomic, readonly) WCTDatabase *database;

- (void)invalidate;

- (BOOL)isValidated;

- (BOOL)validate;

#pragma mark - Execute
- (BOOL)execute:(const WCDB::Statement &)statement;

#pragma mark - Prepare
- (BOOL)prepare:(const WCDB::Statement &)statement;

- (BOOL)isPrepared;

- (void)finalizeStatement;

#pragma mark - Step
- (BOOL)step:(BOOL &)done;

- (BOOL)step;

- (void)reset;

#pragma mark - State
- (long long)getLastInsertedRowID;

- (int)getChanges;

- (BOOL)isStatementReadonly;

#pragma mark - Bind
- (void)bindInteger32:(const int32_t &)value toIndex:(int)index;

- (void)bindInteger64:(const int64_t &)value toIndex:(int)index;

- (void)bindDouble:(const double &)value toIndex:(int)index;

- (void)bindNullToIndex:(int)index;

- (void)bindString:(NSString *)string toIndex:(int)index;

- (void)bindData:(NSData *)data toIndex:(int)index;

- (void)bindNumber:(NSNumber *)number toIndex:(int)index;

- (void)bindValue:(nullable WCTColumnCodingValue *)value
          toIndex:(int)index;

- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index;

- (void)bindProperties:(const WCTProperties &)properties
              ofObject:(WCTObject *)object;

#pragma mark - Extract Row Data
- (int)extractColumnCount;

- (NSString *)extractColumnNameAtIndex:(int)index;

- (NSString *)extractTableNameAtIndex:(int)index;

- (int32_t)extractInteger32AtIndex:(int)index;

- (int64_t)extractInteger64AtIndex:(int)index;

- (double)extractDoubleAtIndex:(int)index;

- (NSString *)extractStringAtIndex:(int)index;

- (NSNumber *)extractNumberAtIndex:(int)index;

- (NSData *)extractDataAtIndex:(int)index;

- (nullable WCTValue *)extractValueAtIndex:(int)index;

- (WCTColumnType)extractTypeAtIndex:(int)index;

- (WCTOneRow *)extractRow;

- (WCTObject *)extractObjectOnResultColumns:(const WCTResultColumns &)resultColumns;

- (WCTMultiObject *)extractMultiObjectOnResultColumns:(const WCTResultColumns &)resultColumns;

- (void)extractValueAtIndex:(int)index
                 toProperty:(const WCTProperty &)property
                   ofObject:(WCTObject *)object;

#pragma mark - Step And Extract All Data
- (nullable WCTOneColumn *)allValues;

- (nullable WCTOneColumn *)allValuesAtIndex:(int)index;

- (nullable WCTColumnsXRows *)allRows;

- (nullable NSArray /* <WCTObject*> */ *)allObjectsOnResultColumns:(const WCTResultColumns &)resultColumns;

- (nullable NSArray<WCTMultiObject *> *)allMultiObjectsOnResultColumns:(const WCTResultColumns &)resultColumns;

#pragma mark - Error
- (WCTError *)error;

@end

NS_ASSUME_NONNULL_END
