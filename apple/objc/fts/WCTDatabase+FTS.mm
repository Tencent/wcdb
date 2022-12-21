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
#import <WCDB/FTSConst.h>
#import <WCDB/WCTDatabase+FTS.h>
#import <WCDB/WCTDatabase+Private.h>

NSString* const WCTTokenizerSimple = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::Simple];
NSString* const WCTTokenizerPorter = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::Porter];
NSString* const WCTTokenizerICU = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::ICU];
NSString* const WCTTokenizerUnicode61 = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::Unicode61];
NSString* const WCTTokenizerOneOrBinary = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::OneOrBinary];
NSString* const WCTTokenizerLegacyOneOrBinary = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::LegacyOneOrBinary];
NSString* const WCTTokenizerVerbatim = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::Verbatim];
NSString* const WCTTokenizerPinyin = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::Pinyin];

NSString* const WCTTokenizerParameter_NeedSymbol = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::Parameter::NeedSymbol];
NSString* const WCTTokenizerParameter_SimplifyChinese = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::Parameter::SimplifyChinese];
NSString* const WCTTokenizerParameter_SkipStemming = [NSString stringWithUTF8String:WCDB::BuiltinTokenizer::Parameter::SkipStemming];

NSString* const WCTModuleFTS3 = [NSString stringWithUTF8String:WCDB::Module::FTS3];
NSString* const WCTModuleFTS4 = [NSString stringWithUTF8String:WCDB::Module::FTS4];
NSString* const WCTModuleFTS5 = [NSString stringWithUTF8String:WCDB::Module::FTS5];

NSString* const WCTAuxiliaryFunction_SubstringMatchInfo = [NSString stringWithUTF8String:WCDB::BuiltinAuxiliaryFunction::SubstringMatchInfo];

@implementation WCTDatabase (FTS)

- (void)enableAutoMergeFTS5Index:(BOOL)enable
{
    WCDB::Core::shared().enableAutoMergeFTSIndex(_database, enable);
}

- (void)addTokenizer:(NSString*)tokenizerName
{
    WCTFTSTokenizerUtil::configDefaultSymbolDetectorAndUnicodeNormalizer();
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
    WCTFTSTokenizerUtil::configPinyinDict(pinyinDict);
}

+ (void)configTraditionalChineseDict:(NSDictionary<NSString*, NSString*>*)traditionalChineseDict
{
    WCTFTSTokenizerUtil::configTraditionalChineseDict(traditionalChineseDict);
}

- (void)addAuxiliaryFunction:(NSString*)auxiliaryFunctionName
{
    WCDB::StringView configName = WCDB::StringView::formatted("%s%s", WCDB::AuxiliaryFunctionConfigPrefix, auxiliaryFunctionName.UTF8String);
    _database->setConfig(configName, WCDB::Core::shared().auxiliaryFunctionConfig(auxiliaryFunctionName), WCDB::Configs::Priority::Higher);
}

+ (void)registerAuxiliaryFunction:(const WCDB::FTS5AuxiliaryFunctionModule&)module named:(NSString*)name
{
    WCTRemedialAssert(name.length > 0, "Module name can't be nil.", return;);
    WCDB::Core::shared().registerAuxiliaryFunction(name, module);
}

@end
