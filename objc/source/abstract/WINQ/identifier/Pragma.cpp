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

#include <WCDB/WINQ.h>

namespace WCDB {

Pragma::Pragma(const char* name)
{
    syntax.name = name;
}

Pragma Pragma::applicationId()
{
    return Pragma("application_id");
}

Pragma Pragma::autoVacuum()
{
    return Pragma("auto_vacuum");
}

Pragma Pragma::automaticIndex()
{
    return Pragma("automatic_index");
}

Pragma Pragma::busyTimeout()
{
    return Pragma("busy_timeout");
}

Pragma Pragma::cacheSize()
{
    return Pragma("cache_size");
}

Pragma Pragma::cacheSpill()
{
    return Pragma("cache_spill");
}

Pragma Pragma::caseSensitiveLike()
{
    return Pragma("case_sensitive_like");
}

Pragma Pragma::cellSizeCheck()
{
    return Pragma("cell_size_check");
}

Pragma Pragma::checkpointFullfsync()
{
    return Pragma("checkpoint_fullfsync");
}

Pragma Pragma::cipher()
{
    return Pragma("cipher");
}

Pragma Pragma::cipherAddRandom()
{
    return Pragma("cipher_add_random");
}

Pragma Pragma::cipherDefaultKdfIter()
{
    return Pragma("cipher_default_kdf_iter");
}

Pragma Pragma::cipherDefaultPageSize()
{
    return Pragma("cipher_default_page_size");
}

Pragma Pragma::cipherDefaultUseHmac()
{
    return Pragma("cipher_default_use_hmac");
}

Pragma Pragma::cipherMigrate()
{
    return Pragma("cipher_migrate");
}

Pragma Pragma::cipherProfile()
{
    return Pragma("cipher_profile");
}

Pragma Pragma::cipherProvider()
{
    return Pragma("cipher_provider");
}

Pragma Pragma::cipherProviderVersion()
{
    return Pragma("cipher_provider_version");
}

Pragma Pragma::cipherUseHmac()
{
    return Pragma("cipher_use_hmac");
}

Pragma Pragma::cipherVersion()
{
    return Pragma("cipher_version");
}

Pragma Pragma::cipherPageSize()
{
    return Pragma("cipher_page_size");
}

Pragma Pragma::collationList()
{
    return Pragma("collation_list");
}

Pragma Pragma::compileOptions()
{
    return Pragma("compile_options");
}

Pragma Pragma::dataVersion()
{
    return Pragma("data_version");
}

Pragma Pragma::databaseList()
{
    return Pragma("database_list");
}

Pragma Pragma::deferForeignKeys()
{
    return Pragma("defer_foreign_keys");
}

Pragma Pragma::encoding()
{
    return Pragma("encoding");
}

Pragma Pragma::foreignKeyCheck()
{
    return Pragma("foreign_key_check");
}

Pragma Pragma::foreignKeyList()
{
    return Pragma("foreign_key_list");
}

Pragma Pragma::foreignKeys()
{
    return Pragma("foreign_keys");
}

Pragma Pragma::freelistCount()
{
    return Pragma("freelist_count");
}

Pragma Pragma::fullfsync()
{
    return Pragma("fullfsync");
}

Pragma Pragma::functionList()
{
    return Pragma("function_list");
}

Pragma Pragma::ignoreCheckConstraints()
{
    return Pragma("ignore_check_constraints");
}

Pragma Pragma::incrementalVacuum()
{
    return Pragma("incremental_vacuum");
}

Pragma Pragma::indexInfo()
{
    return Pragma("index_info");
}

Pragma Pragma::indexList()
{
    return Pragma("index_list");
}

Pragma Pragma::indexXinfo()
{
    return Pragma("index_xinfo");
}

Pragma Pragma::integrityCheck()
{
    return Pragma("integrity_check");
}

Pragma Pragma::journalMode()
{
    return Pragma("journal_mode");
}

Pragma Pragma::journalSizeLimit()
{
    return Pragma("journal_size_limit");
}

Pragma Pragma::key()
{
    return Pragma("key");
}

Pragma Pragma::kdfIter()
{
    return Pragma("kdf_iter");
}

Pragma Pragma::legacyFileFormat()
{
    return Pragma("legacy_file_format");
}

Pragma Pragma::lockingMode()
{
    return Pragma("locking_mode");
}

Pragma Pragma::maxPageCount()
{
    return Pragma("max_page_count");
}

Pragma Pragma::mmapSize()
{
    return Pragma("mmap_size");
}

Pragma Pragma::moduleList()
{
    return Pragma("module_list");
}

Pragma Pragma::optimize()
{
    return Pragma("optimize");
}

Pragma Pragma::pageCount()
{
    return Pragma("page_count");
}

Pragma Pragma::pageSize()
{
    return Pragma("page_size");
}

Pragma Pragma::parserTrace()
{
    return Pragma("parser_trace");
}

Pragma Pragma::pragmaList()
{
    return Pragma("pragma_list");
}

Pragma Pragma::queryOnly()
{
    return Pragma("query_only");
}

Pragma Pragma::quickCheck()
{
    return Pragma("quick_check");
}

Pragma Pragma::readUncommitted()
{
    return Pragma("read_uncommitted");
}

Pragma Pragma::recursiveTriggers()
{
    return Pragma("recursive_triggers");
}

Pragma Pragma::rekey()
{
    return Pragma("rekey");
}

Pragma Pragma::reverseUnorderedSelects()
{
    return Pragma("reverse_unordered_selects");
}

Pragma Pragma::schemaVersion()
{
    return Pragma("schema_version");
}

Pragma Pragma::secureDelete()
{
    return Pragma("secure_delete");
}

Pragma Pragma::shrinkMemory()
{
    return Pragma("shrink_memory");
}

Pragma Pragma::softHeapLimit()
{
    return Pragma("soft_heap_limit");
}

Pragma Pragma::stats()
{
    return Pragma("stats");
}

Pragma Pragma::synchronous()
{
    return Pragma("synchronous");
}

Pragma Pragma::tableInfo()
{
    return Pragma("table_info");
}

Pragma Pragma::tempStore()
{
    return Pragma("temp_store");
}

Pragma Pragma::threads()
{
    return Pragma("threads");
}

Pragma Pragma::userVersion()
{
    return Pragma("user_version");
}

Pragma Pragma::vdbeAddoptrace()
{
    return Pragma("vdbe_addoptrace");
}

Pragma Pragma::vdbeDebug()
{
    return Pragma("vdbe_debug");
}

Pragma Pragma::vdbeListing()
{
    return Pragma("vdbe_listing");
}

Pragma Pragma::vdbeTrace()
{
    return Pragma("vdbe_trace");
}

Pragma Pragma::walAutocheckpoint()
{
    return Pragma("wal_autocheckpoint");
}

Pragma Pragma::walCheckpoint()
{
    return Pragma("wal_checkpoint");
}

Pragma Pragma::writableSchema()
{
    return Pragma("writable_schema");
}

} // namespace WCDB
