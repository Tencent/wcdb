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

#include "BaseTokenizerUtil.hpp"
#include "TokenizerModuleTemplate.hpp"

namespace WCDB {

class WCDB_API PinyinTokenizer final : public AbstractFTSTokenizer {
public:
    PinyinTokenizer(const char *const *azArg, int nArg, void *pCtx);
    void loadInput(const char *pText, int nText, int flags) override;
    int nextToken(const char **ppToken, int *nToken, int *iStart, int *iEnd, int *tflags, int *iPosition) override;

    ~PinyinTokenizer() override;
    int stepNextToken();

private:
    using UnicodeType = BaseTokenizerUtil::UnicodeType;

    const char *m_input;
    int m_inputLength;
    int m_flags;

    int m_startOffset;
    int m_endOffset;

    int m_cursor;
    int m_cursorTokenLength;

    UnicodeType m_cursorTokenType;
    UnicodeType m_preTokenType;

    std::vector<char> m_normalToken;
    int m_normalTokenLength;
    std::vector<StringView> m_pinyinTokenArr;
    int m_pinyinTokenIndex;

    // Can be configed by tokenizer parameters
    bool m_needSymbol;

    void cursorStep();
    void subTokensStep();

    void genNormalToken();
    void genPinyinToken();
};

} //namespace WCDB
