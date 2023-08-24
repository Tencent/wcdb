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

#pragma once

#include "StringView.hpp"
#include <functional>
#include <vector>

namespace WCDB {

class WCDB_API BaseTokenizerUtil {
public:
    enum class UnicodeType : unsigned int {
        None = 0,
        BasicMultilingualPlaneLetter = 0x00000001,
        BasicMultilingualPlaneDigit = 0x00000002,
        BasicMultilingualPlaneSymbol = 0x00000003,
        BasicMultilingualPlaneOther = 0x0000FFFF,
        AuxiliaryPlaneOther = 0xFFFFFFFF,
    };
    static void
    stepOneUnicode(const UnsafeStringView input, UnicodeType& unicodeType, int& unicodeLength);

    typedef unsigned short UnicodeChar;
    typedef std::function<bool(UnicodeChar)> SymbolDetector;
    static void configSymbolDetector(SymbolDetector detector);
    static bool isSymbol(UnicodeChar theChar);

    typedef std::function<StringView(const UnsafeStringView&)> UnicodeNormalizer;
    static void configUnicodeNormalizer(UnicodeNormalizer normalizer);
    static StringView normalizeToken(UnsafeStringView& token);

    static const std::vector<StringView> getPinYin(const UnsafeStringView& chineseCharacter);
    typedef std::function<std::vector<StringView>(const UnsafeStringView&)> PinYinConverter;
    static void configPinyinConverter(PinYinConverter converter);
    static void
    configPinyinDict(WCDB::StringViewMap<std::vector<WCDB::StringView>>* dict);

    static const StringView getSimplifiedChinese(const UnsafeStringView& chineseCharacter);
    typedef std::function<const StringView(const UnsafeStringView&)> TraditionalChineseConverter;
    static void configTraditionalChineseConverter(TraditionalChineseConverter converter);
    static void configTraditionalChineseDict(WCDB::StringViewMap<WCDB::StringView>* dict);

private:
    static PinYinConverter& getPinyinConverter();
    static WCDB::StringViewMap<std::vector<WCDB::StringView>>* g_pinyinDict;

    static SymbolDetector& getSymbolDetector();
    static UnicodeNormalizer& getUnicodeNormalizer();
    static TraditionalChineseConverter& getTraditionalChineseConverter();
    static WCDB::StringViewMap<WCDB::StringView>* g_traditionalChineseDict;
};

} //namespace WCDB
