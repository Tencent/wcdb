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

import org.jetbrains.annotations.NotNull;

public class Pragma extends Identifier {
    @Override
    protected int getType() {
        return CPPType.Pragma;
    }

    public Pragma(@NotNull String name) {
        cppObj = createCppObj(name);
    }

    private static native long createCppObj(String name);

    @NotNull
    public static final Pragma applicationId = new Pragma("application_id");

    @NotNull
    public static final Pragma autoVacuum = new Pragma("auto_vacuum");

    @NotNull
    public static final Pragma automaticIndex = new Pragma("automatic_index");

    @NotNull
    public static final Pragma busyTimeout = new Pragma("busy_timeout");

    @NotNull
    public static final Pragma cacheSize = new Pragma("cache_size");

    @NotNull
    public static final Pragma cacheSpill = new Pragma("cache_spill");

    @NotNull
    public static final Pragma caseSensitiveLike = new Pragma("case_sensitive_like");

    @NotNull
    public static final Pragma cellSizeCheck = new Pragma("cell_size_check");

    @NotNull
    public static final Pragma checkpointFullfsync = new Pragma("checkpoint_fullfsync");

    @NotNull
    public static final Pragma functionList = new Pragma("function_list");

    @NotNull
    public static final Pragma cipher = new Pragma("cipher");

    @NotNull
    public static final Pragma cipherAddRandom = new Pragma("cipher_add_random");

    @NotNull
    public static final Pragma cipherDefaultKdfIter = new Pragma("cipher_default_kdf_iter");

    @NotNull
    public static final Pragma cipherDefaultPageSize = new Pragma("cipher_default_page_size");

    @NotNull
    public static final Pragma cipherDefaultUseHmac = new Pragma("cipher_default_use_hmac");

    @NotNull
    public static final Pragma cipherMigrate = new Pragma("cipher_migrate");

    @NotNull
    public static final Pragma cipherProfile = new Pragma("cipher_profile");

    @NotNull
    public static final Pragma cipherProvider = new Pragma("cipher_provider");

    @NotNull
    public static final Pragma cipherProviderVersion = new Pragma("cipher_provider_version");

    @NotNull
    public static final Pragma cipherUseHmac = new Pragma("cipher_use_hmac");

    @NotNull
    public static final Pragma cipherVersion = new Pragma("cipher_version");

    @NotNull
    public static final Pragma cipherPageSize = new Pragma("cipher_page_size");

    @NotNull
    public static final Pragma collationList = new Pragma("collation_list");

    @NotNull
    public static final Pragma compileOptions = new Pragma("compile_options");

    @NotNull
    public static final Pragma countChanges = new Pragma("count_changes");

    @NotNull
    public static final Pragma dataStoreDirectory = new Pragma("data_store_directory");

    @NotNull
    public static final Pragma dataVersion = new Pragma("data_version");

    @NotNull
    public static final Pragma databaseList = new Pragma("database_list");

    @NotNull
    public static final Pragma defaultCacheSize = new Pragma("default_cache_size");

    @NotNull
    public static final Pragma deferForeignKeys = new Pragma("defer_foreign_keys");

    @NotNull
    public static final Pragma emptyResultCallbacks = new Pragma("empty_result_callbacks");

    @NotNull
    public static final Pragma encoding = new Pragma("encoding");

    @NotNull
    public static final Pragma foreignKeyCheck = new Pragma("foreign_key_check");

    @NotNull
    public static final Pragma foreignKeyList = new Pragma("foreign_key_list");

    @NotNull
    public static final Pragma foreignKeys = new Pragma("foreign_keys");

    @NotNull
    public static final Pragma freelistCount = new Pragma("freelist_count");

    @NotNull
    public static final Pragma fullColumnNames = new Pragma("full_column_names");

    @NotNull
    public static final Pragma fullfsync = new Pragma("fullfsync");

    @NotNull
    public static final Pragma ignoreCheckConstraints = new Pragma("ignore_check_constraints");

