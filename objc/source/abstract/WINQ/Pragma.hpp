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

#ifndef Pragma_hpp
#define Pragma_hpp

#include <WCDB/Describable.hpp>

namespace WCDB {

class Pragma : public DescribableWithLang<Lang::Pragma> {
public:
    //SQLite
    static const Pragma applicationId();
    static const Pragma autoVacuum();
    static const Pragma automaticIndex();
    static const Pragma busyTimeout();
    static const Pragma cacheSize();
    static const Pragma cacheSpill();
    static const Pragma caseSensitiveLike();
    static const Pragma cellSizeCheck();
    static const Pragma checkpointFullfsync();
    static const Pragma collationList();
    static const Pragma compileOptions();
    static const Pragma dataVersion();
    static const Pragma databaseList();
    static const Pragma deferForeignKeys();
    static const Pragma encoding();
    static const Pragma foreignKeyCheck();
    static const Pragma foreignKeyList();
    static const Pragma foreignKeys();
    static const Pragma freelistCount();
    static const Pragma fullfsync();
    static const Pragma functionList();
    static const Pragma ignoreCheckConstraints();
    static const Pragma incrementalVacuum();
    static const Pragma indexInfo();
    static const Pragma indexList();
    static const Pragma indexXinfo();
    static const Pragma integrityCheck();
    static const Pragma journalMode();
    static const Pragma journalSizeLimit();
    static const Pragma legacyFileFormat();
    static const Pragma lockingMode();
    static const Pragma maxPageCount();
    static const Pragma mmapSize();
    static const Pragma moduleList();
    static const Pragma optimize();
    static const Pragma pageCount();
    static const Pragma pageSize();
    static const Pragma parserTrace();
    static const Pragma pragmaList();
    static const Pragma queryOnly();
    static const Pragma quickCheck();
    static const Pragma readUncommitted();
    static const Pragma recursiveTriggers();
    static const Pragma reverseUnorderedSelects();
    static const Pragma schemaVersion();
    static const Pragma secureDelete();
    static const Pragma shrinkMemory();
    static const Pragma softHeapLimit();
    static const Pragma stats();
    static const Pragma synchronous();
    static const Pragma tableInfo();
    static const Pragma tempStore();
    static const Pragma threads();
    static const Pragma userVersion();
    static const Pragma vdbeAddoptrace();
    static const Pragma vdbeDebug();
    static const Pragma vdbeListing();
    static const Pragma vdbeTrace();
    static const Pragma walAutocheckpoint();
    static const Pragma walCheckpoint();
    static const Pragma writableSchema();

    //SQLCipher
    static const Pragma cipher();
    static const Pragma cipherAddRandom();
    static const Pragma cipherDefaultKdfIter();
    static const Pragma cipherDefaultPageSize();
    static const Pragma cipherPageSize();
    static const Pragma cipherDefaultUseHmac();
    static const Pragma cipherMigrate();
    static const Pragma cipherProfile();
    static const Pragma cipherProvider();
    static const Pragma cipherProviderVersion();
    static const Pragma cipherUseHmac();
    static const Pragma cipherVersion();
    static const Pragma key();
    static const Pragma kdfIter();
    static const Pragma rekey();

protected:
    Pragma(const std::string &name);
};

} // namespace WCDB

#endif /* Pragma_hpp */
