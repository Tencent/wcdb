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

#import "WCTFTSTokenizerUtil.h"
#import "WCTCommon.h"
#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>

WCDB::StringView WCTFTSTokenizerUtil::tokenize(NSString* name, ...)
{
    WCDB::StringView tokenizerPrefix = WCDB::Syntax::CreateVirtualTableSTMT::tokenizerPreFix();
    std::ostringstream stream;
    stream << tokenizerPrefix.data() << name.UTF8String;
    va_list pArgs;
    va_start(pArgs, name);
    NSString* parameter = nullptr;
    while ((parameter = va_arg(pArgs, NSString*)) != nullptr) {
        stream << ' ' << parameter.UTF8String;
    }
    return WCDB::StringView(stream.str());
}

void WCTFTSTokenizerUtil::configDefaultSymbolDetectorAndUnicodeNormalizer()
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        //Refer to http://www.fileformat.info/info/unicode/category/index.htm
        //Code: Cc, Cf, Z*, U000A ~ U000D, U0085, M*, P*, S* and illegal character set
        CFMutableCharacterSetRef characterSetRef = CFCharacterSetCreateMutable(CFAllocatorGetDefault());
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetControl));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetNonBase));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetPunctuation));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetSymbol));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetIllegal));
        configSymbolDetector([=](UnicodeChar theChar) {
            bool symbol = false;
            if (characterSetRef != nil) {
                symbol = CFCharacterSetIsCharacterMember(characterSetRef, theChar);
            }
            return symbol;
        });
        configUnicodeNormalizer([](const WCDB::UnsafeStringView& token) {
            @autoreleasepool {
                NSMutableString* nsToken = [[NSMutableString alloc] initWithBytes:token.data() length:token.length() encoding:NSUTF8StringEncoding];
                CFMutableStringRef normalizationFormText = (__bridge_retained CFMutableStringRef)(nsToken);
                CFStringNormalize(normalizationFormText, kCFStringNormalizationFormKD);
                return WCDB::StringView(((__bridge_transfer NSString*) normalizationFormText).UTF8String);
            }
        });
    });
}

void WCTFTSTokenizerUtil::configPinyinDict(NSDictionary<NSString*, NSArray<NSString*>*>* pinyinDict)
{
    WCDB::StringViewMap<std::vector<WCDB::StringView>>* cppPinyinDict = new WCDB::StringViewMap<std::vector<WCDB::StringView>>();
    for (NSString* character in pinyinDict.allKeys) {
        if (character.UTF8String == nil) {
            continue;
        }
        WCDB::StringView key = WCDB::StringView(character.UTF8String);
        std::vector<WCDB::StringView> value;
        for (NSString* piniyn in pinyinDict[character]) {
            if (piniyn.UTF8String == nil) {
                continue;
            }
            value.push_back(WCDB::StringView(piniyn.UTF8String));
        }
        if (value.size() > 0) {
            cppPinyinDict->insert_or_assign(key, value);
        }
    }
    WCDB::BaseTokenizerUtil::configPinyinDict(cppPinyinDict);
}

void WCTFTSTokenizerUtil::configTraditionalChineseDict(NSDictionary<NSString*, NSString*>* traditionalChineseDict)
{
    WCDB::StringViewMap<WCDB::StringView>* cppTraditionalChineseDict = new WCDB::StringViewMap<WCDB::StringView>();
    for (NSString* chinese in traditionalChineseDict.allKeys) {
        NSString* simplifiedChinese = traditionalChineseDict[chinese];
        if (chinese.UTF8String == nil || simplifiedChinese.UTF8String == nil) {
            continue;
        }
        cppTraditionalChineseDict->insert_or_assign(WCDB::StringView(chinese.UTF8String), WCDB::StringView(simplifiedChinese.UTF8String));
    }
    WCDB::BaseTokenizerUtil::configTraditionalChineseDict(cppTraditionalChineseDict);
}
