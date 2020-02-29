//
// Created by 陈秋文 on 2020/2/24.
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

#import <Foundation/Foundation.h>
#import <WCDB/WCTHandleStatement.h>
#import <WCDB/WCTHandleStatement+Private.h>
#import <WCDB/WCTHandle+Private.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTFoundation.h>

#define WCTHandleStatementAssert(remedial) WCTRemedialAssert(_handleStatement != nullptr, "handleStatement is null", remedial;)

@implementation WCTHandleStatement

-(instancetype)initWithHandle:(WCDB::HandleStatement*)handlesStatement andTag:(NSString*)tag
{
    if(self = [super init]){
        _handleStatement = handlesStatement;
        _tag = tag;
    }
    return self;
}

- (WCDB::HandleStatement *)getRawHandleStatement
{
    return _handleStatement;
}

#pragma mark - Execute
- (BOOL)execute:(const WCDB::Statement &)statement
{
    WCTHandleStatementAssert(return NO);
    bool succeed = false;
    if(_handleStatement->prepare(statement)){
        succeed = _handleStatement->step();
        _handleStatement->finalize();
    }
    return succeed;
}

#pragma mark - Prepare
- (BOOL)prepare:(const WCDB::Statement &)statement
{
    WCTHandleStatementAssert(return NO);
    return _handleStatement->prepare(statement);
}

- (BOOL)isPrepared
{
    WCTHandleStatementAssert(return NO);
    return _handleStatement->isPrepared();
}

- (void)finalize
{
    WCTHandleStatementAssert(return);
    _handleStatement->finalize();
}

#pragma mark - Step
- (BOOL)step
{
    WCTHandleStatementAssert(return NO);
    return _handleStatement->step();
}

- (void)reset
{
    WCTHandleStatementAssert(return);
    _handleStatement->reset();
}

- (BOOL)done
{
    WCTHandleStatementAssert(return NO);
    return _handleStatement->done();
}

#pragma mark - State
- (BOOL)isStatementReadonly
{
    WCTHandleStatementAssert(return NO);
    return _handleStatement->isReadonly();
}

#pragma mark - Bind
- (void)bindInteger32:(const int32_t &)value toIndex:(int)index
{
    WCTHandleStatementAssert(return);
    _handleStatement->bindInteger32(value, index);
}

- (void)bindInteger64:(const int64_t &)value toIndex:(int)index
{
    WCTHandleStatementAssert(return);
    _handleStatement->bindInteger64(value, index);
}

- (void)bindDouble:(const double &)value toIndex:(int)index
{
    WCTHandleStatementAssert(return);
    _handleStatement->bindDouble(value, index);
}

- (void)bindNullToIndex:(int)index
{
    WCTHandleStatementAssert(return);
    _handleStatement->bindNull(index);
}

- (void)bindString:(NSString *)string toIndex:(int)index
{
    WCTHandleStatementAssert(return);
    _handleStatement->bindText(WCDB::UnsafeStringView(string), index);
}

- (void)bindData:(NSData *)data toIndex:(int)index
{
    WCTHandleStatementAssert(return);
    _handleStatement->bindBLOB(data, index);
}

- (void)bindNumber:(NSNumber *)number toIndex:(int)index
{
    WCTHandleStatementAssert(return);
    if (number == nil || CFNumberIsFloatType((CFNumberRef) number)) {
        _handleStatement->bindDouble(number.doubleValue, index);
    } else {
        if (CFNumberGetByteSize((CFNumberRef) number) <= 4) {
            _handleStatement->bindInteger32(number.intValue, index);
        } else {
            _handleStatement->bindInteger64(number.integerValue, index);
        }
    }
}

- (void)bindValue:(WCTColumnCodingValue *)value
          toIndex:(int)index
{
    WCTHandleStatementAssert(return);
    WCTValue *archivedValue = [value archivedWCTValue];
    switch ([value.class columnType]) {
    case WCTColumnTypeNil:
        _handleStatement->bindNull(index);
        break;
    case WCTColumnTypeInteger32:
        _handleStatement->bindInteger32(archivedValue.numberValue.intValue, index);
        break;
    case WCTColumnTypeInteger64:
        _handleStatement->bindInteger64(archivedValue.numberValue.longLongValue, index);
        break;
    case WCTColumnTypeDouble:
        _handleStatement->bindDouble(archivedValue.numberValue.doubleValue, index);
        break;
    case WCTColumnTypeString:
        _handleStatement->bindText(WCDB::UnsafeStringView(archivedValue.stringValue), index);
        break;
    case WCTColumnTypeData:
        _handleStatement->bindBLOB((NSData *) archivedValue, index);
        break;
    }
}

