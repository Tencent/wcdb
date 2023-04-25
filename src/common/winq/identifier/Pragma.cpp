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
 * Licensed under the BSD 3-Clause License (the "License";
    return Pragma(StringView::makeConstant(string)); you may not use
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

#include "WINQ.h"

namespace WCDB {

Pragma::Pragma() = default;

Pragma::~Pragma() = default;

Pragma::Pragma(const UnsafeStringView& name)
{
    syntax().name = name;
}

Pragma Pragma::applicationId()
{
    static const char* string = "application_id";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::autoVacuum()
{
    static const char* string = "auto_vacuum";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::automaticIndex()
{
    static const char* string = "automatic_index";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::busyTimeout()
{
    static const char* string = "busy_timeout";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cacheSize()
{
    static const char* string = "cache_size";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cacheSpill()
{
    static const char* string = "cache_spill";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::caseSensitiveLike()
{
    static const char* string = "case_sensitive_like";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cellSizeCheck()
{
    static const char* string = "cell_size_check";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::checkpointFullfsync()
{
    static const char* string = "checkpoint_fullfsync";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipher()
{
    static const char* string = "cipher";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherAddRandom()
{
    static const char* string = "cipher_add_random";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherCompatibility()
{
    static const char* string = "cipher_compatibility";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherDefaultKdfIter()
{
    static const char* string = "cipher_default_kdf_iter";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherDefaultPageSize()
{
    static const char* string = "cipher_default_page_size";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherHmacAlgorithm()
{
    static const char* string = "cipher_hmac_algorithm";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherKdfAlgorithm()
{
    static const char* string = "cipher_kdf_algorithm";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherPlainTextHeaderSize()
{
    static const char* string = "cipher_plaintext_header_size";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherDefaultUseHmac()
{
    static const char* string = "cipher_default_use_hmac";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherMigrate()
{
    static const char* string = "cipher_migrate";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherProfile()
{
    static const char* string = "cipher_profile";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherProvider()
{
    static const char* string = "cipher_provider";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherProviderVersion()
{
    static const char* string = "cipher_provider_version";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherSalt()
{
    static const char* string = "cipher_salt";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherUseHmac()
{
    static const char* string = "cipher_use_hmac";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherVersion()
{
    static const char* string = "cipher_version";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::cipherPageSize()
{
    static const char* string = "cipher_page_size";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::collationList()
{
    static const char* string = "collation_list";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::dataVersion()
{
    static const char* string = "data_version";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::databaseList()
{
    static const char* string = "database_list";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::deferForeignKeys()
{
    static const char* string = "defer_foreign_keys";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::encoding()
{
    static const char* string = "encoding";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::foreignKeyCheck()
{
    static const char* string = "foreign_key_check";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::foreignKeyList()
{
    static const char* string = "foreign_key_list";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::foreignKeys()
{
    static const char* string = "foreign_keys";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::freelistCount()
{
    static const char* string = "freelist_count";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::fullfsync()
{
    static const char* string = "fullfsync";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::functionList()
{
    static const char* string = "function_list";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::ignoreCheckConstraints()
{
    static const char* string = "ignore_check_constraints";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::incrementalVacuum()
{
    static const char* string = "incremental_vacuum";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::indexInfo()
{
    static const char* string = "index_info";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::indexList()
{
    static const char* string = "index_list";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::indexXinfo()
{
    static const char* string = "index_xinfo";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::integrityCheck()
{
    static const char* string = "integrity_check";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::journalMode()
{
    static const char* string = "journal_mode";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::journalSizeLimit()
{
    static const char* string = "journal_size_limit";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::key()
{
    static const char* string = "key";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::kdfIter()
{
    static const char* string = "kdf_iter";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::legacyFileFormat()
{
    static const char* string = "legacy_file_format";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::lockingMode()
{
    static const char* string = "locking_mode";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::maxPageCount()
{
    static const char* string = "max_page_count";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::mmapSize()
{
    static const char* string = "mmap_size";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::moduleList()
{
    static const char* string = "module_list";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::optimize()
{
    static const char* string = "optimize";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::pageCount()
{
    static const char* string = "page_count";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::pageSize()
{
    static const char* string = "page_size";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::parserTrace()
{
    static const char* string = "parser_trace";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::pragmaList()
{
    static const char* string = "pragma_list";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::queryOnly()
{
    static const char* string = "query_only";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::quickCheck()
{
    static const char* string = "quick_check";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::readUncommitted()
{
    static const char* string = "read_uncommitted";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::recursiveTriggers()
{
    static const char* string = "recursive_triggers";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::rekey()
{
    static const char* string = "rekey";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::reverseUnorderedSelects()
{
    static const char* string = "reverse_unordered_selects";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::schemaVersion()
{
    static const char* string = "schema_version";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::secureDelete()
{
    static const char* string = "secure_delete";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::shrinkMemory()
{
    static const char* string = "shrink_memory";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::softHeapLimit()
{
    static const char* string = "soft_heap_limit";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::stats()
{
    static const char* string = "stats";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::synchronous()
{
    static const char* string = "synchronous";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::tableInfo()
{
    static const char* string = "table_info";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::tempStore()
{
    static const char* string = "temp_store";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::threads()
{
    static const char* string = "threads";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::userVersion()
{
    static const char* string = "user_version";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::vdbeAddoptrace()
{
    static const char* string = "vdbe_addoptrace";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::vdbeDebug()
{
    static const char* string = "vdbe_debug";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::vdbeListing()
{
    static const char* string = "vdbe_listing";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::vdbeTrace()
{
    static const char* string = "vdbe_trace";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::walAutocheckpoint()
{
    static const char* string = "wal_autocheckpoint";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::walCheckpoint()
{
    static const char* string = "wal_checkpoint";
    return Pragma(StringView::makeConstant(string));
}

Pragma Pragma::writableSchema()
{
    static const char* string = "writable_schema";
    return Pragma(StringView::makeConstant(string));
}

} // namespace WCDB
