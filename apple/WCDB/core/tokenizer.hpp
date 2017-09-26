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

#ifndef tokenizer_hpp
#define tokenizer_hpp

#include <WCDB/fts_module.hpp>
#include <cstddef>
#include <vector>

namespace WCDB {

namespace FTS {

typedef unsigned short UnicodeChar;
static_assert(sizeof(UnicodeChar) == 2, "UnicodeChar must be 2 byte length");

#pragma mark - Cursor
class WCDBCursorInfo : public CursorInfoBase {
public:
    enum class TokenType : unsigned int {
        None = 0,
        BasicMultilingualPlaneLetter = 0x00000001,
        BasicMultilingualPlaneDigit = 0x00000002,
        BasicMultilingualPlaneSymbol = 0x00000003,
        BasicMultilingualPlaneOther = 0x0000FFFF,
        AuxiliaryPlaneOther = 0xFFFFFFFF,
    };

    WCDBCursorInfo(const char *input,
                   int bytes,
                   TokenizerInfoBase *tokenizerInfo);
    ~WCDBCursorInfo();

    virtual int step(const char **ppToken,
                     int *pnBytes,
                     int *piStartOffset,
                     int *piEndOffset,
                     int *piPosition) override;

protected:
    const char *m_input;
    int m_length;

    int m_position;
    int m_cursor;
    TokenType m_currentTokenType;
    int m_currentTokenLength;
    int cursorStep();
    int setupToken();

    //You must figure out the unicode character set of [symbol] on current platform or implement it refer to http://www.fileformat.info/info/unicode/category/index.htm
    virtual int isSymbol(UnicodeChar theChar, bool *result) = 0;
    virtual int lemmatization(const char *input, int bytes);
    int m_nonLemmaStartOffset;
    int m_nonLemmaLength;
    char *m_lemmaBuffer;
    int m_lemmaBufferCapacity;
    int m_lemmaLength; //>0 lemma is not empty
    int setLemmaBuffer(const char *src, int length);

    std::vector<int> m_subTokensLengthArray;
    int m_subTokensCursor;
    bool m_subTokensDoubleChar;
    void subTokensStep(int *pnBytes, int *piStartOffset);

    char *m_buffer;
    int m_bufferCapacity;
    int setBufferCapacity(int newCapacity);
    int setBuffer(const char *src, int length);
};

} //namespace FTS

} //namespace WCDB

#endif /* tokenizer_hpp */
