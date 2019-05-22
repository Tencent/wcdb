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

// TODO: support FTS5 tokenizer module
namespace WCDB {

class AbstractTokenizerInfo {
public:
    AbstractTokenizerInfo(int argc, const char *const *argv);
    virtual ~AbstractTokenizerInfo() = 0;
};

class AbstractTokenizerCursorInfo {
public:
    AbstractTokenizerCursorInfo(const char *input, int inputLength, AbstractTokenizerInfo *tokenizerInfo);
    virtual ~AbstractTokenizerCursorInfo() = 0;

    virtual int
    step(const char **ppToken, int *pnBytes, int *piStartOffset, int *piEndOffset, int *piPosition)
    = 0;
};

typedef struct Tokenizer Tokenizer;
typedef struct TokenizerCursor TokenizerCursor;

class AbstractTokenizerModuleTemplate {
public:
    AbstractTokenizerModuleTemplate() = delete;
    AbstractTokenizerModuleTemplate(const AbstractTokenizerModuleTemplate &) = delete;
    AbstractTokenizerModuleTemplate &operator=(const AbstractTokenizerModuleTemplate &)
    = delete;

protected:
    static int newTokenizer(Tokenizer **ppTokenizer, AbstractTokenizerInfo *info);
    static AbstractTokenizerInfo *getTokenizerInfo(Tokenizer *pTokenizer);
    static void deleteTokenizer(Tokenizer *pTokenizer);

    static int newCursor(TokenizerCursor **ppCursor, AbstractTokenizerCursorInfo *info);
    static AbstractTokenizerCursorInfo *getCursorInfo(TokenizerCursor *pCursor);
    static void deleteCursor(TokenizerCursor *pCursor);

    static int OK();
    static bool isOK(int rc);
};

class TokenizerModule final {
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

    TokenizerModule();
    TokenizerModule(const Create &create,
                    const Destroy &destroy,
                    const Open &open,
                    const Close &close,
                    const Next &next);

private:
    int m_version;
    Create m_create;
    Destroy m_destroy;
    Open m_open;
    Close m_close;
    Next m_next;
    Languageid m_languageid;
};

} // namespace WCDB
