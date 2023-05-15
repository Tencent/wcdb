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

package com.tencent.wcdbtest.winq;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import com.tencent.wcdb.winq.*;
import static com.tencent.wcdbtest.base.WinqTool.winqEqual;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class PragmaTest {
    @Test
    public void test() {
        winqEqual(new Pragma("testPragma"), "testPragma");
        winqEqual(Pragma.applicationId(), "application_id");
        winqEqual(Pragma.autoVacuum(), "auto_vacuum");
        winqEqual(Pragma.automaticIndex(), "automatic_index");
        winqEqual(Pragma.busyTimeout(), "busy_timeout");
        winqEqual(Pragma.cacheSize(), "cache_size");
        winqEqual(Pragma.cacheSpill(), "cache_spill");
        winqEqual(Pragma.caseSensitiveLike(), "case_sensitive_like");
        winqEqual(Pragma.cellSizeCheck(), "cell_size_check");
        winqEqual(Pragma.checkpointFullfsync(), "checkpoint_fullfsync");
        winqEqual(Pragma.cipher(), "cipher");
        winqEqual(Pragma.cipherAddRandom(), "cipher_add_random");
        winqEqual(Pragma.cipherDefaultKdfIter(), "cipher_default_kdf_iter");
        winqEqual(Pragma.cipherDefaultPageSize(), "cipher_default_page_size");
        winqEqual(Pragma.cipherDefaultUseHmac(), "cipher_default_use_hmac");
        winqEqual(Pragma.cipherMigrate(), "cipher_migrate");
        winqEqual(Pragma.cipherProfile(), "cipher_profile");
        winqEqual(Pragma.cipherProvider(), "cipher_provider");
        winqEqual(Pragma.cipherProviderVersion(), "cipher_provider_version");
        winqEqual(Pragma.cipherUseHmac(), "cipher_use_hmac");
        winqEqual(Pragma.cipherVersion(), "cipher_version");
        winqEqual(Pragma.cipherPageSize(), "cipher_page_size");
        winqEqual(Pragma.collationList(), "collation_list");
        winqEqual(Pragma.dataVersion(), "data_version");
        winqEqual(Pragma.databaseList(), "database_list");
        winqEqual(Pragma.deferForeignKeys(), "defer_foreign_keys");
        winqEqual(Pragma.encoding(), "encoding");
        winqEqual(Pragma.foreignKeyCheck(), "foreign_key_check");
        winqEqual(Pragma.foreignKeyList(), "foreign_key_list");
        winqEqual(Pragma.foreignKeys(), "foreign_keys");
        winqEqual(Pragma.freelistCount(), "freelist_count");
        winqEqual(Pragma.fullfsync(), "fullfsync");
        winqEqual(Pragma.functionList(), "function_list");
        winqEqual(Pragma.ignoreCheckConstraints(), "ignore_check_constraints");
        winqEqual(Pragma.incrementalVacuum(), "incremental_vacuum");
        winqEqual(Pragma.indexInfo(), "index_info");
        winqEqual(Pragma.indexList(), "index_list");
        winqEqual(Pragma.indexXinfo(), "index_xinfo");
        winqEqual(Pragma.integrityCheck(), "integrity_check");
        winqEqual(Pragma.journalMode(), "journal_mode");
        winqEqual(Pragma.journalSizeLimit(), "journal_size_limit");
        winqEqual(Pragma.key(), "key");
        winqEqual(Pragma.kdfIter(), "kdf_iter");
        winqEqual(Pragma.legacyFileFormat(), "legacy_file_format");
        winqEqual(Pragma.lockingMode(), "locking_mode");
        winqEqual(Pragma.maxPageCount(), "max_page_count");
        winqEqual(Pragma.mmapSize(), "mmap_size");
        winqEqual(Pragma.moduleList(), "module_list");
        winqEqual(Pragma.optimize(), "optimize");
        winqEqual(Pragma.pageCount(), "page_count");
        winqEqual(Pragma.pageSize(), "page_size");
        winqEqual(Pragma.parserTrace(), "parser_trace");
        winqEqual(Pragma.pragmaList(), "pragma_list");
        winqEqual(Pragma.queryOnly(), "query_only");
        winqEqual(Pragma.quickCheck(), "quick_check");
        winqEqual(Pragma.readUncommitted(), "read_uncommitted");
        winqEqual(Pragma.recursiveTriggers(), "recursive_triggers");
        winqEqual(Pragma.rekey(), "rekey");
        winqEqual(Pragma.reverseUnorderedSelects(), "reverse_unordered_selects");
        winqEqual(Pragma.schemaVersion(), "schema_version");
        winqEqual(Pragma.secureDelete(), "secure_delete");
        winqEqual(Pragma.shrinkMemory(), "shrink_memory");
        winqEqual(Pragma.softHeapLimit(), "soft_heap_limit");
        winqEqual(Pragma.stats(), "stats");
        winqEqual(Pragma.synchronous(), "synchronous");
        winqEqual(Pragma.tableInfo(), "table_info");
        winqEqual(Pragma.tempStore(), "temp_store");
        winqEqual(Pragma.threads(), "threads");
        winqEqual(Pragma.userVersion(), "user_version");
        winqEqual(Pragma.vdbeAddoptrace(), "vdbe_addoptrace");
        winqEqual(Pragma.vdbeDebug(), "vdbe_debug");
        winqEqual(Pragma.vdbeListing(), "vdbe_listing");
        winqEqual(Pragma.vdbeTrace(), "vdbe_trace");
        winqEqual(Pragma.walAutocheckpoint(), "wal_autocheckpoint");
        winqEqual(Pragma.walCheckpoint(), "wal_checkpoint");
        winqEqual(Pragma.writableSchema(), "writable_schema");
    }
}
