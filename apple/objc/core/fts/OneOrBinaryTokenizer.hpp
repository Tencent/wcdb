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

#pragma once

#include <WCDB/TokenizerModuleTemplate.hpp>
#include <vector>

namespace WCDB {

class OneOrBinaryTokenizerInfo final : public AbstractFTS3TokenizerInfo {
public:
    OneOrBinaryTokenizerInfo(int argc, const char *const *argv);
    ~OneOrBinaryTokenizerInfo() override final;
    bool m_needBinary;
    bool m_needSymbol;
    bool m_needSimplifiedChinese;
};

class OneOrBinaryTokenizer : public AbstractFTS3TokenizerCursorInfo, public AbstractFTS5Tokenizer {
public:
    //for fts3
    OneOrBinaryTokenizer(const char *input, int inputLength, AbstractFTS3TokenizerInfo *tokenizerInfo);
    virtual ~OneOrBinaryTokenizer() override = 0;
    int step(const char **ppToken,
             int *pnBytes,
             int *piStartOffset,
             int *piEndOffset,
             int *piPosition) override final;

    //for fts5
    OneOrBinaryTokenizer(void *pCtx, const char **azArg, int nArg);
    virtual void loadInput(int flags, const char *pText, int nText) override;
    virtual int
    nextToken(int *tflags, const char **ppToken, int *nToken, int *iStart, int *iEnd) override;

    int stepNextToken();

protected:
    //You must figure out the unicode character set of [symbol] on current platform or implement it refer to http://www.fileformat.info/info/unicode/category/index.htm
    typedef unsigned short UnicodeChar;
    virtual std::pair<int, bool> isSymbol(UnicodeChar theChar) const = 0;
    virtual StringView normalizeToken(UnsafeStringView &token) const = 0;
    virtual const std::vector<StringView> *
    getPinYin(const UnsafeStringView chineseCharacter) const = 0;
    virtual const UnsafeStringView
    getSimplifiedChinese(const UnsafeStringView chineseCharacter) const = 0;

private:
    const char *m_input;
    int m_inputLength;

    int m_position;
    int m_startOffset;
    int m_endOffset;

    enum class TokenType : unsigned int {
        None = 0,
        BasicMultilingualPlaneLetter = 0x00000001,
        BasicMultilingualPlaneDigit = 0x00000002,
        BasicMultilingualPlaneSymbol = 0x00000003,
        BasicMultilingualPlaneOther = 0x0000FFFF,
        AuxiliaryPlaneOther = 0xFFFFFFFF,
    };

    int m_cursor;
    int m_cursorTokenLength;
    TokenType m_cursorTokenType;
    TokenType m_preTokenType;

    std::vector<int> m_subTokensLengthArray;
    int m_subTokensCursor;
    bool m_subTokensDoubleChar;

    std::vector<char> m_normalToken;
    int m_normalTokenLength;
    std::set<StringView> m_pinyinTokens;
    int m_pinyinTokenIndex;

    // Can be configed by tokenizer parameters
    bool m_needBinary;
    bool m_ispinyin; //only for fts5
    bool m_needSymbol;
    bool m_needSimplifiedChinese;

    int cursorStep();
    int cursorSetup();
    void subTokensStep();

    int lemmatization(const char *input, int inputLength);
    int genNormalToken();
    void genPinyinToken();
};

} // namespace WCDB
