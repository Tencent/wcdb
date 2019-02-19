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

#ifndef __WCDB_FACTORY_HPP
#define __WCDB_FACTORY_HPP

#include <WCDB/FactoryBackup.hpp>
#include <WCDB/FactoryDepositor.hpp>
#include <WCDB/FactoryRenewer.hpp>
#include <WCDB/FactoryRetriever.hpp>
#include <WCDB/String.hpp>
#include <WCDB/Time.hpp>
#include <future>
#include <list>

namespace WCDB {

namespace Repair {

class Assembler;

class Factory final {
#pragma mark - Factory
public:
    Factory(const String &database);

    const String database;
    const String directory;

    static constexpr const char *restoreDirectoryName = "restore";
    static constexpr const char *renewDirectoryName = "renew";
    String getRestoreDirectory() const;
    String getRenewDirectory() const;
    String getDatabaseName() const;
    bool removeDirectoryIfEmpty() const;
    bool removeDeposited() const;
    std::pair<bool, std::list<String>> getWorkshopDirectories() const;
    std::pair<bool, String> getUniqueWorkshopDiectory() const;
    bool containsDeposited() const;

#pragma mark - Factory Related
public:
    friend class FactoryRelated;
    FactoryDepositor depositor() const;
    FactoryRetriever retriever() const;
    FactoryBackup backup() const;
    FactoryRenewer renewer() const;

    typedef Backup::Filter Filter;
    void filter(const Filter &tableShouldBeBackedUp);
    Filter getFilter() const;

protected:
    Filter m_filter;

#pragma mark - Helper
public:
    static std::list<String> associatedPathsForDatabase(const String &database);
    static std::list<String> databasePathsForDatabase(const String &database);

    static String firstMaterialPathForDatabase(const String &database);
    static String lastMaterialPathForDatabase(const String &database);

    static std::pair<bool, String>
    materialForSerializingForDatabase(const String &database);
    static std::pair<bool, std::list<String>>
    materialsForDeserializingForDatabase(const String &database);

protected:
    static std::pair<bool, Time> getModifiedTimeOr0IfNotExists(const String &path);
};

} //namespace Repair

} //namespace WCDB

#endif /* __WCDB_FACTORY_HPP */
