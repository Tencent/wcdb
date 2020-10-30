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

#pragma once

#include <WCDB/TokenizerModule.hpp>
#include <type_traits>

namespace WCDB {

template<typename TokenizerInfo, typename TokenizerCursorInfo>
class FTS3TokenizerModuleTemplate final : protected AbstractFTS3TokenizerModuleTemplate {
public:
    static_assert(std::is_base_of<AbstractFTS3TokenizerInfo, TokenizerInfo>::value, "");
    static_assert(std::is_base_of<AbstractFTS3TokenizerCursorInfo, TokenizerCursorInfo>::value, "");

    static TokenizerModule specialize()
    {
        return TokenizerModule(std::make_shared<FTS3TokenizerModule>(create, destroy, open, close, next));
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
            AbstractFTS3TokenizerInfo *info = getTokenizerInfo(pTokenizer);
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
            AbstractFTS3TokenizerCursorInfo *info = getCursorInfo(pCursor);
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
        AbstractFTS3TokenizerCursorInfo *info = getCursorInfo(pCursor);
        return info->step(ppToken, pnBytes, piStartOffset, piEndOffset, piPosition);
    }
};

template<typename Fts5Tokenizer>
class FTS5TokenizerModuleTemplate final : protected AbstractFTS5TokenizerModuleTemplate {
public:
    static_assert(std::is_base_of<AbstractFTS5Tokenizer, Fts5Tokenizer>::value, "");

    static TokenizerModule specializeWithContext(void* pCtx)
    {
        return TokenizerModule(std::make_shared<FTS5TokenizerModule>(create, destroy, tokenize, pCtx));
    }

    static int create(void* pCtx, const char **azArg, int nArg, AbstractFTS5Tokenizer **ppTokenizer)
    {
        *ppTokenizer = static_cast<AbstractFTS5Tokenizer*>(new Fts5Tokenizer(pCtx, azArg, nArg)) ;
        return OK();
    }

    static int destroy(AbstractFTS5Tokenizer *pTokenizer)
    {
        delete pTokenizer;
        return OK();
    }

    static int tokenize(AbstractFTS5Tokenizer *pTokenizer,
                         void *pCtx,
                         int flags,
                         const char *pText, int nText,
                         int (*xToken)(
                           void *pCtx,
                           int tflags,
                           const char *pToken,
                           int nToken,
                           int iStart,
                           int iEnd
                         ))
    {
        int rc;
        const char *pToken;
        int nToken;
        int iStart;
        int iEnd;
        int tflags;
        pTokenizer->loadInput(flags, pText, nText);
        while((rc = pTokenizer->nextToken(&tflags, &pToken, &nToken, &iStart, &iEnd)) == OK()){
            rc = xToken(pCtx, tflags, pToken, nToken, iStart, iEnd);
            if(!isOK(rc)){
                break;
            }
        }
        if(isDone(rc)){
            return OK();
        }
        return rc;
    }
};

} // namespace WCDB
