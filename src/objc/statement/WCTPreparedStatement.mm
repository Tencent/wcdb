//
// Created by qiuwenchen on 2020/2/24.
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

#import "WCTPreparedStatement.h"
#import "WCTFoundation.h"
#import "WCTHandle+Private.h"
#import "WCTORM.h"
#import "WCTPreparedStatement+Private.h"
#import <Foundation/Foundation.h>

#define WCTPreparedStatementAssert(remedial) WCTRemedialAssert(_handleStatement != nullptr, "handleStatement is null", remedial;)

@implementation WCTPreparedStatement

- (instancetype)initWithHandleStatement:(WCDB::HandleStatement *)handlesStatement
{
    if (self = [super init]) {
        _handleStatement = handlesStatement;
    }
    return self;
}

- (WCDB::HandleStatement *)getRawHandleStatement
{
    return _handleStatement;
}

#pragma mark - Step
- (BOOL)step
{
    WCTPreparedStatementAssert(return NO);
    return _handleStatement->step();
}

- (void)reset
{
    WCTPreparedStatementAssert(return);
    _handleStatement->reset();
}

- (void)clearBindings
{
    WCTPreparedStatementAssert(return);
    _handleStatement->clearBindings();
}

- (BOOL)done
{
    WCTPreparedStatementAssert(return NO);
    return _handleStatement->done();
}

#pragma mark - State
- (BOOL)isStatementReadonly
{
    WCTPreparedStatementAssert(return NO);
    return _handleStatement->isReadOnly();
}

#pragma mark - Bind
- (void)bindInteger:(const int64_t &)value toIndex:(int)index
{
    WCTPreparedStatementAssert(return);
    _handleStatement->bindInteger(value, index);
}

- (void)bindDouble:(const double &)value toIndex:(int)index
{
    WCTPreparedStatementAssert(return);
    _handleStatement->bindDouble(value, index);
}

- (void)bindNullToIndex:(int)index
{
    WCTPreparedStatementAssert(return);
    _handleStatement->bindNull(index);
}

- (void)bindString:(NSString *)string toIndex:(int)index
{
    WCTPreparedStatementAssert(return);
    _handleStatement->bindText(WCDB::UnsafeStringView(string), index);
}

- (void)bindData:(NSData *)data toIndex:(int)index
{
    WCTPreparedStatementAssert(return);
    _handleStatement->bindBLOB(data, index);
}

- (void)bindNumber:(NSNumber *)number toIndex:(int)index
{
    WCTPreparedStatementAssert(return);
    if (number == nil || CFNumberIsFloatType((CFNumberRef) number)) {
        _handleStatement->bindDouble(number.doubleValue, index);
    } else {
        _handleStatement->bindInteger(number.integerValue, index);
    }
}

