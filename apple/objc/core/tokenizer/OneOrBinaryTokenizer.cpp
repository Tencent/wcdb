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

#include <WCDB/Assertion.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/OneOrBinaryTokenizer.hpp>

extern "C" {
extern int porterStem(char *p, int i, int j);
}

namespace WCDB {

#pragma mark - Tokenizer Info
OneOrBinaryTokenizerInfo::OneOrBinaryTokenizerInfo(int argc, const char *const *argv)
: AbstractTokenizerInfo(argc, argv)
{
}

OneOrBinaryTokenizerInfo::~OneOrBinaryTokenizerInfo() = default;

#pragma mark - Tokenizer Cursor Info
OneOrBinaryTokenizerCursorInfo::OneOrBinaryTokenizerCursorInfo(const char *input,
                                                               int inputLength,
                                                               AbstractTokenizerInfo *tokenizerInfo)
: AbstractTokenizerCursorInfo(input, inputLength, tokenizerInfo)
, m_tokenizerInfo(static_cast<OneOrBinaryTokenizerInfo *>(tokenizerInfo))
, m_input(input)
, m_inputLength(inputLength)
, m_position(0)
, m_startOffset(0)
, m_endOffset(0)
, m_cursor(0)
, m_cursorTokenType(TokenType::None)
, m_cursorTokenLength(0)
, m_lemmaBufferLength(0)
, m_subTokensCursor(0)
, m_subTokensDoubleChar(true)
, m_bufferLength(0)
{
    static_assert(sizeof(UnicodeChar) == 2, "UnicodeChar must be 2 byte length.");

    if (m_input == nullptr) {
        m_inputLength = 0;
    }
    if (m_inputLength < 0) {
        m_inputLength = (int) strlen(m_input);
    }
}

OneOrBinaryTokenizerCursorInfo::~OneOrBinaryTokenizerCursorInfo() = default;

//Inspired by zorrozhang
int OneOrBinaryTokenizerCursorInfo::step(
const char **ppToken, int *pnBytes, int *piStartOffset, int *piEndOffset, int *piPosition)
{
    Error::Code code = Error::Code::OK;
    if (m_position == 0) {
        code = Error::rc2c(cursorSetup());
        if (code != Error::Code::OK) {
            return Error::c2rc(code);
        }
    }

    if (m_subTokensLengthArray.empty()) {
        if (m_cursorTokenType == TokenType::None) {
            return Error::c2rc(Error::Code::Done);
        }

        //Skip symbol
        while (m_cursorTokenType == TokenType::BasicMultilingualPlaneSymbol) {
            code = Error::rc2c(cursorStep());
            if (code != Error::Code::OK) {
                return Error::c2rc(code);
            }
        }

        if (m_cursorTokenType == TokenType::None) {
            return Error::c2rc(Error::Code::Done);
        }

        TokenType type = m_cursorTokenType;
        switch (type) {
        case TokenType::BasicMultilingualPlaneLetter:
        case TokenType::BasicMultilingualPlaneDigit:
            m_startOffset = m_cursor;
            while (((code = Error::rc2c(cursorStep())) == Error::Code::OK)
                   && m_cursorTokenType == type)
                ;
            if (code != Error::Code::OK) {
                return Error::c2rc(code);
            }
            m_endOffset = m_cursor;
            m_bufferLength = m_endOffset - m_startOffset;
            break;
        case TokenType::BasicMultilingualPlaneOther:
        case TokenType::AuxiliaryPlaneOther:
            m_subTokensLengthArray.push_back(m_cursorTokenLength);
            m_subTokensCursor = m_cursor;
            m_subTokensDoubleChar = true;
            while (((code = Error::rc2c(cursorStep())) == Error::Code::OK)
                   && m_cursorTokenType == type) {
                m_subTokensLengthArray.push_back(m_cursorTokenLength);
            }
            if (code != Error::Code::OK) {
                return Error::c2rc(code);
            }
            subTokensStep();
            break;
        default:
            break;
        }
        if (type == TokenType::BasicMultilingualPlaneLetter) {
            code = Error::rc2c(lemmatization(m_input + m_startOffset, m_bufferLength));
            if (code != Error::Code::OK) {
                return Error::c2rc(code);
            }
        } else {
            m_buffer.assign(m_input + m_startOffset, m_input + m_startOffset + m_bufferLength);
        }
    } else {
        subTokensStep();
        m_buffer.assign(m_input + m_startOffset, m_input + m_startOffset + m_bufferLength);
    }

    if (m_lemmaBufferLength == 0) {
        *ppToken = m_buffer.data();
        *pnBytes = m_bufferLength;
    } else {
        *ppToken = m_lemmaBuffer.data();
        *pnBytes = m_lemmaBufferLength;
        m_lemmaBufferLength = 0;
    }

    *piStartOffset = m_startOffset;
    *piEndOffset = m_endOffset;
    *piPosition = m_position++;

    return Error::c2rc(Error::Code::OK);
}

int OneOrBinaryTokenizerCursorInfo::cursorStep()
{
    if (m_cursor + m_cursorTokenLength < m_inputLength) {
        m_cursor += m_cursorTokenLength;
        return cursorSetup();
    }
    m_cursor = m_inputLength;
    m_cursorTokenType = TokenType::None;
    m_cursorTokenLength = 0;
    return Error::c2rc(Error::Code::OK);
}

int OneOrBinaryTokenizerCursorInfo::cursorSetup()
{
    Error::Code code;
    const unsigned char &first = m_input[m_cursor];
    if (first < 0xC0) {
        m_cursorTokenLength = 1;
        if (first >= 0x30 && first <= 0x39) {
            m_cursorTokenType = TokenType::BasicMultilingualPlaneDigit;
        } else if ((first >= 0x41 && first <= 0x5a) || (first >= 0x61 && first <= 0x7a)) {
            m_cursorTokenType = TokenType::BasicMultilingualPlaneLetter;
        } else {
            int rc;
            bool symbol = false;
            std::tie(rc, symbol) = isSymbol(first);
            code = Error::rc2c(rc);
            if (code == Error::Code::OK) {
                if (symbol) {
                    m_cursorTokenType = TokenType::BasicMultilingualPlaneSymbol;
                } else {
                    m_cursorTokenType = TokenType::BasicMultilingualPlaneOther;
                }
            } else {
                return Error::c2rc(code);
            }
        }
    } else if (first < 0xF0) {
        UnicodeChar unicode = 0;
        if (first < 0xE0) {
            m_cursorTokenLength = 2;
            unicode = first & 0x1F;
        } else {
            m_cursorTokenLength = 3;
            unicode = first & 0x0F;
        }
        for (int i = m_cursor + 1; i < m_cursor + m_cursorTokenLength; ++i) {
            if (i < m_inputLength) {
                WCTAssert(((unicode << 6) >> 6) == unicode);
                unicode = (UnicodeChar)(((int) unicode << 6) | (m_input[i] & 0x3F));
            } else {
                m_cursorTokenType = TokenType::None;
                m_cursorTokenLength = m_inputLength - i;
                return Error::c2rc(Error::Code::OK);
            }
        }
        int rc;
        bool symbol = false;
        std::tie(rc, symbol) = isSymbol(unicode);
        code = Error::rc2c(rc);
        if (code == Error::Code::OK) {
            if (symbol) {
                m_cursorTokenType = TokenType::BasicMultilingualPlaneSymbol;
            } else {
                m_cursorTokenType = TokenType::BasicMultilingualPlaneOther;
            }
        } else {
            return Error::c2rc(code);
        }
    } else {
        m_cursorTokenType = TokenType::AuxiliaryPlaneOther;
        if (first < 0xF8) {
            m_cursorTokenLength = 4;
        } else if (first < 0xFC) {
            m_cursorTokenLength = 5;
        } else {
            m_cursorTokenLength = 6;
        }
    }
    return Error::c2rc(Error::Code::OK);
}

int OneOrBinaryTokenizerCursorInfo::lemmatization(const char *input, int inputLength)
{
    // tolower only. You can implement your own lemmatization.
    m_buffer.assign(input, input + inputLength);
    std::transform(m_buffer.begin(), m_buffer.end(), m_buffer.begin(), ::tolower);
    m_bufferLength = porterStem(m_buffer.data(), 0, m_bufferLength - 1) + 1;
    return Error::c2rc(Error::Code::OK);
}

void OneOrBinaryTokenizerCursorInfo::subTokensStep()
{
    m_startOffset = m_subTokensCursor;
    m_bufferLength = m_subTokensLengthArray[0];
    if (m_subTokensDoubleChar) {
        if (m_subTokensLengthArray.size() > 1) {
            m_bufferLength += m_subTokensLengthArray[1];
            m_subTokensDoubleChar = false;
        } else {
            m_subTokensLengthArray.clear();
        }
    } else {
        m_subTokensCursor += m_subTokensLengthArray[0];
        m_subTokensLengthArray.erase(m_subTokensLengthArray.begin());
        m_subTokensDoubleChar = true;
    }
    m_endOffset = m_startOffset + m_bufferLength;
}

} // namespace WCDB
