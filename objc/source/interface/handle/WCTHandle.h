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

#import <WCDB/WCTUnsafeHandle.h>

NS_ASSUME_NONNULL_BEGIN

@interface WCTHandle : WCTUnsafeHandle

- (WCTDatabase *)getDatabase;

#pragma mark - Info
- (long long)getLastInsertedRowID;

- (int)getChanges;

- (BOOL)isStatementReadonly;

#pragma mark - Statement
- (BOOL)prepare:(const WCDB::Statement &)statement;

- (void)finalizeStatement;

#pragma mark - Stepping
- (BOOL)step:(BOOL &)done;

- (BOOL)step;

- (void)reset;

#pragma mark - Bind row
- (void)bindValue:(nullable WCTColumnCodingValue *)value toIndex:(int)index;

- (void)bindBool:(BOOL)value toIndex:(int)index;

- (void)bindInteger32:(const int32_t &)value toIndex:(int)index;

- (void)bindInteger64:(const int64_t &)value toIndex:(int)index;

- (void)bindDouble:(const double &)value toIndex:(int)index;

- (void)bindString:(NSString *)value toIndex:(int)index;

- (void)bindBLOB:(NSData *)value toIndex:(int)index;

- (void)bindNullToIndex:(int)index;

- (void)bindProperty:(const WCTProperty &)property
            ofObject:(nonnull WCTObject *)object
             toIndex:(int)index;

- (void)bindProperties:(const WCTProperties &)properties
              ofObject:(nonnull WCTObject *)object;

#pragma mark - Get row
- (nullable WCTValue *)getValueAtIndex:(int)index;

- (nullable WCTOneRow *)getRow;

- (BOOL)getBoolAtIndex:(int)index;

- (int32_t)getInteger32AtIndex:(int)index;

- (int64_t)getInteger64AtIndex:(int)index;

//TODO AtIndex subfix fixed. Add bindValue, bindNumber, getValue, getNumber. Refactor
- (double)getDouble:(int)index;

- (nullable NSString *)getTextAtIndex:(int)index;

- (nullable NSData *)getBLOB:(int)index;

- (WCDB::ColumnType)getColumnTypeAtIndex:(int)index;

- (void)extractValueAtIndex:(int)index
                 toProperty:(const WCTProperty &)property
                   ofObject:(nonnull WCTObject *)object;

- (nullable WCTObject *)getObjectOfClass:(nonnull Class)cls;

- (nullable WCTObject *)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns;

- (int)getColumnCount;

- (nullable NSString *)getColumnNameAtIndex:(int)index;

- (nullable NSString *)getColumnTableNameAtIndex:(int)index;

#pragma mark - Execute
- (BOOL)execute:(const WCDB::Statement &)statement;

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(nonnull WCTObject *)object;

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(WCTObject *)object
   onProperties:(const WCTProperties &)properties;

- (BOOL)execute:(const WCDB::Statement &)statement
      withValue:(nullable WCTColumnCodingValue *)value;

- (BOOL)execute:(const WCDB::Statement &)statement
        withRow:(WCTOneRow *)row;

@end

NS_ASSUME_NONNULL_END