- (void)bindValue:(WCTColumnCodingValue *)value
          toIndex:(int)index
{
    WCTPreparedStatementAssert(return);
    WCTValue *archivedValue = [value archivedWCTValue];
    switch ([value.class columnType]) {
    case WCTColumnTypeNull:
        _handleStatement->bindNull(index);
        break;
    case WCTColumnTypeInteger:
        _handleStatement->bindInteger(archivedValue.numberValue.longLongValue, index);
        break;
    case WCTColumnTypeFloat:
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
    WCTPreparedStatementAssert(return);
    const std::shared_ptr<const WCTBaseAccessor> &accessor = property.getColumnBinding().getAccessor();
    switch (accessor->getAccessorType()) {
    case WCTAccessorCpp: {
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer: {
            WCTCppAccessor<WCDB::ColumnType::Integer> *integerAccessor = (WCTCppAccessor<WCDB::ColumnType::Integer> *) accessor.get();
            _handleStatement->bindInteger(integerAccessor->getValue(object),
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
            case WCDB::ColumnType::Integer: {
                _handleStatement->bindInteger(((NSNumber *) value).longLongValue, index);
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
    WCTPreparedStatementAssert(return);
    int index = 1;
    for (const auto &property : properties) {
        [self bindProperty:property ofObject:object toIndex:index];
        ++index;
    }
}

- (int)bindParameterIndex:(const WCDB::BindParameter &)parameter
{
    WCTPreparedStatementAssert(return 0);
    return _handleStatement->bindParameterIndex(parameter.getDescription());
}

#pragma mark - Get
- (int64_t)extractIntegerAtIndex:(int)index
{
    WCTPreparedStatementAssert(return 0);
    return _handleStatement->getInteger(index);
}

- (double)extractDoubleAtIndex:(int)index
{
    WCTPreparedStatementAssert(return 0);
    return _handleStatement->getDouble(index);
}

- (NSString *)extractStringAtIndex:(int)index
{
    WCTPreparedStatementAssert(return nil);
    return [NSString stringWithView:_handleStatement->getText(index)];
}

- (NSNumber *)extractNumberAtIndex:(int)index
{
    WCTPreparedStatementAssert(return nil);
    switch (_handleStatement->getType(index)) {
    case WCDB::ColumnType::Integer:
        return [NSNumber numberWithLongLong:_handleStatement->getInteger(index)];
    default:
        return [NSNumber numberWithDouble:_handleStatement->getDouble(index)];
    }
}

- (NSData *)extractDataAtIndex:(int)index
{
    WCTPreparedStatementAssert(return nil);
    const WCDB::UnsafeData blob = _handleStatement->getBLOB(index);
    return [NSData dataWithBytes:blob.buffer() length:blob.size()];
}

- (WCTValue *)extractValueAtIndex:(int)index
{
    WCTPreparedStatementAssert(return nil);
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
    WCTPreparedStatementAssert(return WCTColumnTypeNull);
    return (WCTColumnType) _handleStatement->getType(index);
}

- (int)extractNumberOfColumns
{
    WCTPreparedStatementAssert(return 0);
    return _handleStatement->getNumberOfColumns();
}

- (NSString *)extractOriginColumnNameAtIndex:(int)index
{
    WCTPreparedStatementAssert(return nil);
    return [NSString stringWithView:_handleStatement->getOriginColumnName(index)];
}

- (NSString *)extractColumnNameAtIndex:(int)index
{
    WCTPreparedStatementAssert(return nil);
    return [NSString stringWithView:_handleStatement->getColumnName(index)];
}

- (NSString *)extractTableNameAtIndex:(int)index
{
    WCTPreparedStatementAssert(return nil);
    return [NSString stringWithView:_handleStatement->getColumnTableName(index)];
}

- (WCTOneRow *)extractRow
{
    WCTPreparedStatementAssert(return nil);
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
    WCTPreparedStatementAssert(return nil);
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
    WCTPreparedStatementAssert(return);
    const std::shared_ptr<const WCTBaseAccessor> &accessor = columnBindingHolder.getColumnBinding().getAccessor();
    switch (accessor->getAccessorType()) {
    case WCTAccessorCpp: {
        switch (accessor->getColumnType()) {
        case WCDB::ColumnType::Integer: {
            WCTCppAccessor<WCDB::ColumnType::Integer> *integerAccessor = (WCTCppAccessor<WCDB::ColumnType::Integer> *) accessor.get();
            integerAccessor->setValue(object, _handleStatement->getInteger(index));
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
            case WCDB::ColumnType::Integer:
                value = [NSNumber numberWithLongLong:_handleStatement->getInteger(index)];
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
    WCTPreparedStatementAssert(return nil);
    return [self allValuesAtIndex:0];
}

- (WCTOneColumn *)allValuesAtIndex:(int)index
{
    WCTPreparedStatementAssert(return nil);
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
    WCTPreparedStatementAssert(return nil);
    NSMutableArray *rows = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        [rows addObject:[self extractRow]];
    }
    return succeed ? rows : nil;
}

- (NSArray /* <WCTObject*> */ *)allObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    WCTPreparedStatementAssert(return nil);
    NSMutableArray *objects = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        [objects addObject:[self extractObjectOnResultColumns:resultColumns]];
    }
    return succeed ? objects : nil;
}

- (NSArray<WCTMultiObject *> *)allMultiObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    WCTPreparedStatementAssert(return nil);
    NSMutableArray *multiObjects = [NSMutableArray array];
    BOOL succeed = NO;
    while ((succeed = [self step]) && ![self done]) {
        [multiObjects addObject:[self extractMultiObjectOnResultColumns:resultColumns]];
    }
    return succeed ? multiObjects : nil;
}

@end
