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

#import "Assertion.hpp"
#import "Notifier.hpp"
#import "WCTCancellationSignal+Private.h"
#import "WCTDatabase+Private.h"
#import "WCTError+Private.h"
#import "WCTFoundation.h"
#import "WCTHandle+Private.h"
#import "WCTHandle+Transaction.h"
#import "WCTORM.h"
#import "WCTPreparedStatement+Private.h"

@implementation WCTHandle

#pragma mark - LifeCycle
- (instancetype)initWithDatabase:(WCTDatabase *)database andUnsafeHandle:(WCDB::InnerHandle *)handle
{
    WCTAssert(database != nil);
    WCTAssert(handle != nil);
    if (self = [super init]) {
        _database = database;
        _handle = handle;
        _handleHolder = nullptr;
    }
    return self;
}

- (instancetype)initWithDatabase:(WCTDatabase *)database
{
    WCTAssert(database != nil);
    if (self = [super init]) {
        _database = database;
    }
    return self;
}

- (instancetype)initWithUnsafeHandle:(WCDB::InnerHandle *)handle
{
    WCTAssert(handle != nil);
    if (self = [super init]) {
        _handle = handle;
        _handleHolder = nullptr;
    }
    return self;
}

- (WCDB::InnerHandle *)getOrGenerateHandle
{
    if (_handle == nullptr) {
        _handleHolder = [_database generateHandle];
        if (_handleHolder != nullptr) {
            _handle = _handleHolder.get();
        }
    }
    return _handle;
}

#pragma mark - Statement

- (WCTPreparedStatement *)getOrCreatePreparedStatement:(const WCDB::Statement &)statement
{
    WCDB::InnerHandle *dbHandle = [self getOrGenerateHandle];
    if (dbHandle == nil) {
        return nil;
    }
    WCDB::HandleStatement *preparedHandleStatement = dbHandle->getOrCreatePreparedStatement(statement);
    if (preparedHandleStatement == nullptr) {
        return nullptr;
    }
    return [[WCTPreparedStatement alloc] initWithHandleStatement:preparedHandleStatement];
}

- (void)finalizeAllStatements
{
    if (_handle != nullptr) {
        _handle->finalizeStatements();
    }
}

#pragma mark - Handle
- (BOOL)validate
{
    return [self getOrGenerateHandle] != nullptr;
}

- (void)invalidate
{
    _database = nil;
    _handle = nullptr;
    _handleHolder = nullptr;
}

- (BOOL)isValidated
{
    return _handle != nullptr;
}

- (WCTDatabase *)database
{
    return _database;
}

#pragma mark - Execute
- (BOOL)execute:(const WCDB::Statement &)statement
{
    BOOL succeed = NO;
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->execute(statement);
    }
    return succeed;
}

//rawExecute should no be used to access or modify the data in a migrating table.
- (BOOL)rawExecute:(NSString *)sql
{
    BOOL succeed = NO;
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->execute(sql);
    }
    return succeed;
}

#pragma mark - Prepare
- (BOOL)prepare:(const WCDB::Statement &)statement
{
    BOOL succeed = NO;
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->prepare(statement);
    }
    return succeed;
}

//rawPrepare should no be used to access or modify the data in a migrating table.
- (BOOL)rawPrepare:(NSString *)sql
{
    BOOL succeed = NO;
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->prepare(sql);
    }
    return succeed;
}

- (BOOL)isPrepared
{
    return _handle != nullptr && _handle->isPrepared();
}

- (void)finalizeStatement
{
    if (_handle != nullptr) {
        _handle->finalize();
    }
}

#pragma mark - Step
- (BOOL)step
{
    WCTHandleAssert(return NO;);
    return _handle->step();
}

- (BOOL)done
{
    WCTHandleAssert(return NO;);
    return _handle->done();
}

- (void)reset
{
    WCTHandleAssert(return;);
    _handle->reset();
}

#pragma mark - State
- (long long)getLastInsertedRowID
{
    WCTHandleAssert(return 0;);
    return _handle->getLastInsertedRowID();
}

- (int)getChanges
{
    WCTHandleAssert(return 0;);
    return _handle->getChanges();
}

- (int)getTotalChanges
{
    WCTHandleAssert(return 0;);
    return _handle->getTotalChange();
}

- (BOOL)isStatementReadonly
{
    WCTHandleAssert(return NO;);
    return _handle->isStatementReadonly();
}

#pragma mark - Bind
- (void)bindInteger:(const int64_t &)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindInteger(value, index);
}

