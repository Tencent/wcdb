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

const Pragma &Pragma::applicationId()
{
    static const Pragma s_applicationId("application_id");
    return s_applicationId;
}

const Pragma &Pragma::autoVacuum()
{
    static const Pragma s_autoVacuum("auto_vacuum");
    return s_autoVacuum;
}

const Pragma &Pragma::automaticIndex()
{
    static const Pragma s_automaticIndex("automatic_index");
    return s_automaticIndex;
}

const Pragma &Pragma::busyTimeout()
{
    static const Pragma s_busyTimeout("busy_timeout");
    return s_busyTimeout;
}

const Pragma &Pragma::cacheSize()
{
    static const Pragma s_cacheSize("cache_size");
    return s_cacheSize;
}

const Pragma &Pragma::cacheSpill()
{
    static const Pragma s_cacheSpill("cache_spill");
    return s_cacheSpill;
}

const Pragma &Pragma::caseSensitiveLike()
{
    static const Pragma s_caseSensitiveLike("case_sensitive_like");
    return s_caseSensitiveLike;
}

const Pragma &Pragma::cellSizeCheck()
{
    static const Pragma s_cellSizeCheck("cell_size_check");
    return s_cellSizeCheck;
}

const Pragma &Pragma::checkpointFullfsync()
{
    static const Pragma s_checkpointFullfsync("checkpoint_fullfsync");
    return s_checkpointFullfsync;
}

const Pragma &Pragma::cipher()
{
    static const Pragma s_cipher("cipher");
    return s_cipher;
}

const Pragma &Pragma::cipherAddRandom()
{
    static const Pragma s_cipherAddRandom("cipher_add_random");
    return s_cipherAddRandom;
}

const Pragma &Pragma::cipherDefaultKdfIter()
{
    static const Pragma s_cipherDefaultKdfIter("cipher_default_kdf_iter");
    return s_cipherDefaultKdfIter;
}

const Pragma &Pragma::cipherDefaultPageSize()
{
    static const Pragma s_cipherDefaultPageSize("cipher_default_page_size");
    return s_cipherDefaultPageSize;
}

const Pragma &Pragma::cipherDefaultUseHmac()
{
    static const Pragma s_cipherDefaultUseHmac("cipher_default_use_hmac");
    return s_cipherDefaultUseHmac;
}

const Pragma &Pragma::cipherMigrate()
{
    static const Pragma s_cipherMigrate("cipher_migrate");
    return s_cipherMigrate;
}

const Pragma &Pragma::cipherProfile()
{
    static const Pragma s_cipherProfile("cipher_profile");
    return s_cipherProfile;
}

const Pragma &Pragma::cipherProvider()
{
    static const Pragma s_cipherProvider("cipher_provider");
    return s_cipherProvider;
}

const Pragma &Pragma::cipherProviderVersion()
{
    static const Pragma s_cipherProviderVersion("cipher_provider_version");
    return s_cipherProviderVersion;
}

const Pragma &Pragma::cipherUseHmac()
{
    static const Pragma s_cipherUseHmac("cipher_use_hmac");
    return s_cipherUseHmac;
}

const Pragma &Pragma::cipherVersion()
{
    static const Pragma s_cipherVersion("cipher_version");
    return s_cipherVersion;
}

const Pragma &Pragma::cipherPageSize()
{
    static const Pragma s_cipherPageSize("cipher_page_size");
    return s_cipherPageSize;
}

const Pragma &Pragma::collationList()
{
    static const Pragma s_collationList("collation_list");
    return s_collationList;
}

const Pragma &Pragma::compileOptions()
{
    static const Pragma s_compileOptions("compile_options");
    return s_compileOptions;
}

const Pragma &Pragma::dataVersion()
{
    static const Pragma s_dataVersion("data_version");
    return s_dataVersion;
}

const Pragma &Pragma::databaseList()
{
    static const Pragma s_databaseList("database_list");
    return s_databaseList;
}

const Pragma &Pragma::deferForeignKeys()
{
    static const Pragma s_deferForeignKeys("defer_foreign_keys");
    return s_deferForeignKeys;
}

const Pragma &Pragma::encoding()
{
    static const Pragma s_encoding("encoding");
    return s_encoding;
}

const Pragma &Pragma::foreignKeyCheck()
{
    static const Pragma s_foreignKeyCheck("foreign_key_check");
    return s_foreignKeyCheck;
}

const Pragma &Pragma::foreignKeyList()
{
    static const Pragma s_foreignKeyList("foreign_key_list");
    return s_foreignKeyList;
}

const Pragma &Pragma::foreignKeys()
{
    static const Pragma s_foreignKeys("foreign_keys");
    return s_foreignKeys;
}

const Pragma &Pragma::freelistCount()
{
    static const Pragma s_freelistCount("freelist_count");
    return s_freelistCount;
}

const Pragma &Pragma::fullfsync()
{
    static const Pragma s_fullfsync("fullfsync");
    return s_fullfsync;
}

const Pragma &Pragma::functionList()
{
    static const Pragma s_functionList("function_list");
    return s_functionList;
}

const Pragma &Pragma::ignoreCheckConstraints()
{
    static const Pragma s_ignoreCheckConstraints("ignore_check_constraints");
    return s_ignoreCheckConstraints;
}

const Pragma &Pragma::incrementalVacuum()
{
    static const Pragma s_incrementalVacuum("incremental_vacuum");
    return s_incrementalVacuum;
}

const Pragma &Pragma::indexInfo()
{
    static const Pragma s_indexInfo("index_info");
    return s_indexInfo;
}

const Pragma &Pragma::indexList()
{
    static const Pragma s_indexList("index_list");
    return s_indexList;
}

