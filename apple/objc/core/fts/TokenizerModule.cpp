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

#include <WCDB/Assertion.hpp>
#include <WCDB/SQLite.h>
#include <WCDB/SQLiteFTS3Tokenizer.h>
#include <WCDB/TokenizerModule.hpp>
#include <memory>

namespace WCDB {

struct Tokenizer {
    sqlite3_tokenizer base;
    AbstractFTS3TokenizerInfo *info;
};

struct TokenizerCursor {
    sqlite3_tokenizer_cursor base;
    AbstractFTS3TokenizerCursorInfo *info;
};

#pragma mark - AbstractFTS3TokenizerInfo
AbstractFTS3TokenizerInfo::AbstractFTS3TokenizerInfo(int argc, const char *const *argv){
    WCDB_UNUSED(argc) WCDB_UNUSED(argv)
}

AbstractFTS3TokenizerInfo::~AbstractFTS3TokenizerInfo()
= default;

#pragma mark - AbstractFTS3TokenizerCursorInfo
AbstractFTS3TokenizerCursorInfo::AbstractFTS3TokenizerCursorInfo(
const char *input, int inputLength, AbstractFTS3TokenizerInfo *tokenizerInfo){
    WCDB_UNUSED(input) WCDB_UNUSED(inputLength) WCDB_UNUSED(tokenizerInfo)
}

AbstractFTS3TokenizerCursorInfo::~AbstractFTS3TokenizerCursorInfo()
= default;

#pragma mark - AbstractFTS3TokenizerModuleTemplate
int AbstractFTS3TokenizerModuleTemplate::newTokenizer(Tokenizer **ppTokenizer,
                                                      AbstractFTS3TokenizerInfo *info)
{
    WCTAssert(ppTokenizer != nullptr);
    *ppTokenizer = nullptr;
    if (info == nullptr) {
        return SQLITE_NOMEM;
    }
    Tokenizer *tokenizer = (Tokenizer *) sqlite3_malloc(sizeof(Tokenizer));
    if (tokenizer == nullptr) {
        return SQLITE_NOMEM;
    }
    memset(tokenizer, 0, sizeof(Tokenizer));
    tokenizer->info = info;
    *(sqlite3_tokenizer **) ppTokenizer = &tokenizer->base;
    return SQLITE_OK;
}

AbstractFTS3TokenizerInfo *
AbstractFTS3TokenizerModuleTemplate::getTokenizerInfo(Tokenizer *pTokenizer)
{
    WCTAssert(pTokenizer != nullptr);
    return pTokenizer->info;
}

void AbstractFTS3TokenizerModuleTemplate::deleteTokenizer(Tokenizer *pTokenizer)
{
    WCTAssert(pTokenizer != nullptr);
    sqlite3_free(pTokenizer);
}

int AbstractFTS3TokenizerModuleTemplate::newCursor(TokenizerCursor **ppCursor,
                                                   AbstractFTS3TokenizerCursorInfo *info)
{
    WCTAssert(ppCursor != nullptr);
    *ppCursor = nullptr;
    if (info == nullptr) {
        return SQLITE_NOMEM;
    }
    TokenizerCursor *cursor = (TokenizerCursor *) sqlite3_malloc(sizeof(TokenizerCursor));
    if (cursor == nullptr) {
        return SQLITE_NOMEM;
    }
    memset(cursor, 0, sizeof(TokenizerCursor));
    cursor->info = info;
    *(sqlite3_tokenizer_cursor **) ppCursor = &cursor->base;
    return SQLITE_OK;
}

AbstractFTS3TokenizerCursorInfo *
AbstractFTS3TokenizerModuleTemplate::getCursorInfo(TokenizerCursor *pCursor)
{
    WCTAssert(pCursor != nullptr);
    return pCursor->info;
}

void AbstractFTS3TokenizerModuleTemplate::deleteCursor(TokenizerCursor *pCursor)
{
    WCTAssert(pCursor != nullptr);
    sqlite3_free(pCursor);
}

int AbstractFTS3TokenizerModuleTemplate::OK()
{
    return SQLITE_OK;
}

bool AbstractFTS3TokenizerModuleTemplate::isOK(int rc)
{
    return rc == SQLITE_OK;
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

#pragma mark - AbstractFTS5TokenizerModuleTemplate

int AbstractFTS5TokenizerModuleTemplate::OK()
{
    return SQLITE_OK;
}

bool AbstractFTS5TokenizerModuleTemplate::isOK(int rc)
{
    return rc == SQLITE_OK;
}

bool AbstractFTS5TokenizerModuleTemplate::isDone(int rc)
{
    return rc == SQLITE_DONE;
}

#pragma mark - AbstractFTS5Tokenizer
AbstractFTS5Tokenizer::AbstractFTS5Tokenizer(void *pCtx, const char **azArg, int nArg){
    WCDB_UNUSED(pCtx) WCDB_UNUSED(azArg) WCDB_UNUSED(nArg)
}

AbstractFTS5Tokenizer::~AbstractFTS5Tokenizer()
= default;

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
