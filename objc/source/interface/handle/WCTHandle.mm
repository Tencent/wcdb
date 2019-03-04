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

#import <WCDB/Assertion.hpp>
#import <WCDB/Notifier.hpp>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTHandle+Private.h>
#import <WCDB/WCTORM.h>

@implementation WCTHandle

#pragma mark - LifeCycle
- (instancetype)initWithDatabase:(WCTDatabase *)database andUnsafeHandle:(WCDB::Handle *)handle
{
    WCTInnerAssert(database != nil);
    WCTInnerAssert(handle != nil);
    if (self = [super init]) {
        _database = database;
        _handle = handle;
        _handleHolder = nullptr;
    }
    return self;
}

- (instancetype)initWithDatabase:(WCTDatabase *)database
{
    WCTInnerAssert(database != nil);
    if (self = [super init]) {
        _database = database;
    }
    return self;
}

- (WCDB::Handle *)getOrGenerateHandle
{
    if (_handle == nullptr) {
        _handleHolder = [_database generateHandle];
        if (_handleHolder != nullptr) {
            _handle = _handleHolder.get();
        }
    }
    return _handle;
}

#pragma mark - Handle
- (BOOL)validate
{
    return [self getOrGenerateHandle] != nullptr;
}

- (void)invalidate
{
    _database = nil;
    if (_handle != nullptr) {
        _handle->finalize();
        _handle = nullptr;
    }
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

- (void)enableLazyNestedTransaction:(BOOL)enable
{
    WCTInnerAssert([self isValidated]);
    _handle->enableLazyNestedTransaction(enable);
}

#pragma mark - Execute
- (BOOL)execute:(const WCDB::Statement &)statement
{
    BOOL succeed = NO;
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->execute(statement);
    }
    return succeed;
}

#pragma mark - Prepare
- (BOOL)prepare:(const WCDB::Statement &)statement
{
    BOOL succeed = NO;
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->prepare(statement);
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
- (BOOL)step:(BOOL &)done
{
    WCTHandleAssert(return NO;);
    return _handle->step((bool &) done);
}

- (BOOL)step
{
    WCTHandleAssert(return NO;);
    return _handle->step();
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

- (BOOL)isStatementReadonly
{
    WCTHandleAssert(return NO;);
    return _handle->isStatementReadonly();
}

#pragma mark - Bind
- (void)bindInteger32:(const int32_t &)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindInteger32(value, index);
}

- (void)bindInteger64:(const int64_t &)value toIndex:(int)index
{
    WCTHandleAssert(return;);
    _handle->bindInteger64(value, index);
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
    _handle->bindText(string ? string.UTF8String : WCDB::String::null().c_str(), index);
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
        if (CFNumberGetByteSize((CFNumberRef) number) <= 4) {
            _handle->bindInteger32(number.intValue, index);
        } else {
            _handle->bindInteger64(number.integerValue, index);
        }
    }
}