const Pragma &Pragma::indexXinfo()
{
    static const Pragma s_indexXinfo("index_xinfo");
    return s_indexXinfo;
}

const Pragma &Pragma::integrityCheck()
{
    static const Pragma s_integrityCheck("integrity_check");
    return s_integrityCheck;
}

const Pragma &Pragma::journalMode()
{
    static const Pragma s_journalMode("journal_mode");
    return s_journalMode;
}

const Pragma &Pragma::journalSizeLimit()
{
    static const Pragma s_journalSizeLimit("journal_size_limit");
    return s_journalSizeLimit;
}

const Pragma &Pragma::key()
{
    static const Pragma s_Key("key");
    return s_Key;
}

const Pragma &Pragma::kdfIter()
{
    static const Pragma s_KdfIter("kdf_iter");
    return s_KdfIter;
}

const Pragma &Pragma::legacyFileFormat()
{
    static const Pragma s_legacyFileFormat("legacy_file_format");
    return s_legacyFileFormat;
}

const Pragma &Pragma::lockingMode()
{
    static const Pragma s_LockingMode("locking_mode");
    return s_LockingMode;
}

const Pragma &Pragma::maxPageCount()
{
    static const Pragma s_maxPageCount("max_page_count");
    return s_maxPageCount;
}

const Pragma &Pragma::mmapSize()
{
    static const Pragma s_mmapSize("mmap_size");
    return s_mmapSize;
}

const Pragma &Pragma::moduleList()
{
    static const Pragma s_moduleList("module_list");
    return s_moduleList;
}

const Pragma &Pragma::optimize()
{
    static const Pragma s_optimize("optimize");
    return s_optimize;
}

const Pragma &Pragma::pageCount()
{
    static const Pragma s_pageCount("page_count");
    return s_pageCount;
}

const Pragma &Pragma::pageSize()
{
    static const Pragma s_pageSize("page_size");
    return s_pageSize;
}

const Pragma &Pragma::parserTrace()
{
    static const Pragma s_parserTrace("parser_trace");
    return s_parserTrace;
}

const Pragma &Pragma::pragmaList()
{
    static const Pragma s_pragmaList("pragma_list");
    return s_pragmaList;
}

const Pragma &Pragma::queryOnly()
{
    static const Pragma s_queryOnly("query_only");
    return s_queryOnly;
}

const Pragma &Pragma::quickCheck()
{
    static const Pragma s_quickCheck("quick_check");
    return s_quickCheck;
}

const Pragma &Pragma::readUncommitted()
{
    static const Pragma s_readUncommitted("read_uncommitted");
    return s_readUncommitted;
}

const Pragma &Pragma::recursiveTriggers()
{
    static const Pragma s_recursiveTriggers("recursive_triggers");
    return s_recursiveTriggers;
}

const Pragma &Pragma::rekey()
{
    static const Pragma s_rekey("rekey");
    return s_rekey;
}

const Pragma &Pragma::reverseUnorderedSelects()
{
    static const Pragma s_reverseUnorderedSelects("reverse_unordered_selects");
    return s_reverseUnorderedSelects;
}

const Pragma &Pragma::schemaVersion()
{
    static const Pragma s_schemaVersion("schema_version");
    return s_schemaVersion;
}

const Pragma &Pragma::secureDelete()
{
    static const Pragma s_secureDelete("secure_delete");
    return s_secureDelete;
}

const Pragma &Pragma::shrinkMemory()
{
    static const Pragma s_shrinkMemory("shrink_memory");
    return s_shrinkMemory;
}

const Pragma &Pragma::softHeapLimit()
{
    static const Pragma s_softHeapLimit("soft_heap_limit");
    return s_softHeapLimit;
}

const Pragma &Pragma::stats()
{
    static const Pragma s_stats("stats");
    return s_stats;
}

const Pragma &Pragma::synchronous()
{
    static const Pragma s_synchronous("synchronous");
    return s_synchronous;
}

const Pragma &Pragma::tableInfo()
{
    static const Pragma s_tableInfo("table_info");
    return s_tableInfo;
}

const Pragma &Pragma::tempStore()
{
    static const Pragma s_tempStore("temp_store");
    return s_tempStore;
}

const Pragma &Pragma::threads()
{
    static const Pragma s_threads("threads");
    return s_threads;
}

const Pragma &Pragma::userVersion()
{
    static const Pragma s_userVersion("user_version");
    return s_userVersion;
}

const Pragma &Pragma::vdbeAddoptrace()
{
    static const Pragma s_vdbeAddoptrace("vdbe_addoptrace");
    return s_vdbeAddoptrace;
}

const Pragma &Pragma::vdbeDebug()
{
    static const Pragma s_vdbeDebug("vdbe_debug");
    return s_vdbeDebug;
}

const Pragma &Pragma::vdbeListing()
{
    static const Pragma s_vdbeListing("vdbe_listing");
    return s_vdbeListing;
}

const Pragma &Pragma::vdbeTrace()
{
    static const Pragma s_vdbeTrace("vdbe_trace");
    return s_vdbeTrace;
}

const Pragma &Pragma::walAutocheckpoint()
{
    static const Pragma s_walAutocheckpoint("wal_autocheckpoint");
    return s_walAutocheckpoint;
}

const Pragma &Pragma::walCheckpoint()
{
    static const Pragma s_walCheckpoint("wal_checkpoint");
    return s_walCheckpoint;
}

const Pragma &Pragma::writableSchema()
{
    static const Pragma s_writableSchema("writable_schema");
    return s_writableSchema;
}

Pragma::Pragma(const std::string &name)
{
    Lang::Pragma &lang = getMutableLang();
    lang.name.assign(name);
}

} // namespace WCDB
