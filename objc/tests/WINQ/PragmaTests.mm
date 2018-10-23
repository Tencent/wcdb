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

#import "WINQTestCase.h"
#import <WCDB/WCDB.h>

@interface PragmaTests : WINQTestCase

@end

@implementation PragmaTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::Pragma constructible __attribute((unused));
}

- (void)test
{
    auto testingSQL = WCDB::Pragma(@"testPragma");
    auto testingTypes = { WCDB::SQL::Type::Pragma };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testPragma");
}

- (void)test_builtin
{
    WINQAssertEqual(WCDB::Pragma::applicationId(), @"application_id");
    WINQAssertEqual(WCDB::Pragma::autoVacuum(), @"auto_vacuum");
    WINQAssertEqual(WCDB::Pragma::automaticIndex(), @"automatic_index");
    WINQAssertEqual(WCDB::Pragma::busyTimeout(), @"busy_timeout");
    WINQAssertEqual(WCDB::Pragma::cacheSize(), @"cache_size");
    WINQAssertEqual(WCDB::Pragma::cacheSpill(), @"cache_spill");
    WINQAssertEqual(WCDB::Pragma::caseSensitiveLike(), @"case_sensitive_like");
    WINQAssertEqual(WCDB::Pragma::cellSizeCheck(), @"cell_size_check");
    WINQAssertEqual(WCDB::Pragma::checkpointFullfsync(), @"checkpoint_fullfsync");
    WINQAssertEqual(WCDB::Pragma::cipher(), @"cipher");
    WINQAssertEqual(WCDB::Pragma::cipherAddRandom(), @"cipher_add_random");
    WINQAssertEqual(WCDB::Pragma::cipherDefaultKdfIter(), @"cipher_default_kdf_iter");
    WINQAssertEqual(WCDB::Pragma::cipherDefaultPageSize(), @"cipher_default_page_size");
    WINQAssertEqual(WCDB::Pragma::cipherDefaultUseHmac(), @"cipher_default_use_hmac");
    WINQAssertEqual(WCDB::Pragma::cipherMigrate(), @"cipher_migrate");
    WINQAssertEqual(WCDB::Pragma::cipherProfile(), @"cipher_profile");
    WINQAssertEqual(WCDB::Pragma::cipherProvider(), @"cipher_provider");
    WINQAssertEqual(WCDB::Pragma::cipherProviderVersion(), @"cipher_provider_version");
    WINQAssertEqual(WCDB::Pragma::cipherUseHmac(), @"cipher_use_hmac");
    WINQAssertEqual(WCDB::Pragma::cipherVersion(), @"cipher_version");
    WINQAssertEqual(WCDB::Pragma::cipherPageSize(), @"cipher_page_size");
    WINQAssertEqual(WCDB::Pragma::collationList(), @"collation_list");
    WINQAssertEqual(WCDB::Pragma::compileOptions(), @"compile_options");
    WINQAssertEqual(WCDB::Pragma::dataVersion(), @"data_version");
    WINQAssertEqual(WCDB::Pragma::databaseList(), @"database_list");
    WINQAssertEqual(WCDB::Pragma::deferForeignKeys(), @"defer_foreign_keys");
    WINQAssertEqual(WCDB::Pragma::encoding(), @"encoding");
    WINQAssertEqual(WCDB::Pragma::foreignKeyCheck(), @"foreign_key_check");
    WINQAssertEqual(WCDB::Pragma::foreignKeyList(), @"foreign_key_list");
    WINQAssertEqual(WCDB::Pragma::foreignKeys(), @"foreign_keys");
    WINQAssertEqual(WCDB::Pragma::freelistCount(), @"freelist_count");
    WINQAssertEqual(WCDB::Pragma::fullfsync(), @"fullfsync");
    WINQAssertEqual(WCDB::Pragma::functionList(), @"function_list");
    WINQAssertEqual(WCDB::Pragma::ignoreCheckConstraints(), @"ignore_check_constraints");
    WINQAssertEqual(WCDB::Pragma::incrementalVacuum(), @"incremental_vacuum");
    WINQAssertEqual(WCDB::Pragma::indexInfo(), @"index_info");
    WINQAssertEqual(WCDB::Pragma::indexList(), @"index_list");
    WINQAssertEqual(WCDB::Pragma::indexXinfo(), @"index_xinfo");
    WINQAssertEqual(WCDB::Pragma::integrityCheck(), @"integrity_check");
    WINQAssertEqual(WCDB::Pragma::journalMode(), @"journal_mode");
    WINQAssertEqual(WCDB::Pragma::journalSizeLimit(), @"journal_size_limit");
    WINQAssertEqual(WCDB::Pragma::key(), @"key");
    WINQAssertEqual(WCDB::Pragma::kdfIter(), @"kdf_iter");
    WINQAssertEqual(WCDB::Pragma::legacyFileFormat(), @"legacy_file_format");
    WINQAssertEqual(WCDB::Pragma::lockingMode(), @"locking_mode");
    WINQAssertEqual(WCDB::Pragma::maxPageCount(), @"max_page_count");
    WINQAssertEqual(WCDB::Pragma::mmapSize(), @"mmap_size");
    WINQAssertEqual(WCDB::Pragma::moduleList(), @"module_list");
    WINQAssertEqual(WCDB::Pragma::optimize(), @"optimize");
    WINQAssertEqual(WCDB::Pragma::pageCount(), @"page_count");
    WINQAssertEqual(WCDB::Pragma::pageSize(), @"page_size");
    WINQAssertEqual(WCDB::Pragma::parserTrace(), @"parser_trace");
    WINQAssertEqual(WCDB::Pragma::pragmaList(), @"pragma_list");
    WINQAssertEqual(WCDB::Pragma::queryOnly(), @"query_only");
    WINQAssertEqual(WCDB::Pragma::quickCheck(), @"quick_check");
    WINQAssertEqual(WCDB::Pragma::readUncommitted(), @"read_uncommitted");
    WINQAssertEqual(WCDB::Pragma::recursiveTriggers(), @"recursive_triggers");
    WINQAssertEqual(WCDB::Pragma::rekey(), @"rekey");
    WINQAssertEqual(WCDB::Pragma::reverseUnorderedSelects(), @"reverse_unordered_selects");
    WINQAssertEqual(WCDB::Pragma::schemaVersion(), @"schema_version");
    WINQAssertEqual(WCDB::Pragma::secureDelete(), @"secure_delete");
    WINQAssertEqual(WCDB::Pragma::shrinkMemory(), @"shrink_memory");
    WINQAssertEqual(WCDB::Pragma::softHeapLimit(), @"soft_heap_limit");
    WINQAssertEqual(WCDB::Pragma::stats(), @"stats");
    WINQAssertEqual(WCDB::Pragma::synchronous(), @"synchronous");
    WINQAssertEqual(WCDB::Pragma::tableInfo(), @"table_info");
    WINQAssertEqual(WCDB::Pragma::tempStore(), @"temp_store");
    WINQAssertEqual(WCDB::Pragma::threads(), @"threads");
    WINQAssertEqual(WCDB::Pragma::userVersion(), @"user_version");
    WINQAssertEqual(WCDB::Pragma::vdbeAddoptrace(), @"vdbe_addoptrace");
    WINQAssertEqual(WCDB::Pragma::vdbeDebug(), @"vdbe_debug");
    WINQAssertEqual(WCDB::Pragma::vdbeListing(), @"vdbe_listing");
    WINQAssertEqual(WCDB::Pragma::vdbeTrace(), @"vdbe_trace");
    WINQAssertEqual(WCDB::Pragma::walAutocheckpoint(), @"wal_autocheckpoint");
    WINQAssertEqual(WCDB::Pragma::walCheckpoint(), @"wal_checkpoint");
    WINQAssertEqual(WCDB::Pragma::writableSchema(), @"writable_schema");
}

@end
