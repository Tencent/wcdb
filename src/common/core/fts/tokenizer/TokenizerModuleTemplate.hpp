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

#include "TokenizerModule.hpp"
#include <cstring>
#include <type_traits>

namespace WCDB {

template<typename Fts3Tokenizer>
class FTS3TokenizerModuleTemplate final : protected AbstractFTS3TokenizerModuleTemplate {
public:
    static_assert(std::is_base_of<AbstractFTSTokenizer, Fts3Tokenizer>::value, "");

    static TokenizerModule specialize()
    {
        return TokenizerModule(std::make_shared<FTS3TokenizerModule>(
        create, destroy, open, close, next));
    }

    static int create(int argc, const char *const *argv, FTS3TokenizerWrap **ppTokenizerWrap)
    {
        Fts3Tokenizer *tokenizer = new Fts3Tokenizer(argv, argc, nullptr);
        int rc = newTokenizer(ppTokenizerWrap, tokenizer);
        if (!FTSError::isOK(rc)) {
            if (tokenizer != nullptr) {
                delete tokenizer;
            }
            deleteTokenizerWrap(*ppTokenizerWrap);
            *ppTokenizerWrap = nullptr;
        }
        return rc;
    }

    static int destroy(FTS3TokenizerWrap *tokenizerWrap)
    {
        if (tokenizerWrap != nullptr) {
            AbstractFTSTokenizer *tokenizer = getTokenizerFromWrap(tokenizerWrap);
            deleteTokenizer(static_cast<Fts3Tokenizer *>(tokenizer));
            deleteTokenizerWrap(tokenizerWrap);
        }
        return FTSError::OK();
    }

    static void deleteTokenizer(Fts3Tokenizer *tokenizer)
    {
        if (tokenizer != nullptr) {
            delete tokenizer;
        }
    }

    static int open(FTS3TokenizerWrap *tokenizerWrap,
                    const char *pInput,
                    int nBytes,
                    FTS3TokenizeCursorWrap **ppCursor)
    {
        AbstractFTSTokenizer *tokenizer = getTokenizerFromWrap(tokenizerWrap);
        if (pInput == nullptr) {
            nBytes = 0;
        } else if (nBytes <= 0) {
            nBytes = (int) strlen(pInput);
        }
        tokenizer->loadInput(pInput, nBytes, 0);
        int rc = newCursor(ppCursor, tokenizer);
        if (!FTSError::isOK(rc)) {
            deleteCursor(*ppCursor);
            *ppCursor = nullptr;
        }
        return rc;
    }

    static int close(FTS3TokenizeCursorWrap *pCursor)
    {
        deleteCursor(pCursor);
        return FTSError::OK();
    }

    static int next(FTS3TokenizeCursorWrap *pCursor,
                    const char **ppToken,
                    int *pnBytes,
                    int *piStartOffset,
                    int *piEndOffset,
                    int *piPosition)
    {
        AbstractFTSTokenizer *tokenizer = getTokenizerFromCurser(pCursor);
        return tokenizer->nextToken(
        ppToken, pnBytes, piStartOffset, piEndOffset, nullptr, piPosition);
    }
};

template<typename Fts5Tokenizer>
class FTS5TokenizerModuleTemplate final : protected AbstractFTS5TokenizerModuleTemplate {
public:
    static_assert(std::is_base_of<AbstractFTSTokenizer, Fts5Tokenizer>::value, "");

    static TokenizerModule specializeWithContext(void *pCtx)
    {
        return TokenizerModule(
        std::make_shared<FTS5TokenizerModule>(create, destroy, tokenize, pCtx));
    }

    static int
    create(void *pCtx, const char *const *azArg, int nArg, AbstractFTSTokenizer **ppTokenizer)
    {
        *ppTokenizer
        = static_cast<AbstractFTSTokenizer *>(new Fts5Tokenizer(azArg, nArg, pCtx));
        return FTSError::OK();
    }

    static int destroy(AbstractFTSTokenizer *pTokenizer)
    {
        delete pTokenizer;
        return FTSError::OK();
    }

    static int
    tokenize(AbstractFTSTokenizer *pTokenizer,
             void *pCtx,
             int flags,
             const char *pText,
             int nText,
             int (*xToken)(void *pCtx, int tflags, const char *pToken, int nToken, int iStart, int iEnd))
    {
        int rc;
        const char *pToken;
        int nToken;
        int iStart;
        int iEnd;
        int tflags;
        if (pText == nullptr) {
            nText = 0;
        } else if (nText <= 0) {
            nText = (int) strlen(pText);
        }
        pTokenizer->loadInput(pText, nText, flags);
        while (FTSError::isOK(rc = pTokenizer->nextToken(
                              &pToken, &nToken, &iStart, &iEnd, &tflags, nullptr))) {
            rc = xToken(pCtx, tflags, pToken, nToken, iStart, iEnd);
            if (!FTSError::isOK(rc)) {
                break;
            }
        }
        if (FTSError::isDone(rc)) {
            return FTSError::OK();
        }
        return rc;
    }
};

} // namespace WCDB
