//
// Created by sanhuazhang on 2018/05/31
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

#include "FactoryBackup.hpp"
#include "FactoryDepositor.hpp"
#include "FactoryRenewer.hpp"
#include "FactoryRetriever.hpp"
#include "FactoryVacuum.hpp"
#include "StringView.hpp"
#include "Time.hpp"
#include "WCDBOptional.hpp"
#include <future>
#include <list>

namespace WCDB {

namespace Repair {

class AssembleDelegate;

class Factory final {
#pragma mark - Factory
public:
    Factory(const UnsafeStringView &database);

    const StringView database;
    const StringView directory;

    static constexpr const char *restoreDirectoryName = "restore";
    static constexpr const char *renewDirectoryName = "renew";
    static constexpr const char *vacuumDirectoryName = "vacuum";
    StringView getRestoreDirectory() const;
    StringView getRenewDirectory() const;
    StringView getVacuumDirectory() const;
    StringView getDatabaseName() const;
    bool removeDirectoryIfEmpty() const;
    bool removeDeposited() const;
    Optional<std::list<StringView>> getWorkshopDirectories() const;
    Optional<StringView> getUniqueWorkshopDiectory() const;
    bool containsDeposited() const;
    static bool isWorkshopDirectory(const UnsafeStringView &dir);

#pragma mark - Factory Related
public:
    friend class FactoryRelated;
    FactoryDepositor depositor() const;
    FactoryRetriever retriever() const;
    FactoryVacuum vacuumer() const;
    FactoryBackup backup() const;
    FactoryRenewer renewer() const;

    typedef Backup::Filter Filter;
    void filter(const Filter &tableShouldBeBackedUp);
    Filter getFilter() const;

protected:
    Filter m_filter;

#pragma mark - Helper
public:
    static std::list<StringView>
    associatedPathsForDatabase(const UnsafeStringView &database);
    static std::list<StringView> databasePathsForDatabase(const UnsafeStringView &database);

    static StringView incrementalMaterialPathForDatabase(const UnsafeStringView &database);
    static StringView firstMaterialPathForDatabase(const UnsafeStringView &database);
    static StringView lastMaterialPathForDatabase(const UnsafeStringView &database);
    static StringView factoryPathForDatabase(const UnsafeStringView &database);

    static Optional<StringView>
    materialForSerializingForDatabase(const UnsafeStringView &database);
    static Optional<StringView> latestMaterialForDatabase(const UnsafeStringView &database);
    static Optional<std::list<StringView>>
    materialsForDeserializingForDatabase(const UnsafeStringView &database);

protected:
    static Optional<Time> getModifiedTimeOr0IfNotExists(const UnsafeStringView &path);
};

} //namespace Repair

} //namespace WCDB
