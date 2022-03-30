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

#import <CoreFoundation/CoreFoundation.h>
#import <WCDB/StringView.hpp>
#import <WCDB/WCTCommon.h>
#import <vector>

class WCTFTSTokenizerUtil {
public:
    //Parameters should end with nullptr
    static WCDB::StringView tokenize(NSString* name, ...);

    enum class UnicodeType : unsigned int {
        None = 0,
        BasicMultilingualPlaneLetter = 0x00000001,
        BasicMultilingualPlaneDigit = 0x00000002,
        BasicMultilingualPlaneSymbol = 0x00000003,
        BasicMultilingualPlaneOther = 0x0000FFFF,
        AuxiliaryPlaneOther = 0xFFFFFFFF,
    };
    static int stepOneUnicode(const WCDB::UnsafeStringView input, UnicodeType& unicodeType, int& unicodeLength);

    //Refer to http://www.fileformat.info/info/unicode/category/index.htm
    typedef unsigned short UnicodeChar;
    static std::pair<int, bool> isSymbol(UnicodeChar theChar);

    static WCDB::StringView normalizeToken(WCDB::UnsafeStringView& token);

    static void configPinyinDict(NSDictionary<NSString*, NSArray<NSString*>*>* pinyinDict);
    static const std::vector<WCDB::StringView>* getPinYin(const WCDB::UnsafeStringView chineseCharacter);

    static void configTraditionalChineseDict(NSDictionary<NSString*, NSString*>* traditionalChineseDict);
    static const WCDB::UnsafeStringView getSimplifiedChinese(const WCDB::UnsafeStringView chineseCharacter);

private:
    static CFCharacterSetRef g_symbolCharacterSet;
    static WCDB::StringViewMap<std::vector<WCDB::StringView>>* g_pinyinDict;
    static WCDB::StringViewMap<WCDB::StringView>* g_traditionalChineseDict;
};
