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

#include "PinyinTokenizer.hpp"
#include "Assertion.hpp"
#include "FTSConst.h"
#include "FTSError.hpp"
#include "SQLite.h"
#include <algorithm>

namespace WCDB {

PinyinTokenizer::PinyinTokenizer(const char *const *azArg, int nArg, void *pCtx)
: AbstractFTSTokenizer(azArg, nArg, pCtx)
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
        if (strcmp(azArg[i], BuiltinTokenizer::Parameter::NeedSymbol) == 0) {
            m_needSymbol = true;
        }
    }
}

PinyinTokenizer::~PinyinTokenizer() = default;

void PinyinTokenizer::loadInput(const char *pText, int nText, int flags)
{
    m_input = pText;
    m_inputLength = nText;
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

int PinyinTokenizer::nextToken(
const char **ppToken, int *nToken, int *iStart, int *iEnd, int *tflags, int *iPosition)
{
    WCDB_UNUSED(iPosition);
    if (m_flags & FTS5_TOKENIZE_QUERY || m_pinyinTokenArr.size() == m_pinyinTokenIndex) {
        while (true) {
            int ret = stepNextToken();
            if (!FTSError::isOK(ret)) {
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
        return FTSError::OK();
    } else {
        if (m_pinyinTokenIndex > 0) {
            *tflags = FTS5_TOKEN_COLOCATED;
        }
        const StringView &pinyinToken = m_pinyinTokenArr[m_pinyinTokenIndex];
        *ppToken = pinyinToken.data();
        *nToken = (int) pinyinToken.length();
        *iStart = m_startOffset;
        *iEnd = m_endOffset;
        m_pinyinTokenIndex++;
        return FTSError::OK();
    }
}

int PinyinTokenizer::stepNextToken()
{
    if (m_cursorTokenLength == 0) {
        cursorStep();
    }

    UnicodeType searchType = m_flags & FTS5_TOKENIZE_QUERY ?
                             UnicodeType::BasicMultilingualPlaneLetter :
                             UnicodeType::BasicMultilingualPlaneOther;
    while (m_cursorTokenType != searchType && m_cursorTokenType != UnicodeType::None) {
        if (m_needSymbol && !(m_flags & FTS5_TOKENIZE_QUERY)
            && m_cursorTokenType == UnicodeType::BasicMultilingualPlaneSymbol) {
            break;
        }
        cursorStep();
    }

    m_preTokenType = m_cursorTokenType;
    switch (m_preTokenType) {
    case UnicodeType::BasicMultilingualPlaneLetter:
    case UnicodeType::BasicMultilingualPlaneSymbol:
    case UnicodeType::BasicMultilingualPlaneOther:
        m_startOffset = m_cursor;
        if (m_preTokenType == UnicodeType::BasicMultilingualPlaneLetter) {
            do {
                cursorStep();
            } while (m_cursorTokenType == m_preTokenType);
        } else {
            cursorStep();
        }
        m_endOffset = m_cursor;
        m_normalTokenLength = m_endOffset - m_startOffset;
        break;
    default:
        return FTSError::Done();
    }
    return FTSError::OK();
}

void PinyinTokenizer::cursorStep()
{
    if (m_cursor + m_cursorTokenLength < m_inputLength) {
        m_cursor += m_cursorTokenLength;
        UnsafeStringView currentInput
        = UnsafeStringView(m_input + m_cursor, m_inputLength - m_cursor);
        BaseTokenizerUtil::stepOneUnicode(currentInput, m_cursorTokenType, m_cursorTokenLength);
        return;
    }
    m_cursor = m_inputLength;
    m_cursorTokenType = UnicodeType::None;
    m_cursorTokenLength = 0;
}

void PinyinTokenizer::genNormalToken()
{
    m_normalToken.assign(m_input + m_startOffset, m_input + m_endOffset);
    if (m_preTokenType == UnicodeType::BasicMultilingualPlaneLetter) {
        std::transform(
        m_normalToken.begin(), m_normalToken.end(), m_normalToken.begin(), ::tolower);
    }
}

void PinyinTokenizer::genPinyinToken()
{
    m_pinyinTokenArr.clear();
    m_pinyinTokenIndex = 0;
    StringViewSet pinyinSet;
    UnsafeStringView token = UnsafeStringView(m_input + m_startOffset, m_normalTokenLength);
    const std::vector<StringView> pinyinPtr = BaseTokenizerUtil::getPinYin(token);
    if (pinyinPtr.size() == 0) {
        if (m_preTokenType == UnicodeType::BasicMultilingualPlaneSymbol
            && token.length() > 0) {
            m_pinyinTokenArr.emplace_back(token);
        }
        return;
    }
    for (const StringView &pinyin : pinyinPtr) {
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
        UnsafeStringView shortPinyin = UnsafeStringView(pinyin.data(), 1);
        if (pinyinSet.find(shortPinyin) != pinyinSet.end()) {
            continue;
        }
        //short pinyin
        pinyinSet.emplace(shortPinyin);
        m_pinyinTokenArr.emplace_back(shortPinyin);
    }
}

} //namespace WCDB
