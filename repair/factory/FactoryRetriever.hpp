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

#ifndef FactoryRetriever_hpp
#define FactoryRetriever_hpp

#include <WCDB/Assembler.hpp>
#include <WCDB/CriticalErrorOnly.hpp>
#include <WCDB/FactoryBackup.hpp>
#include <WCDB/FactoryRelated.hpp>
#include <WCDB/Progress.hpp>

namespace WCDB {

namespace Repair {

class FactoryRetriever : public FactoryRelated,
                         public CriticalErrorOnly,
                         public Progress {
#pragma mark - Restorer
public:
    FactoryRetriever(Factory &factory);

    const std::string database;

public:
    double work();

protected:
    bool restore(const std::string &database);
    const std::string databaseFileName;

#pragma mark - Assembler
public:
    void setAssembler(const std::shared_ptr<Assembler> &assembler);

protected:
    std::shared_ptr<Assembler> m_assembler;

#pragma mark - Evaluation and Progress
protected:
    bool calculateWeights(const std::list<std::string> &workshopDirectories);
    bool calculateWeight(const std::string &database, size_t &totalSize);
    void updateProgress(const std::string &database, double increment);

    std::map<std::string, double> m_weights;
    void updateScore(const std::string &database, double increment);
    double m_score;
};

} //namespace Repair

} //namespace WCDB

#endif /* FactoryRetriever_hpp */
