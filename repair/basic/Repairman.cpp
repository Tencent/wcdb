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
#include <WCDB/Cell.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/Repairman.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Repairman::Repairman(const std::string &path)
: m_pager(path), Crawlable(m_pager), Progress(), m_milestone(1000), m_mile(0)
{
}

const std::string &Repairman::getPath() const
{
    return m_pager.getPath();
}

bool Repairman::exit()
{
    finishProgress();
    return !isErrorCritial();
}

bool Repairman::exit(bool result)
{
    finishProgress();
    return result;
}

bool Repairman::isEmptyDatabase()
{
    bool succeed;
    size_t fileSize;
    std::tie(succeed, fileSize) = FileManager::getFileSize(getPath());
    if (fileSize == 0) {
        if (succeed) {
            Error error;
            error.level = Error::Level::Warning;
            error.setCode(Error::Code::Empty, "Repair");
            error.message = "Database is not found or empty.";
            error.infos.set("Path", getPath());
            Notifier::shared()->notify(error);
        } else {
            setCriticalErrorWithSharedThreadedError();
        }
        return true;
    }
    return false;
}

bool Repairman::markAsAssembling()
{
    WCTInnerAssert(!m_assembler->getPath().empty());
    if (m_assembler->markAsAssembling()) {
        return true;
    }
    setCriticalError(m_assembler->getError());
    return false;
}

void Repairman::markAsAssembled()
{
    markAsMilestone();
    if (!m_assembler->markAsAssembled()) {
        setCriticalError(m_assembler->getError());
    }
}

bool Repairman::markAsMilestone()
{
    m_mile = 0;
    if (m_assembler->markAsMilestone()) {
        markSegmentedScoreCounted();
        return true;
    }
    markSegmentedScoreDropped();
    tryUpgrateAssemblerError();
    return false;
}

bool Repairman::towardMilestone(int mile)
{
    m_mile += mile;
    if (m_mile > m_milestone) {
        return markAsMilestone();
    }
    return true;
}

bool Repairman::assembleTable(const std::string &tableName, const std::string &sql)
{
    if (m_assembler->assembleTable(tableName, sql)) {
        if (markAsMilestone()) {
            return true;
        }
    } else {
        tryUpgrateAssemblerError();
    }
    return false;
}

bool Repairman::assembleCell(const Cell &cell)
{
    if (m_assembler->assembleCell(cell)) {
        markCellAsCounted(cell);
        towardMilestone(1);
        return true;
    }
    tryUpgrateAssemblerError();
    return false;
}

bool Repairman::assembleSequence(const std::string &tableName, int64_t sequence)
{
    if (m_assembler->assembleSequence(tableName, sequence)) {
        towardMilestone(1);
        return true;
    }
    tryUpgrateAssemblerError();
    return false;
}

void Repairman::assembleAssociatedSQLs(const std::list<std::string> &sqls)
{
    for (const auto &sql : sqls) {
        //ignore its errors
        m_assembler->assembleSQL(sql);
    }
}

#pragma mark - Crawlable
void Repairman::onCrawlerError()
{
    tryUpgradeCrawlerError();
}

#pragma mark - Error
int Repairman::tryUpgradeCrawlerError()
{
    Error error = m_pager.getError();
    if (error.isCorruption()) {
        error.level = Error::Level::Notice;
    }
    return tryUpgradeError(std::move(error));
}

int Repairman::tryUpgrateAssemblerError()
{
    Error error = m_assembler->getError();
    if (error.code() == Error::Code::Conflict) {
        error.level = Error::Level::Notice;
    }
    return tryUpgradeError(std::move(error));
}

void Repairman::onErrorCritical()
{
    stop();
}

#pragma mark - Evaluation
void Repairman::markCellAsCounted(const Cell &cell)
{
    int cellCount = cell.getPage().getCellCount();
    WCTInnerAssert(cellCount != 0);
    if (cellCount > 0) {
        Fraction cellWeight(1, cellCount);
        increaseScore(m_pageWeight * cellWeight);
    }
}

void Repairman::markPageAsCounted(const Page &page)
{
    if (page.getType() == Page::Type::LeafTable && page.getCellCount() == 0) {
        increaseScore(m_pageWeight);
    }
}

void Repairman::setPageWeight(const Fraction &pageWeight)
{
    WCTInnerAssert(m_pageWeight.value() == 0);
    m_pageWeight = pageWeight;
}

const Fraction &Repairman::getPageWeight() const
{
    return m_pageWeight;
}

} //namespace Repair

} //namespace WCDB
