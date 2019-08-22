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

#pragma mark - Cursor
WCDBCursorInfo::WCDBCursorInfo(const char *input,
                               int inputLength,
                               TokenizerInfoBase *tokenizerInfo)
    : CursorInfoBase(input, inputLength, tokenizerInfo)
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
}

//Inspired by zorrozhang
int WCDBCursorInfo::step(const char **ppToken,
                         int *pnBytes,
                         int *piStartOffset,
                         int *piEndOffset,
                         int *piPosition)
{
    int rc = SQLITE_OK;
    if (m_position == 0) {
        rc = cursorSetup();
        if (rc != SQLITE_OK) {
            return rc;
        }
    }

    if (m_lemmaBufferLength == 0) {
        if (m_subTokensLengthArray.empty()) {
            if (m_cursorTokenType == TokenType::None) {
                return SQLITE_DONE;
            }

            //Skip symbol
            while (m_cursorTokenType ==
                   TokenType::BasicMultilingualPlaneSymbol) {
                rc = cursorStep();
                if (rc != SQLITE_OK) {
                    return rc;
                }
            }

            if (m_cursorTokenType == TokenType::None) {
                return SQLITE_DONE;
            }

            TokenType type = m_cursorTokenType;
            switch (type) {
                case TokenType::BasicMultilingualPlaneLetter:
                case TokenType::BasicMultilingualPlaneDigit:
                    m_startOffset = m_cursor;
                    while (((rc = cursorStep()) == SQLITE_OK) &&
                           m_cursorTokenType == type)
                        ;
                    if (rc != SQLITE_OK) {
                        return rc;
                    }
                    m_endOffset = m_cursor;
                    m_bufferLength = m_endOffset - m_startOffset;
                    break;
                case TokenType::BasicMultilingualPlaneOther:
                case TokenType::AuxiliaryPlaneOther:
                    m_subTokensLengthArray.push_back(m_cursorTokenLength);
                    m_subTokensCursor = m_cursor;
                    m_subTokensDoubleChar = true;
                    while (((rc = cursorStep()) == SQLITE_OK) &&
                           m_cursorTokenType == type) {
                        m_subTokensLengthArray.push_back(m_cursorTokenLength);
                    }
                    if (rc != SQLITE_OK) {
                        return rc;
                    }
                    subTokensStep();
                    break;
                default:
                    break;
            }
            if (type == TokenType::BasicMultilingualPlaneLetter) {
                rc = lemmatization(m_input + m_startOffset, m_bufferLength);
                if (rc != SQLITE_OK) {
                    return rc;
                }
            } else {
                if (m_bufferLength > m_buffer.capacity()) {
                    m_buffer.resize(m_bufferLength);
                }
                memcpy(m_buffer.data(), m_input + m_startOffset,
                       m_bufferLength);
            }
        } else {
            subTokensStep();
            if (m_bufferLength > m_buffer.capacity()) {
                m_buffer.resize(m_bufferLength);
            }
            memcpy(m_buffer.data(), m_input + m_startOffset, m_bufferLength);
        }

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

    return SQLITE_OK;
}

int WCDBCursorInfo::cursorStep()
{
    if (m_cursor + m_cursorTokenLength < m_inputLength) {
        m_cursor += m_cursorTokenLength;
        return cursorSetup();
    }
    m_cursor = m_inputLength;
    m_cursorTokenType = TokenType::None;
    m_cursorTokenLength = 0;
    return SQLITE_OK;
}

int WCDBCursorInfo::cursorSetup()
{
    int rc;
    const unsigned char &first = m_input[m_cursor];
    if (first < 0xC0) {
        m_cursorTokenLength = 1;
        if (first >= 0x30 && first <= 0x39) {
            m_cursorTokenType = TokenType::BasicMultilingualPlaneDigit;
        } else if ((first >= 0x41 && first <= 0x5a) ||
                   (first >= 0x61 && first <= 0x7a)) {
            m_cursorTokenType = TokenType::BasicMultilingualPlaneLetter;
        } else {
            bool result = false;
            rc = isSymbol(first, &result);
            if (rc == SQLITE_OK) {
                if (result) {
                    m_cursorTokenType = TokenType::BasicMultilingualPlaneSymbol;
                } else {
                    m_cursorTokenType = TokenType::BasicMultilingualPlaneOther;
                }
            } else {
                return rc;
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
                unicode = (unicode << 6) | (m_input[i] & 0x3F);
            } else {
                m_cursorTokenType = TokenType::None;
                m_cursorTokenLength = m_inputLength - i;
                return SQLITE_OK;
            }
        }
        bool result = false;
        rc = isSymbol(unicode, &result);
        if (rc == SQLITE_OK) {
            if (result) {
                m_cursorTokenType = TokenType::BasicMultilingualPlaneSymbol;
            } else {
                m_cursorTokenType = TokenType::BasicMultilingualPlaneOther;
            }
        } else {
            return rc;
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
    return SQLITE_OK;
}

int WCDBCursorInfo::lemmatization(const char *input, int inputLength)
{
    //tolower only. You can implement your own lemmatization.
    if (inputLength > m_buffer.capacity()) {
        m_buffer.resize(inputLength);
    }
    for (int i = 0; i < inputLength; ++i) {
        m_buffer.data()[i] = tolower(input[i]);
    }
    return SQLITE_OK;
}

void WCDBCursorInfo::subTokensStep()
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

} //namespace FTS

} //namespace WCDB
