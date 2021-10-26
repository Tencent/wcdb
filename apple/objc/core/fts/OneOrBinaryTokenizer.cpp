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
#include <WCDB/SQLite.h>

extern "C" {
extern int porterStem(char *p, int i, int j);
}

namespace WCDB {

#pragma mark - Tokenizer Info
OneOrBinaryTokenizerInfo::OneOrBinaryTokenizerInfo(int argc, const char *const *argv)
: AbstractFTS3TokenizerInfo(argc, argv)
, m_needBinary(true)
, m_needSymbol(false)
, m_needSimplifiedChinese(false)
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "just_one") == 0) {
            m_needBinary = false;
        } else if (strcmp(argv[i], "need_symbol") == 0) {
            m_needSymbol = true;
        } else if (strcmp(argv[i], "chinese_traditional_to_simplified") == 0) {
            m_needSimplifiedChinese = true;
        }
    }
}

OneOrBinaryTokenizerInfo::~OneOrBinaryTokenizerInfo() = default;

#pragma mark - Tokenizer
OneOrBinaryTokenizer::OneOrBinaryTokenizer(const char *input,
                                           int inputLength,
                                           AbstractFTS3TokenizerInfo *tokenizerInfo)
: AbstractFTS3TokenizerCursorInfo(input, inputLength, tokenizerInfo)
, AbstractFTS5Tokenizer(nullptr, nullptr, 0)
, m_input(input)
, m_inputLength(inputLength)
, m_position(0)
, m_startOffset(0)
, m_endOffset(0)
, m_cursor(0)
, m_cursorTokenLength(0)
, m_cursorTokenType(TokenType::None)
, m_preTokenType(TokenType::None)
, m_subTokensCursor(0)
, m_subTokensDoubleChar(true)
, m_normalTokenLength(0)
, m_pinyinTokenIndex(0)
, m_needBinary(true)
, m_ispinyin(false)
, m_needSymbol(false)
, m_needSimplifiedChinese(false)
{
    static_assert(sizeof(UnicodeChar) == 2, "UnicodeChar must be 2 byte length.");

    if (m_input == nullptr) {
        m_inputLength = 0;
    } else if (m_inputLength <= 0) {
        m_inputLength = (int) strlen(m_input);
    }
    OneOrBinaryTokenizerInfo *oneOrBinaryInfo
    = dynamic_cast<OneOrBinaryTokenizerInfo *>(tokenizerInfo);
    if (oneOrBinaryInfo != nullptr) {
        m_needBinary = oneOrBinaryInfo->m_needBinary;
        m_needSymbol = oneOrBinaryInfo->m_needSymbol;
        m_needSimplifiedChinese = oneOrBinaryInfo->m_needSimplifiedChinese;
    }
}

OneOrBinaryTokenizer::OneOrBinaryTokenizer(void *pCtx, const char **azArg, int nArg)
: AbstractFTS3TokenizerCursorInfo(nullptr, 0, nullptr)
, AbstractFTS5Tokenizer(pCtx, azArg, nArg)
, m_input(nullptr)
, m_inputLength(0)
, m_position(0)
, m_startOffset(0)
, m_endOffset(0)
, m_cursor(0)
, m_cursorTokenLength(0)
, m_cursorTokenType(TokenType::None)
, m_preTokenType(TokenType::None)
, m_subTokensCursor(0)
, m_subTokensDoubleChar(true)
, m_normalTokenLength(0)
, m_pinyinTokenIndex(0)
, m_needBinary(true)
, m_ispinyin(false)
, m_needSymbol(false)
, m_needSimplifiedChinese(false)
{
    for (int i = 0; i < nArg; i++) {
        if (strcmp(azArg[i], "just_one") == 0) {
            m_needBinary = false;
        } else if (strcmp(azArg[i], "pin_yin") == 0) {
            // Pin yin parameter only for fts5 tokenizer
            m_ispinyin = true;
        } else if (strcmp(azArg[i], "need_symbol") == 0) {
            //symbol is ignored in pinyin tokenizer
            m_needSymbol = true;
        } else if (strcmp(azArg[i], "chinese_traditional_to_simplified") == 0) {
            m_needSimplifiedChinese = true;
        }
    }
    if (m_ispinyin) {
        //The pinyin parameter is incompatible with other parameters.
        m_needSymbol = false;
        m_needBinary = false;
        m_needSimplifiedChinese = false;
    }
    static_assert(sizeof(UnicodeChar) == 2, "UnicodeChar must be 2 byte length.");
}

OneOrBinaryTokenizer::~OneOrBinaryTokenizer() = default;

