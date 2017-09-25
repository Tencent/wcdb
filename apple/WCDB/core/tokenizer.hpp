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
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#else
typedef UInt16 UniChar;
#endif //__APPLE__

namespace WCDB {

namespace FTS {

#pragma mark - Tokenizer
class WCDBTokenizerInfo : public TokenizerInfoBase {
public:
    WCDBTokenizerInfo(int argc, const char *const *argv);
    ~WCDBTokenizerInfo();
};

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
    bool cursorStep();
    void setupToken();
    bool isSymbol(UniChar theChar);

    std::vector<int> m_subTokensLengthArray;
    int m_subTokensCursor;
    bool m_subTokensDoubleChar;
    void subTokensStep(int *pnBytes, int *piStartOffset);

    char *m_buffer;
    int m_bufferLength;
    void cutBufferLength(int newLength);
    bool setBuffer(const char *src, int length);
};

#pragma mark - Module
class WCDBModule {
public:
    constexpr static const char Name[] = "WCDB";

private:
    static const std::nullptr_t s_register;
};

} //namespace FTS

} //namespace WCDB

#endif /* tokenizer_hpp */
