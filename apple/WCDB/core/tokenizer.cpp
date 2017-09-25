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

#include <WCDB/fts_module.hpp>
#include <WCDB/tokenizer.hpp>

namespace WCDB {

namespace FTS {

#pragma mark - Tokenizer
WCDBTokenizerInfo::WCDBTokenizerInfo(int argc, const char *const *argv)
    : TokenizerInfoBase(argc, argv)
{
}

WCDBTokenizerInfo::~WCDBTokenizerInfo()
{
}

WCDBCursorInfo::WCDBCursorInfo(const char *input,
                               int bytes,
                               TokenizerInfoBase *tokenizerInfo)
    : CursorInfoBase(input, bytes, tokenizerInfo)
    , m_input(input)
    , m_length(bytes)
    , m_position(0)
    , m_cursor(0)
    , m_buffer(nullptr)
    , m_bufferLength(0)
{
    setupToken();
    m_subTokensLengthArray.clear();
}

WCDBCursorInfo::~WCDBCursorInfo()
{
    if (m_buffer) {
        sqlite3_free(m_buffer);
    }
}

//Inspired by zorrozhang
int WCDBCursorInfo::step(const char **ppToken,
                         int *pnBytes,
                         int *piStartOffset,
                         int *piEndOffset,
                         int *piPosition)
{
    if (m_subTokensLengthArray.empty()) {
        if (m_currentTokenType == TokenType::None) {
            return SQLITE_DONE;
        }

        //Skip symbol
        while (m_currentTokenType == TokenType::BasicMultilingualPlaneSymbol) {
            if (!cursorStep()) {
                return SQLITE_DONE;
            }
        }

        TokenType type = m_currentTokenType;
        switch (type) {
            case TokenType::BasicMultilingualPlaneLetter:
            case TokenType::BasicMultilingualPlaneDigit:
                *piStartOffset = m_cursor;
                while (cursorStep() && m_currentTokenType == type)
                    ;
                *pnBytes = m_cursor - *piStartOffset;
                break;
            case TokenType::BasicMultilingualPlaneOther:
            case TokenType::AuxiliaryPlaneOther:
                m_subTokensLengthArray.push_back(m_currentTokenLength);
                m_subTokensCursor = m_cursor;
                m_subTokensDoubleChar = true;
                while (cursorStep() && m_currentTokenType == type) {
                    m_subTokensLengthArray.push_back(m_currentTokenLength);
                }
                subTokensStep(pnBytes, piStartOffset);
                break;
            default:
                break;
        }
        setBuffer(m_input + *piStartOffset, *pnBytes);
        if (type == TokenType::BasicMultilingualPlaneLetter) {
            for (int i = 0; i < *pnBytes; ++i) {
                m_buffer[i] = tolower(m_buffer[i]);
            }
        }
    } else {
        subTokensStep(pnBytes, piStartOffset);
        setBuffer(m_input + *piStartOffset, *pnBytes);
    }

    *ppToken = m_buffer;
    *piEndOffset = *piStartOffset + *pnBytes;
    *piPosition = m_position++;

    return SQLITE_OK;
}

bool WCDBCursorInfo::cursorStep()
{
    if (m_cursor + m_currentTokenLength < m_length) {
        m_cursor += m_currentTokenLength;
        setupToken();
        return 1;
    }
    m_cursor = m_length;
    m_currentTokenType = TokenType::None;
    m_currentTokenLength = 0;
    return 0;
}

void WCDBCursorInfo::setupToken()
{
    const unsigned char &first = m_input[m_cursor];
    if (first < 0xC0) {
        m_currentTokenLength = 1;
        if (first >= 0x30 && first <= 0x39) {
            m_currentTokenType = TokenType::BasicMultilingualPlaneDigit;
        } else if ((first >= 0x41 && first <= 0x5a) ||
                   (first >= 0x61 && first <= 0x7a)) {
            m_currentTokenType = TokenType::BasicMultilingualPlaneLetter;
        } else {
            if (isSymbol(first)) {
                m_currentTokenType = TokenType::BasicMultilingualPlaneSymbol;
            } else {
                m_currentTokenType = TokenType::BasicMultilingualPlaneOther;
            }
        }
    } else if (first < 0xF0) {
        UniChar unicode = 0;
        if (first < 0xE0) {
            m_currentTokenLength = 2;
            unicode = first & 0x1F;
        } else {
            m_currentTokenLength = 3;
            unicode = first & 0x0F;
        }
        for (int i = m_cursor + 1; i < m_cursor + m_currentTokenLength; ++i) {
            if (i < m_length) {
                unicode = (unicode << 6) | (m_input[i] & 0x3F);
            } else {
                m_currentTokenType = TokenType::None;
                m_currentTokenLength = m_length - i;
                return;
            }
        }
        if (isSymbol(unicode)) {
            m_currentTokenType = TokenType::BasicMultilingualPlaneSymbol;
        } else {
            m_currentTokenType = TokenType::BasicMultilingualPlaneOther;
        }
    } else {
        m_currentTokenType = TokenType::AuxiliaryPlaneOther;
        if (first < 0xF8) {
            m_currentTokenLength = 4;
        } else if (first < 0xFC) {
            m_currentTokenLength = 5;
        } else {
            m_currentTokenLength = 6;
        }
    }
}

bool WCDBCursorInfo::isSymbol(UniChar theChar)
{
#ifdef __APPLE__
    //Code: Cc, Cf, Z*, U000A ~ U000D, U0085, M*, P*, S* and illegal character set
    static CFCharacterSetRef s_symbolCharacterSet = []() {
        CFMutableCharacterSetRef symbolCharacterSet =
            CFCharacterSetCreateMutable(CFAllocatorGetDefault());
        CFCharacterSetUnion(symbolCharacterSet, CFCharacterSetGetPredefined(
                                                    kCFCharacterSetControl));
        CFCharacterSetUnion(
            symbolCharacterSet,
            CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline));
        CFCharacterSetUnion(symbolCharacterSet, CFCharacterSetGetPredefined(
                                                    kCFCharacterSetNonBase));
        CFCharacterSetUnion(
            symbolCharacterSet,
            CFCharacterSetGetPredefined(kCFCharacterSetPunctuation));
        CFCharacterSetUnion(symbolCharacterSet,
                            CFCharacterSetGetPredefined(kCFCharacterSetSymbol));
        CFCharacterSetUnion(symbolCharacterSet, CFCharacterSetGetPredefined(
                                                    kCFCharacterSetIllegal));
        return symbolCharacterSet;
    }();
    return CFCharacterSetIsCharacterMember(s_symbolCharacterSet, theChar);
#else //
#error You must figure out the unicode character set of [symbol] on current platform or implement it refer to http://www.fileformat.info/info/unicode/category/index.htm
#endif
}

void WCDBCursorInfo::subTokensStep(int *pnBytes, int *piStartOffset)
{
    *piStartOffset = m_subTokensCursor;
    *pnBytes = m_subTokensLengthArray[0];
    if (m_subTokensDoubleChar) {
        if (m_subTokensLengthArray.size() > 1) {
            *pnBytes += m_subTokensLengthArray[1];
            m_subTokensDoubleChar = false;
        } else {
            m_subTokensLengthArray.clear();
        }
    } else {
        m_subTokensCursor += m_subTokensLengthArray[0];
        m_subTokensLengthArray.erase(m_subTokensLengthArray.begin());
        m_subTokensDoubleChar = true;
    }
}

void WCDBCursorInfo::cutBufferLength(int newLength)
{
    if (newLength < m_bufferLength) {
        m_bufferLength = newLength;
    }
}
bool WCDBCursorInfo::setBuffer(const char *src, int length)
{
    if (length > m_bufferLength) {
        if (m_buffer) {
            sqlite3_free(m_buffer);
        }
        m_bufferLength = m_bufferLength ? m_bufferLength * 2 : length;
        m_buffer = (char *) sqlite3_malloc(m_bufferLength);
        if (!m_buffer) {
            m_bufferLength = 0;
            return false;
        }
    }
    memcpy(m_buffer, src, length);
    return true;
}

#pragma mark - Module
constexpr const char WCDBModule::Name[];

const std::nullptr_t WCDBModule::s_register = []() {
    FTS::Module<WCDBModule::Name, WCDBTokenizerInfo,
                WCDBCursorInfo>::Register();
    return nullptr;
}();

} //namespace FTS

} //namespace WCDB
