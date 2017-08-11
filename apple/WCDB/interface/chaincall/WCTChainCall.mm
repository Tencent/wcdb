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
#import <WCDB/WCTChainCall.h>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTValue.h>
#import <WCDB/handle_statement.hpp>

@implementation WCTChainCall

- (BOOL)bindProperty:(const WCTProperty &)property ofObject:(WCTObject *)object toStatementHandle:(WCDB::RecyclableStatement &)statementHandle atIndex:(int)index withError:(WCDB::Error &)error
{
    const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
    if (!columnBinding) {
        WCDB::Error::ReportInterface(_core->getTag(),
                                     _core->getPath(),
                                     WCDB::Error::InterfaceOperation::ChainCall,
                                     WCDB::Error::InterfaceCode::ORM,
                                     [NSString stringWithFormat:@"Binding an unknown column named [%s]", property.getName().c_str()].UTF8String,
                                     &error);
        return NO;
    }
    const std::shared_ptr<WCTBaseAccessor> &accessor = columnBinding->accessor;
    switch (accessor->getAccessorType()) {
        case WCTAccessorCpp: {
            switch (accessor->getColumnType()) {
                case WCTColumnTypeInteger32: {
                    WCTCppAccessor<WCTColumnTypeInteger32> *i32Accessor = (WCTCppAccessor<WCTColumnTypeInteger32> *) accessor.get();
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeInteger32>(i32Accessor->getValue(object),
                                                                                     index);
                } break;
                case WCTColumnTypeInteger64: {
                    WCTCppAccessor<WCTColumnTypeInteger64> *i64Accessor = (WCTCppAccessor<WCTColumnTypeInteger64> *) accessor.get();
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeInteger64>(i64Accessor->getValue(object),
                                                                                     index);
                } break;
                case WCTColumnTypeDouble: {
                    WCTCppAccessor<WCTColumnTypeDouble> *floatAccessor = (WCTCppAccessor<WCTColumnTypeDouble> *) accessor.get();
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeDouble>(floatAccessor->getValue(object),
                                                                                  index);
                } break;
                case WCTColumnTypeString: {
                    WCTCppAccessor<WCTColumnTypeString> *textAccessor = (WCTCppAccessor<WCTColumnTypeString> *) accessor.get();
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeString>(textAccessor->getValue(object),
                                                                                  index);
                } break;
                case WCTColumnTypeBinary: {
                    int size = 0;
                    WCTCppAccessor<WCTColumnTypeBinary> *blobAccessor = (WCTCppAccessor<WCTColumnTypeBinary> *) accessor.get();
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeBinary>(blobAccessor->getValue(object, size),
                                                                                  size,
                                                                                  index);
                } break;
                default:
                    WCDB::Error::ReportInterface(_core->getTag(),
                                                 _core->getPath(),
                                                 WCDB::Error::InterfaceOperation::ChainCall,
                                                 WCDB::Error::InterfaceCode::ORM,
                                                 [NSString stringWithFormat:@"Binding column [%s] with unknown type %d", columnBinding->columnName.c_str(), (int) accessor->getColumnType()].UTF8String,
                                                 &error);
                    return NO;
                    break;
            }
        } break;
        case WCTAccessorObjC: {
            WCTObjCAccessor *objcAccessor = (WCTObjCAccessor *) accessor.get();
            switch (accessor->getColumnType()) {
                case WCTColumnTypeInteger32: {
                    NSNumber *number = objcAccessor->getObject(object);
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeInteger32>(number.intValue, index);
                    break;
                }
                case WCTColumnTypeInteger64: {
                    NSNumber *number = objcAccessor->getObject(object);
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeInteger64>(number.longLongValue, index);
                    break;
                }
                case WCTColumnTypeDouble: {
                    NSNumber *number = objcAccessor->getObject(object);
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeDouble>(number.doubleValue, index);
                    break;
                }
                case WCTColumnTypeString: {
                    NSString *string = objcAccessor->getObject(object);
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeString>(string.UTF8String, index);
                    break;
                }
                case WCTColumnTypeBinary: {
                    NSData *data = objcAccessor->getObject(object);
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeBinary>(data.bytes, (int) data.length, index);
                    break;
                }
                default:
                    WCDB::Error::ReportInterface(_core->getTag(),
                                                 _core->getPath(),
                                                 WCDB::Error::InterfaceOperation::ChainCall,
                                                 WCDB::Error::InterfaceCode::ORM,
                                                 [NSString stringWithFormat:@"Binding column [%s] with unknown type %d", columnBinding->columnName.c_str(), (int) accessor->getColumnType()].UTF8String,
                                                 &error);
                    return NO;
                    break;
            }
        } break;
        default:
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::ChainCall,
                                         WCDB::Error::InterfaceCode::ORM,
                                         [NSString stringWithFormat:@"Binding column [%s] with unknown accessor type %d", columnBinding->columnName.c_str(), (int) accessor->getAccessorType()].UTF8String,
                                         &error);
            return NO;
            break;
    }
    return YES;
}

- (BOOL)bindWithValue:(WCTValue *)value toStatementHandle:(WCDB::RecyclableStatement &)statementHandle atIndex:(int)index withError:(WCDB::Error &)error
{
    WCTValueType valueType = [value valueType];
    if (valueType == WCTValueTypeColumnCoding) {
        value = [(id<WCTColumnCoding>) value archivedWCTValue];
        valueType = [value valueType];
    }
    BOOL result = YES;
    switch (valueType) {
        case WCTValueTypeString:
            statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeString>(((NSString *) value).UTF8String, index);
            break;
        case WCTValueTypeNumber: {
            NSNumber *number = (NSNumber *) value;
            if (CFNumberIsFloatType((CFNumberRef) number)) {
                statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeDouble>(number.doubleValue, index);
            } else {
                if (CFNumberGetByteSize((CFNumberRef) number) <= 4) {
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeInteger32>(number.intValue, index);
                } else {
                    statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeInteger64>(number.longLongValue, index);
                }
            }
        } break;
        case WCTValueTypeData: {
            NSData *data = (NSData *) value;
            statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeBinary>(data.bytes, (int) data.length, index);
        } break;
        case WCTValueTypeNil:
            statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeNil>(index);
            break;
        default:
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::ChainCall,
                                         WCDB::Error::InterfaceCode::ORM,
                                         [NSString stringWithFormat:@"Binding with unknown type %@", value.class].UTF8String,
                                         &error);
            result = NO;
            break;
    }
    return result;
}
@end
