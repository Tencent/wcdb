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
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTStatement+Private.h>
#import <WCDB/WCTStatement.h>
#import <WCDB/handle_statement.hpp>

@implementation WCTStatement

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core andStatementHandle:(WCDB::RecyclableStatement &)statementHandle andError:(const WCDB::Error &)error
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
    if ([value isKindOfClass:NSData.class]) {
        NSData *data = (NSData *) value;
        _statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeBinary>(data.bytes, (int) data.length, index);
    } else if ([value isKindOfClass:NSString.class]) {
        NSString *string = (NSString *) value;
        _statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeString>(string.UTF8String, index);
    } else if ([value isKindOfClass:NSNumber.class]) {
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
    } else if ([value isKindOfClass:NSNull.class] || value == nil) {
        _statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeNil>(index);
    } else {
        WCDB::Error::Abort(([NSString stringWithFormat:@"Binding statement with unknown type %@", NSStringFromClass(value.class)].UTF8String));
        return NO;
    }
    return YES;
}

//get value, index begin with 0
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

- (void)resetBinding
{
    _statementHandle->resetBinding();
}

- (BOOL)step
{
    return _statementHandle->step();
}

- (WCTColumnType)getTypeAtIndex:(int)index
{
    return (WCTColumnType) _statementHandle->getType(index);
}

- (int)getCount
{
    return _statementHandle->getColumnCount();
}

- (NSString *)getNameAtIndex:(int)index
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
