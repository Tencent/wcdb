//
// Created by 陈秋文 on 2022/4/22.
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
#import <WCDB/HandleBridge.h>
#import <WCDB/WCTBridgeProperty.h>

NS_ASSUME_NONNULL_BEGIN

@protocol WCTTableCoding;
typedef NSObject<WCTTableCoding> WCTObject;
typedef NSDictionary<NSString * /*table name */, WCTObject *> WCTMultiObject;

@interface WCTAPIBridge : NSObject

+ (BOOL)createTable:(NSString *)name
          withClass:(Class<WCTTableCoding>)tableClass
         withHandle:(CPPHandle)handle;

+ (BOOL)createVirtualTable:(NSString *)name
                 withClass:(Class<WCTTableCoding>)tableClass
                withHandle:(CPPHandle)handle;

+ (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
            intoTable:(NSString *)name
       withProperties:(WCTBridgeProperties *_Nullable)properties
            orReplace:(BOOL)replace
           withHandle:(CPPHandle)handle;

+ (void)bindProperties:(WCTBridgeProperties *)properties
              ofObject:(WCTObject *)object
   withHandleStatement:(CPPHandleStatement)handleStatement;

+ (void)bindProperty:(WCTBridgeProperty *)property
            ofObject:(WCTObject *)object
             toIndex:(int)index
 withHandleStatement:(CPPHandleStatement)handleStatement;

+ (nullable WCTObject *)extractObjectOnResultColumns:(WCTBridgeProperties *)properties
                                 fromHandleStatement:(CPPHandleStatement)handleStatement;

+ (nullable WCTMultiObject *)extractMultiObjectOnResultColumns:(WCTBridgeProperties *)properties
                                           fromHandleStatement:(CPPHandleStatement)handleStatement;

+ (void)configDefaultSymbolDetectorAndUnicodeNormalizer;

+ (void)configPinyinDict:(NSDictionary<NSString *, NSArray<NSString *> *> *)pinyinDict;

+ (void)configTraditionalChineseDict:(NSDictionary<NSString *, NSString *> *)traditionalChineseDict;

@end

NS_ASSUME_NONNULL_END