- (void)bindValue:(WCTColumnCodingValue *)value
          toIndex:(int)index
{
    WCTHandleAssert(return;);
    WCTValue *archivedValue = [value archivedWCTValue];
    switch ([value.class columnType]) {
    case WCTColumnTypeNil:
        _handle->bindNull(index);
        break;
    case WCTColumnTypeInteger32:
        _handle->bindInteger32(((NSNumber *) archivedValue).intValue, index);
        break;
    case WCTColumnTypeInteger64:
        _handle->bindInteger64(((NSNumber *) archivedValue).longLongValue, index);
        break;
    case WCTColumnTypeDouble:
        _handle->bindDouble(((NSNumber *) archivedValue).doubleValue, index);
        break;
    case WCTColumnTypeString:
        _handle->bindText(archivedValue ? ((NSString *) archivedValue).UTF8String : WCDB::String::null().c_str(), index);
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
    const std::shared_ptr<WCTBaseAccessor> &accessor = property.getColumnBinding().getAccessor();
    switch (accessor->getAccessorType()) {
    case WCTAccessorCpp: {
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer32: {
            WCTCppAccessor<WCDB::ColumnType::Integer32> *i32Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer32> *) accessor.get();
            _handle->bindInteger32(i32Accessor->getValue(object),
                                   index);
        } break;
        case WCDB::ColumnType::Integer64: {
            WCTCppAccessor<WCDB::ColumnType::Integer64> *i64Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer64> *) accessor.get();
            _handle->bindInteger64(i64Accessor->getValue(object),
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
            case WCDB::ColumnType::Integer32: {
                _handle->bindInteger32(((NSNumber *) value).intValue, index);
                break;
            }
            case WCDB::ColumnType::Integer64: {
                _handle->bindInteger64(((NSNumber *) value).longLongValue, index);
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

#pragma mark - Get
- (int32_t)extractInteger32AtIndex:(int)index
{
    WCTHandleAssert(return 0;);
    return _handle->getInteger32(index);
}

- (int64_t)extractInteger64AtIndex:(int)index
{
    WCTHandleAssert(return 0;);
    return _handle->getInteger64(index);
}

- (double)extractDoubleAtIndex:(int)index
{
    WCTHandleAssert(return 0;);
    return _handle->getDouble(index);
}

- (NSString *)extractStringAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    return [NSString stringWithUTF8String:_handle->getText(index)];
}

- (NSNumber *)extractNumberAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    switch (_handle->getType(index)) {
    case WCDB::ColumnType::Integer32:
        return [NSNumber numberWithInt:_handle->getInteger32(index)];
    case WCDB::ColumnType::Integer64:
        return [NSNumber numberWithLongLong:_handle->getInteger64(index)];
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
    case WCTColumnTypeInteger32:
        return [NSNumber numberWithInt:[self extractInteger32AtIndex:index]];
    case WCTColumnTypeInteger64:
        return [NSNumber numberWithLongLong:[self extractInteger64AtIndex:index]];
    case WCTColumnTypeDouble:
        return [NSNumber numberWithDouble:[self extractDoubleAtIndex:index]];
    case WCTColumnTypeString:
        return [self extractStringAtIndex:index];
    case WCTColumnTypeData:
        return [self extractDataAtIndex:index];
    case WCTColumnTypeNil:
        return nil;
    }
}

- (WCTColumnType)extractTypeAtIndex:(int)index
{
    WCTHandleAssert(return WCTColumnTypeNil;);
    return _handle->getType(index);
}

- (int)extractNumberOfColumns
{
    WCTHandleAssert(return 0;);
    return _handle->getNumberOfColumns();
}

- (NSString *)extractOriginColumnNameAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    return [NSString stringWithUTF8String:_handle->getOriginColumnName(index)];
}

- (NSString *)extractColumnNameAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    return [NSString stringWithUTF8String:_handle->getColumnName(index)];
}

- (NSString *)extractTableNameAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    return [NSString stringWithUTF8String:_handle->getColumnTableName(index)];
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
        NSString *tableName = [NSString stringWithUTF8String:_handle->getColumnTableName(index)];
        WCTObject *object = [multiObject objectForKey:tableName];
        if (object == nil) {
            object = [[resultColumn.getColumnBinding().getClass() alloc] init];
            [multiObject setObject:object forKey:tableName];
        }
        [self extractValueAtIndex:index toColumnBindingHolder:resultColumn ofObject:object];
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
    const std::shared_ptr<WCTBaseAccessor> &accessor = columnBindingHolder.getColumnBinding().getAccessor();
    switch (accessor->getAccessorType()) {
    case WCTAccessorCpp: {
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer32: {
            WCTCppAccessor<WCDB::ColumnType::Integer32> *i32Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer32> *) accessor.get();
            i32Accessor->setValue(object, _handle->getInteger32(index));
        } break;
        case WCDB::ColumnType::Integer64: {
            WCTCppAccessor<WCDB::ColumnType::Integer64> *i64Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer64> *) accessor.get();
            i64Accessor->setValue(object, _handle->getInteger64(index));
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
            case WCDB::ColumnType::Integer32:
                value = [NSNumber numberWithInt:_handle->getInteger32(index)];
                break;
            case WCDB::ColumnType::Integer64:
                value = [NSNumber numberWithLongLong:_handle->getInteger64(index)];
                break;
            case WCDB::ColumnType::Float:
                value = [NSNumber numberWithDouble:_handle->getDouble(index)];
                break;
            case WCDB::ColumnType::Text:
                value = [NSString stringWithUTF8String:_handle->getText(index)];
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
- (WCTOptionalColumn)allValues
{
    WCTHandleAssert(return nil;);
    return [self allValuesAtIndex:0];
}

- (WCTOptionalColumn)allValuesAtIndex:(int)index
{
    WCTHandleAssert(return nil;);
    NSMutableArray *column = [NSMutableArray array];
    BOOL done = NO;
    while ([self step:done] && !done) {
        WCTValue *value = [self extractValueAtIndex:index];
        [column addObject:value ? value : [NSNull null]];
    }
    return done ? column : nil;
}

- (WCTOptionalColumnsXRows)allRows
{
    WCTHandleAssert(return nil;);
    NSMutableArray *rows = [NSMutableArray array];
    BOOL done = NO;
    while ([self step:done] && !done) {
        [rows addObject:[self extractRow]];
    }
    return done ? rows : nil;
}

- (NSArray /* <WCTObject*> */ *)allObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    WCTHandleAssert(return nil;);
    NSMutableArray *objects = [NSMutableArray array];
    BOOL done = NO;
    while ([self step:done] && !done) {
        [objects addObject:[self extractObjectOnResultColumns:resultColumns]];
    }
    return done ? objects : nil;
}

- (NSArray<WCTMultiObject *> *)allMultiObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    WCTHandleAssert(return nil;);
    NSMutableArray *multiObjects = [NSMutableArray array];
    BOOL done = NO;
    while ([self step:done] && !done) {
        [multiObjects addObject:[self extractMultiObjectOnResultColumns:resultColumns]];
    }
    return done ? multiObjects : nil;
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

@end
