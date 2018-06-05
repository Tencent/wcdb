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

#ifndef Factory_hpp
#define Factory_hpp

#include <WCDB/FactoryBackup.hpp>
#include <WCDB/FactoryDepositor.hpp>
#include <WCDB/FactoryMeta.hpp>
#include <WCDB/FactoryRestorer.hpp>
#include <future>
#include <list>
#include <string>

namespace WCDB {

namespace Repair {

class Assembler;

//Thread safe
class Factory : public SharedThreadedErrorProne {
#pragma mark - Factory
public:
    Factory(const std::string &database);

    const std::string database;
    const std::string directory;

    std::string getRestoreDirectory() const;
    std::pair<bool, std::list<std::string>> getWorkshopDirectories() const;
    std::pair<bool, std::string> generateWorkshopDiectory() const;

#pragma - Factory Derived
public:
    FactoryDepositor depositor() const;
    FactoryRestorer restorer() const;
    FactoryBackup Backup() const;
    FactoryMeta meta;

#pragma mark - Helper
public:
    static std::list<std::string>
    associatedPathsForDatabase(const std::string &database);
    static std::list<std::string>
    databasePathsForDatabase(const std::string &database);

    static std::string
    firstMaterialPathForDatabase(const std::string &database);
    static std::string lastMaterialPathForDatabase(const std::string &database);
    static std::pair<bool, std::string>
    pickMaterialForRestoring(const std::string &database);
    static std::pair<bool, std::string>
    pickMaterialForOverwriting(const std::string &database);
};

} //namespace Repair

} //namespace WCDB

#endif /* Factory_hpp */
