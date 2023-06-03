//
// Created by qiuwenchen on 2022/4/22.
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

#import "WCTAPIBridge.h"
#import "HandleStatement.hpp"
#import "ObjectBridge.hpp"
#import "WCTFTSTokenizerUtil.h"
#import <Foundation/Foundation.h>
#ifdef WCDB_SWIFT_BRIDGE_OBJC
#import "WCTBridgeProperty+CPP.h"
#import "WCTDeclaration.h"
#import "WCTHandle+ChainCall.h"
#import "WCTHandle+Private.h"
#import "WCTHandle+Table.h"
#import "WCTInsert.h"
#import "WCTPreparedStatement+Private.h"
#import "WCTResultColumn.h"
#import "WCTTableCoding.h"
#endif

@implementation WCTAPIBridge

#ifdef WCDB_SWIFT_BRIDGE_OBJC

+ (BOOL)createTable:(NSString*)name
          withClass:(Class)tableClass
         withHandle:(CPPHandle)handle
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, NO);
    WCTHandle* wctHandle = [[WCTHandle alloc] initWithUnsafeHandle:cppHandle];
    return [wctHandle createTable:name withClass:tableClass];
}

+ (BOOL)createVirtualTable:(NSString*)name
                 withClass:(Class)tableClass
                withHandle:(CPPHandle)handle
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, NO);
    WCTHandle* wctHandle = [[WCTHandle alloc] initWithUnsafeHandle:cppHandle];
    return [wctHandle createVirtualTable:name withClass:tableClass];
}

+ (BOOL)insertObjects:(NSArray<WCTObject*>*)objects
            intoTable:(NSString*)name
       withProperties:(WCTBridgeProperties*)properties
            orReplace:(BOOL)replace
             orIgnore:(BOOL)ignore
           withHandle:(CPPHandle)handle
{
    if (objects.count == 0) {
        return YES;
    }
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, NO);
    WCTHandle* wctHandle = [[WCTHandle alloc] initWithUnsafeHandle:cppHandle];
    WCTProperties wctProperties;
    if (properties.count > 0) {
        for (WCTBridgeProperty* property in properties) {
            wctProperties.push_back(property.wctProperty);
        }
    } else {
        wctProperties = [objects.firstObject.class allProperties];
    }
    WCTInsert* insert = [wctHandle prepareInsert];
    [[[insert values:objects] intoTable:name] onProperties:wctProperties];
    if (replace) {
        [insert orReplace];
    } else if (ignore) {
        [insert orIgnore];
    }
    return [insert execute];
}

+ (void)bindProperties:(WCTBridgeProperties*)properties
              ofObject:(WCTObject*)object
   withHandleStatement:(CPPHandleStatement)handleStatement
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    WCTPreparedStatement* wctHandleStatement = [[WCTPreparedStatement alloc] initWithHandleStatement:cppHandleStatement];
    WCTProperties wctProperties;
    for (WCTBridgeProperty* bridgeProperty in properties) {
        wctProperties.push_back(bridgeProperty.wctProperty);
    }
    [wctHandleStatement bindProperties:wctProperties ofObject:object];
}

+ (void)bindProperty:(WCTBridgeProperty*)property
            ofObject:(WCTObject*)object
             toIndex:(int)index
 withHandleStatement:(CPPHandleStatement)handleStatement
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    WCTPreparedStatement* wctHandleStatement = [[WCTPreparedStatement alloc] initWithHandleStatement:cppHandleStatement];
    [wctHandleStatement bindProperty:property.wctProperty ofObject:object toIndex:index];
}

+ (WCTObject*)extractObjectOnResultColumns:(WCTBridgeProperties*)properties
                       fromHandleStatement:(CPPHandleStatement)handleStatement
{
    WCDBGetObjectOrReturnValue(handleStatement, WCDB::HandleStatement, cppHandleStatement, nil);
    WCTPreparedStatement* wctHandleStatement = [[WCTPreparedStatement alloc] initWithHandleStatement:cppHandleStatement];
    WCTResultColumns resultColumns;
    for (WCTBridgeProperty* bridgeProperty in properties) {
        resultColumns.emplace_back(bridgeProperty.wctProperty);
    }
    return [wctHandleStatement extractObjectOnResultColumns:resultColumns];
}

+ (WCTMultiObject*)extractMultiObjectOnResultColumns:(WCTBridgeProperties*)properties
                                 fromHandleStatement:(CPPHandleStatement)handleStatement
{
    WCDBGetObjectOrReturnValue(handleStatement, WCDB::HandleStatement, cppHandleStatement, nil);
    WCTPreparedStatement* wctHandleStatement = [[WCTPreparedStatement alloc] initWithHandleStatement:cppHandleStatement];
    WCTResultColumns resultColumns;
    for (WCTBridgeProperty* bridgeProperty in properties) {
        resultColumns.emplace_back(bridgeProperty.wctProperty);
    }
    return [wctHandleStatement extractMultiObjectOnResultColumns:resultColumns];
}

#endif

+ (void)configDefaultSymbolDetectorAndUnicodeNormalizer
{
    WCTFTSTokenizerUtil::configDefaultSymbolDetectorAndUnicodeNormalizer();
}

+ (void)configPinyinDict:(NSDictionary<NSString*, NSArray<NSString*>*>*)pinyinDict
{
    WCTFTSTokenizerUtil::configPinyinDict(pinyinDict);
}

+ (void)configTraditionalChineseDict:(NSDictionary<NSString*, NSString*>*)traditionalChineseDict
{
    WCTFTSTokenizerUtil::configTraditionalChineseDict(traditionalChineseDict);
}

@end
