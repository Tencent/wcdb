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
    protected CPPType getCppType() {
        return CPPType.Pragma;
    }

    public Pragma(String name) {
        cppObj = createCppObj(name);
    }

    private native long createCppObj(String name);

    public static Pragma applicationId() {
        return new Pragma("application_id");
    }

    public static Pragma autoVacuum() {
        return new Pragma("auto_vacuum");
    }

    public static Pragma automaticIndex() {
        return new Pragma("automatic_index");
    }

    public static Pragma busyTimeout() {
        return new Pragma("busy_timeout");
    }

    public static Pragma cacheSize() {
        return new Pragma("cache_size");
    }

    public static Pragma cacheSpill() {
        return new Pragma("cache_spill");
    }

    public static Pragma caseSensitiveLike() {
        return new Pragma("case_sensitive_like");
    }

    public static Pragma cellSizeCheck() {
        return new Pragma("cell_size_check");
    }

    public static Pragma checkpointFullfsync() {
        return new Pragma("checkpoint_fullfsync");
    }

    public static Pragma functionList() {
        return new Pragma("function_list");
    }

    public static Pragma cipher() {
        return new Pragma("cipher");
    }

    public static Pragma cipherAddRandom() {
        return new Pragma("cipher_add_random");
    }

    public static Pragma cipherDefaultKdfIter() {
        return new Pragma("cipher_default_kdf_iter");
    }

    public static Pragma cipherDefaultPageSize() {
        return new Pragma("cipher_default_page_size");
    }

    public static Pragma cipherDefaultUseHmac() {
        return new Pragma("cipher_default_use_hmac");
    }

    public static Pragma cipherMigrate() {
        return new Pragma("cipher_migrate");
    }

    public static Pragma cipherProfile() {
        return new Pragma("cipher_profile");
    }

    public static Pragma cipherProvider() {
        return new Pragma("cipher_provider");
    }

    public static Pragma cipherProviderVersion() {
        return new Pragma("cipher_provider_version");
    }

    public static Pragma cipherUseHmac() {
        return new Pragma("cipher_use_hmac");
    }

    public static Pragma cipherVersion() {
        return new Pragma("cipher_version");
    }

    public static Pragma cipherPageSize() {
        return new Pragma("cipher_page_size");
    }

    public static Pragma collationList() {
        return new Pragma("collation_list");
    }

    public static Pragma compileOptions() {
        return new Pragma("compile_options");
    }

    public static Pragma countChanges() {
        return new Pragma("count_changes");
    }

    public static Pragma dataStoreDirectory() {
        return new Pragma("data_store_directory");
    }

    public static Pragma dataVersion() {
        return new Pragma("data_version");
    }

    public static Pragma databaseList() {
        return new Pragma("database_list");
    }

    public static Pragma defaultCacheSize() {
        return new Pragma("default_cache_size");
    }

    public static Pragma deferForeignKeys() {
        return new Pragma("defer_foreign_keys");
    }

    public static Pragma emptyResultCallbacks() {
        return new Pragma("empty_result_callbacks");
    }

    public static Pragma encoding() {
        return new Pragma("encoding");
    }

    public static Pragma foreignKeyCheck() {
        return new Pragma("foreign_key_check");
    }

    public static Pragma foreignKeyList() {
        return new Pragma("foreign_key_list");
    }

    public static Pragma foreignKeys() {
        return new Pragma("foreign_keys");
    }

    public static Pragma freelistCount() {
        return new Pragma("freelist_count");
    }

    public static Pragma fullColumnNames() {
        return new Pragma("full_column_names");
    }

    public static Pragma fullfsync() {
        return new Pragma("fullfsync");
    }

    public static Pragma ignoreCheckConstraints() {
        return new Pragma("ignore_check_constraints");
    }

    public static Pragma incrementalVacuum() {
        return new Pragma("incremental_vacuum");
    }

    public static Pragma indexInfo() {
        return new Pragma("index_info");
    }

    public static Pragma indexList() {
        return new Pragma("index_list");
    }

    public static Pragma indexXinfo() {
        return new Pragma("index_xinfo");
    }

    public static Pragma integrityCheck() {
        return new Pragma("integrity_check");
    }

    public static Pragma journalMode() {
        return new Pragma("journal_mode");
    }

    public static Pragma journalSizeLimit() {
        return new Pragma("journal_size_limit");
    }

    public static Pragma key() {
        return new Pragma("key");
    }

    public static Pragma kdfIter() {
        return new Pragma("kdf_iter");
    }

    public static Pragma legacyFileFormat() {
        return new Pragma("legacy_file_format");
    }

    public static Pragma lockingMode() {
        return new Pragma("locking_mode");
    }

    public static Pragma maxPageCount() {
        return new Pragma("max_page_count");
    }

    public static Pragma mmapSize() {
        return new Pragma("mmap_size");
    }

    public static Pragma moduleList() {
        return new Pragma("module_list");
    }

    public static Pragma optimize() {
        return new Pragma("optimize");
    }

    public static Pragma pageCount() {
        return new Pragma("page_count");
    }

    public static Pragma pageSize() {
        return new Pragma("page_size");
    }

    public static Pragma parserTrace() {
        return new Pragma("parser_trace");
    }

    public static Pragma pragmaList() {
        return new Pragma("pragma_list");
    }

    public static Pragma queryOnly() {
        return new Pragma("query_only");
    }

    public static Pragma quickCheck() {
        return new Pragma("quick_check");
    }

    public static Pragma readUncommitted() {
        return new Pragma("read_uncommitted");
    }

    public static Pragma recursiveTriggers() {
        return new Pragma("recursive_triggers");
    }

    public static Pragma rekey() {
        return new Pragma("rekey");
    }

    public static Pragma reverseUnorderedSelects() {
        return new Pragma("reverse_unordered_selects");
    }

    public static Pragma schemaVersion() {
        return new Pragma("schema_version");
    }

    public static Pragma secureDelete() {
        return new Pragma("secure_delete");
    }

    public static Pragma shortColumnNames() {
        return new Pragma("short_column_names");
    }

    public static Pragma shrinkMemory() {
        return new Pragma("shrink_memory");
    }

    public static Pragma softHeapLimit() {
        return new Pragma("soft_heap_limit");
    }

    public static Pragma stats() {
        return new Pragma("stats");
    }

    public static Pragma synchronous() {
        return new Pragma("synchronous");
    }

    public static Pragma tableInfo() {
        return new Pragma("table_info");
    }

    public static Pragma tempStore() {
        return new Pragma("temp_store");
    }

    public static Pragma tempStoreDirectory() {
        return new Pragma("temp_store_directory");
    }

    public static Pragma threads() {
        return new Pragma("threads");
    }

    public static Pragma userVersion() {
        return new Pragma("user_version");
    }

    public static Pragma vdbeAddoptrace() {
        return new Pragma("vdbe_addoptrace");
    }

    public static Pragma vdbeDebug() {
        return new Pragma("vdbe_debug");
    }

    public static Pragma vdbeListing() {
        return new Pragma("vdbe_listing");
    }

    public static Pragma vdbeTrace() {
        return new Pragma("vdbe_trace");
    }

    public static Pragma walAutocheckpoint() {
        return new Pragma("wal_autocheckpoint");
    }

    public static Pragma walCheckpoint() {
        return new Pragma("wal_checkpoint");
    }

    public static Pragma writableSchema() {
        return new Pragma("writable_schema");
    }
}
