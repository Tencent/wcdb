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

#import <WCDB/WCTCore.h>

@interface WCTUnsafeHandle : WCTCore

- (BOOL)execute:(const WCDB::Statement &)statement;

- (BOOL)prepare:(const WCDB::Statement &)statement;

- (BOOL)step:(BOOL &)done;

- (BOOL)step;

- (void)finalizeStatement;

- (BOOL)isStatementReadonly;

- (void)reset;

- (void)bindValue:(WCTValue *)value toIndex:(int)index;

- (void)bindInteger32:(const int32_t &)value toIndex:(int)index;

- (void)bindInteger64:(const int64_t &)value toIndex:(int)index;

- (void)bindDouble:(const double &)value toIndex:(int)index;

- (void)bindText:(const char *)value toIndex:(int)index;

- (void)bindBLOB:(NSData *)value toIndex:(int)index;

- (void)bindNullToIndex:(int)index;

- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index;

- (WCTValue *)getValueAtIndex:(int)index;

- (WCTOneRow *)getRow;

- (int32_t)getInteger32AtIndex:(int)index;

- (int64_t)getInteger64AtIndex:(int)index;

- (double)getDouble:(int)index;

- (const char *)getTextAtIndex:(int)index;

- (NSData *)getBLOB:(int)index;

- (WCDB::ColumnType)getTypeAtIndex:(int)index;

- (int)getColumnCount;

- (void)extractValueAtIndex:(int)index
                 toProperty:(const WCTProperty &)property
                   ofObject:(const WCTObject *)object;

- (const char *)getColumnNameAtIndex:(int)index;

- (const char *)getColumnTableNameAtIndex:(int)index;

- (long long)getLastInsertedRowID;

- (const char *)getErrorMessage;

- (int)getExtendedErrorCode;

- (WCTError *)getError;

- (int)getChanges;

@end
