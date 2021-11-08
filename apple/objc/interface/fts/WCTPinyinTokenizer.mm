//
// Created by qwchen on 2021/11/8.
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

#import <WCDB/Assertion.hpp>
#import <WCDB/Error.hpp>
#import <WCDB/SQLite.h>
#import <WCDB/WCTPinyinTokenizer.h>

WCTPinyinTokenizer::WCTPinyinTokenizer(void *pCtx, const char **azArg, int nArg)
: WCDB::AbstractFTS5Tokenizer(pCtx, azArg, nArg)
, m_input(nullptr)
, m_inputLength(0)
, m_flags(0)
, m_startOffset(0)
, m_endOffset(0)
, m_cursor(0)
, m_cursorTokenLength(0)
, m_cursorTokenType(UnicodeType::None)
, m_preTokenType(UnicodeType::None)
, m_normalTokenLength(0)
, m_pinyinTokenIndex(0)
, m_needSymbol(false)
{
    for (int i = 0; i < nArg; i++) {
        if (strcmp(azArg[i], "need_symbol") == 0) {
            m_needSymbol = true;
        }
    }
}

WCTPinyinTokenizer::~WCTPinyinTokenizer() = default;

void WCTPinyinTokenizer::loadInput(int flags, const char *pText, int nText)
{
    m_input = pText;
    m_inputLength = nText;
    if (m_input == nullptr) {
        m_inputLength = 0;
    } else if (m_inputLength <= 0) {
        m_inputLength = (int) strlen(m_input);
    }
    m_flags = flags;
    m_startOffset = 0;
    m_endOffset = 0;
    m_cursor = 0;
    m_cursorTokenLength = 0;
    m_cursorTokenType = UnicodeType::None;
    m_preTokenType = UnicodeType::None;
    m_normalToken.clear();
    m_normalTokenLength = 0;
    m_pinyinTokenArr.clear();
    m_pinyinTokenIndex = 0;
}

int WCTPinyinTokenizer::nextToken(int *tflags, const char **ppToken, int *nToken, int *iStart, int *iEnd)
{
    if (m_flags & FTS5_TOKENIZE_QUERY
        || m_pinyinTokenArr.size() == m_pinyinTokenIndex) {
        while (true) {
            int ret = stepNextToken();
            if (WCDB::Error::rc2c(ret) != WCDB::Error::Code::OK) {
                return ret;
            }
            if (m_flags & FTS5_TOKENIZE_QUERY) {
                genNormalToken();
                break;
            } else {
                genPinyinToken();
                if (m_pinyinTokenArr.size() > 0) {
                    break;
                }
            }
        }
    }
    *tflags = 0;
    if (m_flags & FTS5_TOKENIZE_QUERY) {
        *ppToken = m_normalToken.data();
        *nToken = m_normalTokenLength;
        *iStart = m_startOffset;
        *iEnd = m_endOffset;
        return WCDB::Error::c2rc(WCDB::Error::Code::OK);
    } else {
        if (m_pinyinTokenIndex > 0) {
            *tflags = FTS5_TOKEN_COLOCATED;
        }
        const WCDB::StringView &pinyinToken = m_pinyinTokenArr[m_pinyinTokenIndex];
        *ppToken = pinyinToken.data();
        *nToken = (int) pinyinToken.length();
        *iStart = m_startOffset;
        *iEnd = m_endOffset;
        m_pinyinTokenIndex++;
        return WCDB::Error::c2rc(WCDB::Error::Code::OK);
    }
}

