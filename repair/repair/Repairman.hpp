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
#include <WCDB/ErrorProne.hpp>
#include <WCDB/Progress.hpp>
#include <WCDB/Scoreable.hpp>
#include <WCDB/UpgradeableErrorProne.hpp>

namespace WCDB {

namespace Repair {

class Repairman : public Crawlable,
                  public UpgradeableErrorProne,
                  public Progress,
                  public SegmentedScoreable {
#pragma mark - Initialize
public:
    Repairman(const std::string &path);

    const std::string &getPath() const;

protected:
    bool isEmptyDatabase();

#pragma mark - Crawlable
protected:
    void onCrawlerError() override;
    Pager m_pager;

#pragma mark - Error
protected:
    int tryUpgrateAssemblerError();
    int tryUpgradeCrawlerError();

    void onErrorCritical() override;

#pragma mark - Assembler
public:
    void setAssembler(const std::shared_ptr<Assembler> &assembler);

protected:
    bool markAsAssembling();
    void markAsAssembled();

    bool assembleTable(const std::string &tableName, const std::string &sql);
    bool assembleCell(const Cell &cell);
    bool assembleSequence(const std::string &tableName, int64_t sequence);

protected:
    bool towardMilestone(int mile);

private:
    bool markAsMilestone();
    int m_milestone;
    int m_mile;
    std::shared_ptr<Assembler> m_assembler;

#pragma mark - Evaluation
protected:
    void setPageWeight(const Fraction &pageWeight);
    const Fraction &getPageWeight() const;
    void markCellAsCounted(const Cell &cell);

private:
    Fraction m_pageWeight;
};

} //namespace Repair

} //namespace WCDB

#endif /* Repairman_hpp */
