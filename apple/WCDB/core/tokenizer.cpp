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
                               int bytes,
                               TokenizerInfoBase *tokenizerInfo)
    : CursorInfoBase(input, bytes, tokenizerInfo)
    , m_input(input)
    , m_length(bytes)
    , m_position(0)
    , m_cursor(0)
    , m_buffer(nullptr)
    , m_bufferCapacity(0)
    , m_nonLemmaLength(0)
    , m_nonLemmaStartOffset(0)
    , m_lemmaBuffer(nullptr)
    , m_lemmaBufferCapacity(0)
    , m_lemmaLength(0)
{
}

WCDBCursorInfo::~WCDBCursorInfo()
{
    if (m_buffer) {
        sqlite3_free(m_buffer);
    }
    if (m_lemmaBuffer) {
        sqlite3_free(m_lemmaBuffer);
    }
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
        rc = setupToken();
        if (rc != SQLITE_OK) {
            return rc;
        }
    }

    if (m_lemmaLength == 0) {
        if (m_subTokensLengthArray.empty()) {
            if (m_currentTokenType == TokenType::None) {
                return SQLITE_DONE;
            }

            //Skip symbol
            while (m_currentTokenType ==
                   TokenType::BasicMultilingualPlaneSymbol) {
                rc = cursorStep();
                if (rc != SQLITE_OK) {
                    return rc;
                }
            }

            TokenType type = m_currentTokenType;
            switch (type) {
                case TokenType::BasicMultilingualPlaneLetter:
                case TokenType::BasicMultilingualPlaneDigit:
                    *piStartOffset = m_cursor;
                    while (((rc = cursorStep()) == SQLITE_OK) &&
                           m_currentTokenType == type)
                        ;
                    if (rc != SQLITE_OK) {
                        return rc;
                    }
                    *pnBytes = m_cursor - *piStartOffset;
                    break;
                case TokenType::BasicMultilingualPlaneOther:
                case TokenType::AuxiliaryPlaneOther:
                    m_subTokensLengthArray.push_back(m_currentTokenLength);
                    m_subTokensCursor = m_cursor;
                    m_subTokensDoubleChar = true;
                    while (((rc = cursorStep()) == SQLITE_OK) &&
                           m_currentTokenType == type) {
                        m_subTokensLengthArray.push_back(m_currentTokenLength);
                    }
                    if (rc != SQLITE_OK) {
                        return rc;
                    }
                    subTokensStep(pnBytes, piStartOffset);
                    break;
                default:
                    break;
            }
            if (type == TokenType::BasicMultilingualPlaneLetter) {
                rc = lemmatization(m_input + *piStartOffset, *pnBytes);
                if (rc != SQLITE_OK) {
                    return rc;
                }
                if (m_lemmaLength > 0) {
                    m_nonLemmaStartOffset = *piStartOffset;
                    m_nonLemmaLength = *pnBytes;
                }
            } else {
                rc = setBuffer(m_input + *piStartOffset, *pnBytes);
                if (rc != SQLITE_OK) {
                    return rc;
                }
            }
        } else {
            subTokensStep(pnBytes, piStartOffset);
            rc = setBuffer(m_input + *piStartOffset, *pnBytes);
            if (rc != SQLITE_OK) {
                return rc;
            }
        }

        *ppToken = m_buffer;
        *piEndOffset = *piStartOffset + *pnBytes;
        *piPosition = m_position++;
    } else {
        *ppToken = m_lemmaBuffer;
        *piStartOffset = m_nonLemmaStartOffset;
        *pnBytes = m_lemmaLength;
        *piEndOffset = *piStartOffset + m_nonLemmaLength;
        m_lemmaLength = 0;
    }

    return SQLITE_OK;
}

int WCDBCursorInfo::cursorStep()
{
    if (m_cursor + m_currentTokenLength < m_length) {
        m_cursor += m_currentTokenLength;
        return setupToken();
    }
    m_cursor = m_length;
    m_currentTokenType = TokenType::None;
    m_currentTokenLength = 0;
    return SQLITE_OK;
}

int WCDBCursorInfo::setupToken()
{
    int rc;
    const unsigned char &first = m_input[m_cursor];
    if (first < 0xC0) {
        m_currentTokenLength = 1;
        if (first >= 0x30 && first <= 0x39) {
            m_currentTokenType = TokenType::BasicMultilingualPlaneDigit;
        } else if ((first >= 0x41 && first <= 0x5a) ||
                   (first >= 0x61 && first <= 0x7a)) {
            m_currentTokenType = TokenType::BasicMultilingualPlaneLetter;
        } else {
            bool result = false;
            rc = isSymbol(first, &result);
            if (rc == SQLITE_OK) {
                if (result) {
                    m_currentTokenType =
                        TokenType::BasicMultilingualPlaneSymbol;
                } else {
                    m_currentTokenType = TokenType::BasicMultilingualPlaneOther;
                }
            } else {
                return rc;
            }
        }
    } else if (first < 0xF0) {
        UnicodeChar unicode = 0;
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
                return SQLITE_OK;
            }
        }
        bool result = false;
        rc = isSymbol(unicode, &result);
        if (rc == SQLITE_OK) {
            if (result) {
                m_currentTokenType = TokenType::BasicMultilingualPlaneSymbol;
            } else {
                m_currentTokenType = TokenType::BasicMultilingualPlaneOther;
            }
        } else {
            return rc;
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
    return SQLITE_OK;
}

int WCDBCursorInfo::lemmatization(const char *input, int bytes)
{
    //Tolower only. You can implement your own lemmatization here.
    int rc = setBufferCapacity(bytes);
    if (rc != SQLITE_OK) {
        return rc;
    }
    for (int i = 0; i < bytes; ++i) {
        m_buffer[i] = tolower(input[i]);
    }
    return SQLITE_OK;
}

int WCDBCursorInfo::setLemmaBuffer(const char *src, int length)
{
    if (length > m_lemmaBufferCapacity) {
        if (m_lemmaBuffer) {
            sqlite3_free(m_lemmaBuffer);
        }
        m_lemmaBufferCapacity *= 2;
        if (m_lemmaBufferCapacity < length) {
            m_lemmaBufferCapacity = length;
        }
        m_lemmaBuffer = (char *) sqlite3_malloc(m_lemmaBufferCapacity);
        if (!m_lemmaBuffer) {
            m_lemmaBufferCapacity = 0;
            return SQLITE_NOMEM;
        }
    }
    memcpy(m_lemmaBuffer, src, length);
    m_lemmaLength = length;
    return SQLITE_OK;
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

int WCDBCursorInfo::setBufferCapacity(int newCapacity)
{
    if (newCapacity > m_bufferCapacity) {
        if (m_buffer) {
            sqlite3_free(m_buffer);
        }
        m_bufferCapacity *= 2;
        if (m_bufferCapacity < newCapacity) {
            m_bufferCapacity = newCapacity;
        }
        m_buffer = (char *) sqlite3_malloc(m_bufferCapacity);
        if (!m_buffer) {
            m_bufferCapacity = 0;
            return SQLITE_NOMEM;
        }
    }
    return SQLITE_OK;
}

int WCDBCursorInfo::setBuffer(const char *src, int length)
{
    int rc = setBufferCapacity(length);
    if (rc == SQLITE_OK) {
        memcpy(m_buffer, src, length);
    }
    return rc;
}

} //namespace FTS

} //namespace WCDB
