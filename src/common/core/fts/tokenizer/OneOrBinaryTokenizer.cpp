//
// Created by qiuwenchen on 2021/11/8.
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

#include "OneOrBinaryTokenizer.hpp"
#include "Assertion.hpp"
#include "FTSConst.h"
#include "FTSError.hpp"
#include <algorithm>

extern "C" {
extern int porterStem(char *p, int i, int j);
}

namespace WCDB {

#pragma mark - Tokenizer
OneOrBinaryTokenizer::OneOrBinaryTokenizer(const char *const *azArg, int nArg, void *pCtx)
: AbstractFTSTokenizer(azArg, nArg, pCtx)
, m_input(nullptr)
, m_inputLength(0)
, m_position(0)
, m_startOffset(0)
, m_endOffset(0)
, m_cursor(0)
, m_cursorTokenLength(0)
, m_cursorTokenType(UnicodeType::None)
, m_preTokenType(UnicodeType::None)
, m_subTokensCursor(0)
, m_subTokensDoubleChar(true)
, m_tokenLength(0)
, m_needBinary(false)
, m_needSymbol(false)
, m_needSimplifiedChinese(false)
, m_skipStemming(false)
{
    for (int i = 0; i < nArg; i++) {
        if (strcmp(azArg[i], BuiltinTokenizer::Parameter::NeedSymbol) == 0) {
            m_needSymbol = true;
        } else if (strcmp(azArg[i], BuiltinTokenizer::Parameter::SimplifyChinese) == 0) {
            m_needSimplifiedChinese = true;
        } else if (strcmp(azArg[i], BuiltinTokenizer::Parameter::SkipStemming) == 0) {
            m_skipStemming = true;
        }
    }
}

OneOrBinaryTokenizer::~OneOrBinaryTokenizer() = default;

void OneOrBinaryTokenizer::loadInput(const char *pText, int nText, int flags)
{
    WCDB_UNUSED(flags);
    m_input = pText;
    m_inputLength = nText;
    m_position = 0;
    m_startOffset = 0;
    m_endOffset = 0;
    m_cursor = 0;
    m_cursorTokenLength = 0;
    m_cursorTokenType = UnicodeType::None;
    m_preTokenType = UnicodeType::None;
    m_subTokensLengthArray.clear();
    m_subTokensCursor = 0;
    m_subTokensDoubleChar = true;
    m_token.clear();
    m_tokenLength = 0;
}

int OneOrBinaryTokenizer::nextToken(
const char **ppToken, int *nToken, int *iStart, int *iEnd, int *tflags, int *iPosition)
{
    int ret = stepNextToken();
    if (!FTSError::isOK(ret)) {
        return ret;
    }
    genToken();
    if (tflags != nullptr) {
        *tflags = 0;
    }
    *ppToken = m_token.data();
    *nToken = m_tokenLength;
    *iStart = m_startOffset;
    *iEnd = m_endOffset;
    if (iPosition != nullptr) {
        *iPosition = m_position;
    }
    return FTSError::OK();
}

//Inspired by zorrozhang
int OneOrBinaryTokenizer::stepNextToken()
{
    if (m_position == 0) {
        cursorStep();
    }
    if (m_subTokensLengthArray.empty()) {
        if (!m_needSymbol) { //Skip symbol
            while (m_cursorTokenType == UnicodeType::BasicMultilingualPlaneSymbol) {
                cursorStep();
            }
        }
        m_preTokenType = m_cursorTokenType;
        switch (m_preTokenType) {
        case UnicodeType::BasicMultilingualPlaneLetter:
        case UnicodeType::BasicMultilingualPlaneDigit:
            m_startOffset = m_cursor;
            do {
                cursorStep();
            } while (m_cursorTokenType == m_preTokenType);
            m_endOffset = m_cursor;
            m_tokenLength = m_endOffset - m_startOffset;
            break;
        case UnicodeType::BasicMultilingualPlaneSymbol:
        case UnicodeType::BasicMultilingualPlaneOther:
        case UnicodeType::AuxiliaryPlaneOther:
            m_subTokensCursor = m_cursor;
            m_subTokensDoubleChar = m_needBinary;
            do {
                m_subTokensLengthArray.push_back(m_cursorTokenLength);
                cursorStep();
            } while (m_cursorTokenType == m_preTokenType);
            subTokensStep();
            break;
        case UnicodeType::None:
            return FTSError::Done();
        }
    } else {
        subTokensStep();
    }
    return FTSError::OK();
}

void OneOrBinaryTokenizer::cursorStep()
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

void OneOrBinaryTokenizer::lemmatization(const char *input, int inputLength)
{
    // tolower only. You can implement your own lemmatization.
    m_token.assign(input, input + inputLength);
    std::transform(m_token.begin(), m_token.end(), m_token.begin(), ::tolower);
    if (!m_skipStemming) {
        m_tokenLength = porterStem(m_token.data(), 0, m_tokenLength - 1) + 1;
    }
}

void OneOrBinaryTokenizer::subTokensStep()
{
    m_startOffset = m_subTokensCursor;
    m_tokenLength = m_subTokensLengthArray[0];
    if (m_subTokensDoubleChar) {
        if (m_subTokensLengthArray.size() > 1) {
            m_tokenLength += m_subTokensLengthArray[1];
            if (m_needBinary) {
                m_subTokensDoubleChar = false;
            }
        } else {
            m_subTokensLengthArray.clear();
        }
    } else {
        m_subTokensCursor += m_subTokensLengthArray[0];
        m_subTokensLengthArray.erase(m_subTokensLengthArray.begin());
        if (m_needBinary) {
            m_subTokensDoubleChar = true;
        }
    }
    m_endOffset = m_startOffset + m_tokenLength;
}

void OneOrBinaryTokenizer::genToken()
{
    if (m_preTokenType == UnicodeType::BasicMultilingualPlaneLetter) {
        lemmatization(m_input + m_startOffset, m_tokenLength);
    } else if (m_preTokenType != UnicodeType::BasicMultilingualPlaneOther
               || !m_needSimplifiedChinese) {
        UnsafeStringView token = UnsafeStringView(m_input + m_startOffset, m_tokenLength);
        StringView nomalizeToken = BaseTokenizerUtil::normalizeToken(token);
        m_tokenLength = (int) nomalizeToken.length();
        m_token.assign(nomalizeToken.data(), nomalizeToken.data() + m_tokenLength);
    } else if (!m_needBinary || m_subTokensDoubleChar) {
        UnsafeStringView token = UnsafeStringView(m_input + m_startOffset, m_tokenLength);
        StringView nomalizeToken = BaseTokenizerUtil::normalizeToken(token);
        StringView simplifiedToken = BaseTokenizerUtil::getSimplifiedChinese(nomalizeToken);
        m_tokenLength = (int) simplifiedToken.length();
        m_token.assign(simplifiedToken.data(), simplifiedToken.data() + m_tokenLength);
    } else {
        UnsafeStringView firstChar
        = UnsafeStringView(m_input + m_startOffset, m_subTokensLengthArray[0]);
        UnsafeStringView secondChar = UnsafeStringView(
        m_input + m_startOffset + m_subTokensLengthArray[0], m_subTokensLengthArray[1]);
        firstChar = BaseTokenizerUtil::getSimplifiedChinese(firstChar);
        secondChar = BaseTokenizerUtil::getSimplifiedChinese(secondChar);
        StringView normalizeFirstChar = BaseTokenizerUtil::normalizeToken(firstChar);
        StringView normalizeSecondChar = BaseTokenizerUtil::normalizeToken(secondChar);
        m_tokenLength
        = (int) normalizeFirstChar.length() + (int) normalizeSecondChar.length();
        m_token.assign(normalizeFirstChar.data(),
                       normalizeFirstChar.data() + normalizeFirstChar.length());
        m_token.insert(m_token.end(),
                       normalizeSecondChar.data(),
                       normalizeSecondChar.data() + normalizeSecondChar.length());
    }
    m_position++;
}

} //namespace WCDB
