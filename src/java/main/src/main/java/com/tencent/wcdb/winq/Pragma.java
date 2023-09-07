// Created by chenqiuwen on 2023/4/9.
//

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

package com.tencent.wcdb.winq;

public class Pragma extends Identifier{
    @Override
    protected CPPType getType() {
        return CPPType.Pragma;
    }

    public Pragma(String name) {
        cppObj = createCppObj(name);
    }

    private static native long createCppObj(String name);

    public static final Pragma applicationId = new Pragma("application_id");

    public static final Pragma autoVacuum = new Pragma("auto_vacuum");

    public static final Pragma automaticIndex = new Pragma("automatic_index");

    public static final Pragma busyTimeout = new Pragma("busy_timeout");

    public static final Pragma cacheSize = new Pragma("cache_size");

    public static final Pragma cacheSpill = new Pragma("cache_spill");

    public static final Pragma caseSensitiveLike = new Pragma("case_sensitive_like");

    public static final Pragma cellSizeCheck = new Pragma("cell_size_check");

    public static final Pragma checkpointFullfsync = new Pragma("checkpoint_fullfsync");

    public static final Pragma functionList = new Pragma("function_list");

    public static final Pragma cipher = new Pragma("cipher");

    public static final Pragma cipherAddRandom = new Pragma("cipher_add_random");

    public static final Pragma cipherDefaultKdfIter = new Pragma("cipher_default_kdf_iter");

    public static final Pragma cipherDefaultPageSize = new Pragma("cipher_default_page_size");

    public static final Pragma cipherDefaultUseHmac = new Pragma("cipher_default_use_hmac");

    public static final Pragma cipherMigrate = new Pragma("cipher_migrate");

    public static final Pragma cipherProfile = new Pragma("cipher_profile");

    public static final Pragma cipherProvider = new Pragma("cipher_provider");

    public static final Pragma cipherProviderVersion = new Pragma("cipher_provider_version");

    public static final Pragma cipherUseHmac = new Pragma("cipher_use_hmac");

    public static final Pragma cipherVersion = new Pragma("cipher_version");

    public static final Pragma cipherPageSize = new Pragma("cipher_page_size");

    public static final Pragma collationList = new Pragma("collation_list");

    public static final Pragma compileOptions = new Pragma("compile_options");

    public static final Pragma countChanges = new Pragma("count_changes");

    public static final Pragma dataStoreDirectory = new Pragma("data_store_directory");

    public static final Pragma dataVersion = new Pragma("data_version");

    public static final Pragma databaseList = new Pragma("database_list");

    public static final Pragma defaultCacheSize = new Pragma("default_cache_size");

    public static final Pragma deferForeignKeys = new Pragma("defer_foreign_keys");

    public static final Pragma emptyResultCallbacks = new Pragma("empty_result_callbacks");

    public static final Pragma encoding = new Pragma("encoding");

    public static final Pragma foreignKeyCheck = new Pragma("foreign_key_check");

    public static final Pragma foreignKeyList = new Pragma("foreign_key_list");

    public static final Pragma foreignKeys = new Pragma("foreign_keys");

    public static final Pragma freelistCount = new Pragma("freelist_count");

    public static final Pragma fullColumnNames = new Pragma("full_column_names");

    public static final Pragma fullfsync = new Pragma("fullfsync");

    public static final Pragma ignoreCheckConstraints = new Pragma("ignore_check_constraints");

    public static final Pragma incrementalVacuum = new Pragma("incremental_vacuum");

    public static final Pragma indexInfo = new Pragma("index_info");

    public static final Pragma indexList = new Pragma("index_list");

    public static final Pragma indexXinfo = new Pragma("index_xinfo");

    public static final Pragma integrityCheck = new Pragma("integrity_check");

    public static final Pragma journalMode = new Pragma("journal_mode");

    public static final Pragma journalSizeLimit = new Pragma("journal_size_limit");

    public static final Pragma key = new Pragma("key");

    public static final Pragma kdfIter = new Pragma("kdf_iter");

    public static final Pragma legacyFileFormat = new Pragma("legacy_file_format");

    public static final Pragma lockingMode = new Pragma("locking_mode");

    public static final Pragma maxPageCount = new Pragma("max_page_count");

    public static final Pragma mmapSize = new Pragma("mmap_size");

    public static final Pragma moduleList = new Pragma("module_list");

    public static final Pragma optimize = new Pragma("optimize");

    public static final Pragma pageCount = new Pragma("page_count");

    public static final Pragma pageSize = new Pragma("page_size");

    public static final Pragma parserTrace = new Pragma("parser_trace");

    public static final Pragma pragmaList = new Pragma("pragma_list");

    public static final Pragma queryOnly = new Pragma("query_only");

    public static final Pragma quickCheck = new Pragma("quick_check");

    public static final Pragma readUncommitted = new Pragma("read_uncommitted");

    public static final Pragma recursiveTriggers = new Pragma("recursive_triggers");

    public static final Pragma rekey = new Pragma("rekey");

    public static final Pragma reverseUnorderedSelects = new Pragma("reverse_unordered_selects");

    public static final Pragma schemaVersion = new Pragma("schema_version");

    public static final Pragma secureDelete = new Pragma("secure_delete");

    public static final Pragma shortColumnNames = new Pragma("short_column_names");

    public static final Pragma shrinkMemory = new Pragma("shrink_memory");

    public static final Pragma softHeapLimit = new Pragma("soft_heap_limit");

    public static final Pragma stats = new Pragma("stats");

    public static final Pragma synchronous = new Pragma("synchronous");

    public static final Pragma tableInfo = new Pragma("table_info");

    public static final Pragma tempStore = new Pragma("temp_store");

    public static final Pragma tempStoreDirectory = new Pragma("temp_store_directory");

    public static final Pragma threads = new Pragma("threads");

    public static final Pragma userVersion = new Pragma("user_version");

    public static final Pragma vdbeAddoptrace = new Pragma("vdbe_addoptrace");

    public static final Pragma vdbeDebug = new Pragma("vdbe_debug");

    public static final Pragma vdbeListing = new Pragma("vdbe_listing");

    public static final Pragma vdbeTrace = new Pragma("vdbe_trace");

    public static final Pragma walAutocheckpoint = new Pragma("wal_autocheckpoint");

    public static final Pragma walCheckpoint = new Pragma("wal_checkpoint");

    public static final Pragma writableSchema = new Pragma("writable_schema");
}
