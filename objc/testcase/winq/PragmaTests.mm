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

#import "WINQAssertion.h"

@interface PragmaTests : BaseTestCase

@end

@implementation PragmaTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::Pragma constructible;
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::Pragma().getType(), WCDB::SQL::Type::Pragma);
    TestCaseAssertEqual(WCDB::Pragma::type, WCDB::SQL::Type::Pragma);
}

- (void)test
{
    auto testingSQL = WCDB::Pragma(@"testPragma");
    auto testingTypes = { WCDB::SQL::Type::Pragma };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testPragma");
}

- (void)test_builtin
{
    TestCaseAssertSQLEqual(WCDB::Pragma::applicationId(), @"application_id");
    TestCaseAssertSQLEqual(WCDB::Pragma::autoVacuum(), @"auto_vacuum");
    TestCaseAssertSQLEqual(WCDB::Pragma::automaticIndex(), @"automatic_index");
    TestCaseAssertSQLEqual(WCDB::Pragma::busyTimeout(), @"busy_timeout");
    TestCaseAssertSQLEqual(WCDB::Pragma::cacheSize(), @"cache_size");
    TestCaseAssertSQLEqual(WCDB::Pragma::cacheSpill(), @"cache_spill");
    TestCaseAssertSQLEqual(WCDB::Pragma::caseSensitiveLike(), @"case_sensitive_like");
    TestCaseAssertSQLEqual(WCDB::Pragma::cellSizeCheck(), @"cell_size_check");
    TestCaseAssertSQLEqual(WCDB::Pragma::checkpointFullfsync(), @"checkpoint_fullfsync");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipher(), @"cipher");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherAddRandom(), @"cipher_add_random");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherDefaultKdfIter(), @"cipher_default_kdf_iter");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherDefaultPageSize(), @"cipher_default_page_size");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherDefaultUseHmac(), @"cipher_default_use_hmac");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherMigrate(), @"cipher_migrate");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherProfile(), @"cipher_profile");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherProvider(), @"cipher_provider");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherProviderVersion(), @"cipher_provider_version");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherUseHmac(), @"cipher_use_hmac");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherVersion(), @"cipher_version");
    TestCaseAssertSQLEqual(WCDB::Pragma::cipherPageSize(), @"cipher_page_size");
    TestCaseAssertSQLEqual(WCDB::Pragma::collationList(), @"collation_list");
    TestCaseAssertSQLEqual(WCDB::Pragma::compileOptions(), @"compile_options");
    TestCaseAssertSQLEqual(WCDB::Pragma::dataVersion(), @"data_version");
    TestCaseAssertSQLEqual(WCDB::Pragma::databaseList(), @"database_list");
    TestCaseAssertSQLEqual(WCDB::Pragma::deferForeignKeys(), @"defer_foreign_keys");
    TestCaseAssertSQLEqual(WCDB::Pragma::encoding(), @"encoding");
    TestCaseAssertSQLEqual(WCDB::Pragma::foreignKeyCheck(), @"foreign_key_check");
    TestCaseAssertSQLEqual(WCDB::Pragma::foreignKeyList(), @"foreign_key_list");
    TestCaseAssertSQLEqual(WCDB::Pragma::foreignKeys(), @"foreign_keys");
    TestCaseAssertSQLEqual(WCDB::Pragma::freelistCount(), @"freelist_count");
    TestCaseAssertSQLEqual(WCDB::Pragma::fullfsync(), @"fullfsync");
    TestCaseAssertSQLEqual(WCDB::Pragma::functionList(), @"function_list");
    TestCaseAssertSQLEqual(WCDB::Pragma::ignoreCheckConstraints(), @"ignore_check_constraints");
    TestCaseAssertSQLEqual(WCDB::Pragma::incrementalVacuum(), @"incremental_vacuum");
    TestCaseAssertSQLEqual(WCDB::Pragma::indexInfo(), @"index_info");
    TestCaseAssertSQLEqual(WCDB::Pragma::indexList(), @"index_list");
    TestCaseAssertSQLEqual(WCDB::Pragma::indexXinfo(), @"index_xinfo");
    TestCaseAssertSQLEqual(WCDB::Pragma::integrityCheck(), @"integrity_check");
    TestCaseAssertSQLEqual(WCDB::Pragma::journalMode(), @"journal_mode");
    TestCaseAssertSQLEqual(WCDB::Pragma::journalSizeLimit(), @"journal_size_limit");
    TestCaseAssertSQLEqual(WCDB::Pragma::key(), @"key");
    TestCaseAssertSQLEqual(WCDB::Pragma::kdfIter(), @"kdf_iter");
    TestCaseAssertSQLEqual(WCDB::Pragma::legacyFileFormat(), @"legacy_file_format");
    TestCaseAssertSQLEqual(WCDB::Pragma::lockingMode(), @"locking_mode");
    TestCaseAssertSQLEqual(WCDB::Pragma::maxPageCount(), @"max_page_count");
    TestCaseAssertSQLEqual(WCDB::Pragma::mmapSize(), @"mmap_size");
    TestCaseAssertSQLEqual(WCDB::Pragma::moduleList(), @"module_list");
    TestCaseAssertSQLEqual(WCDB::Pragma::optimize(), @"optimize");
    TestCaseAssertSQLEqual(WCDB::Pragma::pageCount(), @"page_count");
    TestCaseAssertSQLEqual(WCDB::Pragma::pageSize(), @"page_size");
    TestCaseAssertSQLEqual(WCDB::Pragma::parserTrace(), @"parser_trace");
    TestCaseAssertSQLEqual(WCDB::Pragma::pragmaList(), @"pragma_list");
    TestCaseAssertSQLEqual(WCDB::Pragma::queryOnly(), @"query_only");
    TestCaseAssertSQLEqual(WCDB::Pragma::quickCheck(), @"quick_check");
    TestCaseAssertSQLEqual(WCDB::Pragma::readUncommitted(), @"read_uncommitted");
    TestCaseAssertSQLEqual(WCDB::Pragma::recursiveTriggers(), @"recursive_triggers");
    TestCaseAssertSQLEqual(WCDB::Pragma::rekey(), @"rekey");
    TestCaseAssertSQLEqual(WCDB::Pragma::reverseUnorderedSelects(), @"reverse_unordered_selects");
    TestCaseAssertSQLEqual(WCDB::Pragma::schemaVersion(), @"schema_version");
    TestCaseAssertSQLEqual(WCDB::Pragma::secureDelete(), @"secure_delete");
    TestCaseAssertSQLEqual(WCDB::Pragma::shrinkMemory(), @"shrink_memory");
    TestCaseAssertSQLEqual(WCDB::Pragma::softHeapLimit(), @"soft_heap_limit");
    TestCaseAssertSQLEqual(WCDB::Pragma::stats(), @"stats");
    TestCaseAssertSQLEqual(WCDB::Pragma::synchronous(), @"synchronous");
    TestCaseAssertSQLEqual(WCDB::Pragma::tableInfo(), @"table_info");
    TestCaseAssertSQLEqual(WCDB::Pragma::tempStore(), @"temp_store");
    TestCaseAssertSQLEqual(WCDB::Pragma::threads(), @"threads");
    TestCaseAssertSQLEqual(WCDB::Pragma::userVersion(), @"user_version");
    TestCaseAssertSQLEqual(WCDB::Pragma::vdbeAddoptrace(), @"vdbe_addoptrace");
    TestCaseAssertSQLEqual(WCDB::Pragma::vdbeDebug(), @"vdbe_debug");
    TestCaseAssertSQLEqual(WCDB::Pragma::vdbeListing(), @"vdbe_listing");
    TestCaseAssertSQLEqual(WCDB::Pragma::vdbeTrace(), @"vdbe_trace");
    TestCaseAssertSQLEqual(WCDB::Pragma::walAutocheckpoint(), @"wal_autocheckpoint");
    TestCaseAssertSQLEqual(WCDB::Pragma::walCheckpoint(), @"wal_checkpoint");
    TestCaseAssertSQLEqual(WCDB::Pragma::writableSchema(), @"writable_schema");
}

@end
