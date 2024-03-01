//
// Created by sanhuazhang on 2018/09/26
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

#include "Repairman.hpp"
#include "Assertion.hpp"
#include "Cell.hpp"
#include "CoreConst.h"
#include "FileManager.hpp"
#include "Notifier.hpp"
#include "Page.hpp"
#include "ThreadedErrors.hpp"

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Repairman::Repairman(const UnsafeStringView &path)
: Crawlable(), Progress(), m_pager(path), m_milestone(1000), m_mile(0)
{
    setAssociatedPager(&m_pager);
}

Repairman::~Repairman() = default;

const StringView &Repairman::getPath() const
{
    return m_pager.getPath();
}

int64_t Repairman::getTotalPageCount() const
{
    if (m_pageWeight.value() > 0) {
        return (int64_t) (1.0 / m_pageWeight.value());
    }
    return 0;
}

int Repairman::getDisposedWalPageCount() const
{
    return m_pager.getDisposedWalPages();
}

bool Repairman::exit()
{
    if (!isErrorCritial()) {
        return finishProgress();
    }
    return false;
}

bool Repairman::exit(bool result)
{
    if (result) {
        return finishProgress();
    }
    return result;
}

Optional<bool> Repairman::isEmptyDatabase()
{
    auto fileSize = FileManager::getFileSize(getPath());
    if (fileSize.succeed()) {
        if (fileSize.value() == 0) {
            Error error(Error::Code::Empty, Error::Level::Warning, "Database is not found or empty.");
            error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
            error.infos.insert_or_assign(ErrorStringKeyAssociatePath, getPath());
            Notifier::shared().notify(error);
            return true;
        } else {
            return false;
        }
    } else {
        setCriticalErrorWithSharedThreadedError();
        return NullOpt;
    }
}

bool Repairman::markAsAssembling()
{
    if (m_assembleDelegate->markAsAssembling()) {
        return true;
    }
    setCriticalError(m_assembleDelegate->getAssembleError());
    return false;
}

void Repairman::markAsAssembled()
{
    markAsMilestone();
    if (!isErrorCritial() && !m_assembleDelegate->markAsAssembled()) {
        setCriticalError(m_assembleDelegate->getAssembleError());
    }
}

bool Repairman::markAsMilestone()
{
    m_mile = 0;
    if (m_assembleDelegate->markAsMilestone()) {
        markSegmentedScoreCounted();
        return true;
    }
    markSegmentedScoreDropped();
    tryUpgrateAssembleError();
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

bool Repairman::assembleTable(const UnsafeStringView &tableName, const UnsafeStringView &sql)
{
    if (m_assembleDelegate->assembleTable(tableName, sql)) {
        if (markAsMilestone()) {
            return true;
        }
    } else {
        tryUpgrateAssembleError();
    }
    return false;
}

bool Repairman::assembleCell(const Cell &cell)
{
    if (m_assembleDelegate->assembleCell(cell)) {
        markCellAsCounted(cell);
        towardMilestone(1);
        return true;
    }
    tryUpgrateAssembleError();
    return false;
}

bool Repairman::assembleSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    if (m_assembleDelegate->assembleSequence(tableName, sequence)) {
        towardMilestone(1);
        return true;
    }
    tryUpgrateAssembleError();
    return false;
}

void Repairman::assembleAssociatedSQLs(const std::list<StringView> &sqls)
{
    for (const auto &sql : sqls) {
        //ignore its errors
        m_assembleDelegate->assembleSQL(sql);
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

int Repairman::tryUpgrateAssembleError()
{
    Error error = m_assembleDelegate->getAssembleError();
    if (error.code() == Error::Code::Constraint && !isErrorCritial()) {
        error.level = Error::Level::Notice;
    }
    return tryUpgradeError(std::move(error));
}

void Repairman::onErrorCritical()
{
    suspend();
}

#pragma mark - Evaluation
void Repairman::markCellAsCounted(const Cell &cell)
{
    if (cell.getPage().isIndexPage()) {
        return;
    }
    int numberOfCells = cell.getPage().getNumberOfCells();
    WCTAssert(numberOfCells != 0);
    if (numberOfCells > 0) {
        Fraction cellWeight(1, numberOfCells);
        increaseScore(m_pageWeight * cellWeight);
    }
}

void Repairman::markPageAsCounted(const Page &page)
{
    if (page.getType() == Page::Type::LeafTable && page.getNumberOfCells() == 0) {
        increaseScore(m_pageWeight);
    }
}

void Repairman::setPageWeight(const Fraction &pageWeight)
{
    WCTAssert(m_pageWeight.value() == 0);
    m_pageWeight = pageWeight;
}

const Fraction &Repairman::getPageWeight() const
{
    return m_pageWeight;
}

} //namespace Repair

} //namespace WCDB