int WCTPinyinTokenizer::stepNextToken()
{
    WCDB::Error::Code code = WCDB::Error::Code::OK;
    if (m_cursorTokenLength == 0) {
        int ret = cursorStep();
        code = WCDB::Error::rc2c(ret);
        if (code != WCDB::Error::Code::OK) {
            return WCDB::Error::c2rc(code);
        }
    }

    UnicodeType searchType = m_flags & FTS5_TOKENIZE_QUERY ? UnicodeType::BasicMultilingualPlaneLetter : UnicodeType::BasicMultilingualPlaneOther;
    while (m_cursorTokenType != searchType && m_cursorTokenType != UnicodeType::None) {
        if (m_needSymbol && !(m_flags & FTS5_TOKENIZE_QUERY) && m_cursorTokenType == UnicodeType::BasicMultilingualPlaneSymbol) {
            break;
        }
        code = WCDB::Error::rc2c(cursorStep());
        if (code != WCDB::Error::Code::OK) {
            return WCDB::Error::c2rc(code);
        }
    }

    m_preTokenType = m_cursorTokenType;
    switch (m_preTokenType) {
    case UnicodeType::BasicMultilingualPlaneLetter:
    case UnicodeType::BasicMultilingualPlaneSymbol:
    case UnicodeType::BasicMultilingualPlaneOther:
        m_startOffset = m_cursor;
        if (m_preTokenType == UnicodeType::BasicMultilingualPlaneLetter) {
            while (((code = WCDB::Error::rc2c(cursorStep())) == WCDB::Error::Code::OK)
                   && m_cursorTokenType == m_preTokenType)
                ;
        } else {
            code = WCDB::Error::rc2c(cursorStep());
        }
        if (code != WCDB::Error::Code::OK) {
            return WCDB::Error::c2rc(code);
        }
        m_endOffset = m_cursor;
        m_normalTokenLength = m_endOffset - m_startOffset;
        break;
    default:
        return WCDB::Error::c2rc(WCDB::Error::Code::Done);
    }
    return WCDB::Error::c2rc(WCDB::Error::Code::OK);
}

int WCTPinyinTokenizer::cursorStep()
{
    if (m_cursor + m_cursorTokenLength < m_inputLength) {
        m_cursor += m_cursorTokenLength;
        WCDB::UnsafeStringView currentInput = WCDB::UnsafeStringView(m_input + m_cursor, m_inputLength - m_cursor);
        return WCTFTSTokenizerUtil::stepOneUnicode(currentInput, m_cursorTokenType, m_cursorTokenLength);
    }
    m_cursor = m_inputLength;
    m_cursorTokenType = UnicodeType::None;
    m_cursorTokenLength = 0;
    return WCDB::Error::c2rc(WCDB::Error::Code::OK);
}

void WCTPinyinTokenizer::genNormalToken()
{
    m_normalToken.assign(m_input + m_startOffset, m_input + m_endOffset);
    if (m_preTokenType == UnicodeType::BasicMultilingualPlaneLetter) {
        std::transform(
        m_normalToken.begin(), m_normalToken.end(), m_normalToken.begin(), ::tolower);
    }
}

void WCTPinyinTokenizer::genPinyinToken()
{
    m_pinyinTokenArr.clear();
    m_pinyinTokenIndex = 0;
    WCDB::StringViewSet pinyinSet;
    WCDB::UnsafeStringView token = WCDB::UnsafeStringView(m_input + m_startOffset, m_normalTokenLength);
    const std::vector<WCDB::StringView> *pinyinPtr = WCTFTSTokenizerUtil::getPinYin(token);
    if (pinyinPtr == nullptr) {
        if (m_preTokenType == UnicodeType::BasicMultilingualPlaneSymbol
            && token.length() > 0) {
            m_pinyinTokenArr.emplace_back(token);
        }
        return;
    }
    for (const WCDB::StringView &pinyin : *pinyinPtr) {
        if (pinyin.length() == 0) {
            continue;
        }
        if (pinyinSet.find(pinyin) != pinyinSet.end()) {
            continue;
        }
        //full pinyin
        m_pinyinTokenArr.emplace_back(pinyin);
        pinyinSet.emplace(pinyin);
        if (pinyin.length() <= 1) {
            continue;
        }
        WCDB::UnsafeStringView shortPinyin = WCDB::UnsafeStringView(pinyin.data(), 1);
        if (pinyinSet.find(shortPinyin) != pinyinSet.end()) {
            continue;
        }
        //short pinyin
        pinyinSet.emplace(shortPinyin);
        m_pinyinTokenArr.emplace_back(shortPinyin);
    }
}
