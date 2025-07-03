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

import Foundation
import WCDB_Private

public final class Pragma: Identifier<CPPPragma> {

    public init(named name: String) {
        super.init(with: WCDBPragmaCreateWithName(name.cString))
    }

    nonisolated(unsafe) public static let applicationId = Pragma(named: "application_id")
    nonisolated(unsafe) public static let autoVacuum = Pragma(named: "auto_vacuum")
    nonisolated(unsafe) public static let automaticIndex = Pragma(named: "automatic_index")
    nonisolated(unsafe) public static let busyTimeout = Pragma(named: "busy_timeout")
    nonisolated(unsafe) public static let cacheSize = Pragma(named: "cache_size")
    nonisolated(unsafe) public static let cacheSpill = Pragma(named: "cache_spill")
    nonisolated(unsafe) public static let caseSensitiveLike = Pragma(named: "case_sensitive_like")
    nonisolated(unsafe) public static let cellSizeCheck = Pragma(named: "cell_size_check")
    nonisolated(unsafe) public static let checkpointFullfsync = Pragma(named: "checkpoint_fullfsync")
    nonisolated(unsafe) public static let cipher = Pragma(named: "cipher")
    nonisolated(unsafe) public static let cipherAddRandom = Pragma(named: "cipher_add_random")
    nonisolated(unsafe) public static let cipherDefaultKdfIter = Pragma(named: "cipher_default_kdf_iter")
    nonisolated(unsafe) public static let cipherDefaultPageSize = Pragma(named: "cipher_default_page_size")
    nonisolated(unsafe) public static let cipherDefaultUseHmac = Pragma(named: "cipher_default_use_hmac")
    nonisolated(unsafe) public static let cipherMigrate = Pragma(named: "cipher_migrate")
    nonisolated(unsafe) public static let cipherProfile = Pragma(named: "cipher_profile")
    nonisolated(unsafe) public static let cipherProvider = Pragma(named: "cipher_provider")
    nonisolated(unsafe) public static let cipherProviderVersion = Pragma(named: "cipher_provider_version")
    nonisolated(unsafe) public static let cipherUseHmac = Pragma(named: "cipher_use_hmac")
    nonisolated(unsafe) public static let cipherVersion = Pragma(named: "cipher_version")
    nonisolated(unsafe) public static let cipherPageSize = Pragma(named: "cipher_page_size")
    nonisolated(unsafe) public static let cipherSalt = Pragma(named: "cipher_salt")
    nonisolated(unsafe) public static let cipherPlainTextHeaderSize = Pragma(named: "cipher_plaintext_header_size")
    nonisolated(unsafe) public static let collationList = Pragma(named: "collation_list")
    nonisolated(unsafe) public static let compileOptions = Pragma(named: "compile_options")
    nonisolated(unsafe) public static let countChanges = Pragma(named: "count_changes")
    nonisolated(unsafe) public static let dataStoreDirectory = Pragma(named: "data_store_directory")
    nonisolated(unsafe) public static let dataVersion = Pragma(named: "data_version")
    nonisolated(unsafe) public static let databaseList = Pragma(named: "database_list")
    nonisolated(unsafe) public static let defaultCacheSize = Pragma(named: "default_cache_size")
    nonisolated(unsafe) public static let deferForeignKeys = Pragma(named: "defer_foreign_keys")
    nonisolated(unsafe) public static let emptyResultCallbacks = Pragma(named: "empty_result_callbacks")
    nonisolated(unsafe) public static let encoding = Pragma(named: "encoding")
    nonisolated(unsafe) public static let foreignKeyCheck = Pragma(named: "foreign_key_check")
    nonisolated(unsafe) public static let foreignKeyList = Pragma(named: "foreign_key_list")
    nonisolated(unsafe) public static let foreignKeys = Pragma(named: "foreign_keys")
    nonisolated(unsafe) public static let freelistCount = Pragma(named: "freelist_count")
    nonisolated(unsafe) public static let fullColumnNames = Pragma(named: "full_column_names")
    nonisolated(unsafe) public static let fullfsync = Pragma(named: "fullfsync")
    nonisolated(unsafe) public static let ignoreCheckConstraints = Pragma(named: "ignore_check_constraints")
    nonisolated(unsafe) public static let incrementalVacuum = Pragma(named: "incremental_vacuum")
    nonisolated(unsafe) public static let indexInfo = Pragma(named: "index_info")
    nonisolated(unsafe) public static let indexList = Pragma(named: "index_list")
    nonisolated(unsafe) public static let indexXinfo = Pragma(named: "index_xinfo")
    nonisolated(unsafe) public static let integrityCheck = Pragma(named: "integrity_check")
    nonisolated(unsafe) public static let journalMode = Pragma(named: "journal_mode")
    nonisolated(unsafe) public static let journalSizeLimit = Pragma(named: "journal_size_limit")
    nonisolated(unsafe) public static let key = Pragma(named: "key")
    nonisolated(unsafe) public static let kdfIter = Pragma(named: "kdf_iter")
    nonisolated(unsafe) public static let legacyFileFormat = Pragma(named: "legacy_file_format")
    nonisolated(unsafe) public static let lockingMode = Pragma(named: "locking_mode")
    nonisolated(unsafe) public static let maxPageCount = Pragma(named: "max_page_count")
    nonisolated(unsafe) public static let mmapSize = Pragma(named: "mmap_size")
    nonisolated(unsafe) public static let pageCount = Pragma(named: "page_count")
    nonisolated(unsafe) public static let pageSize = Pragma(named: "page_size")
    nonisolated(unsafe) public static let parserTrace = Pragma(named: "parser_trace")
    nonisolated(unsafe) public static let queryOnly = Pragma(named: "query_only")
    nonisolated(unsafe) public static let quickCheck = Pragma(named: "quick_check")
    nonisolated(unsafe) public static let readUncommitted = Pragma(named: "read_uncommitted")
    nonisolated(unsafe) public static let recursiveTriggers = Pragma(named: "recursive_triggers")
    nonisolated(unsafe) public static let rekey = Pragma(named: "rekey")
    nonisolated(unsafe) public static let reverseUnorderedSelects = Pragma(named: "reverse_unordered_selects")
    nonisolated(unsafe) public static let schemaVersion = Pragma(named: "schema_version")
    nonisolated(unsafe) public static let secureDelete = Pragma(named: "secure_delete")
    nonisolated(unsafe) public static let shortColumnNames = Pragma(named: "short_column_names")
    nonisolated(unsafe) public static let shrinkMemory = Pragma(named: "shrink_memory")
    nonisolated(unsafe) public static let softHeapLimit = Pragma(named: "soft_heap_limit")
    nonisolated(unsafe) public static let stats = Pragma(named: "stats")
    nonisolated(unsafe) public static let synchronous = Pragma(named: "synchronous")
    nonisolated(unsafe) public static let tableInfo = Pragma(named: "table_info")
    nonisolated(unsafe) public static let tempStore = Pragma(named: "temp_store")
    nonisolated(unsafe) public static let tempStoreDirectory = Pragma(named: "temp_store_directory")
    nonisolated(unsafe) public static let threads = Pragma(named: "threads")
    nonisolated(unsafe) public static let userVersion = Pragma(named: "user_version")
    nonisolated(unsafe) public static let vdbeAddoptrace = Pragma(named: "vdbe_addoptrace")
    nonisolated(unsafe) public static let vdbeDebug = Pragma(named: "vdbe_debug")
    nonisolated(unsafe) public static let vdbeListing = Pragma(named: "vdbe_listing")
    nonisolated(unsafe) public static let vdbeTrace = Pragma(named: "vdbe_trace")
    nonisolated(unsafe) public static let walAutocheckpoint = Pragma(named: "wal_autocheckpoint")
    nonisolated(unsafe) public static let walCheckpoint = Pragma(named: "wal_checkpoint")
    nonisolated(unsafe) public static let writableSchema = Pragma(named: "writable_schema")
}
