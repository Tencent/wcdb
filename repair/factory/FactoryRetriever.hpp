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

#ifndef __WCDB_FACTORY_RETRIEVER_HPP
#define __WCDB_FACTORY_RETRIEVER_HPP

#include <WCDB/Assembler.hpp>
#include <WCDB/FactoryBackup.hpp>
#include <WCDB/FactoryRelated.hpp>
#include <WCDB/Progress.hpp>
#include <WCDB/Scoreable.hpp>
#include <WCDB/Time.hpp>
#include <WCDB/UpgradeableErrorProne.hpp>

namespace WCDB {

namespace Repair {

class FactoryRetriever final : public FactoryRelated,
                               public UpgradeableErrorProne,
                               public Progress,
                               public Scoreable,
                               public AssemblerHolder,
                               public LockerHolder {
#pragma mark - Retriever
public:
    FactoryRetriever(const Factory &factory);

    const String database;

public:
    bool work();

protected:
    bool exit(bool result);

protected:
    bool restore(const String &database);
    const String databaseFileName;

#pragma mark - Report
protected:
    void reportMechanic(const Fraction &score,
                        const String &path,
                        const SteadyClock &cost,
                        const Time &material);
    void reportFullCrawler(const Fraction &score, const String &path, const SteadyClock &cost);
    void reportSummary(const SteadyClock &cost);

    void finishReportOfPerformance(Error &error, const String &database, const SteadyClock &cost);

#pragma mark - Evaluation and Progress
protected:
    bool calculateSizes(const std::list<String> &workshopDirectories);
    bool calculateSize(const String &database);
    void increaseProgress(const String &database, bool useMaterial, double progress, double increment);
    static size_t
    iterateSize(const size_t previous, const std::pair<String, size_t> &element);

    Fraction getWeight(const String &database);

    std::map<String, size_t> m_sizes;
    size_t m_totalSize;
    void increaseScore(const String &database, const Fraction &increment);
};

} //namespace Repair

} //namespace WCDB

#endif /* __WCDB_FACTORY_RETRIEVER_HPP */