void OneOrBinaryTokenizer::loadInput(int flags, const char *pText, int nText)
{
    m_input = pText;
    m_inputLength = nText;
    if (m_input == nullptr) {
        m_inputLength = 0;
    } else if (m_inputLength <= 0) {
        m_inputLength = (int) strlen(m_input);
    }
    if (flags & FTS5_TOKENIZE_QUERY) {
        m_ispinyin = false;
    }
    m_position = 0;
    m_startOffset = 0;
    m_endOffset = 0;
    m_cursor = 0;
    m_cursorTokenLength = 0;
    m_cursorTokenType = TokenType::None;
    m_preTokenType = TokenType::None;
    m_subTokensLengthArray.clear();
    m_subTokensCursor = 0;
    m_subTokensDoubleChar = true;
    m_normalToken.clear();
    m_normalTokenLength = 0;
    m_pinyinTokens.clear();
    m_pinyinTokenIndex = 0;
}

// for fts5
int OneOrBinaryTokenizer::nextToken(int *tflags, const char **ppToken, int *nToken, int *iStart, int *iEnd)
{
    if (!m_ispinyin || m_pinyinTokens.size() == 0) {
        while (true) {
            int ret = stepNextToken();
            if (Error::rc2c(ret) != Error::Code::OK) {
                return ret;
            }
            if (!m_ispinyin) {
                ret = genNormalToken();
                if (Error::rc2c(ret) != Error::Code::OK) {
                    return ret;
                }
                break;
            } else {
                genPinyinToken();
                if (m_pinyinTokens.size() > 0) {
                    break;
                }
            }
        }
    }
    *tflags = 0;
    if (!m_ispinyin) {
        *ppToken = m_normalToken.data();
        *nToken = m_normalTokenLength;
        *iStart = m_startOffset;
        *iEnd = m_endOffset;
        return Error::c2rc(Error::Code::OK);
    } else {
        if (m_pinyinTokenIndex > 0) {
            *tflags = FTS5_TOKEN_COLOCATED;
        }
        const auto pinyinToken = m_pinyinTokens.begin();
        *ppToken = pinyinToken->data();
        *nToken = (int) pinyinToken->length();
        *iStart = m_startOffset;
        *iEnd = m_endOffset;
        m_pinyinTokenIndex++;
        m_pinyinTokens.erase(pinyinToken);
        return Error::c2rc(Error::Code::OK);
    }
}

// for fts3
int OneOrBinaryTokenizer::step(
const char **ppToken, int *pnBytes, int *piStartOffset, int *piEndOffset, int *piPosition)
{
    int ret = stepNextToken();
    if (Error::rc2c(ret) != Error::Code::OK) {
        return ret;
    }
    ret = genNormalToken();
    if (Error::rc2c(ret) != Error::Code::OK) {
        return ret;
    }
    *ppToken = m_normalToken.data();
    *pnBytes = m_normalTokenLength;
    *piStartOffset = m_startOffset;
    *piEndOffset = m_endOffset;
    *piPosition = m_position;
    return Error::c2rc(Error::Code::OK);
}

//Inspired by zorrozhang
int OneOrBinaryTokenizer::stepNextToken()
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
        if (!m_needSymbol) {
            while (m_cursorTokenType == TokenType::BasicMultilingualPlaneSymbol) {
                code = Error::rc2c(cursorStep());
                if (code != Error::Code::OK) {
                    return Error::c2rc(code);
                }
            }
        } else if (m_ispinyin) {
            while (m_cursorTokenType != TokenType::BasicMultilingualPlaneOther
                   && m_cursorTokenType != TokenType::None) {
                code = Error::rc2c(cursorStep());
                if (code != Error::Code::OK) {
                    return Error::c2rc(code);
                }
            }
        }

        m_preTokenType = m_cursorTokenType;
        switch (m_preTokenType) {
        case TokenType::BasicMultilingualPlaneLetter:
        case TokenType::BasicMultilingualPlaneDigit:
            m_startOffset = m_cursor;
            while (((code = Error::rc2c(cursorStep())) == Error::Code::OK)
                   && m_cursorTokenType == m_preTokenType)
                ;
            if (code != Error::Code::OK) {
                return Error::c2rc(code);
            }
            m_endOffset = m_cursor;
            m_normalTokenLength = m_endOffset - m_startOffset;
            break;
        case TokenType::BasicMultilingualPlaneSymbol:
        case TokenType::BasicMultilingualPlaneOther:
        case TokenType::AuxiliaryPlaneOther:
            m_subTokensLengthArray.push_back(m_cursorTokenLength);
            m_subTokensCursor = m_cursor;
            m_subTokensDoubleChar = m_needBinary;
            while (((code = Error::rc2c(cursorStep())) == Error::Code::OK)
                   && m_cursorTokenType == m_preTokenType) {
                m_subTokensLengthArray.push_back(m_cursorTokenLength);
            }
            if (code != Error::Code::OK) {
                return Error::c2rc(code);
            }
            subTokensStep();
            break;
        case TokenType::None:
            return Error::c2rc(Error::Code::Done);
        }
    } else {
        subTokensStep();
    }
    return Error::c2rc(Error::Code::OK);
}

