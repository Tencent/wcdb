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

#ifndef __WCDB_ONE_OR_BINARY_TOKENIZER_HPP
#define __WCDB_ONE_OR_BINARY_TOKENIZER_HPP

#include <WCDB/TokenizerModuleTemplate.hpp>
#include <vector>

namespace WCDB {

class OneOrBinaryTokenizerInfo final : public AbstractTokenizerInfo {
public:
    OneOrBinaryTokenizerInfo(int argc, const char *const *argv);
    ~OneOrBinaryTokenizerInfo();
};

class OneOrBinaryTokenizerCursorInfo : public AbstractTokenizerCursorInfo {
public:
    OneOrBinaryTokenizerCursorInfo(const char *input,
                                   int inputLength,
                                   AbstractTokenizerInfo *tokenizerInfo);
    virtual ~OneOrBinaryTokenizerCursorInfo() = 0;

    int step(const char **ppToken,
             int *pnBytes,
             int *piStartOffset,
             int *piEndOffset,
             int *piPosition) override final;

protected:
    //You must figure out the unicode character set of [symbol] on current platform or implement it refer to http://www.fileformat.info/info/unicode/category/index.htm
    typedef unsigned short UnicodeChar;
    virtual std::pair<int, bool> isSymbol(UnicodeChar theChar) = 0;

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
    TokenType m_cursorTokenType;
    int m_cursorTokenLength;
    int cursorStep();
    int cursorSetup();

    int lemmatization(const char *input, int inputLength);
    std::vector<char> m_lemmaBuffer;
    int m_lemmaBufferLength; //>0 lemma is not empty

    std::vector<int> m_subTokensLengthArray;
    int m_subTokensCursor;
    bool m_subTokensDoubleChar;
    void subTokensStep();

    std::vector<char> m_buffer;
    int m_bufferLength;

    OneOrBinaryTokenizerInfo *m_tokenizerInfo;
};

} // namespace WCDB

#endif /* __WCDB_ONE_OR_BINARY_TOKENIZER_HPP */
