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

#ifndef Tokenize_hpp
#define Tokenize_hpp

#warning Make it public and add template for this

#include <WCDB/Lock.hpp>
#include <WCDB/SQLite.h>
#include <map>
#include <stdlib.h>
#include <string>
#include <vector>

namespace WCDB {

namespace FTS {

class TokenizerInfoBase {
public:
    TokenizerInfoBase(int argc, const char *const *argv);
};

class CursorInfoBase {
public:
    CursorInfoBase(const char *input, int inputLength, TokenizerInfoBase *tokenizerInfo);

    virtual int
    step(const char **ppToken, int *pnBytes, int *piStartOffset, int *piEndOffset, int *piPosition)
    = 0;

protected:
    TokenizerInfoBase *m_tokenizerInfo;
};

class ModuleBase {
};

template<typename TokenizerInfo /* = TokenizerInfoBase */, typename CursorInfo /* = CursorInfoBase */>
class Module {
public:
    struct Tokenizer {
        sqlite3_tokenizer base;
        TokenizerInfo *info;
    };
    typedef struct Tokenizer Tokenizer;

    struct Cursor {
        sqlite3_tokenizer_cursor base;
        CursorInfo *info;
    };
    typedef struct Cursor Cursor;

    static int create(int argc, const char *const *argv, sqlite3_tokenizer **ppTokenizer)
    {
        Tokenizer *tokenizer = (Tokenizer *) sqlite3_malloc(sizeof(Tokenizer));
        if (!tokenizer) {
            return SQLITE_NOMEM;
        }
        memset(tokenizer, 0, sizeof(Tokenizer));
        int rc = createTokenizerInfo<TokenizerInfo>(tokenizer, argc, argv);
        if (rc != SQLITE_OK) {
            return rc;
        }
        *ppTokenizer = &tokenizer->base;
        return SQLITE_OK;
    }

    static int destroy(sqlite3_tokenizer *pTokenizer)
    {
        if (pTokenizer) {
            Tokenizer *tokenizer = (Tokenizer *) pTokenizer;
            destroyTokenizerInfo<TokenizerInfo>(tokenizer);
            sqlite3_free(pTokenizer);
        }
        return SQLITE_OK;
    }

    template<typename T /* = TokenizerInfo */>
    static typename std::enable_if<!std::is_same<T, void>::value, int>::type
    createTokenizerInfo(Tokenizer *tokenizer, int argc, const char *const *argv)
    {
        tokenizer->info = new TokenizerInfo(argc, argv);
        if (!tokenizer->info) {
            return SQLITE_NOMEM;
        }
        return SQLITE_OK;
    }

    template<typename T /* = void */>
    static typename std::enable_if<std::is_same<T, void>::value, int>::type
    createTokenizerInfo(Tokenizer *tokenizer, int argc, const char *const *argv)
    {
        return SQLITE_OK;
    }

    template<typename T /* = TokenizerInfo */>
    static typename std::enable_if<!std::is_same<T, void>::value, void>::type
    destroyTokenizerInfo(Tokenizer *tokenizer)
    {
        if (tokenizer->info) {
            delete tokenizer->info;
            tokenizer->info = nullptr;
        }
    }

    template<typename T /* = void */>
    static typename std::enable_if<std::is_same<T, void>::value, void>::type
    destroyTokenizerInfo(Tokenizer *tokenizer)
    {
    }

    static int open(sqlite3_tokenizer *pTokenizer,
                    const char *pInput,
                    int nBytes,
                    sqlite3_tokenizer_cursor **ppCursor)
    {
        if (pInput == nullptr) {
            nBytes = 0;
        }
        if (nBytes < 0) {
            nBytes = (int) strlen(pInput);
        }
        Cursor *cursor = (Cursor *) sqlite3_malloc(sizeof(Cursor));
        if (!cursor) {
            return SQLITE_NOMEM;
        }
        memset(cursor, 0, sizeof(Cursor));
        Tokenizer *tokenizer = (Tokenizer *) pTokenizer;
        cursor->info
        = new CursorInfo(pInput, nBytes, (TokenizerInfoBase *) tokenizer->info);
        if (!cursor->info) {
            return SQLITE_NOMEM;
        }
        *ppCursor = &cursor->base;
        return SQLITE_OK;
    }

    static int close(sqlite3_tokenizer_cursor *pCursor)
    {
        if (pCursor) {
            Cursor *cursor = (Cursor *) pCursor;
            if (cursor->info) {
                delete cursor->info;
                cursor->info = nullptr;
            }
            sqlite3_free(pCursor);
        }
        return SQLITE_OK;
    }

    static int next(sqlite3_tokenizer_cursor *pCursor,
                    const char **ppToken,
                    int *pnBytes,
                    int *piStartOffset,
                    int *piEndOffset,
                    int *piPosition)
    {
        if (pCursor) {
            CursorInfo *info = ((Cursor *) pCursor)->info;
            if (info) {
                return info->step(ppToken, pnBytes, piStartOffset, piEndOffset, piPosition);
            }
        }
        return SQLITE_NOMEM;
    }

    static unsigned char *address()
    {
        static sqlite3_tokenizer_module *s_module
        = new sqlite3_tokenizer_module({ 0,
                                         Module<TokenizerInfo, CursorInfo>::create,
                                         Module<TokenizerInfo, CursorInfo>::destroy,
                                         Module<TokenizerInfo, CursorInfo>::open,
                                         Module<TokenizerInfo, CursorInfo>::close,
                                         Module<TokenizerInfo, CursorInfo>::next });
        return (unsigned char *) &s_module;
    }
};

} // namespace FTS

} //namespace WCDB

#endif /* Tokenize_hpp */
