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

#import <WCDB/NSData+noCopyData.h>
#import <WCDB/WCDB.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTUnsafeHandle

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
                       andHandle:(WCDB::Handle *)handle
{
    if (handle == nullptr) {
        return nil;
    }
    if (self = [super initWithDatabase:database]) {
        _handle = handle;
    }
    return self;
}

- (NSError *)getError
{
    return [WCTError errorWithWCDBError:_handle->getError()];
}

- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index
{
    const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
    const std::shared_ptr<WCTBaseAccessor> &accessor = columnBinding->accessor;
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
            if (value) {
                switch (accessor->getColumnType()) {
                    case WCDB::ColumnType::Integer32: {
                        NSNumber *number = (NSNumber *) value;
                        _handle->bindInteger32(number.intValue, index);
                        break;
                    }
                    case WCDB::ColumnType::Integer64: {
                        NSNumber *number = (NSNumber *) value;
                        _handle->bindInteger64(number.longLongValue, index);
                        break;
                    }
                    case WCDB::ColumnType::Float: {
                        NSNumber *number = (NSNumber *) value;
                        _handle->bindDouble(number.doubleValue, index);
                        break;
                    }
                    case WCDB::ColumnType::Text: {
                        NSString *string = (NSString *) value;
                        _handle->bindText(string.UTF8String, index);
                        break;
                    }
                    case WCDB::ColumnType::BLOB: {
                        NSData *data = (NSData *) value;
                        _handle->bindBLOB(data.noCopyData, index);
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

- (void)bindValue:(WCTValue *)value
          toIndex:(int)index
{
    value = [(id<WCTColumnCoding>) value archivedWCTValue];
    if ([value isKindOfClass:NSData.class]) {
        NSData *data = (NSData *) value;
        _handle->bindBLOB(data.noCopyData, index);
    } else if ([value isKindOfClass:NSString.class]) {
        NSString *string = (NSString *) value;
        _handle->bindText(string.UTF8String, (int) string.length);
    } else if ([value isKindOfClass:NSNumber.class]) {
        NSNumber *number = (NSNumber *) value;
        if (CFNumberIsFloatType((CFNumberRef) number)) {
            _handle->bindDouble(number.doubleValue, index);
        } else {
            if (CFNumberGetByteSize((CFNumberRef) number) <= 4) {
                _handle->bindInteger32(number.intValue, index);
            } else {
                _handle->bindInteger64(number.longLongValue, index);
            }
        }
    } else {
        _handle->bindNull(index);
    }
}

- (void)extractValueAtIndex:(int)index
                 toProperty:(const WCTProperty &)property
                   ofObject:(const WCTObject *)object
{
    const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
    const std::shared_ptr<WCTBaseAccessor> &accessor = columnBinding->accessor;
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
                    case WCDB::ColumnType::BLOB:
                        value = [NSData dataWithNoCopyData:_handle->getBLOB(index)];
                        break;
                    case WCDB::ColumnType::Null:
                        value = nil;
                        break;
                }
            }
            objcAccessor->setObject(object, value);
        } break;
    }
}

- (WCTValue *)getValueAtIndex:(int)index
{
    switch (_handle->getType(index)) {
        case WCDB::ColumnType::Integer32:
            return [NSNumber numberWithInt:_handle->getInteger32(index)];
        case WCDB::ColumnType::Integer64:
            return [NSNumber numberWithLongLong:_handle->getInteger64(index)];
        case WCDB::ColumnType::Float:
            return [NSNumber numberWithDouble:_handle->getDouble(index)];
        case WCDB::ColumnType::Text:
            return [NSString stringWithUTF8String:_handle->getText(index)];
        case WCDB::ColumnType::BLOB:
            return [NSData dataWithNoCopyData:_handle->getBLOB(index)];
        case WCDB::ColumnType::Null:
            return nil;
    }
}

- (WCTOneRow *)getRow
{
    NSMutableArray *row = [[NSMutableArray alloc] init];
    for (int i = 0; i < _handle->getColumnCount(); ++i) {
        NSObject *value = nil;
        switch (_handle->getType(i)) {
            case WCDB::ColumnType::Integer32:
                value = [NSNumber numberWithInt:_handle->getInteger32(i)];
                break;
            case WCDB::ColumnType::Integer64:
                value = [NSNumber numberWithLongLong:_handle->getInteger64(i)];
                break;
            case WCDB::ColumnType::Float:
                value = [NSNumber numberWithDouble:_handle->getDouble(i)];
                break;
            case WCDB::ColumnType::Text:
                value = [NSString stringWithUTF8String:_handle->getText(i)];
                break;
            case WCDB::ColumnType::BLOB:
                value = [NSData dataWithNoCopyData:_handle->getBLOB(i)];
                break;
            case WCDB::ColumnType::Null:
                value = [NSNull null];
                break;
        }
        [row addObject:value];
    }
    return row;
}

- (int)getChanges
{
    return _handle->getChanges();
}

- (BOOL)execute:(const WCDB::Statement &)statement
{
    return _handle->execute(statement);
}

- (BOOL)prepare:(const WCDB::Statement &)statement
{
    return _handle->prepare(statement);
}

- (BOOL)step:(BOOL &)done
{
    return _handle->step((bool &) done);
}

- (BOOL)step
{
    return _handle->step();
}

- (void)finalizeStatement
{
    _handle->finalize();
}

- (BOOL)isStatementReadonly
{
    return _handle->isStatementReadonly();
}

- (void)reset
{
    _handle->reset();
}

- (void)bindInteger32:(const int32_t &)value toIndex:(int)index
{
    _handle->bindInteger32(value, index);
}

- (void)bindInteger64:(const int64_t &)value toIndex:(int)index
{
    _handle->bindInteger64(value, index);
}

- (void)bindDouble:(const double &)value toIndex:(int)index
{
    _handle->bindDouble(value, index);
}

- (void)bindText:(const char *)value toIndex:(int)index
{
    _handle->bindText(value, index);
}

- (void)bindBLOB:(NSData *)value toIndex:(int)index
{
    _handle->bindBLOB(value.noCopyData, index);
}

- (void)bindNullToIndex:(int)index
{
    _handle->bindNull(index);
}

- (int32_t)getInteger32AtIndex:(int)index
{
    return _handle->getInteger32(index);
}

- (int64_t)getInteger64AtIndex:(int)index
{
    return _handle->getInteger64(index);
}

- (double)getDouble:(int)index
{
    return _handle->getDouble(index);
}

- (const char *)getTextAtIndex:(int)index
{
    return _handle->getText(index);
}

- (NSData *)getBLOB:(int)index
{
    return [NSData dataWithNoCopyData:_handle->getBLOB(index)];
}

- (WCDB::ColumnType)getTypeAtIndex:(int)index
{
    return _handle->getType(index);
}

- (int)getColumnCount
{
    return _handle->getColumnCount();
}

- (const char *)getColumnNameAtIndex:(int)index
{
    return _handle->getColumnName(index);
}

- (const char *)getColumnTableNameAtIndex:(int)index
{
    return _handle->getColumnTableName(index);
}

- (long long)getLastInsertedRowID
{
    return _handle->getLastInsertedRowID();
}

- (const char *)getErrorMessage
{
    return _handle->getErrorMessage();
}

- (int)getExtendedErrorCode
{
    return _handle->getExtendedErrorCode();
}

- (int)getResultCode
{
    return _handle->getResultCode();
}

@end
