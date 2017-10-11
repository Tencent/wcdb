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

#import <WCDB/WCTCoding.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTStatement+Private.h>
#import <WCDB/WCTStatement.h>
#import <WCDB/WCTValue.h>
#import <WCDB/handle_statement.hpp>

@implementation WCTStatement

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core andStatementHandle:(WCDB::RecyclableStatement &)statementHandle
{
    if (self = [super initWithCore:core]) {
        _statementHandle = statementHandle;
    }
    return self;
}

- (WCTError *)getError
{
    const WCDB::Error &error = _statementHandle->getError();
    if (error.isOK()) {
        return nil;
    }
    return [WCTError errorWithWCDBError:error];
}

- (BOOL)bindValue:(WCTValue *)value toIndex:(int)index
{
    WCTValueType valueType = [value valueType];
    if (valueType == WCTValueTypeColumnCoding) {
        value = [(id<WCTColumnCoding>) value archivedWCTValue];
        valueType = [value valueType];
    }
    BOOL result = YES;
    switch (valueType) {
        case WCTValueTypeString:
            _statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeString>(((NSString *) value).UTF8String, index);
            break;
        case WCTValueTypeNumber: {
            NSNumber *number = (NSNumber *) value;
            if (CFNumberIsFloatType((CFNumberRef) number)) {
                _statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeDouble>(number.doubleValue, index);
            } else {
                if (CFNumberGetByteSize((CFNumberRef) number) <= 4) {
                    _statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeInteger32>(number.intValue, index);
                } else {
                    _statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeInteger64>(number.longLongValue, index);
                }
            }
        } break;
        case WCTValueTypeData: {
            NSData *data = (NSData *) value;
            _statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeBinary>(data.bytes, (int) data.length, index);
        } break;
        case WCTValueTypeNil:
            _statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeNil>(index);
            break;
        default:
            WCDB::Error::Abort(([NSString stringWithFormat:@"Binding statement with unknown type %@", NSStringFromClass(value.class)].UTF8String));
            result = NO;
            break;
    }
    return result;
}

- (BOOL)bindValue:(WCTValue *)value byColumnName:(NSString *)columnName
{
    int index = [self getIndexByColumnName:columnName];
    if (index != INT_MAX) {
        return [self bindValue:value toIndex:index];
    }
    return NO;
}

- (WCTValue *)getValueAtIndex:(int)index
{
    WCTValue *value = nil;
    switch ((WCTColumnType) _statementHandle->getType(index)) {
        case WCTColumnTypeInteger32:
            value = [NSNumber numberWithInt:_statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeInteger32>(index)];
            break;
        case WCTColumnTypeInteger64:
            value = [NSNumber numberWithLongLong:_statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeInteger64>(index)];
            break;
        case WCTColumnTypeDouble:
            value = [NSNumber numberWithDouble:_statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeDouble>(index)];
            break;
        case WCTColumnTypeString: {
            const char *string = _statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeString>(index);
            value = string ? [NSString stringWithUTF8String:string] : nil;
        } break;
        case WCTColumnTypeBinary: {
            int size = 0;
            const void *data = _statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeBinary>(index, size);
            value = data ? [NSData dataWithBytes:data length:size] : nil;
        } break;
        case WCTColumnTypeNil: {
            value = nil;
        } break;
        default:
            break;
    }
    return value;
}

- (WCTValue *)getValueByColumnName:(NSString *)columnName
{
    int index = [self getIndexByColumnName:columnName];
    if (index != INT_MAX) {
        return [self getValueAtIndex:index];
    }
    return nil;
}

- (int)getIndexByColumnName:(NSString *)columnName
{
    if (columnName) {
        const char *rightColumnName = columnName.UTF8String;
        for (int i = 0; i < _statementHandle->getColumnCount(); ++i) {
            const char *leftColumnName = _statementHandle->getColumnName(i);
            if (leftColumnName && strcmp(leftColumnName, rightColumnName) == 0) {
                return i;
            }
        }
    }
    return INT_MAX;
}

- (void)reset
{
    _statementHandle->reset();
}

- (BOOL)step
{
    return _statementHandle->step();
}

- (WCTColumnType)getTypeAtIndex:(int)index
{
    return (WCTColumnType) _statementHandle->getType(index);
}

- (WCTColumnType)getTypeByColumnName:(NSString *)columnName
{
    int index = [self getIndexByColumnName:columnName];
    if (index != INT_MAX) {
        return [self getTypeAtIndex:index];
    }
    return WCTColumnTypeNil;
}

- (int)getColumnCount
{
    return _statementHandle->getColumnCount();
}

- (NSString *)getColumnNameAtIndex:(int)index
{
    const char *columnName = _statementHandle->getColumnName(index);
    return columnName ? @(columnName) : nil;
}

- (NSString *)getTableNameAtIndex:(int)index
{
    const char *tableName = _statementHandle->getColumnTableName(index);
    return tableName ? @(tableName) : nil;
}

- (void)finalize
{
    _statementHandle->finalize();
}

@end
