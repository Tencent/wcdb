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

#import <WCDB/WCTBinding.h>
#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTExpr.h>
#import <WCDB/WCTResult.h>
#import <WCDB/WCTSelect+Private.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTSelectBase+Private.h>
#import <WCDB/handle_statement.hpp>
#import <WCDB/utility.hpp>

@implementation WCTSelect {
    WCTResultList _resultList;
    Class _cls;
}

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core andResults:(const WCTResultList &)resultList fromTable:(NSString *)tableName
{
    if (self = [super initWithCore:core]) {
        if (resultList.size() == 0) {
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::Select,
                                         WCDB::Error::InterfaceCode::Misuse,
                                         [NSString stringWithFormat:@"Selecting nothing from %@", tableName].UTF8String,
                                         &_error);
            return self;
        }
        if (tableName.length == 0) {
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::Select,
                                         WCDB::Error::InterfaceCode::Misuse,
                                         @"Nil table name".UTF8String,
                                         &_error);
            return self;
        }
        _resultList.insert(_resultList.begin(), resultList.begin(), resultList.end());
        _statement.select(_resultList, _resultList.isDistinct()).from(tableName.UTF8String);
        _cls = nil;
        for (const WCTResult &result : _resultList) {
            Class cls = result.getBindingClass();
            if (!cls) {
                WCDB::Error::ReportInterface(_core->getTag(),
                                             _core->getPath(),
                                             WCDB::Error::InterfaceOperation::Select,
                                             WCDB::Error::InterfaceCode::Misuse,
                                             "This Result does not belong to any class",
                                             &_error);
                return self;
            }
            if (_cls) {
                if (_cls != cls) {
                    WCDB::Error::ReportInterface(_core->getTag(),
                                                 _core->getPath(),
                                                 WCDB::Error::InterfaceOperation::Select,
                                                 WCDB::Error::InterfaceCode::Inconsistent, "The WCTSelect query properties do not belong to the same class. To do a multi-class select, use [WCTMultiSelect] instead.",
                                                 &_error);
                    return self;
                }
            } else {
                _cls = cls;
                if (![_cls conformsToProtocol:@protocol(WCTTableCoding)]) {
                    WCDB::Error::ReportInterface(_core->getTag(),
                                                 _core->getPath(),
                                                 WCDB::Error::InterfaceOperation::Select,
                                                 WCDB::Error::InterfaceCode::Misuse,
                                                 [NSString stringWithFormat:@"%@ should conform to protocol WCTTableCoding", NSStringFromClass(_cls)].UTF8String,
                                                 &_error);
                    return self;
                }
                if (!result.getColumnBinding()) {
                    WCDB::Error::ReportInterface(_core->getTag(),
                                                 _core->getPath(),
                                                 WCDB::Error::InterfaceOperation::Select,
                                                 WCDB::Error::InterfaceCode::Misuse,
                                                 "This Result does not contain any column binding",
                                                 &_error);
                    return self;
                }
            }
        }
    }
    return self;
}

- (NSArray /* <WCTObject*> */ *)allObjects
{
    WCDB::ScopedTicker scopedTicker(_ticker);
    if ([self lazyPrepare]) {
        NSMutableArray *objects = [[NSMutableArray alloc] init];
        WCTObject *object = nil;
        int index = 0;
        while ([self next]) {
            object = [[_cls alloc] init];
            index = 0;
            for (const WCTResult &result : _resultList) {
                if ([self extractPropertyToObject:object
                                          atIndex:index
                                withColumnBinding:result.getColumnBinding()]) {
                    ++index;
                } else {
                    return nil;
                }
            }
            [objects addObject:object];
        }
        return _error.isOK() ? objects : nil;
    }
    return nil;
}

- (id /* WCTObject* */)nextObject
{
    WCDB::ScopedTicker scopedTicker(_ticker);
    if ([self lazyPrepare] && [self next]) {
        WCTObject *object = [[_cls alloc] init];
        int index = 0;
        for (const WCTResult &result : _resultList) {
            if ([self extractPropertyToObject:object
                                      atIndex:index
                            withColumnBinding:result.getColumnBinding()]) {
                ++index;
            } else {
                return nil;
            }
        }
        return object;
    }
    return nil;
}

@end
