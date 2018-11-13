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

- (void)test_get_type
{
    XCTAssertEqual(WCDB::Pragma().getType(), WCDB::SQL::Type::Pragma);
    XCTAssertEqual(WCDB::Pragma::type, WCDB::SQL::Type::Pragma);
}

- (void)test
{
    auto testingSQL = WCDB::Pragma(@"testPragma");
    auto testingTypes = { WCDB::SQL::Type::Pragma };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"testPragma");
}

- (void)test_builtin
{
    SQLAssertEqual(WCDB::Pragma::applicationId(), @"application_id");
    SQLAssertEqual(WCDB::Pragma::autoVacuum(), @"auto_vacuum");
    SQLAssertEqual(WCDB::Pragma::automaticIndex(), @"automatic_index");
    SQLAssertEqual(WCDB::Pragma::busyTimeout(), @"busy_timeout");
    SQLAssertEqual(WCDB::Pragma::cacheSize(), @"cache_size");
    SQLAssertEqual(WCDB::Pragma::cacheSpill(), @"cache_spill");
    SQLAssertEqual(WCDB::Pragma::caseSensitiveLike(), @"case_sensitive_like");
    SQLAssertEqual(WCDB::Pragma::cellSizeCheck(), @"cell_size_check");
    SQLAssertEqual(WCDB::Pragma::checkpointFullfsync(), @"checkpoint_fullfsync");
    SQLAssertEqual(WCDB::Pragma::cipher(), @"cipher");
    SQLAssertEqual(WCDB::Pragma::cipherAddRandom(), @"cipher_add_random");
    SQLAssertEqual(WCDB::Pragma::cipherDefaultKdfIter(), @"cipher_default_kdf_iter");
    SQLAssertEqual(WCDB::Pragma::cipherDefaultPageSize(), @"cipher_default_page_size");
    SQLAssertEqual(WCDB::Pragma::cipherDefaultUseHmac(), @"cipher_default_use_hmac");
    SQLAssertEqual(WCDB::Pragma::cipherMigrate(), @"cipher_migrate");
    SQLAssertEqual(WCDB::Pragma::cipherProfile(), @"cipher_profile");
    SQLAssertEqual(WCDB::Pragma::cipherProvider(), @"cipher_provider");
    SQLAssertEqual(WCDB::Pragma::cipherProviderVersion(), @"cipher_provider_version");
    SQLAssertEqual(WCDB::Pragma::cipherUseHmac(), @"cipher_use_hmac");
    SQLAssertEqual(WCDB::Pragma::cipherVersion(), @"cipher_version");
    SQLAssertEqual(WCDB::Pragma::cipherPageSize(), @"cipher_page_size");
    SQLAssertEqual(WCDB::Pragma::collationList(), @"collation_list");
    SQLAssertEqual(WCDB::Pragma::compileOptions(), @"compile_options");
    SQLAssertEqual(WCDB::Pragma::dataVersion(), @"data_version");
    SQLAssertEqual(WCDB::Pragma::databaseList(), @"database_list");
    SQLAssertEqual(WCDB::Pragma::deferForeignKeys(), @"defer_foreign_keys");
    SQLAssertEqual(WCDB::Pragma::encoding(), @"encoding");
    SQLAssertEqual(WCDB::Pragma::foreignKeyCheck(), @"foreign_key_check");
    SQLAssertEqual(WCDB::Pragma::foreignKeyList(), @"foreign_key_list");
    SQLAssertEqual(WCDB::Pragma::foreignKeys(), @"foreign_keys");
    SQLAssertEqual(WCDB::Pragma::freelistCount(), @"freelist_count");
    SQLAssertEqual(WCDB::Pragma::fullfsync(), @"fullfsync");
    SQLAssertEqual(WCDB::Pragma::functionList(), @"function_list");
    SQLAssertEqual(WCDB::Pragma::ignoreCheckConstraints(), @"ignore_check_constraints");
    SQLAssertEqual(WCDB::Pragma::incrementalVacuum(), @"incremental_vacuum");
    SQLAssertEqual(WCDB::Pragma::indexInfo(), @"index_info");
    SQLAssertEqual(WCDB::Pragma::indexList(), @"index_list");
    SQLAssertEqual(WCDB::Pragma::indexXinfo(), @"index_xinfo");
    SQLAssertEqual(WCDB::Pragma::integrityCheck(), @"integrity_check");
    SQLAssertEqual(WCDB::Pragma::journalMode(), @"journal_mode");
    SQLAssertEqual(WCDB::Pragma::journalSizeLimit(), @"journal_size_limit");
    SQLAssertEqual(WCDB::Pragma::key(), @"key");
    SQLAssertEqual(WCDB::Pragma::kdfIter(), @"kdf_iter");
    SQLAssertEqual(WCDB::Pragma::legacyFileFormat(), @"legacy_file_format");
    SQLAssertEqual(WCDB::Pragma::lockingMode(), @"locking_mode");
    SQLAssertEqual(WCDB::Pragma::maxPageCount(), @"max_page_count");
    SQLAssertEqual(WCDB::Pragma::mmapSize(), @"mmap_size");
    SQLAssertEqual(WCDB::Pragma::moduleList(), @"module_list");
    SQLAssertEqual(WCDB::Pragma::optimize(), @"optimize");
    SQLAssertEqual(WCDB::Pragma::pageCount(), @"page_count");
    SQLAssertEqual(WCDB::Pragma::pageSize(), @"page_size");
    SQLAssertEqual(WCDB::Pragma::parserTrace(), @"parser_trace");
    SQLAssertEqual(WCDB::Pragma::pragmaList(), @"pragma_list");
    SQLAssertEqual(WCDB::Pragma::queryOnly(), @"query_only");
    SQLAssertEqual(WCDB::Pragma::quickCheck(), @"quick_check");
    SQLAssertEqual(WCDB::Pragma::readUncommitted(), @"read_uncommitted");
    SQLAssertEqual(WCDB::Pragma::recursiveTriggers(), @"recursive_triggers");
    SQLAssertEqual(WCDB::Pragma::rekey(), @"rekey");
    SQLAssertEqual(WCDB::Pragma::reverseUnorderedSelects(), @"reverse_unordered_selects");
    SQLAssertEqual(WCDB::Pragma::schemaVersion(), @"schema_version");
    SQLAssertEqual(WCDB::Pragma::secureDelete(), @"secure_delete");
    SQLAssertEqual(WCDB::Pragma::shrinkMemory(), @"shrink_memory");
    SQLAssertEqual(WCDB::Pragma::softHeapLimit(), @"soft_heap_limit");
    SQLAssertEqual(WCDB::Pragma::stats(), @"stats");
    SQLAssertEqual(WCDB::Pragma::synchronous(), @"synchronous");
    SQLAssertEqual(WCDB::Pragma::tableInfo(), @"table_info");
    SQLAssertEqual(WCDB::Pragma::tempStore(), @"temp_store");
    SQLAssertEqual(WCDB::Pragma::threads(), @"threads");
    SQLAssertEqual(WCDB::Pragma::userVersion(), @"user_version");
    SQLAssertEqual(WCDB::Pragma::vdbeAddoptrace(), @"vdbe_addoptrace");
    SQLAssertEqual(WCDB::Pragma::vdbeDebug(), @"vdbe_debug");
    SQLAssertEqual(WCDB::Pragma::vdbeListing(), @"vdbe_listing");
    SQLAssertEqual(WCDB::Pragma::vdbeTrace(), @"vdbe_trace");
    SQLAssertEqual(WCDB::Pragma::walAutocheckpoint(), @"wal_autocheckpoint");
    SQLAssertEqual(WCDB::Pragma::walCheckpoint(), @"wal_checkpoint");
    SQLAssertEqual(WCDB::Pragma::writableSchema(), @"writable_schema");
}

@end
