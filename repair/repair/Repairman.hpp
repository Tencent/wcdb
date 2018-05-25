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

#ifndef Repairman_hpp
#define Repairman_hpp

#include <WCDB/Assembler.hpp>
#include <WCDB/Crawlable.hpp>
#include <WCDB/ThreadedErrorProne.hpp>

namespace WCDB {

namespace Repair {

class Repairman {
#pragma mark - Initialize
public:
    Repairman();
#pragma mark - Assembler
public:
    void setAssembler(const std::shared_ptr<Assembler> &assembler);

protected:
    bool markAsAssembling(const std::string &tableName);
    bool markAsAssembled();

    bool markAsMilestone();

    bool assembleTable(const std::string &sql);
    bool assembleCell(const Cell &cell);

    bool canAssembled() const;

private:
    int m_maxCellsPerMilestone;
    int m_cells;
    std::shared_ptr<Assembler> m_assembler;

#pragma mark - Evaluation
public:
    double getScore() const;

protected:
    void setPageWeight(double pageWeight);
    void markCellCount(int cellCount);
    void markCellAsCounted();

private:
    double m_uncountedScore;
    double m_pageWeight;
    double m_cellWeight;
    double m_score;
};

} //namespace Repair

} //namespace WCDB

#endif /* Repairman_hpp */
