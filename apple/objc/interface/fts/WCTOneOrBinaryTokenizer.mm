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

#import <WCDB/Assertion.hpp>
#import <WCDB/Error.hpp>
#import <WCDB/WCTOneOrBinaryTokenizer.h>

extern "C" {
extern int porterStem(char *p, int i, int j);
}

#pragma mark - Tokenizer Info
WCTOneOrBinaryTokenizerInfo::WCTOneOrBinaryTokenizerInfo(int argc, const char *const *argv)
: WCDB::AbstractFTS3TokenizerInfo(argc, argv)
, m_needSymbol(false)
, m_needSimplifiedChinese(false)
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "need_symbol") == 0) {
            m_needSymbol = true;
        } else if (strcmp(argv[i], "chinese_traditional_to_simplified") == 0) {
            m_needSimplifiedChinese = true;
        }
    }
}

WCTOneOrBinaryTokenizerInfo::~WCTOneOrBinaryTokenizerInfo() = default;

#pragma mark - Tokenizer
WCTOneOrBinaryTokenizer::WCTOneOrBinaryTokenizer(const char *input,
                                                 int inputLength,
                                                 WCDB::AbstractFTS3TokenizerInfo *tokenizerInfo)
: WCDB::AbstractFTS3TokenizerCursorInfo(input, inputLength, tokenizerInfo)
, WCDB::AbstractFTS5Tokenizer(nullptr, nullptr, 0)
, m_input(input)
, m_inputLength(inputLength)
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
, m_needBinary(true)
, m_needSymbol(false)
, m_needSimplifiedChinese(false)
{
    if (m_input == nullptr) {
        m_inputLength = 0;
    } else if (m_inputLength <= 0) {
        m_inputLength = (int) strlen(m_input);
    }
    WCTOneOrBinaryTokenizerInfo *oneOrBinaryInfo
    = dynamic_cast<WCTOneOrBinaryTokenizerInfo *>(tokenizerInfo);
    if (oneOrBinaryInfo != nullptr) {
        m_needSymbol = oneOrBinaryInfo->m_needSymbol;
        m_needSimplifiedChinese = oneOrBinaryInfo->m_needSimplifiedChinese;
    }
}

WCTOneOrBinaryTokenizer::WCTOneOrBinaryTokenizer(void *pCtx, const char **azArg, int nArg)
: AbstractFTS3TokenizerCursorInfo(nullptr, 0, nullptr)
, AbstractFTS5Tokenizer(pCtx, azArg, nArg)
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
{
    for (int i = 0; i < nArg; i++) {
        if (strcmp(azArg[i], "need_symbol") == 0) {
            m_needSymbol = true;
        } else if (strcmp(azArg[i], "chinese_traditional_to_simplified") == 0) {
            m_needSimplifiedChinese = true;
        }
    }
}

WCTOneOrBinaryTokenizer::~WCTOneOrBinaryTokenizer() = default;