int OneOrBinaryTokenizer::cursorStep()
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

int OneOrBinaryTokenizer::cursorSetup()
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
                unicode = (UnicodeChar) (((int) unicode << 6) | (m_input[i] & 0x3F));
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

int OneOrBinaryTokenizer::lemmatization(const char *input, int inputLength)
{
    // tolower only. You can implement your own lemmatization.
    m_normalToken.assign(input, input + inputLength);
    std::transform(
    m_normalToken.begin(), m_normalToken.end(), m_normalToken.begin(), ::tolower);
    m_normalTokenLength
    = porterStem(m_normalToken.data(), 0, m_normalTokenLength - 1) + 1;
    return Error::c2rc(Error::Code::OK);
}

void OneOrBinaryTokenizer::subTokensStep()
{
    m_startOffset = m_subTokensCursor;
    m_normalTokenLength = m_subTokensLengthArray[0];
    if (m_subTokensDoubleChar) {
        if (m_subTokensLengthArray.size() > 1) {
            m_normalTokenLength += m_subTokensLengthArray[1];
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
    m_endOffset = m_startOffset + m_normalTokenLength;
}

int OneOrBinaryTokenizer::genNormalToken()
{
    if (m_preTokenType == TokenType::BasicMultilingualPlaneLetter) {
        Error::Code code
        = Error::rc2c(lemmatization(m_input + m_startOffset, m_normalTokenLength));
        if (code != Error::Code::OK) {
            return Error::c2rc(code);
        }
    } else if (m_preTokenType != TokenType::BasicMultilingualPlaneOther
               || !m_needSimplifiedChinese) {
        UnsafeStringView token
        = UnsafeStringView(m_input + m_startOffset, m_normalTokenLength);
        StringView nomalizeToken = normalizeToken(token);
        m_normalTokenLength = (int) nomalizeToken.length();
        m_normalToken.assign(nomalizeToken.data(), nomalizeToken.data() + m_normalTokenLength);
    } else if (!m_needBinary || m_subTokensDoubleChar) {
        UnsafeStringView token
        = UnsafeStringView(m_input + m_startOffset, m_normalTokenLength);
        StringView nomalizeToken = normalizeToken(token);
        UnsafeStringView simplifiedToken = getSimplifiedChinese(nomalizeToken);
        m_normalTokenLength = (int) simplifiedToken.length();
        m_normalToken.assign(simplifiedToken.data(), simplifiedToken.data() + m_normalTokenLength);
    } else {
        UnsafeStringView firstChar
        = UnsafeStringView(m_input + m_startOffset, m_subTokensLengthArray[0]);
        UnsafeStringView secondChar = UnsafeStringView(
        m_input + m_startOffset + m_subTokensLengthArray[0], m_subTokensLengthArray[1]);
        firstChar = getSimplifiedChinese(firstChar);
        secondChar = getSimplifiedChinese(secondChar);
        StringView normalizeFirstChar = normalizeToken(firstChar);
        StringView normalizeSecondChar = normalizeToken(secondChar);
        m_normalTokenLength
        = (int) normalizeFirstChar.length() + (int) normalizeSecondChar.length();
        m_normalToken.assign(normalizeFirstChar.data(),
                             normalizeFirstChar.data() + normalizeFirstChar.length());
        m_normalToken.insert(m_normalToken.end(),
                             normalizeSecondChar.data(),
                             normalizeSecondChar.data() + normalizeSecondChar.length());
    }
    m_position++;
    return Error::c2rc(Error::Code::OK);
}

void OneOrBinaryTokenizer::genPinyinToken()
{
    m_pinyinTokens.clear();
    m_pinyinTokenIndex = 0;
    m_position++;
    UnsafeStringView token = UnsafeStringView(m_input + m_startOffset, m_normalTokenLength);
    const std::vector<StringView> *pinyinPtr = getPinYin(token);
    if (pinyinPtr == nullptr) {
        return;
    }
    for (const StringView &pinyin : *pinyinPtr) {
        if (pinyin.length() == 0) {
            continue;
        }
        //full pinyin
        m_pinyinTokens.emplace(pinyin);
        if (pinyin.length() > 1) {
            //short pinyin
            m_pinyinTokens.emplace(pinyin.data(), 1);
        }
    }
}

} // namespace WCDB