- (void)bindProperty:(const WCTProperty &)property
            ofObject:(WCTObject *)object
             toIndex:(int)index
{
    WCTHandleStatementAssert(return);
    const std::shared_ptr<const WCTBaseAccessor> &accessor = property.getColumnBinding().getAccessor();
    switch (accessor->getAccessorType()) {
    case WCTAccessorCpp: {
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer32: {
            WCTCppAccessor<WCDB::ColumnType::Integer32> *i32Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer32> *) accessor.get();
            _handleStatement->bindInteger32(i32Accessor->getValue(object),
                                   index);
        } break;
        case WCDB::ColumnType::Integer64: {
            WCTCppAccessor<WCDB::ColumnType::Integer64> *i64Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer64> *) accessor.get();
            _handleStatement->bindInteger64(i64Accessor->getValue(object),
                                   index);
        } break;
        case WCDB::ColumnType::Float: {
            WCTCppAccessor<WCDB::ColumnType::Float> *floatAccessor = (WCTCppAccessor<WCDB::ColumnType::Float> *) accessor.get();
            _handleStatement->bindDouble(floatAccessor->getValue(object),
                                index);
        } break;
        case WCDB::ColumnType::Text: {
            WCTCppAccessor<WCDB::ColumnType::Text> *textAccessor = (WCTCppAccessor<WCDB::ColumnType::Text> *) accessor.get();
            _handleStatement->bindText(textAccessor->getValue(object),
                              index);
        } break;
        case WCDB::ColumnType::BLOB: {
            WCTCppAccessor<WCDB::ColumnType::BLOB> *blobAccessor = (WCTCppAccessor<WCDB::ColumnType::BLOB> *) accessor.get();
            _handleStatement->bindBLOB(blobAccessor->getValue(object), index);
        } break;
        case WCDB::ColumnType::Null:
            _handleStatement->bindNull(index);
            break;
        }
    } break;
    case WCTAccessorObjC: {
        WCTObjCAccessor *objcAccessor = (WCTObjCAccessor *) accessor.get();
        NSObject *value = objcAccessor->getObject(object);
        if (value != nil) {
            switch (accessor->getColumnType()) {
            case WCDB::ColumnType::Integer32: {
                _handleStatement->bindInteger32(((NSNumber *) value).intValue, index);
                break;
            }
            case WCDB::ColumnType::Integer64: {
                _handleStatement->bindInteger64(((NSNumber *) value).longLongValue, index);
                break;
            }
            case WCDB::ColumnType::Float: {
                _handleStatement->bindDouble(((NSNumber *) value).doubleValue, index);
                break;
            }
            case WCDB::ColumnType::Text: {
                _handleStatement->bindText((NSString *) value, index);
                break;
            }
            case WCDB::ColumnType::BLOB: {
                _handleStatement->bindBLOB((NSData *) value, index);
                break;
            }
            case WCDB::ColumnType::Null:
                _handleStatement->bindNull(index);
                break;
            }
        } else {
            _handleStatement->bindNull(index);
        }
    } break;
    }
}

- (void)bindProperties:(const WCTProperties &)properties
              ofObject:(WCTObject *)object
{
    WCTHandleStatementAssert(return);
    int index = 1;
    for (const auto &property : properties) {
        [self bindProperty:property ofObject:object toIndex:index];
        ++index;
    }
}

- (int)bindParameterIndex:(const WCDB::BindParameter &)parameter
{
    WCTHandleStatementAssert(return 0);
    return _handleStatement->bindParameterIndex(parameter.getDescription());
}

#pragma mark - Get
- (int32_t)extractInteger32AtIndex:(int)index
{
    WCTHandleStatementAssert(return 0);
    return _handleStatement->getInteger32(index);
}

- (int64_t)extractInteger64AtIndex:(int)index
{
    WCTHandleStatementAssert(return 0);
    return _handleStatement->getInteger64(index);
}

- (double)extractDoubleAtIndex:(int)index
{
    WCTHandleStatementAssert(return 0);
    return _handleStatement->getDouble(index);
}

- (NSString *)extractStringAtIndex:(int)index
{
    WCTHandleStatementAssert(return nil);
    return [NSString stringWithView:_handleStatement->getText(index)];
}

- (NSNumber *)extractNumberAtIndex:(int)index
{
    WCTHandleStatementAssert(return nil);
    switch (_handleStatement->getType(index)) {
    case WCDB::ColumnType::Integer32:
        return [NSNumber numberWithInt:_handleStatement->getInteger32(index)];
    case WCDB::ColumnType::Integer64:
        return [NSNumber numberWithLongLong:_handleStatement->getInteger64(index)];
    default:
        return [NSNumber numberWithDouble:_handleStatement->getDouble(index)];
    }
}

- (NSData *)extractDataAtIndex:(int)index
{
    WCTHandleStatementAssert(return nil);
    const WCDB::UnsafeData blob = _handleStatement->getBLOB(index);
    return [NSData dataWithBytes:blob.buffer() length:blob.size()];
}