- (void)bindDouble:(const double &)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindDouble(value, index);
}

- (void)bindNullToIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindNull(index);
}

- (void)bindString:(NSString *)string toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindText(WCDB::UnsafeStringView(string), index);
}

- (void)bindData:(NSData *)data toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindBLOB(data, index);
}

- (void)bindNumber:(NSNumber *)number toIndex:(int)index
{
    WCTHandleAssert(return;);
    if (number == nil || CFNumberIsFloatType((CFNumberRef) number)) {
        _handle->bindDouble(number.doubleValue, index);
    } else {
        _handle->bindInteger(number.integerValue, index);
    }
}

- (void)bindValue:(WCTColumnCodingValue *)value
          toIndex:(int)index
{
    WCTHandleAssert(return;);
    WCTValue *archivedValue = [value archivedWCTValue];
    switch ([value.class columnType]) {
    case WCTColumnTypeNull:
        _handle->bindNull(index);
        break;
    case WCTColumnTypeInteger:
        _handle->bindInteger(archivedValue.numberValue.longLongValue, index);
        break;
    case WCTColumnTypeFloat:
        _handle->bindDouble(archivedValue.numberValue.doubleValue, index);
        break;
    case WCTColumnTypeString:
        _handle->bindText(WCDB::UnsafeStringView(archivedValue.stringValue), index);
        break;
    case WCTColumnTypeData:
        _handle->bindBLOB((NSData *) archivedValue, index);
        break;
    }
}

- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index
{
    WCTHandleAssert(return;);
    const std::shared_ptr<const WCTBaseAccessor> &accessor = property.getColumnBinding().getAccessor();
    switch (accessor->getAccessorType()) {
    case WCTAccessorCpp: {
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer: {
            WCTCppAccessor<WCDB::ColumnType::Integer> *integerAccessor = (WCTCppAccessor<WCDB::ColumnType::Integer> *) accessor.get();
            _handle->bindInteger(integerAccessor->getValue(object),
                                 index);
        } break;
        case WCDB::ColumnType::Float: {
            WCTCppAccessor<WCDB::ColumnType::Float> *floatAccessor = (WCTCppAccessor<WCDB::ColumnType::Float> *) accessor.get();
            _handle->bindDouble(floatAccessor->getValue(object),
                                index);
        } break;
        case WCDB::ColumnType::Text: {
            WCTCppAccessor<WCDB::ColumnType::Text> *textAccessor = (WCTCppAccessor<WCDB::ColumnType::Text> *) accessor.get();
            _handle->bindText(textAccessor->getValue(object),
                              index);
        } break;
        case WCDB::ColumnType::BLOB: {
            WCTCppAccessor<WCDB::ColumnType::BLOB> *blobAccessor = (WCTCppAccessor<WCDB::ColumnType::BLOB> *) accessor.get();
            _handle->bindBLOB(blobAccessor->getValue(object), index);
        } break;
        case WCDB::ColumnType::Null:
            _handle->bindNull(index);
            break;
        }
    } break;
    case WCTAccessorObjC: {
        WCTObjCAccessor *objcAccessor = (WCTObjCAccessor *) accessor.get();
        NSObject *value = objcAccessor->getObject(object);
        if (value != nil) {
            switch (accessor->getColumnType()) {
            case WCDB::ColumnType::Integer: {
                _handle->bindInteger(((NSNumber *) value).longLongValue, index);
                break;
            }
            case WCDB::ColumnType::Float: {
                _handle->bindDouble(((NSNumber *) value).doubleValue, index);
                break;
            }
            case WCDB::ColumnType::Text: {
                _handle->bindText((NSString *) value, index);
                break;
            }
            case WCDB::ColumnType::BLOB: {
                _handle->bindBLOB((NSData *) value, index);
                break;
            }
            case WCDB::ColumnType::Null:
                _handle->bindNull(index);
                break;
            }
        } else {
            _handle->bindNull(index);
        }
    } break;
    }
}

- (void)bindProperties:(const WCTProperties &)properties
              ofObject:(WCTObject *)object
{
    WCTHandleAssert(return;);
    int index = 1;
    for (const auto &property : properties) {
        [self bindProperty:property ofObject:object toIndex:index];
        ++index;
    }
}

- (int)bindParameterIndex:(const WCDB::BindParameter &)parameter
{
    WCTHandleAssert(return 0;);
    return _handle->bindParameterIndex(parameter.getDescription());
}

