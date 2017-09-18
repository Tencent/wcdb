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

#ifndef fts_module_hpp
#define fts_module_hpp

#include <WCDB/fts_modules.hpp>
#include <sqlcipher/fts3_tokenizer.h>
#include <sqlcipher/sqlite3.h>
#include <stdlib.h>

namespace WCDB {

namespace FTS {

template <const char name[] = nullptr>
class Module {
public:
    struct Tokenizer {
        sqlite3_tokenizer base;
        void *info;
    };
    typedef struct Tokenizer Tokenizer;

    struct Cursor {
        sqlite3_tokenizer_cursor base;
        void *info;
    };
    typedef struct Cursor Cursor;

    static int
    Create(int argc, const char *const *argv, sqlite3_tokenizer **ppTokenizer)
    {
        Tokenizer *tokenizer = (Tokenizer *) sqlite3_malloc(sizeof(Tokenizer));
        if (!tokenizer) {
            return SQLITE_NOMEM;
        }
        memset(tokenizer, 0, sizeof(Tokenizer));
        if (ShouldCreateTokenizerInfo()) {
            tokenizer->info = CreateTokenizerInfo(argc, argv);
            if (!tokenizer->info) {
                return SQLITE_NOMEM;
            }
        }
        *ppTokenizer = &tokenizer->base;
        return SQLITE_OK;
    }

    static int Destroy(sqlite3_tokenizer *pTokenizer)
    {
        if (pTokenizer) {
            Tokenizer *tokenizer = (Tokenizer *) pTokenizer;
            DestroyTokenizerInfo(tokenizer->info);
            sqlite3_free(pTokenizer);
        }
        return SQLITE_OK;
    }

    static int Open(sqlite3_tokenizer *pTokenizer,
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
        if (Module<nullptr>::CreateCursorInfo != CreateCursorInfo) {
            cursor->info = CreateCursorInfo(pInput, nBytes);
            if (!cursor->info) {
                return SQLITE_NOMEM;
            }
        }
        *ppCursor = &cursor->base;
        return SQLITE_OK;
    }

    static int Close(sqlite3_tokenizer_cursor *pCursor)
    {
        if (pCursor) {
            Cursor *cursor = (Cursor *) pCursor;
            DestroyCursorInfo(cursor->info);
            sqlite3_free(pCursor);
        }
        return SQLITE_OK;
    }

    static int Next(sqlite3_tokenizer_cursor *pCursor,
                    const char **ppToken,
                    int *pnBytes,
                    int *piStartOffset,
                    int *piEndOffset,
                    int *piPosition)
    {
        void *info = nullptr;
        if (pCursor) {
            info = ((Cursor *) pCursor)->info;
        }
        return CursorStep(info, ppToken, pnBytes, piStartOffset, piEndOffset,
                          piPosition);
    }

    static bool ShouldCreateTokenizerInfo() { return false; }

    static void *CreateTokenizerInfo(int argc, const char *const *argv)
    {
        return nullptr;
    }

    static void DestroyTokenizerInfo(void *tokenizerInfo) {}

    static void *CreateCursorInfo(const char *pInput, int nBytes)
    {
        return nullptr;
    }

    static int CursorStep(void *cursorInfo,
                          const char **ppToken,
                          int *pnBytes,
                          int *piStartOffset,
                          int *piEndOffset,
                          int *piPosition)
    {
        return SQLITE_ERROR;
    }

    static void DestroyCursorInfo(void *cursorInfo) {}

    static void Register()
    {
        std::shared_ptr<void> module(new sqlite3_tokenizer_module({
            0, Create, Destroy, Open, Close, Next,
        }));
        Modules::SharedModules()->addModule(name, module);
    }
};

}; // namespace FTS

} //namespace WCDB

#endif /* fts_module_hpp */
