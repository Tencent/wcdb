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

#import "WTCAssert.h"
#import <WINQ/abstract.h>

@interface WTCPragmaTests : XCTestCase

@end

@implementation WTCPragmaTests

- (void)testPragmaSQLite
{
    WINQAssertEqual(WCDB::Pragma::ApplicationId, @"application_id");

    WINQAssertEqual(WCDB::Pragma::AutoVacuum, @"auto_vacuum");

    WINQAssertEqual(WCDB::Pragma::AutomaticIndex, @"automatic_index");

    WINQAssertEqual(WCDB::Pragma::BusyTimeout, @"busy_timeout");

    WINQAssertEqual(WCDB::Pragma::CacheSize, @"cache_size");

    WINQAssertEqual(WCDB::Pragma::CacheSpill, @"cache_spill");

    WINQAssertEqual(WCDB::Pragma::CaseSensitiveLike, @"case_sensitive_like");

    WINQAssertEqual(WCDB::Pragma::CellSizeCheck, @"cell_size_check");

    WINQAssertEqual(WCDB::Pragma::CheckpointFullfsync, @"checkpoint_fullfsync");

    WINQAssertEqual(WCDB::Pragma::CollationList, @"collation_list");

    WINQAssertEqual(WCDB::Pragma::CompileOptions, @"compile_options");

    WINQAssertEqual(WCDB::Pragma::DataVersion, @"data_version");

    WINQAssertEqual(WCDB::Pragma::DatabaseList, @"database_list");

    WINQAssertEqual(WCDB::Pragma::DeferForeignKeys, @"defer_foreign_keys");

    WINQAssertEqual(WCDB::Pragma::Encoding, @"encoding");

    WINQAssertEqual(WCDB::Pragma::ForeignKeyCheck, @"foreign_key_check");

    WINQAssertEqual(WCDB::Pragma::ForeignKeyList, @"foreign_key_list");

    WINQAssertEqual(WCDB::Pragma::ForeignKeys, @"foreign_keys");

    WINQAssertEqual(WCDB::Pragma::FreelistCount, @"freelist_count");

    WINQAssertEqual(WCDB::Pragma::Fullfsync, @"fullfsync");

    WINQAssertEqual(WCDB::Pragma::FunctionList, @"function_list");

    WINQAssertEqual(WCDB::Pragma::IgnoreCheckConstraints, @"ignore_check_constraints");

    WINQAssertEqual(WCDB::Pragma::IncrementalVacuum, @"incremental_vacuum");

    WINQAssertEqual(WCDB::Pragma::IndexInfo, @"index_info");

    WINQAssertEqual(WCDB::Pragma::IndexList, @"index_list");

    WINQAssertEqual(WCDB::Pragma::IndexXinfo, @"index_xinfo");

    WINQAssertEqual(WCDB::Pragma::IntegrityCheck, @"integrity_check");

    WINQAssertEqual(WCDB::Pragma::JournalMode, @"journal_mode");

    WINQAssertEqual(WCDB::Pragma::JournalSizeLimit, @"journal_size_limit");

    WINQAssertEqual(WCDB::Pragma::LegacyFileFormat, @"legacy_file_format");

    WINQAssertEqual(WCDB::Pragma::LockingMode, @"locking_mode");

    WINQAssertEqual(WCDB::Pragma::MaxPageCount, @"max_page_count");

    WINQAssertEqual(WCDB::Pragma::MmapSize, @"mmap_size");

    WINQAssertEqual(WCDB::Pragma::ModuleList, @"module_list");

    WINQAssertEqual(WCDB::Pragma::Optimize, @"optimize");

    WINQAssertEqual(WCDB::Pragma::PageCount, @"page_count");

    WINQAssertEqual(WCDB::Pragma::PageSize, @"page_size");

    WINQAssertEqual(WCDB::Pragma::ParserTrace, @"parser_trace");

    WINQAssertEqual(WCDB::Pragma::PragmaList, @"pragma_list");

    WINQAssertEqual(WCDB::Pragma::QueryOnly, @"query_only");

    WINQAssertEqual(WCDB::Pragma::QuickCheck, @"quick_check");

    WINQAssertEqual(WCDB::Pragma::ReadUncommitted, @"read_uncommitted");

    WINQAssertEqual(WCDB::Pragma::RecursiveTriggers, @"recursive_triggers");

    WINQAssertEqual(WCDB::Pragma::ReverseUnorderedSelects, @"reverse_unordered_selects");

    WINQAssertEqual(WCDB::Pragma::SchemaVersion, @"schema_version");

    WINQAssertEqual(WCDB::Pragma::SecureDelete, @"secure_delete");

    WINQAssertEqual(WCDB::Pragma::ShrinkMemory, @"shrink_memory");

    WINQAssertEqual(WCDB::Pragma::SoftHeapLimit, @"soft_heap_limit");

    WINQAssertEqual(WCDB::Pragma::Stats, @"stats");

    WINQAssertEqual(WCDB::Pragma::Synchronous, @"synchronous");

    WINQAssertEqual(WCDB::Pragma::TableInfo, @"table_info");

    WINQAssertEqual(WCDB::Pragma::TempStore, @"temp_store");

    WINQAssertEqual(WCDB::Pragma::Threads, @"threads");

    WINQAssertEqual(WCDB::Pragma::UserVersion, @"user_version");

    WINQAssertEqual(WCDB::Pragma::VdbeAddoptrace, @"vdbe_addoptrace");

    WINQAssertEqual(WCDB::Pragma::VdbeDebug, @"vdbe_debug");

    WINQAssertEqual(WCDB::Pragma::VdbeListing, @"vdbe_listing");

    WINQAssertEqual(WCDB::Pragma::VdbeTrace, @"vdbe_trace");

    WINQAssertEqual(WCDB::Pragma::WalAutocheckpoint, @"wal_autocheckpoint");

    WINQAssertEqual(WCDB::Pragma::WalCheckpoint, @"wal_checkpoint");

    WINQAssertEqual(WCDB::Pragma::WritableSchema, @"writable_schema");
}

- (void)testPragmaSQLCipher
{
    WINQAssertEqual(WCDB::Pragma::Key, @"key");

    WINQAssertEqual(WCDB::Pragma::Cipher, @"cipher");

    WINQAssertEqual(WCDB::Pragma::KdfIter, @"kdf_iter");

    WINQAssertEqual(WCDB::Pragma::CipherDefaultKdfIter, @"cipher_default_kdf_iter");

    WINQAssertEqual(WCDB::Pragma::CipherPageSize, @"cipher_page_size");

    WINQAssertEqual(WCDB::Pragma::CipherDefaultPageSize, @"cipher_default_page_size");

    WINQAssertEqual(WCDB::Pragma::CipherUseHmac, @"cipher_use_hmac");

    WINQAssertEqual(WCDB::Pragma::CipherDefaultUseHmac, @"cipher_default_use_hmac");

    WINQAssertEqual(WCDB::Pragma::Rekey, @"rekey");

    WINQAssertEqual(WCDB::Pragma::CipherMigrate, @"cipher_migrate");

    WINQAssertEqual(WCDB::Pragma::CipherProfile, @"cipher_profile");

    WINQAssertEqual(WCDB::Pragma::CipherAddRandom, @"cipher_add_random");

    WINQAssertEqual(WCDB::Pragma::CipherProvider, @"cipher_provider");

    WINQAssertEqual(WCDB::Pragma::CipherProviderVersion, @"cipher_provider_version");

    WINQAssertEqual(WCDB::Pragma::CipherVersion, @"cipher_version");
}

@end
