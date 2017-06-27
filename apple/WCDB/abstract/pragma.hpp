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

#ifndef pragma_hpp
#define pragma_hpp

#include <WCDB/describable.hpp>

namespace WCDB {

class Pragma : public Describable {
public:
    static const Pragma ApplicationId;
    static const Pragma AutoVacuum;
    static const Pragma AutomaticIndex;
    static const Pragma BusyTimeout;
    static const Pragma CacheSize;
    static const Pragma CacheSpill;
    static const Pragma CaseSensitiveLike;
    static const Pragma CellSizeCheck;
    static const Pragma CheckpointFullfsync;
    static const Pragma Cipher;
    static const Pragma CipherAddRandom;
    static const Pragma CipherDefaultKdfIter;
    static const Pragma CipherDefaultPageSize;
    static const Pragma CipherPageSize;
    static const Pragma CipherDefaultUseHmac;
    static const Pragma CipherMigrate;
    static const Pragma CipherProfile;
    static const Pragma CipherProvider;
    static const Pragma CipherProviderVersion;
    static const Pragma CipherUseHmac;
    static const Pragma CipherVersion;
    static const Pragma CollationList;
    static const Pragma CompileOptions;
    static const Pragma CountChanges;
    static const Pragma DataStoreDirectory;
    static const Pragma DataVersion;
    static const Pragma DatabaseList;
    static const Pragma DefaultCacheSize;
    static const Pragma DeferForeignKeys;
    static const Pragma EmptyResultCallbacks;
    static const Pragma Encoding;
    static const Pragma ForeignKeyCheck;
    static const Pragma ForeignKeyList;
    static const Pragma ForeignKeys;
    static const Pragma FreelistCount;
    static const Pragma FullColumnNames;
    static const Pragma Fullfsync;
    static const Pragma IgnoreCheckConstraints;
    static const Pragma IncrementalVacuum;
    static const Pragma IndexInfo;
    static const Pragma IndexList;
    static const Pragma IndexXinfo;
    static const Pragma IntegrityCheck;
    static const Pragma JournalMode;
    static const Pragma JournalSizeLimit;
    static const Pragma Key;
    static const Pragma KdfIter;
    static const Pragma LegacyFileFormat;
    static const Pragma LockingMode;
    static const Pragma MaxPageCount;
    static const Pragma MmapSize;
    static const Pragma PageCount;
    static const Pragma PageSize;
    static const Pragma ParserTrace;
    static const Pragma QueryOnly;
    static const Pragma QuickCheck;
    static const Pragma ReadUncommitted;
    static const Pragma RecursiveTriggers;
    static const Pragma Rekey;
    static const Pragma ReverseUnorderedSelects;
    static const Pragma SchemaVersion;
    static const Pragma SecureDelete;
    static const Pragma ShortColumnNames;
    static const Pragma ShrinkMemory;
    static const Pragma SoftHeapLimit;
    static const Pragma Stats;
    static const Pragma Synchronous;
    static const Pragma TableInfo;
    static const Pragma TempStore;
    static const Pragma TempStoreDirectory;
    static const Pragma Threads;
    static const Pragma UserVersion;
    static const Pragma VdbeAddoptrace;
    static const Pragma VdbeDebug;
    static const Pragma VdbeListing;
    static const Pragma VdbeTrace;
    static const Pragma WalAutocheckpoint;
    static const Pragma WalCheckpoint;
    static const Pragma WritableSchema;

    const std::string &getName() const;

protected:
    Pragma(const char *name);
};

} //namespace WCDB

#endif /* pragma_hpp */
