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

#include <WCDB/pragma.hpp>

namespace WCDB {

const Pragma Pragma::ApplicationId("application_id");
const Pragma Pragma::AutoVacuum("auto_vacuum");
const Pragma Pragma::AutomaticIndex("automatic_index");
const Pragma Pragma::BusyTimeout("busy_timeout");
const Pragma Pragma::CacheSize("cache_size");
const Pragma Pragma::CacheSpill("cache_spill");
const Pragma Pragma::CaseSensitiveLike("case_sensitive_like");
const Pragma Pragma::CellSizeCheck("cell_size_check");
const Pragma Pragma::CheckpointFullfsync("checkpoint_fullfsync");
const Pragma Pragma::Cipher("cipher");
const Pragma Pragma::CipherAddRandom("cipher_add_random");
const Pragma Pragma::CipherDefaultKdfIter("cipher_default_kdf_iter");
const Pragma Pragma::CipherDefaultPageSize("cipher_default_page_size");
const Pragma Pragma::CipherDefaultUseHmac("cipher_default_use_hmac");
const Pragma Pragma::CipherMigrate("cipher_migrate");
const Pragma Pragma::CipherProfile("cipher_profile");
const Pragma Pragma::CipherProvider("cipher_provider");
const Pragma Pragma::CipherProviderVersion("cipher_provider_version");
const Pragma Pragma::CipherUseHmac("cipher_use_hmac");
const Pragma Pragma::CipherVersion("cipher_version");
const Pragma Pragma::CipherPageSize("cipher_page_size");
const Pragma Pragma::CollationList("collation_list");
const Pragma Pragma::CompileOptions("compile_options");
const Pragma Pragma::CountChanges("count_changes");
const Pragma Pragma::DataStoreDirectory("data_store_directory");
const Pragma Pragma::DataVersion("data_version");
const Pragma Pragma::DatabaseList("database_list");
const Pragma Pragma::DefaultCacheSize("default_cache_size");
const Pragma Pragma::DeferForeignKeys("defer_foreign_keys");
const Pragma Pragma::EmptyResultCallbacks("empty_result_callbacks");
const Pragma Pragma::Encoding("encoding");
const Pragma Pragma::ForeignKeyCheck("foreign_key_check");
const Pragma Pragma::ForeignKeyList("foreign_key_list");
const Pragma Pragma::ForeignKeys("foreign_keys");
const Pragma Pragma::FreelistCount("freelist_count");
const Pragma Pragma::FullColumnNames("full_column_names");
const Pragma Pragma::Fullfsync("fullfsync");
const Pragma Pragma::IgnoreCheckConstraints("ignore_check_constraints");
const Pragma Pragma::IncrementalVacuum("incremental_vacuum");
const Pragma Pragma::IndexInfo("index_info");
const Pragma Pragma::IndexList("index_list");
const Pragma Pragma::IndexXinfo("index_xinfo");
const Pragma Pragma::IntegrityCheck("integrity_check");
const Pragma Pragma::JournalMode("journal_mode");
const Pragma Pragma::JournalSizeLimit("journal_size_limit");
const Pragma Pragma::Key("key");
const Pragma Pragma::KdfIter("kdf_iter");
const Pragma Pragma::LegacyFileFormat("legacy_file_format");
const Pragma Pragma::LockingMode("locking_mode");
const Pragma Pragma::MaxPageCount("max_page_count");
const Pragma Pragma::MmapSize("mmap_size");
const Pragma Pragma::PageCount("page_count");
const Pragma Pragma::PageSize("page_size");
const Pragma Pragma::ParserTrace("parser_trace");
const Pragma Pragma::QueryOnly("query_only");
const Pragma Pragma::QuickCheck("quick_check");
const Pragma Pragma::ReadUncommitted("read_uncommitted");
const Pragma Pragma::RecursiveTriggers("recursive_triggers");
const Pragma Pragma::Rekey("rekey");
const Pragma Pragma::ReverseUnorderedSelects("reverse_unordered_selects");
const Pragma Pragma::SchemaVersion("schema_version");
const Pragma Pragma::SecureDelete("secure_delete");
const Pragma Pragma::ShortColumnNames("short_column_names");
const Pragma Pragma::ShrinkMemory("shrink_memory");
const Pragma Pragma::SoftHeapLimit("soft_heap_limit");
const Pragma Pragma::Stats("stats");
const Pragma Pragma::Synchronous("synchronous");
const Pragma Pragma::TableInfo("table_info");
const Pragma Pragma::TempStore("temp_store");
const Pragma Pragma::TempStoreDirectory("temp_store_directory");
const Pragma Pragma::Threads("threads");
const Pragma Pragma::UserVersion("user_version");
const Pragma Pragma::VdbeAddoptrace("vdbe_addoptrace");
const Pragma Pragma::VdbeDebug("vdbe_debug");
const Pragma Pragma::VdbeListing("vdbe_listing");
const Pragma Pragma::VdbeTrace("vdbe_trace");
const Pragma Pragma::WalAutocheckpoint("wal_autocheckpoint");
const Pragma Pragma::WalCheckpoint("wal_checkpoint");
const Pragma Pragma::WritableSchema("writable_schema");

Pragma::Pragma(const char *name) : Describable(name)
{
}

const std::string &Pragma::getName() const
{
    return m_description;
}

} //namespace WCDB