void WCTOneOrBinaryTokenizer::loadInput(int flags, const char *pText, int nText)
{
    WCDB_UNUSED(flags);
    m_input = pText;
    m_inputLength = nText;
    if (m_input == nullptr) {
        m_inputLength = 0;
    } else if (m_inputLength <= 0) {
        m_inputLength = (int) strlen(m_input);
    }
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

// for fts5
int WCTOneOrBinaryTokenizer::nextToken(int *tflags, const char **ppToken, int *nToken, int *iStart, int *iEnd)
{
    int ret = stepNextToken();
    if (WCDB::Error::rc2c(ret) != WCDB::Error::Code::OK) {
        return ret;
    }
    genToken();
    *tflags = 0;
    *ppToken = m_token.data();
    *nToken = m_tokenLength;
    *iStart = m_startOffset;
    *iEnd = m_endOffset;
    return WCDB::Error::c2rc(WCDB::Error::Code::OK);
}

// for fts3
int WCTOneOrBinaryTokenizer::step(
const char **ppToken, int *pnBytes, int *piStartOffset, int *piEndOffset, int *piPosition)
{
    int ret = stepNextToken();
    if (WCDB::Error::rc2c(ret) != WCDB::Error::Code::OK) {
        return ret;
    }
    genToken();
    *ppToken = m_token.data();
    *pnBytes = m_tokenLength;
    *piStartOffset = m_startOffset;
    *piEndOffset = m_endOffset;
    *piPosition = m_position;
    return WCDB::Error::c2rc(WCDB::Error::Code::OK);
}

//Inspired by zorrozhang
int WCTOneOrBinaryTokenizer::stepNextToken()
{
    WCDB::Error::Code code = WCDB::Error::Code::OK;
    if (m_position == 0) {
        int ret = cursorStep();
        code = WCDB::Error::rc2c(ret);
        if (code != WCDB::Error::Code::OK) {
            return WCDB::Error::c2rc(code);
        }
    }

    if (m_subTokensLengthArray.empty()) {
        if (!m_needSymbol) { //Skip symbol
            while (m_cursorTokenType == UnicodeType::BasicMultilingualPlaneSymbol) {
                code = WCDB::Error::rc2c(cursorStep());
                if (code != WCDB::Error::Code::OK) {
                    return WCDB::Error::c2rc(code);
                }
            }
        }

        m_preTokenType = m_cursorTokenType;
        switch (m_preTokenType) {
        case UnicodeType::BasicMultilingualPlaneLetter:
        case UnicodeType::BasicMultilingualPlaneDigit:
            m_startOffset = m_cursor;
            while (((code = WCDB::Error::rc2c(cursorStep())) == WCDB::Error::Code::OK)
                   && m_cursorTokenType == m_preTokenType)
                ;
            if (code != WCDB::Error::Code::OK) {
                return WCDB::Error::c2rc(code);
            }
            m_endOffset = m_cursor;
            m_tokenLength = m_endOffset - m_startOffset;
            break;
        case UnicodeType::BasicMultilingualPlaneSymbol:
        case UnicodeType::BasicMultilingualPlaneOther:
        case UnicodeType::AuxiliaryPlaneOther:
            m_subTokensLengthArray.push_back(m_cursorTokenLength);
            m_subTokensCursor = m_cursor;
            m_subTokensDoubleChar = m_needBinary;
            while (((code = WCDB::Error::rc2c(cursorStep())) == WCDB::Error::Code::OK)
                   && m_cursorTokenType == m_preTokenType) {
                m_subTokensLengthArray.push_back(m_cursorTokenLength);
            }
            if (code != WCDB::Error::Code::OK) {
                return WCDB::Error::c2rc(code);
            }
            subTokensStep();
            break;
        case UnicodeType::None:
            return WCDB::Error::c2rc(WCDB::Error::Code::Done);
        }
    } else {
        subTokensStep();
    }
    return WCDB::Error::c2rc(WCDB::Error::Code::OK);
}

int WCTOneOrBinaryTokenizer::cursorStep()
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

void WCTOneOrBinaryTokenizer::lemmatization(const char *input, int inputLength)
{
    // tolower only. You can implement your own lemmatization.
    m_token.assign(input, input + inputLength);
    std::transform(
    m_token.begin(), m_token.end(), m_token.begin(), ::tolower);
    m_tokenLength
    = porterStem(m_token.data(), 0, m_tokenLength - 1) + 1;
}

void WCTOneOrBinaryTokenizer::subTokensStep()
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

void WCTOneOrBinaryTokenizer::genToken()
{
    if (m_preTokenType == UnicodeType::BasicMultilingualPlaneLetter) {
        lemmatization(m_input + m_startOffset, m_tokenLength);
    } else if (m_preTokenType != UnicodeType::BasicMultilingualPlaneOther
               || !m_needSimplifiedChinese) {
        WCDB::UnsafeStringView token
        = WCDB::UnsafeStringView(m_input + m_startOffset, m_tokenLength);
        WCDB::StringView nomalizeToken = WCTFTSTokenizerUtil::normalizeToken(token);
        m_tokenLength = (int) nomalizeToken.length();
        m_token.assign(nomalizeToken.data(), nomalizeToken.data() + m_tokenLength);
    } else if (!m_needBinary || m_subTokensDoubleChar) {
        WCDB::UnsafeStringView token
        = WCDB::UnsafeStringView(m_input + m_startOffset, m_tokenLength);
        WCDB::StringView nomalizeToken = WCTFTSTokenizerUtil::normalizeToken(token);
        WCDB::UnsafeStringView simplifiedToken = WCTFTSTokenizerUtil::getSimplifiedChinese(nomalizeToken);
        m_tokenLength = (int) simplifiedToken.length();
        m_token.assign(simplifiedToken.data(), simplifiedToken.data() + m_tokenLength);
    } else {
        WCDB::UnsafeStringView firstChar
        = WCDB::UnsafeStringView(m_input + m_startOffset, m_subTokensLengthArray[0]);
        WCDB::UnsafeStringView secondChar = WCDB::UnsafeStringView(
        m_input + m_startOffset + m_subTokensLengthArray[0], m_subTokensLengthArray[1]);
        firstChar = WCTFTSTokenizerUtil::getSimplifiedChinese(firstChar);
        secondChar = WCTFTSTokenizerUtil::getSimplifiedChinese(secondChar);
        WCDB::StringView normalizeFirstChar = WCTFTSTokenizerUtil::normalizeToken(firstChar);
        WCDB::StringView normalizeSecondChar = WCTFTSTokenizerUtil::normalizeToken(secondChar);
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
