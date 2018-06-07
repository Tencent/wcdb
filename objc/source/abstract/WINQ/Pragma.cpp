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
#include <WCDB/WINQ.h>

namespace WCDB {

const Pragma Pragma::applicationId()
{
    return Pragma("application_id");
}

const Pragma Pragma::autoVacuum()
{
    return Pragma("auto_vacuum");
}

const Pragma Pragma::automaticIndex()
{
    return Pragma("automatic_index");
}

const Pragma Pragma::busyTimeout()
{
    return Pragma("busy_timeout");
}

const Pragma Pragma::cacheSize()
{
    return Pragma("cache_size");
}

const Pragma Pragma::cacheSpill()
{
    return Pragma("cache_spill");
}

const Pragma Pragma::caseSensitiveLike()
{
    return Pragma("case_sensitive_like");
}

const Pragma Pragma::cellSizeCheck()
{
    return Pragma("cell_size_check");
}

const Pragma Pragma::checkpointFullfsync()
{
    return Pragma("checkpoint_fullfsync");
}

const Pragma Pragma::cipher()
{
    return Pragma("cipher");
}

const Pragma Pragma::cipherAddRandom()
{
    return Pragma("cipher_add_random");
}

const Pragma Pragma::cipherDefaultKdfIter()
{
    return Pragma("cipher_default_kdf_iter");
}

const Pragma Pragma::cipherDefaultPageSize()
{
    return Pragma("cipher_default_page_size");
}

const Pragma Pragma::cipherDefaultUseHmac()
{
    return Pragma("cipher_default_use_hmac");
}

const Pragma Pragma::cipherMigrate()
{
    return Pragma("cipher_migrate");
}

const Pragma Pragma::cipherProfile()
{
    return Pragma("cipher_profile");
}

const Pragma Pragma::cipherProvider()
{
    return Pragma("cipher_provider");
}

const Pragma Pragma::cipherProviderVersion()
{
    return Pragma("cipher_provider_version");
}

const Pragma Pragma::cipherUseHmac()
{
    return Pragma("cipher_use_hmac");
}

const Pragma Pragma::cipherVersion()
{
    return Pragma("cipher_version");
}

const Pragma Pragma::cipherPageSize()
{
    return Pragma("cipher_page_size");
}

const Pragma Pragma::collationList()
{
    return Pragma("collation_list");
}

const Pragma Pragma::compileOptions()
{
    return Pragma("compile_options");
}

const Pragma Pragma::dataVersion()
{
    return Pragma("data_version");
}

const Pragma Pragma::databaseList()
{
    return Pragma("database_list");
}

const Pragma Pragma::deferForeignKeys()
{
    return Pragma("defer_foreign_keys");
}

const Pragma Pragma::encoding()
{
    return Pragma("encoding");
}

const Pragma Pragma::foreignKeyCheck()
{
    return Pragma("foreign_key_check");
}

const Pragma Pragma::foreignKeyList()
{
    return Pragma("foreign_key_list");
}

const Pragma Pragma::foreignKeys()
{
    return Pragma("foreign_keys");
}

const Pragma Pragma::freelistCount()
{
    return Pragma("freelist_count");
}

const Pragma Pragma::fullfsync()
{
    return Pragma("fullfsync");
}

const Pragma Pragma::functionList()
{
    return Pragma("function_list");
}

const Pragma Pragma::ignoreCheckConstraints()
{
    return Pragma("ignore_check_constraints");
}

const Pragma Pragma::incrementalVacuum()
{
    return Pragma("incremental_vacuum");
}

const Pragma Pragma::indexInfo()
{
    return Pragma("index_info");
}

const Pragma Pragma::indexList()
{
    return Pragma("index_list");
}

const Pragma Pragma::indexXinfo()
{
    return Pragma("index_xinfo");
}

const Pragma Pragma::integrityCheck()
{
    return Pragma("integrity_check");
}

const Pragma Pragma::journalMode()
{
    return Pragma("journal_mode");
}

const Pragma Pragma::journalSizeLimit()
{
    return Pragma("journal_size_limit");
}

const Pragma Pragma::key()
{
    return Pragma("key");
}

const Pragma Pragma::kdfIter()
{
    return Pragma("kdf_iter");
}

const Pragma Pragma::legacyFileFormat()
{
    return Pragma("legacy_file_format");
}

const Pragma Pragma::lockingMode()
{
    return Pragma("locking_mode");
}

const Pragma Pragma::maxPageCount()
{
    return Pragma("max_page_count");
}

const Pragma Pragma::mmapSize()
{
    return Pragma("mmap_size");
}

const Pragma Pragma::moduleList()
{
    return Pragma("module_list");
}

const Pragma Pragma::optimize()
{
    return Pragma("optimize");
}

const Pragma Pragma::pageCount()
{
    return Pragma("page_count");
}

const Pragma Pragma::pageSize()
{
    return Pragma("page_size");
}

const Pragma Pragma::parserTrace()
{
    return Pragma("parser_trace");
}

const Pragma Pragma::pragmaList()
{
    return Pragma("pragma_list");
}

const Pragma Pragma::queryOnly()
{
    return Pragma("query_only");
}

const Pragma Pragma::quickCheck()
{
    return Pragma("quick_check");
}

const Pragma Pragma::readUncommitted()
{
    return Pragma("read_uncommitted");
}

const Pragma Pragma::recursiveTriggers()
{
    return Pragma("recursive_triggers");
}

const Pragma Pragma::rekey()
{
    return Pragma("rekey");
}

const Pragma Pragma::reverseUnorderedSelects()
{
    return Pragma("reverse_unordered_selects");
}

const Pragma Pragma::schemaVersion()
{
    return Pragma("schema_version");
}

const Pragma Pragma::secureDelete()
{
    return Pragma("secure_delete");
}

const Pragma Pragma::shrinkMemory()
{
    return Pragma("shrink_memory");
}

const Pragma Pragma::softHeapLimit()
{
    return Pragma("soft_heap_limit");
}

const Pragma Pragma::stats()
{
    return Pragma("stats");
}

const Pragma Pragma::synchronous()
{
    return Pragma("synchronous");
}

const Pragma Pragma::tableInfo()
{
    return Pragma("table_info");
}

const Pragma Pragma::tempStore()
{
    return Pragma("temp_store");
}

const Pragma Pragma::threads()
{
    return Pragma("threads");
}

const Pragma Pragma::userVersion()
{
    return Pragma("user_version");
}

const Pragma Pragma::vdbeAddoptrace()
{
    return Pragma("vdbe_addoptrace");
}

const Pragma Pragma::vdbeDebug()
{
    return Pragma("vdbe_debug");
}

const Pragma Pragma::vdbeListing()
{
    return Pragma("vdbe_listing");
}

const Pragma Pragma::vdbeTrace()
{
    return Pragma("vdbe_trace");
}

const Pragma Pragma::walAutocheckpoint()
{
    return Pragma("wal_autocheckpoint");
}

const Pragma Pragma::walCheckpoint()
{
    return Pragma("wal_checkpoint");
}

const Pragma Pragma::writableSchema()
{
    return Pragma("writable_schema");
}

Pragma::Pragma(const std::string &name)
{
    Lang::Pragma &lang = getMutableLang();
    lang.name.assign(name);
}

} // namespace WCDB
