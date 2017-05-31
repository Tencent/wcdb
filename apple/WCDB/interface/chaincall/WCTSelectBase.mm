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

#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTExpr.h>
#import <WCDB/WCTSelectBase+Private.h>
#import <WCDB/WCTSelectBase.h>

@implementation WCTSelectBase

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core
{
    if (self = [super initWithCore:core]) {
        _prepared = NO;
    }
    return self;
}

- (BOOL)lazyPrepare
{
    if (_error.isOK()) {
        if (!_prepared) {
            _prepared = YES;
            _statementHandle = _core->prepare(_statement, _error);
            return _error.isOK();
        }
        return YES;
    }
    return NO;
}

- (instancetype)where:(const WCTCondition &)condition
{
    _statement.where(condition);
    return self;
}

- (instancetype)orderBy:(const WCTOrderByList &)orderList
{
    _statement.orderBy(orderList);
    return self;
}

- (instancetype)limit:(const WCTLimit &)limit
{
    _statement.limit(limit);
    return self;
}

- (instancetype)offset:(const WCTOffset &)offset
{
    _statement.offset(offset);
    return self;
}

- (instancetype)groupBy:(const WCTGroupByList &)groupByList
{
    WCDB::ExprList exprList;
    for (const WCTGroupBy &groupBy : groupByList) {
        exprList.push_back(groupBy);
    }
    _statement.groupBy(exprList);
    return self;
}

- (instancetype)having:(const WCTHaving &)having
{
    _statement.having(having);
    return self;
}

- (id /* WCTValue* */)extractValueAtIndex:(int)i
{
    //TODO I can know the result type from WCTProperty
    id value = nil;
    switch ((WCTColumnType) _statementHandle->getType(i)) {
        case WCTColumnTypeDouble:
            value = [NSNumber numberWithDouble:_statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeDouble>(i)];
            break;
        case WCTColumnTypeInteger32:
            value = [NSNumber numberWithInt:_statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeInteger32>(i)];
            break;
        case WCTColumnTypeInteger64:
            value = [NSNumber numberWithLongLong:_statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeInteger64>(i)];
            break;
        case WCTColumnTypeString: {
            const char *string = _statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeString>(i);
            value = string ? [NSString stringWithUTF8String:string] : nil;
        } break;
        case WCTColumnTypeBinary: {
            int size = 0;
            const void *data = _statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeBinary>(i, size);
            value = data ? [NSData dataWithBytes:data length:size] : nil;
        } break;
        case WCTColumnTypeNil: {
            value = nil;
            break;
        }
        default:
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::Select,
                                         WCDB::Error::InterfaceCode::Misuse,
                                         [NSString stringWithFormat:@"Extracting statement [%s] with unknown type %d", _statementHandle->getColumnName(i), (int) _statementHandle->getType(i)].UTF8String,
                                         &_error);
            return nil;
    }
    return value;
}

- (BOOL)extractPropertyToObject:(WCTObject *)object
                        atIndex:(int)index
              withColumnBinding:(const std::shared_ptr<WCTColumnBinding> &)columnBinding
{
    BOOL result = YES;
    const std::shared_ptr<WCTBaseAccessor> &accessor = columnBinding->accessor;
    switch (accessor->getAccessorType()) {
        case WCTAccessorCpp: {
            switch (accessor->getColumnType()) {
                case WCTColumnTypeInteger32: {
                    WCTCppAccessor<WCTColumnTypeInteger32> *i32Accessor = (WCTCppAccessor<WCTColumnTypeInteger32> *) accessor.get();
                    i32Accessor->setValue(object,
                                          _statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeInteger32>(index));
                } break;
                case WCTColumnTypeInteger64: {
                    WCTCppAccessor<WCTColumnTypeInteger64> *i64Accessor = (WCTCppAccessor<WCTColumnTypeInteger64> *) accessor.get();
                    i64Accessor->setValue(object,
                                          _statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeInteger64>(index));
                } break;
                case WCTColumnTypeDouble: {
                    WCTCppAccessor<WCTColumnTypeDouble> *floatAccessor = (WCTCppAccessor<WCTColumnTypeDouble> *) accessor.get();
                    floatAccessor->setValue(object,
                                            _statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeDouble>(index));
                } break;
                case WCTColumnTypeString: {
                    WCTCppAccessor<WCTColumnTypeString> *textAccessor = (WCTCppAccessor<WCTColumnTypeString> *) accessor.get();
                    textAccessor->setValue(object,
                                           _statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeString>(index));
                } break;
                case WCTColumnTypeBinary: {
                    WCTCppAccessor<WCTColumnTypeBinary> *blobAccessor = (WCTCppAccessor<WCTColumnTypeBinary> *) accessor.get();
                    int size = 0;
                    blobAccessor->setValue(object,
                                           _statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeBinary>(index, size),
                                           size);
                } break;
                default:
                    WCDB::Error::ReportInterface(_core->getTag(),
                                                 _core->getPath(),
                                                 WCDB::Error::InterfaceOperation::Select,
                                                 WCDB::Error::InterfaceCode::Misuse,
                                                 [NSString stringWithFormat:@"Extracting column [%s] with unknown type %d", columnBinding->columnName.c_str(), (int) accessor->getColumnType()].UTF8String,
                                                 &_error);
                    result = NO;
                    break;
            }
        } break;
        case WCTAccessorObjC: {
            WCTObjCAccessor *objcAccessor = (WCTObjCAccessor *) accessor.get();
            id value = nil;
            switch (accessor->getColumnType()) {
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
                    value = [NSData dataWithBytes:data length:size];
                } break;
                default:
                    WCDB::Error::ReportInterface(_core->getTag(),
                                                 _core->getPath(),
                                                 WCDB::Error::InterfaceOperation::Select,
                                                 WCDB::Error::InterfaceCode::Misuse,
                                                 [NSString stringWithFormat:@"Extracting column [%s] with unknown type %d", columnBinding->columnName.c_str(), (int) accessor->getColumnType()].UTF8String,
                                                 &_error);
                    result = NO;
                    break;
            }
            objcAccessor->setObject(object, value);
        } break;
        default:
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::Select,
                                         WCDB::Error::InterfaceCode::Misuse,
                                         [NSString stringWithFormat:@"Extracting column [%s] with unknown accessor type %d", columnBinding->columnName.c_str(), (int) accessor->getAccessorType()].UTF8String,
                                         &_error);
            result = NO;
            break;
    }
    return result;
}

@end
