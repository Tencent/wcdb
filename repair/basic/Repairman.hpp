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

#ifndef _WCDB_REPAIRMAN_HPP
#define _WCDB_REPAIRMAN_HPP

#include <WCDB/Assembler.hpp>
#include <WCDB/Crawlable.hpp>
#include <WCDB/ErrorProne.hpp>
#include <WCDB/Progress.hpp>
#include <WCDB/Scoreable.hpp>
#include <WCDB/UpgradeableErrorProne.hpp>
#include <list>

namespace WCDB {

namespace Repair {

class Repairman : public Crawlable,
                  public UpgradeableErrorProne,
                  public Progress,
                  public SegmentedScoreable,
                  public AssemblerHolder {
#pragma mark - Initialize
public:
    Repairman(const String &path);
    virtual ~Repairman();

    const String &getPath() const;

protected:
    bool isEmptyDatabase();
    bool exit();
    bool exit(bool result);

#pragma mark - Crawlable
protected:
    void onCrawlerError() override final;
    Pager m_pager;

#pragma mark - Error
protected:
    int tryUpgrateAssemblerError();
    int tryUpgradeCrawlerError();

    virtual void onErrorCritical() override;

#pragma mark - Assembler
protected:
    bool markAsAssembling();
    void markAsAssembled();

    bool assembleTable(const String &tableName, const String &sql);
    bool assembleCell(const Cell &cell);
    bool assembleSequence(const String &tableName, int64_t sequence);
    void assembleAssociatedSQLs(const std::list<String> &sqls);

protected:
    bool towardMilestone(int mile);

private:
    bool markAsMilestone();
    int m_milestone;
    int m_mile;

#pragma mark - Evaluation
protected:
    void setPageWeight(const Fraction &pageWeight);
    const Fraction &getPageWeight() const;
    void markCellAsCounted(const Cell &cell);
    void markPageAsCounted(const Page &page);

private:
    Fraction m_pageWeight;
};

} //namespace Repair

} //namespace WCDB

#endif /* _WCDB_REPAIRMAN_HPP */
