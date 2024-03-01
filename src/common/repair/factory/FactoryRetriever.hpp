//
// Created by sanhuazhang on 2018/06/06
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

#include "Assemble.hpp"
#include "FactoryBackup.hpp"
#include "FactoryRelated.hpp"
#include "Progress.hpp"
#include "Scoreable.hpp"
#include "Time.hpp"
#include "UpgradeableErrorProne.hpp"
#include "WCDBOptional.hpp"

namespace WCDB {

namespace Repair {

class Mechanic;
class FullCrawler;

class FactoryRetriever final : public FactoryRelated,
                               public UpgradeableErrorProne,
                               public Progress,
                               public Scoreable,
                               public CipherDelegateHolder,
                               public AssembleDelegateHolder,
                               public BackupDelegateHolder {
#pragma mark - Retriever
public:
    FactoryRetriever(const Factory &factory);
    ~FactoryRetriever() override;

    const StringView database;

public:
    bool work();

protected:
    bool exit(bool result);

protected:
    bool restore(const UnsafeStringView &database);
    const StringView databaseFileName;

#pragma mark - Report
protected:
    void reportMechanic(const Mechanic &mechanic,
                        const UnsafeStringView &path,
                        double cost,
                        const Time &material);
    void reportFullCrawler(const FullCrawler &fullcrawler, const UnsafeStringView &path, double cost);
    void reportSummary(double cost);

    void finishReportOfPerformance(Error &error, const UnsafeStringView &database, double cost);

#pragma mark - Evaluation and Progress
protected:
    bool calculateSizes(const std::list<StringView> &workshopDirectories);
    bool calculateSize(const UnsafeStringView &database);
    bool increaseProgress(const UnsafeStringView &database,
                          bool useMaterial,
                          double progress,
                          double increment);
    static size_t
    iterateSize(const size_t previous, const std::pair<StringView, size_t> &element);

    Fraction getWeight(const UnsafeStringView &database);

    StringViewMap<size_t> m_sizes;
    size_t m_totalSize;
    void increaseScore(const UnsafeStringView &database, const Fraction &increment);
};

} //namespace Repair

} //namespace WCDB
