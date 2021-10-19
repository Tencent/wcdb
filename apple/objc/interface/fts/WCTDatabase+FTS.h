//
// Created by sanhuazhang on 2019/05/02
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

#import <WCDB/WCTCommon.h>
#import <WCDB/WCTDatabase.h>
#import <WCDB/WCTFTSTokenizer.h>

NS_ASSUME_NONNULL_BEGIN

WCDB_EXTERN NSString* const WCTModuleFTS3;
WCDB_EXTERN NSString* const WCTModuleFTS5;

WCDB_EXTERN NSString* const WCTTokenizerSimple;
WCDB_EXTERN NSString* const WCTTokenizerPorter;
WCDB_EXTERN NSString* const WCTTokenizerICU;
WCDB_EXTERN NSString* const WCTTokenizerUnicode61;

// WCDB implemented tokenizer
WCDB_EXTERN NSString* const WCTTokenizerOneOrBinary;
WCDB_EXTERN NSString* const WCTTokenizerOneOrBinary_FTS5;
WCDB_EXTERN NSString* const WCTTokenizerLegacyOneOrBinary; // compatible for WCTTokenizerNameWCDB before v1.1.0

// Configurable parameters for WCDB implemented tokenizers
WCDB_EXTERN NSString* const WCTTokenizerParameter_OneWord;
WCDB_EXTERN NSString* const WCTTokenizerParameter_PinYin;
WCDB_EXTERN NSString* const WCTTokenizerParameter_NeedSymbol;

@interface WCTDatabase (FTS)

- (void)enableAutoMergeFTS5Index:(BOOL)enable;

- (void)addTokenizer:(NSString*)tokenizerName;

+ (void)registerTokenizer:(const WCDB::TokenizerModule&)module named:(NSString*)name;

// No thread safe
+ (void)configPinYinDict:(NSDictionary<NSString*, NSArray<NSString*>*>*)pinyinDict;

@end

NS_ASSUME_NONNULL_END
