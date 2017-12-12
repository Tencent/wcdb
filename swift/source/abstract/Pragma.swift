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

public struct Pragma: Describable {
    public private(set) var description: String

    fileprivate init(named name: String) {
        description = name
    }

    public static let applicationId = Pragma(named: "application_id")
    public static let autoVacuum = Pragma(named: "auto_vacuum")
    public static let automaticIndex = Pragma(named: "automatic_index")
    public static let busyTimeout = Pragma(named: "busy_timeout")
    public static let cacheSize = Pragma(named: "cache_size")
    public static let cacheSpill = Pragma(named: "cache_spill")
    public static let caseSensitiveLike = Pragma(named: "case_sensitive_like")
    public static let cellSizeCheck = Pragma(named: "cell_size_check")
    public static let checkpointFullfsync = Pragma(named: "checkpoint_fullfsync")
    public static let cipher = Pragma(named: "cipher")
    public static let cipherAddRandom = Pragma(named: "cipher_add_random")
    public static let cipherDefaultKdfIter = Pragma(named: "cipher_default_kdf_iter")
    public static let cipherDefaultPageSize = Pragma(named: "cipher_default_page_size")
    public static let cipherDefaultUseHmac = Pragma(named: "cipher_default_use_hmac")
    public static let cipherMigrate = Pragma(named: "cipher_migrate")
    public static let cipherProfile = Pragma(named: "cipher_profile")
    public static let cipherProvider = Pragma(named: "cipher_provider")
    public static let cipherProviderVersion = Pragma(named: "cipher_provider_version")
    public static let cipherUseHmac = Pragma(named: "cipher_use_hmac")
    public static let cipherVersion = Pragma(named: "cipher_version")
    public static let cipherPageSize = Pragma(named: "cipher_page_size")
    public static let collationList = Pragma(named: "collation_list")
    public static let compileOptions = Pragma(named: "compile_options")
    public static let countChanges = Pragma(named: "count_changes")
    public static let dataStoreDirectory = Pragma(named: "data_store_directory")
    public static let dataVersion = Pragma(named: "data_version")
    public static let databaseList = Pragma(named: "database_list")
    public static let defaultCacheSize = Pragma(named: "default_cache_size")
    public static let deferForeignKeys = Pragma(named: "defer_foreign_keys")
    public static let emptyResultCallbacks = Pragma(named: "empty_result_callbacks")
    public static let encoding = Pragma(named: "encoding")
    public static let foreignKeyCheck = Pragma(named: "foreign_key_check")
    public static let foreignKeyList = Pragma(named: "foreign_key_list")
    public static let foreignKeys = Pragma(named: "foreign_keys")
    public static let freelistCount = Pragma(named: "freelist_count")
    public static let fullColumnNames = Pragma(named: "full_column_names")
    public static let fullfsync = Pragma(named: "fullfsync")
    public static let ignoreCheckConstraints = Pragma(named: "ignore_check_constraints")
    public static let incrementalVacuum = Pragma(named: "incremental_vacuum")
    public static let indexInfo = Pragma(named: "index_info")
    public static let indexList = Pragma(named: "index_list")
    public static let indexXinfo = Pragma(named: "index_xinfo")
    public static let integrityCheck = Pragma(named: "integrity_check")
    public static let journalMode = Pragma(named: "journal_mode")
    public static let journalSizeLimit = Pragma(named: "journal_size_limit")
    public static let key = Pragma(named: "key")
    public static let kdfIter = Pragma(named: "kdf_iter")
    public static let legacyFileFormat = Pragma(named: "legacy_file_format")
    public static let lockingMode = Pragma(named: "locking_mode")
    public static let maxPageCount = Pragma(named: "max_page_count")
    public static let mmapSize = Pragma(named: "mmap_size")
    public static let pageCount = Pragma(named: "page_count")
    public static let pageSize = Pragma(named: "page_size")
    public static let parserTrace = Pragma(named: "parser_trace")
    public static let queryOnly = Pragma(named: "query_only")
    public static let quickCheck = Pragma(named: "quick_check")
    public static let readUncommitted = Pragma(named: "read_uncommitted")
    public static let recursiveTriggers = Pragma(named: "recursive_triggers")
    public static let rekey = Pragma(named: "rekey")
    public static let reverseUnorderedSelects = Pragma(named: "reverse_unordered_selects")
    public static let schemaVersion = Pragma(named: "schema_version")
    public static let secureDelete = Pragma(named: "secure_delete")
    public static let shortColumnNames = Pragma(named: "short_column_names")
    public static let shrinkMemory = Pragma(named: "shrink_memory")
    public static let softHeapLimit = Pragma(named: "soft_heap_limit")
    public static let stats = Pragma(named: "stats")
    public static let synchronous = Pragma(named: "synchronous")
    public static let tableInfo = Pragma(named: "table_info")
    public static let tempStore = Pragma(named: "temp_store")
    public static let tempStoreDirectory = Pragma(named: "temp_store_directory")
    public static let threads = Pragma(named: "threads")
    public static let userVersion = Pragma(named: "user_version")
    public static let vdbeAddoptrace = Pragma(named: "vdbe_addoptrace")
    public static let vdbeDebug = Pragma(named: "vdbe_debug")
    public static let vdbeListing = Pragma(named: "vdbe_listing")
    public static let vdbeTrace = Pragma(named: "vdbe_trace")
    public static let walAutocheckpoint = Pragma(named: "wal_autocheckpoint")
    public static let walCheckpoint = Pragma(named: "wal_checkpoint")
    public static let writableSchema = Pragma(named: "writable_schema")
}
