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
#include <memory>

namespace WCDB {

#pragma mark - fts3

class AbstractFTS3TokenizerInfo {
public:
    AbstractFTS3TokenizerInfo(int argc, const char *const *argv);
    virtual ~AbstractFTS3TokenizerInfo() = 0;
};

class AbstractFTS3TokenizerCursorInfo {
public:
    AbstractFTS3TokenizerCursorInfo(const char *input,
                                    int inputLength,
                                    AbstractFTS3TokenizerInfo *tokenizerInfo);
    virtual ~AbstractFTS3TokenizerCursorInfo() = 0;

    virtual int
    step(const char **ppToken, int *pnBytes, int *piStartOffset, int *piEndOffset, int *piPosition)
    = 0;
};

typedef struct Tokenizer Tokenizer;
typedef struct TokenizerCursor TokenizerCursor;

class AbstractFTS3TokenizerModuleTemplate {
public:
    AbstractFTS3TokenizerModuleTemplate() = delete;
    AbstractFTS3TokenizerModuleTemplate(const AbstractFTS3TokenizerModuleTemplate &) = delete;
    AbstractFTS3TokenizerModuleTemplate &
    operator=(const AbstractFTS3TokenizerModuleTemplate &)
    = delete;

protected:
    static int newTokenizer(Tokenizer **ppTokenizer, AbstractFTS3TokenizerInfo *info);
    static AbstractFTS3TokenizerInfo *getTokenizerInfo(Tokenizer *pTokenizer);
    static void deleteTokenizer(Tokenizer *pTokenizer);

    static int
    newCursor(TokenizerCursor **ppCursor, AbstractFTS3TokenizerCursorInfo *info);
    static AbstractFTS3TokenizerCursorInfo *getCursorInfo(TokenizerCursor *pCursor);
    static void deleteCursor(TokenizerCursor *pCursor);

    static int OK();
    static bool isOK(int rc);
};

class FTS3TokenizerModule final {
public:
    typedef int (*Create)(int argc, const char *const *argv, Tokenizer **ppTokenizer);
    typedef int (*Destroy)(Tokenizer *pTokenizer);
    typedef int (*Open)(Tokenizer *pTokenizer, const char *pInput, int nBytes, TokenizerCursor **ppCursor);
    typedef int (*Close)(TokenizerCursor *pCursor);
    typedef int (*Next)(TokenizerCursor *pCursor,
                        const char **ppToken,
                        int *pnBytes,
                        int *piStartOffset,
                        int *piEndOffset,
                        int *piPosition);
    typedef int (*Languageid)(TokenizerCursor *pCursor, int iLangid);

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

class AbstractFTS5TokenizerModuleTemplate {
public:
    AbstractFTS5TokenizerModuleTemplate() = delete;
    AbstractFTS5TokenizerModuleTemplate(const AbstractFTS5TokenizerModuleTemplate &) = delete;
    AbstractFTS5TokenizerModuleTemplate &
    operator=(const AbstractFTS5TokenizerModuleTemplate &)
    = delete;

protected:
    static int OK();
    static bool isOK(int rc);
    static bool isDone(int rc);
};

class AbstractFTS5Tokenizer {
public:
    AbstractFTS5Tokenizer(void *pCtx, const char **azArg, int nArg);
    virtual ~AbstractFTS5Tokenizer() = 0;
    virtual void loadInput(int flags, const char *pText, int nText) = 0;
    virtual int
    nextToken(int *tflags, const char **ppToken, int *nToken, int *iStart, int *iEnd)
    = 0;
};

class FTS5TokenizerModule final {
public:
    typedef int (*Create)(void *, const char **azArg, int nArg, AbstractFTS5Tokenizer **ppTokenizer);
    typedef int (*Destroy)(AbstractFTS5Tokenizer *pTokenizer);
    typedef int (*TokenCallback)(
    void *pCtx, int tflags, const char *pToken, int nToken, int iStart, int iEnd);
    typedef int (*Tokenize)(AbstractFTS5Tokenizer *pTokenizer,
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

class TokenizerModule {
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
