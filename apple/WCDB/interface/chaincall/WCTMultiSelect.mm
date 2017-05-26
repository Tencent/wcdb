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

#import <WCDB/WCTMultiSelect.h>
#import <WCDB/WCTMultiSelect+Private.h>
#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTSelectBase+Private.h>
#import <WCDB/WCTResult.h>
#import <WCDB/WCTDeclare.h>

@implementation WCTMultiSelect {
    WCTResultList _resultList;
}

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase>&)core andResults:(const WCTResultList&)resultList fromTables:(NSArray<NSString*>*)tableNames
{
    if (self = [super initWithCore:core]) {
        if (resultList.size()==0) {
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::Select,
                                         WCDB::Error::InterfaceCode::Misuse,
                                         [NSString stringWithFormat:@"Selecting nothing from %@ is invalid", tableNames].UTF8String,
                                         &_error);
            return self;
        }
        _resultList.insert(_resultList.begin(), resultList.begin(), resultList.end());
        for (const WCTResult& result : _resultList) {
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
            if (![cls conformsToProtocol:@protocol(WCTTableCoding)]) {
                WCDB::Error::ReportInterface(_core->getTag(),
                                             _core->getPath(),
                                             WCDB::Error::InterfaceOperation::Select,
                                             WCDB::Error::InterfaceCode::Misuse,
                                             [NSString stringWithFormat:@"%@ should conform to protocol WCTTableCoding", NSStringFromClass(cls)].UTF8String,
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
        
        WCDB::SubqueryList subqueryList;
        for (NSString* tableName in tableNames) {
            subqueryList.push_back(tableName.UTF8String);
        }
        _statement.select(_resultList, _resultList.isDistinct()).from(subqueryList);
    }
    return self;
}

- (WCTMultiObject*)nextMultiObject
{
    WCDB::ScopedTicker scopedTicker(_ticker);
    if (![self lazyPrepare]) {
        return nil;
    }
    return [self _nextMultiObject];
}

- (NSArray<WCTMultiObject*>*)allMultiObjects
{
    WCDB::ScopedTicker scopedTicker(_ticker);
    if (![self lazyPrepare]) {
        return nil;
    }
    NSMutableArray* allMultiObjects = [[NSMutableArray alloc] init];
    WCTMultiObject* multiObject = nil;
    while ((multiObject = [self _nextMultiObject])) {
        [allMultiObjects addObject:multiObject];
    }
    return _error.isOK()?allMultiObjects:nil;
}

- (WCTMultiObject*)_nextMultiObject
{
    if (!_statementHandle->step()
        ||!_error.isOK()) {
        _statementHandle->finalize();
        return nil;
    }
    NSMutableDictionary* multiObject = [[NSMutableDictionary alloc] init];
    int index = 0;
    for (const WCTResult& result : _resultList) {
        const char* columnTableName = _statementHandle->getColumnTableName(index);
        Class cls = result.getBindingClass();
        if (columnTableName&&cls) {
            NSString* tableName = @(columnTableName);
            WCTObject* object = [multiObject objectForKey:tableName];
            if (!object) {
                object = [[cls alloc] init];
                [multiObject setObject:object forKey:tableName];
            }
            if (![self extractPropertyToObject:object 
                                       atIndex:index 
                             withColumnBinding:result.getColumnBinding()]) {
                return nil;
            };
        }else {
            if (!columnTableName) {
                WCDB::Error::Warning("Extracting multi object with an empty table name");
            }
            if (!cls) {
                WCDB::Error::Warning("Extracting multi object with an empty binding cls");
            }
        }
        ++index;
    }
    return multiObject;
    
}

@end
