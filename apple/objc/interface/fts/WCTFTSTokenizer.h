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
#import <WCDB/OneOrBinaryTokenizer.hpp>
#import <WCDB/StringView.hpp>
#import <WCDB/WCTCommon.h>
#include <vector>

class WCTFTSTokenizerUtil {
public:
    //Parameters should end with nullptr
    static WCDB::StringView tokenize(NSString* name, ...);
};

class WCTOneOrBinaryTokenizer final : public WCDB::OneOrBinaryTokenizer {
public:
    WCTOneOrBinaryTokenizer(const char* input,
                            int inputLength,
                            WCDB::AbstractFTS3TokenizerInfo* tokenizerInfo);
    WCTOneOrBinaryTokenizer(void* pCtx, const char** azArg, int nArg);
    WCTOneOrBinaryTokenizer(const WCTOneOrBinaryTokenizer& other);

    WCTOneOrBinaryTokenizer& operator=(const WCTOneOrBinaryTokenizer& other);

    static void configPinyinDict(NSDictionary<NSString*, NSArray<NSString*>*>* pinyinDict);
    static void configTraditionalChineseDict(NSDictionary<NSString*, NSString*>* traditionalChineseDict);

    ~WCTOneOrBinaryTokenizer() override final;

protected:
    CFCharacterSetRef m_symbolCharacterSet;
    static WCDB::StringViewMap<std::vector<WCDB::StringView>>* m_pinyinDict;
    static WCDB::StringViewMap<WCDB::StringView>* m_traditionalChineseDict;
    static CFCharacterSetRef generateSymbolCharacterSet();

    std::pair<int, bool> isSymbol(UnicodeChar theChar) const override final;
    WCDB::StringView normalizeToken(WCDB::UnsafeStringView& token) const override final;
    const std::vector<WCDB::StringView>* getPinYin(const WCDB::UnsafeStringView chineseCharacter) const override final;
    const WCDB::UnsafeStringView
    getSimplifiedChinese(const WCDB::UnsafeStringView chineseCharacter) const override final;
};
