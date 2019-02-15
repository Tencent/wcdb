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
#include <WCDB/TokenizerModule.hpp>
#include <sqlcipher/fts3_tokenizer.h>

namespace WCDB {

struct Tokenizer {
    sqlite3_tokenizer base;
    AbstractTokenizerInfo *info;
};

struct TokenizerCursor {
    sqlite3_tokenizer_cursor base;
    AbstractTokenizerCursorInfo *info;
};

#pragma mark - AbstractTokenizerInfo
AbstractTokenizerInfo::AbstractTokenizerInfo(int argc, const char *const *argv)
{
}

AbstractTokenizerInfo::~AbstractTokenizerInfo()
{
}

#pragma mark - AbstractTokenizerCursorInfo
AbstractTokenizerCursorInfo::AbstractTokenizerCursorInfo(const char *input,
                                                         int inputLength,
                                                         AbstractTokenizerInfo *tokenizerInfo)
{
}

AbstractTokenizerCursorInfo::~AbstractTokenizerCursorInfo()
{
}

#pragma mark - AbstractTokenizerModuleTemplate
int AbstractTokenizerModuleTemplate::newTokenizer(Tokenizer **ppTokenizer,
                                                  AbstractTokenizerInfo *info)
{
    WCTInnerAssert(ppTokenizer != nullptr);
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

AbstractTokenizerInfo *AbstractTokenizerModuleTemplate::getTokenizerInfo(Tokenizer *pTokenizer)
{
    WCTInnerAssert(pTokenizer != nullptr);
    return pTokenizer->info;
}

void AbstractTokenizerModuleTemplate::deleteTokenizer(Tokenizer *pTokenizer)
{
    WCTInnerAssert(pTokenizer != nullptr);
    sqlite3_free(pTokenizer);
}

int AbstractTokenizerModuleTemplate::newCursor(TokenizerCursor **ppCursor,
                                               AbstractTokenizerCursorInfo *info)
{
    WCTInnerAssert(ppCursor != nullptr);
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

AbstractTokenizerCursorInfo *
AbstractTokenizerModuleTemplate::getCursorInfo(TokenizerCursor *pCursor)
{
    WCTInnerAssert(pCursor != nullptr);
    return pCursor->info;
}

void AbstractTokenizerModuleTemplate::deleteCursor(TokenizerCursor *pCursor)
{
    WCTInnerAssert(pCursor != nullptr);
    sqlite3_free(pCursor);
}

int AbstractTokenizerModuleTemplate::OK()
{
    return SQLITE_OK;
}

bool AbstractTokenizerModuleTemplate::isOK(int rc)
{
    return rc == SQLITE_OK;
}

#pragma mark - TokenizerModule
TokenizerModule::TokenizerModule()
: m_version(0)
, m_create(nullptr)
, m_destroy(nullptr)
, m_open(nullptr)
, m_close(nullptr)
, m_next(nullptr)
, m_languageid(nullptr)
{
}

TokenizerModule::TokenizerModule(const TokenizerModule::Create &create,
                                 const TokenizerModule::Destroy &destroy,
                                 const TokenizerModule::Open &open,
                                 const TokenizerModule::Close &close,
                                 const TokenizerModule::Next &next)
: m_version(0)
, m_create(create)
, m_destroy(destroy)
, m_open(open)
, m_close(close)
, m_next(next)
, m_languageid(nullptr)
{
    static_assert(sizeof(TokenizerModule) == sizeof(sqlite3_tokenizer_module), "");
    static_assert(
    offsetof(TokenizerModule, m_version) == offsetof(sqlite3_tokenizer_module, iVersion), "");
    static_assert(
    offsetof(TokenizerModule, m_create) == offsetof(sqlite3_tokenizer_module, xCreate), "");
    static_assert(
    offsetof(TokenizerModule, m_destroy) == offsetof(sqlite3_tokenizer_module, xDestroy), "");
    static_assert(
    offsetof(TokenizerModule, m_open) == offsetof(sqlite3_tokenizer_module, xOpen), "");
    static_assert(
    offsetof(TokenizerModule, m_close) == offsetof(sqlite3_tokenizer_module, xClose), "");
    static_assert(
    offsetof(TokenizerModule, m_next) == offsetof(sqlite3_tokenizer_module, xNext), "");
    static_assert(offsetof(TokenizerModule, m_languageid)
                  == offsetof(sqlite3_tokenizer_module, xLanguageid),
                  "");
}

} // namespace WCDB
