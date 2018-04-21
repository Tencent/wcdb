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

#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTUnsafeHandle.h>

#define WCTHandleAssert(remedial) WCTRemedialAssert(_handle != nullptr, "[prepare] or [execute] should be called before this.", remedial)

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, WCTFinalizeLevel) {
    WCTFinalizeLevelStatement = 0,
    WCTFinalizeLevelHandle,
    WCTFinalizeLevelDatabase,
};

@interface WCTUnsafeHandle () {
@protected
    WCDB::Handle *_handle;
    WCDB::RecyclableHandle _recyclableHandle;
    WCTCoreError *_nonHandleError;
    WCTFinalizeLevel _finalizeLevel;
}

#pragma mark - Initialize

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database NS_DESIGNATED_INITIALIZER;

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
             andRecyclableHandle:(const WCDB::RecyclableHandle &)recyclableHandle NS_DESIGNATED_INITIALIZER;

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
                       andHandle:(nonnull WCDB::Handle *)handle NS_DESIGNATED_INITIALIZER;

#pragma mark - Safety

- (nullable WCDB::Handle *)getOrGenerateHandle;

- (void)generateIfNeeded;

- (void)finalizeHandleIfGeneratedAndKeepError:(BOOL)keepError;

- (BOOL)isSafe;

- (void)finalizeStatement;

#pragma mark - Convenient
- (void)extractValueAtIndex:(int)index
                 toProperty:(const WCTProperty &)property
                   ofObject:(WCTObject *)object;

- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index;

- (void)bindProperties:(const WCTPropertyList &)properties
              ofObject:(WCTObject *)object;

- (void)bindValue:(nullable WCTColumnCodingValue *)value
          toIndex:(int)index;

- (nullable WCTObject *)getObjectOfClass:(nonnull Class)cls
                            onProperties:(const WCTPropertyList &)properties;

- (nullable WCTValue *)getValueAtIndex:(int)index;

- (nullable WCTOneRow *)getRow;

- (BOOL)execute:(const WCDB::Statement &)statement;

- (BOOL)prepare:(const WCDB::Statement &)statement;

- (nullable WCTValue *)nextValueAtIndex:(int)index orDone:(BOOL &)isDone;

- (nullable WCTOneColumn *)allValuesAtIndex:(int)index;

- (nullable WCTOneRow *)nextRowOrDone:(BOOL &)isDone;

- (nullable WCTColumnsXRows *)allRows;

- (nullable id /* WCTObject* */)nextObjectOfClass:(nonnull Class)cls orDone:(BOOL &)isDone;

- (nullable id /* WCTObject* */)nextObjectOnProperties:(const WCTPropertyList &)properties orDone:(BOOL &)isDone;

- (nullable NSArray /* <WCTObject*> */ *)allObjectsOfClass:(nonnull Class)cls;

- (nullable NSArray /* <WCTObject*> */ *)allObjectsOnProperties:(const WCTPropertyList &)properties;

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(WCTObject *)object;

- (BOOL)execute:(const WCDB::Statement &)statement
      withObject:(WCTObject *)object
    onProperties:(const WCTPropertyList &)properties;

- (BOOL)execute:(const WCDB::Statement &)statement
      withValue:(WCTColumnCodingValue *)value;

- (BOOL)execute:(const WCDB::Statement &)statement
        withRow:(WCTOneRow *)row;

- (BOOL)rebindTable:(NSString *)tableName toClass:(Class<WCTTableCoding>)cls;

- (instancetype)autoFinalizeStatement;
- (instancetype)autoFinalizeHandle;
- (instancetype)autoFinalizeDatabase;
- (void)finalizeDatabase:(BOOL)keepError;
- (void)doAutoFinalize:(BOOL)keepError;

@end

NS_ASSUME_NONNULL_END
