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

#import <WCDB/NSData+NoCopyData.h>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTCoreStatement+Private.h>
#import <WCDB/WCTCoreStatement.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTValue.h>

@implementation WCTCoreStatement

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core andStatementHandle:(WCDB::RecyclableStatement &)handleStatement
{
    if (self = [super initWithCore:core]) {
        _statementHandle = handleStatement;
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
            _statementHandle->bind<WCTColumnTypeString>(((NSString *) value).UTF8String, index);
            break;
        case WCTValueTypeNumber: {
            NSNumber *number = (NSNumber *) value;
            if (CFNumberIsFloatType((CFNumberRef) number)) {
                _statementHandle->bind<WCTColumnTypeDouble>(number.doubleValue, index);
            } else {
                if (CFNumberGetByteSize((CFNumberRef) number) <= 4) {
                    _statementHandle->bind<WCTColumnTypeInteger32>(number.intValue, index);
                } else {
                    _statementHandle->bind<WCTColumnTypeInteger64>(number.longLongValue, index);
                }
            }
        } break;
        case WCTValueTypeData: {
            NSData *data = (NSData *) value;
            _statementHandle->bind<WCTColumnTypeBinary>(data.noCopyData, index);
        } break;
        case WCTValueTypeNil:
            _statementHandle->bind<WCTColumnTypeNull>(index);
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
    int index = [self indexByColumnName:columnName];
    if (index != INT_MAX) {
        return [self bindValue:value toIndex:index];
    }
    return NO;
}

- (WCTValue *)valueAtIndex:(int)index
{
    WCTValue *value = nil;
    switch (_statementHandle->getType(index)) {
        case WCTColumnTypeInteger32:
            value = [NSNumber numberWithInt:_statementHandle->getValue<WCTColumnTypeInteger32>(index)];
            break;
        case WCTColumnTypeInteger64:
            value = [NSNumber numberWithLongLong:_statementHandle->getValue<WCTColumnTypeInteger64>(index)];
            break;
        case WCTColumnTypeDouble:
            value = [NSNumber numberWithDouble:_statementHandle->getValue<WCTColumnTypeDouble>(index)];
            break;
        case WCTColumnTypeString: {
            const char *string = _statementHandle->getValue<WCTColumnTypeString>(index);
            value = string ? [NSString stringWithUTF8String:string] : nil;
        } break;
        case WCTColumnTypeBinary: {
            WCDB::NoCopyData data = _statementHandle->getValue<WCTColumnTypeBinary>(index);
            value = [NSData dataWithNoCopyData:data];
        } break;
        case WCTColumnTypeNull: {
            value = nil;
        } break;
        default:
            break;
    }
    return value;
}

- (WCTValue *)valueByColumnName:(NSString *)columnName
{
    int index = [self indexByColumnName:columnName];
    if (index != INT_MAX) {
        return [self valueAtIndex:index];
    }
    return nil;
}

- (int)indexByColumnName:(NSString *)columnName
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

- (BOOL)reset
{
    return _statementHandle->reset();
}

- (BOOL)step
{
    return _statementHandle->step();
}

- (WCDB::ColumnType)columnTypeAtIndex:(int)index
{
    return _statementHandle->getType(index);
}

- (WCDB::ColumnType)columnTypeByName:(NSString *)columnName
{
    int index = [self indexByColumnName:columnName];
    if (index != INT_MAX) {
        return [self columnTypeAtIndex:index];
    }
    return WCTColumnTypeNull;
}

- (int)columnCount
{
    return _statementHandle->getColumnCount();
}

- (NSString *)columnNameAtIndex:(int)index
{
    const char *columnName = _statementHandle->getColumnName(index);
    return columnName ? @(columnName) : nil;
}

- (NSString *)tableNameAtIndex:(int)index
{
    const char *tableName = _statementHandle->getColumnTableName(index);
    return tableName ? @(tableName) : nil;
}

- (BOOL)finalize
{
    bool result = true;
    if (_statementHandle) {
        _statementHandle->finalize();
        _statementHandle = nullptr;
    }
    return result;
}

- (long long)lastInsertedRowID
{
    return _statementHandle->getLastInsertedRowID();
}

- (int)changes
{
    return _statementHandle->getChanges();
}

@end
