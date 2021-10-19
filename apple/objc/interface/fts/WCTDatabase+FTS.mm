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

#import <WCDB/Assertion.hpp>
#import <WCDB/WCTDatabase+FTS.h>
#import <WCDB/WCTDatabase+Private.h>

NSString* const WCTTokenizerSimple = @"simple";
NSString* const WCTTokenizerPorter = @"porter";
NSString* const WCTTokenizerICU = @"icu";
NSString* const WCTTokenizerUnicode61 = @"unicode61";
NSString* const WCTTokenizerOneOrBinary = @"wcdb_one_or_binary";
NSString* const WCTTokenizerOneOrBinary_FTS5 = @"wcdb_one_or_binary_fts5";
NSString* const WCTTokenizerLegacyOneOrBinary = @"WCDB";

NSString* const WCTTokenizerParameter_OneWord = @"just_one";
NSString* const WCTTokenizerParameter_PinYin = @"pin_yin";
NSString* const WCTTokenizerParameter_NeedSymbol = @"need_symbol";

NSString* const WCTModuleFTS3 = @"fts3";
NSString* const WCTModuleFTS5 = @"fts5";

static NSDictionary* g_pinyinDict = nil;

static std::nullptr_t initialize()
{
    WCDB::Core::shared().registerTokenizer(WCTTokenizerOneOrBinary, WCDB::FTS3TokenizerModuleTemplate<WCDB::OneOrBinaryTokenizerInfo, WCTOneOrBinaryTokenizer>::specialize());
    WCDB::Core::shared().registerTokenizer(WCTTokenizerLegacyOneOrBinary, WCDB::FTS3TokenizerModuleTemplate<WCDB::OneOrBinaryTokenizerInfo, WCTOneOrBinaryTokenizer>::specialize());
    [WCTDatabase registerTokenizer:WCDB::FTS5TokenizerModuleTemplate<WCTOneOrBinaryTokenizer>::specializeWithContext(nullptr) named:WCTTokenizerOneOrBinary_FTS5];
    return nullptr;
}

@implementation WCTDatabase (FTS)

- (void)enableAutoMergeFTS5Index:(BOOL)enable
{
    WCDB::Core::shared().enableAutoMergeFTSIndex(_database, enable);
}

- (void)addTokenizer:(NSString*)tokenizerName
{
    WCDB_ONCE(initialize());

    WCDB::StringView configName = WCDB::StringView::formatted("%s%s", WCDB::TokenizeConfigPrefix, tokenizerName.UTF8String);
    _database->setConfig(configName, WCDB::Core::shared().tokenizerConfig(tokenizerName), WCDB::Configs::Priority::Higher);
}

+ (void)registerTokenizer:(const WCDB::TokenizerModule&)module named:(NSString*)name
{
    WCTRemedialAssert(name.length > 0, "Module name can't be nil.", return;);
    WCDB::Core::shared().registerTokenizer(name, module);
}

+ (void)configPinYinDict:(NSDictionary<NSString*, NSArray<NSString*>*>*)pinyinDict
{
    WCTOneOrBinaryTokenizer::configPinyinDict(pinyinDict);
}

@end
