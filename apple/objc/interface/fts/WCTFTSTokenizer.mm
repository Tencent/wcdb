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
#import <WCDB/Error.hpp>
#import <WCDB/WCTFTSTokenizer.h>

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

WCTOneOrBinaryTokenizer::WCTOneOrBinaryTokenizer(const char* input,
                                                 int inputLength,
                                                 WCDB::AbstractFTS3TokenizerInfo* tokenizerInfo)
: WCDB::OneOrBinaryTokenizer(input, inputLength, tokenizerInfo)
, m_symbolCharacterSet(generateSymbolCharacterSet())
{
}

WCTOneOrBinaryTokenizer::WCTOneOrBinaryTokenizer(void* pCtx, const char** azArg, int nArg)
: WCDB::OneOrBinaryTokenizer(pCtx, azArg, nArg)
, m_symbolCharacterSet(generateSymbolCharacterSet())
{
}

WCTOneOrBinaryTokenizer::WCTOneOrBinaryTokenizer(const WCTOneOrBinaryTokenizer& other)
: WCDB::OneOrBinaryTokenizer(other)
, m_symbolCharacterSet(other.m_symbolCharacterSet)
{
    if (m_symbolCharacterSet != nil) {
        CFRetain(m_symbolCharacterSet);
    }
}

WCTOneOrBinaryTokenizer& WCTOneOrBinaryTokenizer::operator=(const WCTOneOrBinaryTokenizer& other)
{
    WCDB::OneOrBinaryTokenizer::operator=(other);
    m_symbolCharacterSet = other.m_symbolCharacterSet;
    if (m_symbolCharacterSet != nil) {
        CFRetain(m_symbolCharacterSet);
    }
    return *this;
}

WCTOneOrBinaryTokenizer::~WCTOneOrBinaryTokenizer()
{
    if (m_symbolCharacterSet != nil) {
        CFRelease(m_symbolCharacterSet);
        m_symbolCharacterSet = nil;
    }
}

CFCharacterSetRef WCTOneOrBinaryTokenizer::generateSymbolCharacterSet()
{
    //Code: Cc, Cf, Z*, U000A ~ U000D, U0085, M*, P*, S* and illegal character set
    CFMutableCharacterSetRef characterSetRef = CFCharacterSetCreateMutable(CFAllocatorGetDefault());
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetControl));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetNonBase));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetPunctuation));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetSymbol));
    CFCharacterSetUnion(characterSetRef, CFCharacterSetGetPredefined(kCFCharacterSetIllegal));
    return characterSetRef;
}

std::pair<int, bool> WCTOneOrBinaryTokenizer::isSymbol(UnicodeChar theChar) const
{
    bool symbol = false;
    WCDB::Error::Code code = WCDB::Error::Code::NoMemory;
    if (m_symbolCharacterSet != nil) {
        symbol = CFCharacterSetIsCharacterMember(m_symbolCharacterSet, theChar);
        code = WCDB::Error::Code::OK;
    }
    return { WCDB::Error::c2rc(code), symbol };
}

WCDB::StringView WCTOneOrBinaryTokenizer::normalizeToken(WCDB::UnsafeStringView& token) const
{
    NSMutableString* nsToken = [[NSMutableString alloc] initWithBytes:token.data() length:token.length() encoding:NSUTF8StringEncoding];
    CFMutableStringRef normalizationFormText = (__bridge_retained CFMutableStringRef)(nsToken);
    CFStringNormalize(normalizationFormText, kCFStringNormalizationFormKD);
    return WCDB::StringView((__bridge_transfer NSString*) normalizationFormText);
}

WCDB::StringViewMap<std::vector<WCDB::StringView>>* WCTOneOrBinaryTokenizer::m_pinyinDict = new WCDB::StringViewMap<std::vector<WCDB::StringView>>();

void WCTOneOrBinaryTokenizer::configPinyinDict(NSDictionary<NSString*, NSArray<NSString*>*>* pinyinDict)
{
    m_pinyinDict->clear();
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
            m_pinyinDict->insert_or_assign(key, value);
        }
    }
}

WCDB::StringViewMap<WCDB::StringView>* WCTOneOrBinaryTokenizer::m_traditionalChineseDict = new WCDB::StringViewMap<WCDB::StringView>();

void WCTOneOrBinaryTokenizer::configTraditionalChineseDict(NSDictionary<NSString*, NSString*>* traditionalChineseDict)
{
    m_traditionalChineseDict->clear();
    for (NSString* chinese in traditionalChineseDict.allKeys) {
        NSString* simplifiedChinese = traditionalChineseDict[chinese];
        if (chinese.UTF8String == nil || simplifiedChinese.UTF8String == nil) {
            continue;
        }
        m_traditionalChineseDict->insert_or_assign(WCDB::StringView(chinese.UTF8String), WCDB::StringView(simplifiedChinese.UTF8String));
    }
}

const std::vector<WCDB::StringView>* WCTOneOrBinaryTokenizer::getPinYin(const WCDB::UnsafeStringView chineseCharacter) const
{
    auto iter = m_pinyinDict->find(chineseCharacter);
    if (iter != m_pinyinDict->end()) {
        return &iter->second;
    }
    return nullptr;
}

const WCDB::UnsafeStringView WCTOneOrBinaryTokenizer::getSimplifiedChinese(const WCDB::UnsafeStringView chineseCharacter) const
{
    auto iter = m_traditionalChineseDict->find(chineseCharacter);
    if (iter != m_traditionalChineseDict->end()) {
        return iter->second;
    }
    return chineseCharacter;
}