    @NotNull
    public static final Pragma incrementalVacuum = new Pragma("incremental_vacuum");

    @NotNull
    public static final Pragma indexInfo = new Pragma("index_info");

    @NotNull
    public static final Pragma indexList = new Pragma("index_list");

    @NotNull
    public static final Pragma indexXinfo = new Pragma("index_xinfo");

    @NotNull
    public static final Pragma integrityCheck = new Pragma("integrity_check");

    @NotNull
    public static final Pragma journalMode = new Pragma("journal_mode");

    @NotNull
    public static final Pragma journalSizeLimit = new Pragma("journal_size_limit");

    @NotNull
    public static final Pragma key = new Pragma("key");

    @NotNull
    public static final Pragma kdfIter = new Pragma("kdf_iter");

    @NotNull
    public static final Pragma legacyFileFormat = new Pragma("legacy_file_format");

    @NotNull
    public static final Pragma lockingMode = new Pragma("locking_mode");

    @NotNull
    public static final Pragma maxPageCount = new Pragma("max_page_count");

    @NotNull
    public static final Pragma mmapSize = new Pragma("mmap_size");

    @NotNull
    public static final Pragma moduleList = new Pragma("module_list");

    @NotNull
    public static final Pragma optimize = new Pragma("optimize");

    @NotNull
    public static final Pragma pageCount = new Pragma("page_count");

    @NotNull
    public static final Pragma pageSize = new Pragma("page_size");

    @NotNull
    public static final Pragma parserTrace = new Pragma("parser_trace");

    @NotNull
    public static final Pragma pragmaList = new Pragma("pragma_list");

    @NotNull
    public static final Pragma queryOnly = new Pragma("query_only");

    @NotNull
    public static final Pragma quickCheck = new Pragma("quick_check");

    @NotNull
    public static final Pragma readUncommitted = new Pragma("read_uncommitted");

    @NotNull
    public static final Pragma recursiveTriggers = new Pragma("recursive_triggers");

    @NotNull
    public static final Pragma rekey = new Pragma("rekey");

    @NotNull
    public static final Pragma reverseUnorderedSelects = new Pragma("reverse_unordered_selects");

    @NotNull
    public static final Pragma schemaVersion = new Pragma("schema_version");

    @NotNull
    public static final Pragma secureDelete = new Pragma("secure_delete");

    @NotNull
    public static final Pragma shortColumnNames = new Pragma("short_column_names");

    @NotNull
    public static final Pragma shrinkMemory = new Pragma("shrink_memory");

    @NotNull
    public static final Pragma softHeapLimit = new Pragma("soft_heap_limit");

    @NotNull
    public static final Pragma stats = new Pragma("stats");

    @NotNull
    public static final Pragma synchronous = new Pragma("synchronous");

    @NotNull
    public static final Pragma tableInfo = new Pragma("table_info");

    @NotNull
    public static final Pragma tempStore = new Pragma("temp_store");

    @NotNull
    public static final Pragma tempStoreDirectory = new Pragma("temp_store_directory");

    @NotNull
    public static final Pragma threads = new Pragma("threads");

    @NotNull
    public static final Pragma userVersion = new Pragma("user_version");

    @NotNull
    public static final Pragma vdbeAddoptrace = new Pragma("vdbe_addoptrace");

    @NotNull
    public static final Pragma vdbeDebug = new Pragma("vdbe_debug");

    @NotNull
    public static final Pragma vdbeListing = new Pragma("vdbe_listing");

    @NotNull
    public static final Pragma vdbeTrace = new Pragma("vdbe_trace");

    @NotNull
    public static final Pragma walAutocheckpoint = new Pragma("wal_autocheckpoint");

    @NotNull
    public static final Pragma walCheckpoint = new Pragma("wal_checkpoint");

    @NotNull
    public static final Pragma writableSchema = new Pragma("writable_schema");
}
