//
// Created by sanhuazhang on 2019/05/02
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

#pragma once

#include "SQL.hpp"

namespace WCDB {

class WCDB_API Pragma final : public SpecifiedSyntax<Syntax::Pragma, SQL> {
#pragma mark - Initializer
public:
    using SpecifiedSyntax<Syntax::Pragma, SQL>::SpecifiedSyntax;
    Pragma();
    ~Pragma() override;

    template<typename T, typename Enable = typename std::enable_if<ColumnIsTextType<T>::value>::type>
    Pragma(const T& t) : Pragma(ColumnIsTextType<T>::asUnderlyingType(t))
    {
    }

    Pragma(const UnsafeStringView& name);

#pragma mark - SQLite
public:
    static Pragma applicationId();
    static Pragma autoVacuum();
    static Pragma automaticIndex();
    static Pragma busyTimeout();
    static Pragma cacheSize();
    static Pragma cacheSpill();
    static Pragma caseSensitiveLike();
    static Pragma cellSizeCheck();
    static Pragma checkpointFullfsync();
    static Pragma collationList();
    static Pragma dataVersion();
    static Pragma databaseList();
    static Pragma deferForeignKeys();
    static Pragma encoding();
    static Pragma foreignKeyCheck();
    static Pragma foreignKeyList();
    static Pragma foreignKeys();
    static Pragma freelistCount();
    static Pragma fullfsync();
    static Pragma functionList();
    static Pragma ignoreCheckConstraints();
    static Pragma incrementalVacuum();
    static Pragma indexInfo();
    static Pragma indexList();
    static Pragma indexXinfo();
    static Pragma integrityCheck();
    static Pragma journalMode();
    static Pragma journalSizeLimit();
    static Pragma legacyFileFormat();
    static Pragma lockingMode();
    static Pragma maxPageCount();
    static Pragma mmapSize();
    static Pragma moduleList();
    static Pragma optimize();
    static Pragma pageCount();
    static Pragma pageSize();
    static Pragma parserTrace();
    static Pragma pragmaList();
    static Pragma queryOnly();
    static Pragma quickCheck();
    static Pragma readUncommitted();
    static Pragma recursiveTriggers();
    static Pragma reverseUnorderedSelects();
    static Pragma schemaVersion();
    static Pragma secureDelete();
    static Pragma shrinkMemory();
    static Pragma softHeapLimit();
    static Pragma stats();
    static Pragma synchronous();
    static Pragma tableInfo();
    static Pragma tempStore();
    static Pragma threads();
    static Pragma userVersion();
    static Pragma vdbeAddoptrace();
    static Pragma vdbeDebug();
    static Pragma vdbeListing();
    static Pragma vdbeTrace();
    static Pragma walAutocheckpoint();
    static Pragma walCheckpoint();
    static Pragma writableSchema();

#pragma mark - SQLCipher
public:
    static Pragma cipher();
    static Pragma cipherAddRandom();
    static Pragma cipherCompatibility();
    static Pragma cipherDefaultKdfIter();
    static Pragma cipherDefaultPageSize();
    static Pragma cipherHmacAlgorithm();
    static Pragma cipherKdfAlgorithm();
    static Pragma cipherPageSize();
    static Pragma cipherPlainTextHeaderSize();
    static Pragma cipherDefaultUseHmac();
    static Pragma cipherMigrate();
    static Pragma cipherProfile();
    static Pragma cipherProvider();
    static Pragma cipherProviderVersion();
    static Pragma cipherSalt();
    static Pragma cipherUseHmac();
    static Pragma cipherVersion();
    static Pragma key();
    static Pragma kdfIter();
    static Pragma rekey();
};

} // namespace WCDB
