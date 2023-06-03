//
// Created by qiuwenchen on 2022/10/27.
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

#include "BaseTokenizerUtil.hpp"
#include "Assertion.hpp"
#include "FTSError.hpp"

namespace WCDB {

#pragma mark - Token Classify

void BaseTokenizerUtil::stepOneUnicode(const UnsafeStringView input,
                                       UnicodeType& unicodeType,
                                       int& unicodeLength)
{
    if (input.length() == 0) {
        unicodeType = UnicodeType::None;
        unicodeLength = 0;
        return;
    }

    const unsigned char& first = input.at(0);
    if (first < 0xC0) {
        unicodeLength = 1;
        if (first >= 0x30 && first <= 0x39) {
            unicodeType = UnicodeType::BasicMultilingualPlaneDigit;
        } else if ((first >= 0x41 && first <= 0x5a) || (first >= 0x61 && first <= 0x7a)) {
            unicodeType = UnicodeType::BasicMultilingualPlaneLetter;
        } else {
            if (isSymbol(first)) {
                unicodeType = UnicodeType::BasicMultilingualPlaneSymbol;
            } else {
                unicodeType = UnicodeType::BasicMultilingualPlaneOther;
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
        if (isSymbol(unicode)) {
            unicodeType = UnicodeType::BasicMultilingualPlaneSymbol;
        } else {
            unicodeType = UnicodeType::BasicMultilingualPlaneOther;
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
}

#pragma mark - Symbol Detect

bool BaseTokenizerUtil::isSymbol(UnicodeChar theChar)
{
    if (getSymbolDetector() == nullptr) {
        return false;
    }
    return getSymbolDetector()(theChar);
}

void BaseTokenizerUtil::configSymbolDetector(SymbolDetector detector)
{
    getSymbolDetector() = detector;
}

BaseTokenizerUtil::SymbolDetector& BaseTokenizerUtil::getSymbolDetector()
{
    static SymbolDetector& g_detector = *new SymbolDetector();
    return g_detector;
}

#pragma mark - Unicode Normalize

StringView BaseTokenizerUtil::normalizeToken(UnsafeStringView& token)
{
    if (getUnicodeNormalizer() == nullptr) {
        return StringView(token);
    }
    return getUnicodeNormalizer()(token);
}

void BaseTokenizerUtil::configUnicodeNormalizer(UnicodeNormalizer normalizer)
{
    getUnicodeNormalizer() = normalizer;
}

BaseTokenizerUtil::UnicodeNormalizer& BaseTokenizerUtil::getUnicodeNormalizer()
{
    static UnicodeNormalizer& g_normalizer = *new UnicodeNormalizer();
    return g_normalizer;
}

#pragma mark - Pinyin

const std::vector<StringView>
BaseTokenizerUtil::getPinYin(const UnsafeStringView& chineseCharacter)
{
    WCTAssert(g_pinyinDict != nullptr || getPinyinConverter() != nullptr);
    if (g_pinyinDict != nullptr) {
        auto iter = g_pinyinDict->find(chineseCharacter);
        if (iter != g_pinyinDict->end()) {
            return iter->second;
        }
    } else if (getPinyinConverter() != nullptr) {
        return getPinyinConverter()(chineseCharacter);
    }
    return std::vector<StringView>();
}

WCDB::StringViewMap<std::vector<WCDB::StringView>>* BaseTokenizerUtil::g_pinyinDict = nullptr;
void BaseTokenizerUtil::configPinyinDict(WCDB::StringViewMap<std::vector<WCDB::StringView>>* dict)
{
    if (g_pinyinDict != nullptr) {
        delete g_pinyinDict;
        g_pinyinDict = nullptr;
    }
    g_pinyinDict = dict;
}

void BaseTokenizerUtil::configPinyinConverter(PinYinConverter converter)
{
    if (g_pinyinDict != nullptr) {
        delete g_pinyinDict;
        g_pinyinDict = nullptr;
    }
    getPinyinConverter() = converter;
}

BaseTokenizerUtil::PinYinConverter& BaseTokenizerUtil::getPinyinConverter()
{
    static PinYinConverter& converter = *new PinYinConverter();
    return converter;
}

#pragma mark - Traditional Chinese

const StringView BaseTokenizerUtil::getSimplifiedChinese(const UnsafeStringView& chineseCharacter)
{
    WCTAssert(g_traditionalChineseDict != nullptr
              || getTraditionalChineseConverter() != nullptr);
    if (g_traditionalChineseDict != nullptr) {
        auto iter = g_traditionalChineseDict->find(chineseCharacter);
        if (iter != g_traditionalChineseDict->end() && iter->second.length() > 0) {
            return iter->second;
        }
    } else if (getTraditionalChineseConverter() != nullptr) {
        const StringView traditionalChinese
        = getTraditionalChineseConverter()(chineseCharacter);
        if (traditionalChinese.length() > 0) {
            return traditionalChinese;
        }
    }
    return StringView(chineseCharacter);
}

WCDB::StringViewMap<WCDB::StringView>* BaseTokenizerUtil::g_traditionalChineseDict = nullptr;
void BaseTokenizerUtil::configTraditionalChineseDict(WCDB::StringViewMap<WCDB::StringView>* dict)
{
    if (g_traditionalChineseDict != nullptr) {
        delete g_traditionalChineseDict;
        g_traditionalChineseDict = nullptr;
    }
    g_traditionalChineseDict = dict;
}

void BaseTokenizerUtil::configTraditionalChineseConverter(TraditionalChineseConverter converter)
{
    if (g_traditionalChineseDict != nullptr) {
        delete g_traditionalChineseDict;
        g_traditionalChineseDict = nullptr;
    }
    getTraditionalChineseConverter() = converter;
}

BaseTokenizerUtil::TraditionalChineseConverter&
BaseTokenizerUtil::getTraditionalChineseConverter()
{
    static TraditionalChineseConverter& converter = *new TraditionalChineseConverter();
    return converter;
}

} //namespace WCDB
