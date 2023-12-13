//
// Created by qiuwenchen on 2023/11/17.
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

#include "MigrateHandleOperator.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "Time.hpp"
#include <cmath>

namespace WCDB {

MigrateHandleOperator::MigrateHandleOperator(InnerHandle* handle)
: HandleOperator(handle)
, m_migratingInfo(nullptr)
, m_migrateStatement(handle->getStatement(DecoratorMigratingHandleStatement))
, m_removeMigratedStatement(handle->getStatement(DecoratorMigratingHandleStatement))
, m_samplePointing(0)
{
}

MigrateHandleOperator::~MigrateHandleOperator()
{
    finalizeMigrationStatement();
    getHandle()->returnStatement(m_migrateStatement);
    getHandle()->returnStatement(m_removeMigratedStatement);
}

void MigrateHandleOperator::onDecorationChange()
{
    finalizeMigrationStatement();

    InnerHandle* handle = getHandle();

    handle->returnStatement(m_migrateStatement);
    m_migrateStatement = handle->getStatement(DecoratorMigratingHandleStatement);

    handle->returnStatement(m_removeMigratedStatement);
    m_removeMigratedStatement = handle->getStatement(DecoratorMigratingHandleStatement);
}

bool MigrateHandleOperator::reAttach(const MigrationBaseInfo* info)
{
    WCTAssert(!getHandle()->isInTransaction());
    WCTAssert(!getHandle()->isPrepared());

    bool succeed = true;
    if (!m_attached.syntax().isTargetingSameSchema(
        info->getSchemaForSourceDatabase().syntax())) {
        succeed = detach() && attach(info);
    }
    m_migratingInfo = nullptr;
    finalizeMigrationStatement();
    return succeed;
}

bool MigrateHandleOperator::attach(const MigrationBaseInfo* info)
{
    WCTAssert(!getHandle()->isInTransaction());
    WCTAssert(!getHandle()->isPrepared());
    WCTAssert(m_attached.syntax().isMain());

    const StatementAttach& attach = info->getStatementForAttachingSchema();
    const Data& cipher = info->getSourceCipher();
    HandleStatement handleStatement(getHandle());
    bool succeed = handleStatement.prepare(attach);
    if (succeed) {
        if (!cipher.empty()) {
            handleStatement.bindBLOB(cipher);
        }
        succeed = handleStatement.step();
        handleStatement.finalize();
    }
    if (succeed) {
        if (info->needRawSourceCipher()) {
            info->setRawSourceCipher(
            getHandle()->getRawCipherKey(info->getSchemaForSourceDatabase()));
        }
        m_attached = info->getSchemaForSourceDatabase();
    }
    return succeed;
}

bool MigrateHandleOperator::detach()
{
    WCTAssert(!getHandle()->isInTransaction());
    WCTAssert(!getHandle()->isPrepared());

    bool succeed = true;
    if (!m_attached.syntax().isMain()) {
        succeed = getHandle()->execute(WCDB::StatementDetach().detach(m_attached));
        if (succeed) {
            m_attached = Schema::main();
        }
    }
    return succeed;
}

#pragma mark - Stepper
Optional<StringViewSet> MigrateHandleOperator::getAllTables()
{
    Column name("name");
    Column type("type");
    StringView sqlitePattern
    = StringView::formatted("%s%%", Syntax::builtinTablePrefix.data());
    StringView wcdbPattern
    = StringView::formatted("%s%%", Syntax::builtinWCDBTablePrefix.data());
    return getHandle()->getValues(
    StatementSelect()
    .select(name)
    .from(TableOrSubquery::master())
    .where(type == "table" && name.notLike(sqlitePattern) && name.notLike(wcdbPattern)),
    0);
}

bool MigrateHandleOperator::dropSourceTable(const MigrationInfo* info)
{
    WCTAssert(info != nullptr);
    if (!reAttach(info)) {
        return false;
    }
    m_migratingInfo = info;
    InnerHandle* handle = getHandle();
    if (!handle->prepare(m_migratingInfo->getStatementForSelectingAnyRowFromSourceTable())) {
        return false;
    }
    if (!handle->step()) {
        handle->finalize();
        return false;
    }
    bool hasContent = !handle->done();
    handle->finalize();
    if (!hasContent) {
        return handle->execute(m_migratingInfo->getStatementForDroppingSourceTable());
    }
    return true;
}

Optional<bool> MigrateHandleOperator::migrateRows(const MigrationInfo* info)
{
    WCTAssert(info != nullptr);
    if (m_migratingInfo != info) {
        if (!reAttach(info)) {
            return NullOpt;
        }
        m_migratingInfo = info;
    }

    if (!m_migrateStatement->isPrepared()
        && !m_migrateStatement->prepare(m_migratingInfo->getStatementForMigratingOneRow())) {
        return NullOpt;
    }

    if (!m_removeMigratedStatement->isPrepared()
        && !m_removeMigratedStatement->prepare(
        m_migratingInfo->getStatementForDeletingMigratedOneRow())) {
        return NullOpt;
    }

    double timeIntervalWithinTransaction = calculateTimeIntervalWithinTransaction();
    SteadyClock beforeTransaction = SteadyClock::now();
    Optional<bool> migrated;
    if (getHandle()->runTransaction(
        [&migrated, &beforeTransaction, &timeIntervalWithinTransaction, this](InnerHandle*) -> bool {
            double cost = 0;
            do {
                migrated = migrateRow();
                cost = SteadyClock::timeIntervalSinceSteadyClockToNow(beforeTransaction);
            } while (migrated.succeed() && !migrated.value()
                     && cost < timeIntervalWithinTransaction);
            timeIntervalWithinTransaction = cost;
            return migrated.succeed();
        })) {
        // update only if succeed
        double timeIntervalWholeTranscation
        = SteadyClock::timeIntervalSinceSteadyClockToNow(beforeTransaction);
        addSample(timeIntervalWithinTransaction, timeIntervalWholeTranscation);

        WCTAssert(migrated.succeed());
        return migrated;
    }
    return NullOpt;
}

Optional<bool> MigrateHandleOperator::migrateRow()
{
    WCTAssert(m_migrateStatement->isPrepared() && m_removeMigratedStatement->isPrepared());
    WCTAssert(getHandle()->isInTransaction());
    Optional<bool> migrated;
    if (m_migrateStatement->step()) {
        if (getHandle()->getChanges() != 0) {
            if (m_removeMigratedStatement->step()) {
                migrated = false;
            }
        } else {
            migrated = true;
        }
    }
    m_migrateStatement->reset();
    m_removeMigratedStatement->reset();
    return migrated;
}

void MigrateHandleOperator::finalizeMigrationStatement()
{
    m_migrateStatement->finalize();
    m_removeMigratedStatement->finalize();
}

#pragma mark - Sample
MigrateHandleOperator::Sample::Sample()
: timeIntervalWithinTransaction(0), timeIntervalWholeTransaction(0)
{
}

void MigrateHandleOperator::addSample(double timeIntervalWithinTransaction,
                                      double timeIntervalForWholeTransaction)
{
    WCTAssert(timeIntervalWithinTransaction > 0);
    WCTAssert(timeIntervalForWholeTransaction > 0);
    WCTAssert(m_samplePointing < numberOfSamples);
    WCTAssert(timeIntervalForWholeTransaction > timeIntervalWithinTransaction);

    Sample& sample = m_samples[m_samplePointing];
    sample.timeIntervalWithinTransaction = timeIntervalWithinTransaction;
    sample.timeIntervalWholeTransaction = timeIntervalForWholeTransaction;
    ++m_samplePointing;
    if (m_samplePointing >= numberOfSamples) {
        m_samplePointing = 0;
    }
}

double MigrateHandleOperator::calculateTimeIntervalWithinTransaction() const
{
    double totalTimeIntervalWithinTransaction = 0;
    double totalTimeIntervalWholeTransaction = 0;
    for (const auto& sample : m_samples) {
        if (sample.timeIntervalWithinTransaction > 0
            && sample.timeIntervalWholeTransaction > 0) {
            totalTimeIntervalWithinTransaction += sample.timeIntervalWithinTransaction;
            totalTimeIntervalWholeTransaction += sample.timeIntervalWholeTransaction;
        }
    }
    double timeIntervalWithinTransaction = MigrateMaxExpectingDuration * totalTimeIntervalWithinTransaction
                                           / totalTimeIntervalWholeTransaction;
    if (timeIntervalWithinTransaction > MigrateMaxExpectingDuration
        || timeIntervalWithinTransaction <= 0 || std::isnan(timeIntervalWithinTransaction)) {
        timeIntervalWithinTransaction = MigrateMaxInitializeDuration;
    }
    return timeIntervalWithinTransaction;
}

#pragma mark - Info Initializer
bool MigrateHandleOperator::attachSourceDatabase(const MigrationUserInfo& userInfo)
{
    return reAttach(&userInfo);
}

InnerHandle* MigrateHandleOperator::getCurrentHandle() const
{
    return getHandle();
}

const StringView& MigrateHandleOperator::getDatabasePath() const
{
    return getHandle()->getPath();
}

} //namespace WCDB
