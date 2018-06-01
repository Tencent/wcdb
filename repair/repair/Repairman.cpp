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

#include <WCDB/Assertion.hpp>
#include <WCDB/Repairman.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Repairman::Repairman(const std::string &path)
    : Crawlable()
    , Progress()
    , m_pager(path)
    , m_assembler(nullptr)
    , m_milestone(5000)
    , m_mile(0)
    , m_scorePool(0)
    , m_pageWeight(0)
    , m_cellWeight(0)
    , m_score(0)
{
}

#pragma mark - Crawlable
Pager &Repairman::getPager()
{
    return m_pager;
}

#pragma mark - Assemble
void Repairman::setAssembler(const std::shared_ptr<Assembler> &assembler)
{
    m_assembler = assembler;
}

bool Repairman::markAsAssembling()
{
    WCTInnerAssert(!m_assembler->getPath().empty());
    if (m_assembler->markAsAssembling()) {
        return true;
    }
    tryUpgrateAssemblerError();
    return false;
}

void Repairman::markAsAssembled()
{
    markAsMilestone();
    m_assembler->markAsAssembled();
    finishProgress();
}

void Repairman::markAsMilestone()
{
    if (m_assembler->markAsMilestone()) {
        m_score += m_scorePool;
    } else {
        tryUpgrateAssemblerError();
    }
    m_scorePool = 0;
    m_mile = 0;
}

void Repairman::towardMilestone(int mile)
{
    m_mile += mile;
    if (m_mile > m_milestone) {
        markAsMilestone();
    }
}

bool Repairman::assembleTable(const std::string &tableName,
                              const std::string &sql)
{
    if (m_assembler->assembleTable(tableName, sql)) {
        towardMilestone(100);
        return true;
    }
    tryUpgrateAssemblerError();
    return false;
}

void Repairman::assembleCell(const Cell &cell)
{
    if (!m_assembler->assembleCell(cell)) {
        tryUpgrateAssemblerError();
    } else {
        markCellAsCounted();
        towardMilestone(1);
    }
}

void Repairman::onCrawlerError()
{
    tryUpgradeCrawlerError();
}

void Repairman::tryUpgradeCrawlerError()
{
    Error error = std::move(ThreadedErrors::shared()->moveThreadedError());
    if (error.code() == Error::Code::Corrupt ||
        error.code() == Error::Code::NotADatabase) {
        error.level = Error::Level::Warning;
    }
    tryUpgradeError(std::move(error));
}

void Repairman::tryUpgrateAssemblerError()
{
    tryUpgradeError(m_assembler->getError());
}

#pragma mark - Evaluation
void Repairman::markCellAsCounted()
{
    m_scorePool += m_cellWeight;
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
