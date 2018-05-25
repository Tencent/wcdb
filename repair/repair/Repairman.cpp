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

#include <WCDB/Repairman.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Repairman::Repairman()
    : m_assembler(nullptr)
    , m_maxCellsPerMilestone(5000)
    , m_cells(0)
    , m_uncountedScore(0)
    , m_pageWeight(0)
    , m_cellWeight(0)
    , m_score(0)
{
}

#pragma mark - Assemble
bool Repairman::canAssembled() const
{
    return m_assembler != nullptr;
}

void Repairman::setAssembler(const std::shared_ptr<Assembler> &assembler)
{
    m_assembler = assembler;
}

bool Repairman::markAsAssembling(const std::string &tableName)
{
    return m_assembler->markAsAssembling(tableName);
}

bool Repairman::markAsAssembled()
{
    return markAsMilestone() && m_assembler->markAsAssembled();
}

bool Repairman::markAsMilestone()
{
    bool result = m_assembler->markAsMilestone();
    if (result) {
        m_score += m_uncountedScore;
    }
    m_uncountedScore = 0;
    m_cells = 0;
    return result;
}

bool Repairman::assembleTable(const std::string &sql)
{
    return m_assembler->assembleTable(sql);
}

bool Repairman::assembleCell(const Cell &cell)
{
    if (!m_assembler->assembleCell(cell)) {
        return false;
    }
    markCellAsCounted();
    if (++m_cells < m_maxCellsPerMilestone) {
        return true;
    }
    if (markAsMilestone()) {
        return true;
    }
    return false;
}

#pragma mark - Evaluation
void Repairman::markCellAsCounted()
{
    m_uncountedScore += m_cellWeight;
}

void Repairman::markCellCount(int cellCount)
{
    m_cellWeight = (double) 1.0 / cellCount * m_pageWeight;
}

void Repairman::setPageWeight(double pageWeight)
{
    m_pageWeight = pageWeight;
}

} //namespace Repair

} //namespace WCDB