- (WCTValue *)extractValueAtIndex:(int)index
{
    WCTHandleStatementAssert(return nil);
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
    WCTHandleStatementAssert(return WCTColumnTypeNil);
    return _handleStatement->getType(index);
}

- (int)extractNumberOfColumns
{
    WCTHandleStatementAssert(return 0);
    return _handleStatement->getNumberOfColumns();
}

- (NSString *)extractOriginColumnNameAtIndex:(int)index
{
    WCTHandleStatementAssert(return nil);
    return [NSString stringWithView:_handleStatement->getOriginColumnName(index)];
}

- (NSString *)extractColumnNameAtIndex:(int)index
{
    WCTHandleStatementAssert(return nil);
    return [NSString stringWithView:_handleStatement->getColumnName(index)];
}

- (NSString *)extractTableNameAtIndex:(int)index
{
    WCTHandleStatementAssert(return nil);
    return [NSString stringWithView:_handleStatement->getColumnTableName(index)];
}

- (WCTOneRow *)extractRow
{
    WCTHandleStatementAssert(return nil);
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
    WCTHandleStatementAssert(return nil);
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
        NSString *tableName = [NSString stringWithView:_handleStatement->getColumnTableName(index)];
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
    WCTHandleStatementAssert(return);
    const std::shared_ptr<const WCTBaseAccessor> &accessor = columnBindingHolder.getColumnBinding().getAccessor();
    switch (accessor->getAccessorType()) {
    case WCTAccessorCpp: {
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer32: {
            WCTCppAccessor<WCDB::ColumnType::Integer32> *i32Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer32> *) accessor.get();
            i32Accessor->setValue(object, _handleStatement->getInteger32(index));
        } break;
        case WCDB::ColumnType::Integer64: {
            WCTCppAccessor<WCDB::ColumnType::Integer64> *i64Accessor = (WCTCppAccessor<WCDB::ColumnType::Integer64> *) accessor.get();
            i64Accessor->setValue(object, _handleStatement->getInteger64(index));
        } break;
        case WCDB::ColumnType::Float: {
            WCTCppAccessor<WCDB::ColumnType::Float> *floatAccessor = (WCTCppAccessor<WCDB::ColumnType::Float> *) accessor.get();
            floatAccessor->setValue(object, _handleStatement->getDouble(index));
        } break;
        case WCDB::ColumnType::Text: {
            WCTCppAccessor<WCDB::ColumnType::Text> *textAccessor = (WCTCppAccessor<WCDB::ColumnType::Text> *) accessor.get();
            textAccessor->setValue(object, _handleStatement->getText(index));
        } break;
        case WCDB::ColumnType::BLOB: {
            WCTCppAccessor<WCDB::ColumnType::BLOB> *blobAccessor = (WCTCppAccessor<WCDB::ColumnType::BLOB> *) accessor.get();
            blobAccessor->setValue(object, _handleStatement->getBLOB(index));
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
        if (_handleStatement->getType(index) != WCDB::ColumnType::Null) {
            switch (accessor->getColumnType()) {
            case WCDB::ColumnType::Integer32:
                value = [NSNumber numberWithInt:_handleStatement->getInteger32(index)];
                break;
            case WCDB::ColumnType::Integer64:
                value = [NSNumber numberWithLongLong:_handleStatement->getInteger64(index)];
                break;
            case WCDB::ColumnType::Float:
                value = [NSNumber numberWithDouble:_handleStatement->getDouble(index)];
                break;
            case WCDB::ColumnType::Text:
                value = [NSString stringWithView:_handleStatement->getText(index)];
                break;
            case WCDB::ColumnType::BLOB: {
                const WCDB::UnsafeData data = _handleStatement->getBLOB(index);
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
    WCTHandleStatementAssert(return nil);
    return [self allValuesAtIndex:0];
}

- (WCTOneColumn *)allValuesAtIndex:(int)index
{
    WCTHandleStatementAssert(return nil);
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
    WCTHandleStatementAssert(return nil);
    NSMutableArray *rows = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        [rows addObject:[self extractRow]];
    }
    return succeed ? rows : nil;
}

- (NSArray /* <WCTObject*> */ *)allObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    WCTHandleStatementAssert(return nil);
    NSMutableArray *objects = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        [objects addObject:[self extractObjectOnResultColumns:resultColumns]];
    }
    return succeed ? objects : nil;
}

- (NSArray<WCTMultiObject *> *)allMultiObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    WCTHandleStatementAssert(return nil);
    NSMutableArray *multiObjects = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        [multiObjects addObject:[self extractMultiObjectOnResultColumns:resultColumns]];
    }
    return succeed ? multiObjects : nil;
}


@end