#pragma mark - Get
- (int64_t)extractIntegerAtIndex:(int)index
{
    WCTHandleAssert(return 0;);
    return _handle->getInteger(index);
}

- (double)extractDoubleAtIndex:(int)index
{
    WCTHandleAssert(return 0;);
    return _handle->getDouble(index);
}

- (NSString *)extractStringAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    return [NSString stringWithView:_handle->getText(index)];
}

- (NSNumber *)extractNumberAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    switch (_handle->getType(index)) {
    case WCDB::ColumnType::Integer:
        return [NSNumber numberWithLongLong:_handle->getInteger(index)];
    default:
        return [NSNumber numberWithDouble:_handle->getDouble(index)];
    }
}

- (NSData *)extractDataAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    const WCDB::UnsafeData blob = _handle->getBLOB(index);
    return [NSData dataWithBytes:blob.buffer() length:blob.size()];
}

- (WCTValue *)extractValueAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    switch ([self extractTypeAtIndex:index]) {
    case WCTColumnTypeInteger:
        return [NSNumber numberWithLongLong:[self extractIntegerAtIndex:index]];
    case WCTColumnTypeFloat:
        return [NSNumber numberWithDouble:[self extractDoubleAtIndex:index]];
    case WCTColumnTypeString:
        return [self extractStringAtIndex:index];
    case WCTColumnTypeData:
        return [self extractDataAtIndex:index];
    case WCTColumnTypeNull:
        return nil;
    }
}

- (WCTColumnType)extractTypeAtIndex:(int)index
{
    WCTHandleAssert(return WCTColumnTypeNull;);
    return (WCTColumnType) _handle->getType(index);
}

- (int)extractNumberOfColumns
{
    WCTHandleAssert(return 0;);
    return _handle->getNumberOfColumns();
}

- (NSString *)extractOriginColumnNameAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    return [NSString stringWithView:_handle->getOriginColumnName(index)];
}

- (NSString *)extractColumnNameAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    return [NSString stringWithView:_handle->getColumnName(index)];
}

- (NSString *)extractTableNameAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    return [NSString stringWithView:_handle->getColumnTableName(index)];
}

- (WCTOneRow *)extractRow
{
    WCTHandleAssert(return nil;);
    int count = [self extractNumberOfColumns];
    NSMutableArray *row = [NSMutableArray arrayWithCapacity:count];
    for (int index = 0; index < count; ++index) {
        WCTValue *value = [self extractValueAtIndex:index];
        [row addObject:value ? value : [NSNull null]];
    }
    return row;
}

- (WCTObject *)extractObjectOnResultColumns:(const WCTResultColumns &)resultColumns
{
    WCTHandleAssert(return nil;);
    Class cls = resultColumns.front().getColumnBinding().getClass();
    WCTObject *object = [[cls alloc] init];
    int index = 0;
    for (const auto &resultColumn : resultColumns) {
        [self extractValueAtIndex:index toColumnBindingHolder:resultColumn ofObject:object];
        ++index;
    }
    return object;
}

- (WCTMultiObject *)extractMultiObjectOnResultColumns:(const WCTResultColumns &)resultColumns
{
    NSMutableDictionary *multiObject = [NSMutableDictionary dictionary];
    int index = 0;
    for (const WCTResultColumn &resultColumn : resultColumns) {
        NSString *tableName = [NSString stringWithView:_handle->getColumnTableName(index)];
        WCTObject *object = [multiObject objectForKey:tableName];
        if (object == nil) {
            object = [[resultColumn.getColumnBinding().getClass() alloc] init];
            [multiObject setObject:object forKey:tableName];
        }
        [self extractValueAtIndex:index
            toColumnBindingHolder:resultColumn
                         ofObject:object];
        ++index;
    }
    return multiObject;
}

- (void)extractValueAtIndex:(int)index toProperty:(const WCTProperty &)property ofObject:(WCTObject *)object
{
    [self extractValueAtIndex:index toColumnBindingHolder:property ofObject:object];
}

