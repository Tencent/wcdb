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

#ifndef __WCDB_TOKENIZER_MODULE_TEMPLATE_HPP
#define __WCDB_TOKENIZER_MODULE_TEMPLATE_HPP

#include <WCDB/TokenizerModule.hpp>
#include <type_traits>

namespace WCDB {

template<typename TokenizerInfo, typename TokenizerCursorInfo>
class TokenizerModuleTemplate final : protected AbstractTokenizerModuleTemplate {
public:
    static_assert(std::is_base_of<AbstractTokenizerInfo, TokenizerInfo>::value, "");
    static_assert(std::is_base_of<AbstractTokenizerCursorInfo, TokenizerCursorInfo>::value, "");

    static TokenizerModule specialize()
    {
        return TokenizerModule(create, destroy, open, close, next);
    }

    static int create(int argc, const char *const *argv, Tokenizer **ppTokenizer)
    {
        TokenizerInfo *info = new TokenizerInfo(argc, argv);
        int rc = newTokenizer(ppTokenizer, info);
        if (!isOK(rc)) {
            deleteTokenizerInfo(info);
            *ppTokenizer = nullptr;
        }
        return rc;
    }

    static int destroy(Tokenizer *pTokenizer)
    {
        if (pTokenizer != nullptr) {
            AbstractTokenizerInfo *info = getTokenizerInfo(pTokenizer);
            deleteTokenizerInfo(static_cast<TokenizerInfo *>(info));
            deleteTokenizer(pTokenizer);
        }
        return OK();
    }

    static void deleteTokenizerInfo(TokenizerInfo *info)
    {
        if (info != nullptr) {
            delete info;
        }
    }

    static int
    open(Tokenizer *pTokenizer, const char *pInput, int nBytes, TokenizerCursor **ppCursor)
    {
        TokenizerCursorInfo *info
        = new TokenizerCursorInfo(pInput, nBytes, getTokenizerInfo(pTokenizer));
        int rc = newCursor(ppCursor, info);
        if (!isOK(rc)) {
            deleteCursorInfo(info);
            *ppCursor = nullptr;
        }
        return rc;
    }

    static int close(TokenizerCursor *pCursor)
    {
        if (pCursor != nullptr) {
            AbstractTokenizerCursorInfo *info = getCursorInfo(pCursor);
            deleteCursorInfo(static_cast<TokenizerCursorInfo *>(info));
            deleteCursor(pCursor);
        }
        return OK();
    }

    static void deleteCursorInfo(TokenizerCursorInfo *info)
    {
        if (info != nullptr) {
            delete info;
        }
    }

    static int next(TokenizerCursor *pCursor,
                    const char **ppToken,
                    int *pnBytes,
                    int *piStartOffset,
                    int *piEndOffset,
                    int *piPosition)
    {
        AbstractTokenizerCursorInfo *info = getCursorInfo(pCursor);
        return info->step(ppToken, pnBytes, piStartOffset, piEndOffset, piPosition);
    }
};

} // namespace WCDB

#endif /* __WCDB_TOKENIZER_MODULE_TEMPLATE_HPP */
