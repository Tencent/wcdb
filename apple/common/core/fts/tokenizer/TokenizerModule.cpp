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

#include "TokenizerModule.hpp"
#include "Assertion.hpp"
#include "SQLite.h"
#include "SQLiteFTS3Tokenizer.h"
#include <cstring>
#include <memory>

namespace WCDB {

#ifndef offsetof
#define offsetof(STRUCTURE, FIELD) ((int) ((char *) &((STRUCTURE *) 0)->FIELD))
#endif

#pragma mark - AbstractFTS5Tokenizer
AbstractFTSTokenizer::AbstractFTSTokenizer(const char *const *azArg, int nArg, void *pCtx){
    WCDB_UNUSED(pCtx) WCDB_UNUSED(azArg) WCDB_UNUSED(nArg)
}

AbstractFTSTokenizer::~AbstractFTSTokenizer()
= default;

struct FTS3TokenizerWrap {
    sqlite3_tokenizer base;
    AbstractFTSTokenizer *tokenizer;
};

struct FTS3TokenizeCursorWrap {
    sqlite3_tokenizer_cursor base;
    AbstractFTSTokenizer *tokenizer;
};

#pragma mark - AbstractFTS3TokenizerModuleTemplate
int AbstractFTS3TokenizerModuleTemplate::newTokenizer(FTS3TokenizerWrap **ppTokenizerWrap,
                                                      AbstractFTSTokenizer *tokenizer)
{
    WCTAssert(ppTokenizerWrap != nullptr);
    *ppTokenizerWrap = nullptr;
    if (tokenizer == nullptr) {
        return FTSError::NoMem();
    }
    FTS3TokenizerWrap *tokenizerWrap
    = (FTS3TokenizerWrap *) sqlite3_malloc(sizeof(FTS3TokenizerWrap));
    if (tokenizer == nullptr) {
        return FTSError::NoMem();
    }
    memset(tokenizerWrap, 0, sizeof(FTS3TokenizerWrap));
    tokenizerWrap->tokenizer = tokenizer;
    *(sqlite3_tokenizer **) ppTokenizerWrap = &tokenizerWrap->base;
    return FTSError::OK();
}

AbstractFTSTokenizer *
AbstractFTS3TokenizerModuleTemplate::getTokenizerFromWrap(FTS3TokenizerWrap *tokenizerWrap)
{
    WCTAssert(tokenizerWrap != nullptr);
    return tokenizerWrap->tokenizer;
}

void AbstractFTS3TokenizerModuleTemplate::deleteTokenizerWrap(FTS3TokenizerWrap *ppTokenizerWrap)
{
    if (ppTokenizerWrap != nullptr) {
        sqlite3_free(ppTokenizerWrap);
    }
}

int AbstractFTS3TokenizerModuleTemplate::newCursor(FTS3TokenizeCursorWrap **ppCursor,
                                                   AbstractFTSTokenizer *tokenizer)
{
    WCTAssert(ppCursor != nullptr);
    *ppCursor = nullptr;
    if (tokenizer == nullptr) {
        return FTSError::NoMem();
    }
    FTS3TokenizeCursorWrap *cursor
    = (FTS3TokenizeCursorWrap *) sqlite3_malloc(sizeof(FTS3TokenizeCursorWrap));
    if (cursor == nullptr) {
        return FTSError::NoMem();
    }
    memset(cursor, 0, sizeof(FTS3TokenizeCursorWrap));
    cursor->tokenizer = tokenizer;
    *(sqlite3_tokenizer_cursor **) ppCursor = &cursor->base;
    return FTSError::OK();
}

AbstractFTSTokenizer *
AbstractFTS3TokenizerModuleTemplate::getTokenizerFromCurser(FTS3TokenizeCursorWrap *pCursor)
{
    WCTAssert(pCursor != nullptr);
    return pCursor->tokenizer;
}

void AbstractFTS3TokenizerModuleTemplate::deleteCursor(FTS3TokenizeCursorWrap *pCursor)
{
    if (pCursor != nullptr) {
        sqlite3_free(pCursor);
    }
}

#pragma mark - FTS3TokenizerModule
FTS3TokenizerModule::FTS3TokenizerModule()
: m_version(0)
, m_create(nullptr)
, m_destroy(nullptr)
, m_open(nullptr)
, m_close(nullptr)
, m_next(nullptr)
, m_languageid(nullptr)
{
}

FTS3TokenizerModule::FTS3TokenizerModule(const FTS3TokenizerModule::Create &create,
                                         const FTS3TokenizerModule::Destroy &destroy,
                                         const FTS3TokenizerModule::Open &open,
                                         const FTS3TokenizerModule::Close &close,
                                         const FTS3TokenizerModule::Next &next)
: m_version(0)
, m_create(create)
, m_destroy(destroy)
, m_open(open)
, m_close(close)
, m_next(next)
, m_languageid(nullptr)
{
    static_assert(sizeof(FTS3TokenizerModule) == sizeof(sqlite3_tokenizer_module), "");
    static_assert(offsetof(FTS3TokenizerModule, m_version)
                  == offsetof(sqlite3_tokenizer_module, iVersion),
                  "");
    static_assert(offsetof(FTS3TokenizerModule, m_create)
                  == offsetof(sqlite3_tokenizer_module, xCreate),
                  "");
    static_assert(offsetof(FTS3TokenizerModule, m_destroy)
                  == offsetof(sqlite3_tokenizer_module, xDestroy),
                  "");
    static_assert(
    offsetof(FTS3TokenizerModule, m_open) == offsetof(sqlite3_tokenizer_module, xOpen), "");
    static_assert(
    offsetof(FTS3TokenizerModule, m_close) == offsetof(sqlite3_tokenizer_module, xClose), "");
    static_assert(
    offsetof(FTS3TokenizerModule, m_next) == offsetof(sqlite3_tokenizer_module, xNext), "");
    static_assert(offsetof(FTS3TokenizerModule, m_languageid)
                  == offsetof(sqlite3_tokenizer_module, xLanguageid),
                  "");
}

#pragma mark - FTS5TokenizerModule
FTS5TokenizerModule::FTS5TokenizerModule()
: m_create(nullptr), m_destroy(nullptr), m_tokenize(nullptr), m_pCtx(nullptr)
{
}

FTS5TokenizerModule::FTS5TokenizerModule(const Create &create,
                                         const Destroy &destroy,
                                         const Tokenize &tokenize,
                                         void *pCtx)
: m_create(create), m_destroy(destroy), m_tokenize(tokenize), m_pCtx(pCtx)
{
    static_assert(
    sizeof(FTS5TokenizerModule) == sizeof(fts5_tokenizer) + sizeof(m_pCtx), "");
    static_assert(
    offsetof(FTS5TokenizerModule, m_create) == offsetof(fts5_tokenizer, xCreate), "");
    static_assert(
    offsetof(FTS5TokenizerModule, m_destroy) == offsetof(fts5_tokenizer, xDelete), "");
    static_assert(
    offsetof(FTS5TokenizerModule, m_tokenize) == offsetof(fts5_tokenizer, xTokenize), "");
}

void *FTS5TokenizerModule::getContext()
{
    return m_pCtx;
}

#pragma mark - TokenizerModule

TokenizerModule::TokenizerModule(std::shared_ptr<FTS3TokenizerModule> fts3Module)
: m_fts3Module(fts3Module), m_fts5Module(nullptr)
{
}

TokenizerModule::TokenizerModule(std::shared_ptr<FTS5TokenizerModule> fts5Module)
: m_fts3Module(nullptr), m_fts5Module(fts5Module)
{
}

std::shared_ptr<FTS3TokenizerModule> TokenizerModule::getFts3Module() const
{
    WCTAssert(m_fts3Module == nullptr || m_fts5Module == nullptr);
    return m_fts3Module;
}

std::shared_ptr<FTS5TokenizerModule> TokenizerModule::getFts5Module() const
{
    WCTAssert(m_fts3Module == nullptr || m_fts5Module == nullptr);
    return m_fts5Module;
}
} // namespace WCDB
