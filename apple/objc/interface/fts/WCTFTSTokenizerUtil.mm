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

#import <Foundation/Foundation.h>
#import <WCDB/Assertion.hpp>
#import <WCDB/Error.hpp>
#import <WCDB/WCTFTSTokenizerUtil.h>

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

int WCTFTSTokenizerUtil::stepOneUnicode(const WCDB::UnsafeStringView input, UnicodeType& unicodeType, int& unicodeLength)
{
    if (input.length() == 0) {
        unicodeType = UnicodeType::None;
        unicodeLength = 0;
        return WCDB::Error::c2rc(WCDB::Error::Code::OK);
    }

    WCDB::Error::Code code;
    const unsigned char& first = input.at(0);
    if (first < 0xC0) {
        unicodeLength = 1;
        if (first >= 0x30 && first <= 0x39) {
            unicodeType = UnicodeType::BasicMultilingualPlaneDigit;
        } else if ((first >= 0x41 && first <= 0x5a) || (first >= 0x61 && first <= 0x7a)) {
            unicodeType = UnicodeType::BasicMultilingualPlaneLetter;
        } else {
            int rc;
            bool symbol = false;
            std::tie(rc, symbol) = isSymbol(first);
            code = WCDB::Error::rc2c(rc);
            if (code == WCDB::Error::Code::OK) {
                if (symbol) {
                    unicodeType = UnicodeType::BasicMultilingualPlaneSymbol;
                } else {
                    unicodeType = UnicodeType::BasicMultilingualPlaneOther;
                }
            } else {
                return WCDB::Error::c2rc(code);
            }
        }
    } else if (first < 0xF0) {
        UnicodeChar unicode = 0;
        if (first < 0xE0) {
            unicodeLength = 2;
            unicode = first & 0x1F;
        } else {
            unicodeLength = 3;
            unicode = first & 0x0F;
        }
        for (int i = 1; i < unicodeLength; ++i) {
            if (i < input.length()) {
                WCTAssert(((unicode << 6) >> 6) == unicode);
                unicode = (UnicodeChar) (((int) unicode << 6) | (input.at(i) & 0x3F));
            } else {
                break;
            }
        }
        int rc;
        bool symbol = false;
        std::tie(rc, symbol) = isSymbol(unicode);
        code = WCDB::Error::rc2c(rc);
        if (code == WCDB::Error::Code::OK) {
            if (symbol) {
                unicodeType = UnicodeType::BasicMultilingualPlaneSymbol;
            } else {
                unicodeType = UnicodeType::BasicMultilingualPlaneOther;
            }
        } else {
            return WCDB::Error::c2rc(code);
        }
    } else {
        unicodeType = UnicodeType::AuxiliaryPlaneOther;
        if (first < 0xF8) {
            unicodeLength = 4;
        } else if (first < 0xFC) {
            unicodeLength = 5;
        } else {
            unicodeLength = 6;
        }
    }
    if (unicodeLength > input.length()) {
        unicodeType = UnicodeType::None;
        unicodeLength = (int) input.length();
    }
    return WCDB::Error::c2rc(WCDB::Error::Code::OK);
}

CFCharacterSetRef WCTFTSTokenizerUtil::g_symbolCharacterSet = CFCharacterSetCreateMutable(CFAllocatorGetDefault());

std::pair<int, bool> WCTFTSTokenizerUtil::isSymbol(UnicodeChar theChar)
{
    static_assert(sizeof(UnicodeChar) == 2, "UnicodeChar must be 2 byte length.");

    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        //Code: Cc, Cf, Z*, U000A ~ U000D, U0085, M*, P*, S* and illegal character set
        CFMutableCharacterSetRef characterSetRef = CFCharacterSetCreateMutable(CFAllocatorGetDefault());
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetControl));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetNonBase));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetPunctuation));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetSymbol));
        CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetIllegal));
        g_symbolCharacterSet = characterSetRef;
    });
    bool symbol = false;
    WCDB::Error::Code code = WCDB::Error::Code::NoMemory;
    if (g_symbolCharacterSet != nil) {
        symbol = CFCharacterSetIsCharacterMember(g_symbolCharacterSet, theChar);
        code = WCDB::Error::Code::OK;
    }
    return { WCDB::Error::c2rc(code), symbol };
}

WCDB::StringView WCTFTSTokenizerUtil::normalizeToken(WCDB::UnsafeStringView& token)
{
    NSMutableString* nsToken = [[NSMutableString alloc] initWithBytes:token.data() length:token.length() encoding:NSUTF8StringEncoding];
    CFMutableStringRef normalizationFormText = (__bridge_retained CFMutableStringRef)(nsToken);
    CFStringNormalize(normalizationFormText, kCFStringNormalizationFormKD);
    return WCDB::StringView((__bridge_transfer NSString*) normalizationFormText);
}

WCDB::StringViewMap<std::vector<WCDB::StringView>>* WCTFTSTokenizerUtil::g_pinyinDict = new WCDB::StringViewMap<std::vector<WCDB::StringView>>();

void WCTFTSTokenizerUtil::configPinyinDict(NSDictionary<NSString*, NSArray<NSString*>*>* pinyinDict)
{
    g_pinyinDict->clear();
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
            g_pinyinDict->insert_or_assign(key, value);
        }
    }
}

WCDB::StringViewMap<WCDB::StringView>* WCTFTSTokenizerUtil::g_traditionalChineseDict = new WCDB::StringViewMap<WCDB::StringView>();

void WCTFTSTokenizerUtil::configTraditionalChineseDict(NSDictionary<NSString*, NSString*>* traditionalChineseDict)
{
    g_traditionalChineseDict->clear();
    for (NSString* chinese in traditionalChineseDict.allKeys) {
        NSString* simplifiedChinese = traditionalChineseDict[chinese];
        if (chinese.UTF8String == nil || simplifiedChinese.UTF8String == nil) {
            continue;
        }
        g_traditionalChineseDict->insert_or_assign(WCDB::StringView(chinese.UTF8String), WCDB::StringView(simplifiedChinese.UTF8String));
    }
}

const std::vector<WCDB::StringView>* WCTFTSTokenizerUtil::getPinYin(const WCDB::UnsafeStringView chineseCharacter)
{
    auto iter = g_pinyinDict->find(chineseCharacter);
    if (iter != g_pinyinDict->end()) {
        return &iter->second;
    }
    return nullptr;
}

const WCDB::UnsafeStringView WCTFTSTokenizerUtil::getSimplifiedChinese(const WCDB::UnsafeStringView chineseCharacter)
{
    auto iter = g_traditionalChineseDict->find(chineseCharacter);
    if (iter != g_traditionalChineseDict->end()) {
        return iter->second;
    }
    return chineseCharacter;
}
