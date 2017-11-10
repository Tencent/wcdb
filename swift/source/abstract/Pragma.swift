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

public enum Pragmas: String {
    case applicationId = "application_id"
    case autoVacuum = "auto_vacuum"
    case automaticIndex = "automatic_index"
    case busyTimeout = "busy_timeout"
    case cacheSize = "cache_size"
    case cacheSpill = "cache_spill"
    case caseSensitiveLike = "case_sensitive_like"
    case cellSizeCheck = "cell_size_check"
    case checkpointFullfsync = "checkpoint_fullfsync"
    case cipher = "cipher"
    case cipherAddRandom = "cipher_add_random"
    case cipherDefaultKdfIter = "cipher_default_kdf_iter"
    case cipherDefaultPageSize = "cipher_default_page_size"
    case cipherDefaultUseHmac = "cipher_default_use_hmac"
    case cipherMigrate = "cipher_migrate"
    case cipherProfile = "cipher_profile"
    case cipherProvider = "cipher_provider"
    case cipherProviderVersion = "cipher_provider_version"
    case cipherUseHmac = "cipher_use_hmac"
    case cipherVersion = "cipher_version"
    case cipherPageSize = "cipher_page_size"
    case collationList = "collation_list"
    case compileOptions = "compile_options"
    case countChanges = "count_changes"
    case dataStoreDirectory = "data_store_directory"
    case dataVersion = "data_version"
    case databaseList = "database_list"
    case defaultCacheSize = "default_cache_size"
    case deferForeignKeys = "defer_foreign_keys"
    case emptyResultCallbacks = "empty_result_callbacks"
    case encoding = "encoding"
    case foreignKeyCheck = "foreign_key_check"
    case foreignKeyList = "foreign_key_list"
    case foreignKeys = "foreign_keys"
    case freelistCount = "freelist_count"
    case fullColumnNames = "full_column_names"
    case fullfsync = "fullfsync"
    case ignoreCheckConstraints = "ignore_check_constraints"
    case incrementalVacuum = "incremental_vacuum"
    case indexInfo = "index_info"
    case indexList = "index_list"
    case indexXinfo = "index_xinfo"
    case integrityCheck = "integrity_check"
    case journalMode = "journal_mode"
    case journalSizeLimit = "journal_size_limit"
    case key = "key"
    case kdfIter = "kdf_iter"
    case legacyFileFormat = "legacy_file_format"
    case lockingMode = "locking_mode"
    case maxPageCount = "max_page_count"
    case mmapSize = "mmap_size"
    case pageCount = "page_count"
    case pageSize = "page_size"
    case parserTrace = "parser_trace"
    case queryOnly = "query_only"
    case quickCheck = "quick_check"
    case readUncommitted = "read_uncommitted"
    case recursiveTriggers = "recursive_triggers"
    case rekey = "rekey"
    case reverseUnorderedSelects = "reverse_unordered_selects"
    case schemaVersion = "schema_version"
    case secureDelete = "secure_delete"
    case shortColumnNames = "short_column_names"
    case shrinkMemory = "shrink_memory"
    case softHeapLimit = "soft_heap_limit"
    case stats = "stats"
    case synchronous = "synchronous"
    case tableInfo = "table_info"
    case tempStore = "temp_store"
    case tempStoreDirectory = "temp_store_directory"
    case threads = "threads"
    case userVersion = "user_version"
    case vdbeAddoptrace = "vdbe_addoptrace"
    case vdbeDebug = "vdbe_debug"
    case vdbeListing = "vdbe_listing"
    case vdbeTrace = "vdbe_trace"
    case walAutocheckpoint = "wal_autocheckpoint"
    case walCheckpoint = "wal_checkpoint"
    case writableSchema = "writable_schema"
    
    func asPragma() -> Pragma {
        return Pragma(named: self.rawValue)
    }
}

public class Pragma: Describable
{
    fileprivate init(named name: String) {
        super.init(name)
    }
    
    fileprivate convenience override init(_ name: String) {
        self.init(named: name)
    }

    public var name: String {
        return description
    }
}

