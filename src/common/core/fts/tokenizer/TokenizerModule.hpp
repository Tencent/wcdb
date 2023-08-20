//
// Created by sanhuazhang on 2019/05/19
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
#include "FTSError.hpp"
#include <memory>

namespace WCDB {

class WCDB_API AbstractFTSTokenizer {
public:
    AbstractFTSTokenizer(const char *const *azArg, int nArg, void *pCtx /* pCtx is only used in FTS5 */);
    virtual ~AbstractFTSTokenizer() = 0;
    virtual void
    loadInput(const char *pText, int nText, int flags /* flags is only used in FTS5 */)
    = 0;
    virtual int nextToken(const char **ppToken,
                          int *nToken,
                          int *iStart,
                          int *iEnd,
                          int *tflags,   //tflags is only used in FTS5
                          int *iPosition //iPosition is only used in FTS3/4
                          )
    = 0;
};

typedef struct FTS3TokenizerWrap FTS3TokenizerWrap;
typedef struct FTS3TokenizeCursorWrap FTS3TokenizeCursorWrap;

class WCDB_API AbstractFTS3TokenizerModuleTemplate {
public:
    AbstractFTS3TokenizerModuleTemplate() = delete;
    AbstractFTS3TokenizerModuleTemplate(const AbstractFTS3TokenizerModuleTemplate &) = delete;
    AbstractFTS3TokenizerModuleTemplate &
    operator=(const AbstractFTS3TokenizerModuleTemplate &)
    = delete;

protected:
    static int
    newTokenizer(FTS3TokenizerWrap **ppTokenizerWrap, AbstractFTSTokenizer *tokenizer);
    static AbstractFTSTokenizer *getTokenizerFromWrap(FTS3TokenizerWrap *tokenizerWrap);
    static void deleteTokenizerWrap(FTS3TokenizerWrap *ppTokenizer);

    static int newCursor(FTS3TokenizeCursorWrap **ppCursor, AbstractFTSTokenizer *tokenizer);
    static AbstractFTSTokenizer *getTokenizerFromCurser(FTS3TokenizeCursorWrap *pCursor);
    static void deleteCursor(FTS3TokenizeCursorWrap *pCursor);
};

class WCDB_API FTS3TokenizerModule final {
public:
    typedef int (*Create)(int argc, const char *const *argv, FTS3TokenizerWrap **ppTokenizer);
    typedef int (*Destroy)(FTS3TokenizerWrap *pTokenizer);
    typedef int (*Open)(FTS3TokenizerWrap *pTokenizer,
                        const char *pInput,
                        int nBytes,
                        FTS3TokenizeCursorWrap **ppCursor);
    typedef int (*Close)(FTS3TokenizeCursorWrap *pCursor);
    typedef int (*Next)(FTS3TokenizeCursorWrap *pCursor,
                        const char **ppToken,
                        int *pnBytes,
                        int *piStartOffset,
                        int *piEndOffset,
                        int *piPosition);
    typedef int (*Languageid)(FTS3TokenizeCursorWrap *pCursor, int iLangid);

    FTS3TokenizerModule();
    FTS3TokenizerModule(const Create &create,
                        const Destroy &destroy,
                        const Open &open,
                        const Close &close,
                        const Next &next);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
private:
    int m_version;
    Create m_create;
    Destroy m_destroy;
    Open m_open;
    Close m_close;
    Next m_next;
    Languageid m_languageid;
#pragma GCC diagnostic pop
};

#pragma mark - fts5

class WCDB_API AbstractFTS5TokenizerModuleTemplate {
public:
    AbstractFTS5TokenizerModuleTemplate() = delete;
    AbstractFTS5TokenizerModuleTemplate(const AbstractFTS5TokenizerModuleTemplate &) = delete;
    AbstractFTS5TokenizerModuleTemplate &
    operator=(const AbstractFTS5TokenizerModuleTemplate &)
    = delete;
};

class WCDB_API FTS5TokenizerModule final {
public:
    typedef int (*Create)(void *, const char *const *azArg, int nArg, AbstractFTSTokenizer **ppTokenizer);
    typedef int (*Destroy)(AbstractFTSTokenizer *pTokenizer);
    typedef int (*TokenCallback)(
    void *pCtx, int tflags, const char *pToken, int nToken, int iStart, int iEnd);
    typedef int (*Tokenize)(AbstractFTSTokenizer *pTokenizer,
                            void *pCtx,
                            int flags,
                            const char *pText,
                            int nText,
                            TokenCallback callback);

    FTS5TokenizerModule();
    FTS5TokenizerModule(const Create &create,
                        const Destroy &destroy,
                        const Tokenize &tokenize,
                        void *pCtx);
    void *getContext();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
private:
    Create m_create;
    Destroy m_destroy;
    Tokenize m_tokenize;
    void *m_pCtx;
#pragma GCC diagnostic pop
};

#pragma mark - fts module wrapper

class WCDB_API TokenizerModule {
public:
    TokenizerModule(std::shared_ptr<FTS3TokenizerModule> fts3Module);
    TokenizerModule(std::shared_ptr<FTS5TokenizerModule> fts3Module);
    TokenizerModule() = delete;
    std::shared_ptr<FTS3TokenizerModule> getFts3Module() const;
    std::shared_ptr<FTS5TokenizerModule> getFts5Module() const;

private:
    std::shared_ptr<FTS3TokenizerModule> m_fts3Module;
    std::shared_ptr<FTS5TokenizerModule> m_fts5Module;
};

} // namespace WCDB