- (void)extractValueAtIndex:(int)index
      toColumnBindingHolder:(const WCTColumnBindingHolder &)columnBindingHolder
                   ofObject:(WCTObject *)object
{
    WCTHandleAssert(return;);
    const std::shared_ptr<const WCTBaseAccessor> &accessor = columnBindingHolder.getColumnBinding().getAccessor();
    switch (accessor->getAccessorType()) {
    case WCTAccessorCpp: {
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer: {
            WCTCppAccessor<WCDB::ColumnType::Integer> *integerAccessor = (WCTCppAccessor<WCDB::ColumnType::Integer> *) accessor.get();
            integerAccessor->setValue(object, _handle->getInteger(index));
        } break;
        case WCDB::ColumnType::Float: {
            WCTCppAccessor<WCDB::ColumnType::Float> *floatAccessor = (WCTCppAccessor<WCDB::ColumnType::Float> *) accessor.get();
            floatAccessor->setValue(object, _handle->getDouble(index));
        } break;
        case WCDB::ColumnType::Text: {
            WCTCppAccessor<WCDB::ColumnType::Text> *textAccessor = (WCTCppAccessor<WCDB::ColumnType::Text> *) accessor.get();
            textAccessor->setValue(object, _handle->getText(index));
        } break;
        case WCDB::ColumnType::BLOB: {
            WCTCppAccessor<WCDB::ColumnType::BLOB> *blobAccessor = (WCTCppAccessor<WCDB::ColumnType::BLOB> *) accessor.get();
            blobAccessor->setValue(object, _handle->getBLOB(index));
        } break;
        case WCDB::ColumnType::Null: {
            WCTCppAccessor<WCDB::ColumnType::Null> *nullAccessor = (WCTCppAccessor<WCDB::ColumnType::Null> *) accessor.get();
            nullAccessor->setValue(object, nullptr);
        } break;
        }
    } break;
    case WCTAccessorObjC: {
        WCTObjCAccessor *objcAccessor = (WCTObjCAccessor *) accessor.get();
        id value = nil;
        if (_handle->getType(index) != WCDB::ColumnType::Null) {
            switch (accessor->getColumnType()) {
            case WCDB::ColumnType::Integer:
                value = [NSNumber numberWithLongLong:_handle->getInteger(index)];
                break;
            case WCDB::ColumnType::Float:
                value = [NSNumber numberWithDouble:_handle->getDouble(index)];
                break;
            case WCDB::ColumnType::Text:
                value = [NSString stringWithView:_handle->getText(index)];
                break;
            case WCDB::ColumnType::BLOB: {
                const WCDB::UnsafeData data = _handle->getBLOB(index);
                value = [NSData dataWithBytes:data.buffer() length:data.size()];
            } break;
            case WCDB::ColumnType::Null:
                value = nil;
                break;
            }
        }
        objcAccessor->setObject(object, value);
    } break;
    }
}

#pragma mark - Get All
- (WCTOneColumn *)allValues
{
    WCTHandleAssert(return nil;);
    return [self allValuesAtIndex:0];
}

- (WCTOneColumn *)allValuesAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    NSMutableArray *column = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        WCTValue *value = [self extractValueAtIndex:index];
        [column addObject:value ? value : [NSNull null]];
    }
    return succeed ? column : nil;
}

- (WCTColumnsXRows *)allRows
{
    WCTHandleAssert(return nil;);
    NSMutableArray *rows = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        [rows addObject:[self extractRow]];
    }
    return succeed ? rows : nil;
}

- (NSArray /* <WCTObject*> */ *)allObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    WCTHandleAssert(return nil;);
    NSMutableArray *objects = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        [objects addObject:[self extractObjectOnResultColumns:resultColumns]];
    }
    return succeed ? objects : nil;
}

- (NSArray<WCTMultiObject *> *)allMultiObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    WCTHandleAssert(return nil;);
    NSMutableArray *multiObjects = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        [multiObjects addObject:[self extractMultiObjectOnResultColumns:resultColumns]];
    }
    return succeed ? multiObjects : nil;
}

#pragma mark - Cancellation signal
- (void)attachCancellationSignal:(WCTCancellationSignal *)signal
{
    [self getOrGenerateHandle];
    WCTHandleAssert(return;);
    _handle->attachCancellationSignal([signal getInnerSignal]);
}

- (void)detachCancellationSignal
{
    WCTHandleAssert(return;);
    _handle->detachCancellationSignal();
}

#pragma mark - Error
- (WCTError *)error
{
    if (_handle != nullptr) {
        return [[WCTError alloc] initWithError:_handle->getError()];
    } else {
        return _database.error;
    }
}

#pragma mark - Helper
- (BOOL)lazyRunTransaction:(WCTTransactionBlock)transaction
{
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle == nullptr) {
        return NO;
    }
    if (handle->isInTransaction()) {
        return transaction(self);
    } else {
        handle->markErrorNotAllowedWithinTransaction();
        return [self runTransaction:transaction];
    }
}

@end
