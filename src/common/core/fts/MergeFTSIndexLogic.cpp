//
// Created by qiuwenchen(qiuwenchen) on 2020/10/23.
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

#include "MergeFTSIndexLogic.hpp"
#include "Assertion.hpp"
#include "Core.hpp"
#include "CoreConst.h"
#include "Notifier.hpp"
#include "WCDBError.hpp"
#include <cmath>

namespace WCDB {

MergeFTSIndexHandleProvider::~MergeFTSIndexHandleProvider() = default;

MergeFTSIndexLogic::MergeFTSIndexLogic(MergeFTSIndexHandleProvider *handleProvider)
: m_handleProvider(handleProvider)
, m_hasInit(false)
, m_processing(false)
, m_errorCount(0)
, m_getTableStatement(StatementSelect()
                      .select(Column("name"))
                      .from("sqlite_master")
                      .where(Column("type") == "table"
                             && Column("sql").like("CREATE VIRTUAL TABLE % USING fts5(%")))
{
}

bool MergeFTSIndexLogic::tryInit(InnerHandle &handle)
{
    if (m_hasInit) {
        return true;
    }
    m_mergedTables.clear();
    m_mergingTables.clear();

    if (!handle.prepare(m_getTableStatement)) {
        return false;
    }
    bool success = false;
    while ((success = handle.step()) && !handle.done()) {
        UnsafeStringView table = handle.getText(0);
        WCTAssert(!table.empty());
        m_mergingTables.emplace(table);
    }
    handle.finalize();
    if (!success) {
        return false;
    }
    for (const StringView &element : m_mergingTables) {
        if (!tryConfigUserMerge(handle, element, false)) {
            return false;
        }
    }

    m_hasInit = true;
    return true;
}

bool MergeFTSIndexLogic::tryConfigUserMerge(InnerHandle &handle,
                                            const UnsafeStringView &table,
                                            bool isNew)
{
    bool needConfig = isNew;
    if (!isNew) {
        Statement selectConfig = StatementSelect()
                                 .select(Column("v"))
                                 .from(StringView().formatted("%s_config", table.data()))
                                 .where(Column("k") == "usermerge");
        if (!handle.prepare(selectConfig)) {
            return false;
        }
        if (!handle.step()) {
            handle.finalize();
            return false;
        }
        if (handle.done() || handle.getInteger(0) != 2) {
            needConfig = true;
        }
        handle.finalize();
    }
    if (needConfig) {
        if (!handle.execute(StatementInsert()
                            .insertIntoTable(table)
                            .columns({ Column(table), Column("rank") })
                            .values({ "usermerge", 2 }))) {
            return false;
        }
    }
    return true;
}

Optional<bool> MergeFTSIndexLogic::triggerMerge(TableArray newTables, TableArray modifiedTables)
{
    RecyclableHandle recyclableHandle = m_handleProvider->getMergeIndexHandle();
    if (recyclableHandle == nullptr) {
        return false;
    }
    WCTRemedialAssert(!recyclableHandle->isInTransaction(),
                      "Merge Index can't be run in transaction.",
                      return NullOpt;);

    InnerHandle *handle = recyclableHandle.get();
    handle->markAsCanBeSuspended(true);
    handle->markErrorAsIgnorable(Error::Code::Busy);
    handle->setTableMonitorEnable(false);

    Optional<bool> done = triggerMerge(*handle, newTables, modifiedTables);
    if (!done.succeed() && handle->getError().isIgnorable()) {
        done = false;
    }
    handle->setTableMonitorEnable(true);
    return done;
}

Optional<bool>
MergeFTSIndexLogic::triggerMerge(InnerHandle &handle, TableArray newTables, TableArray modifiedTables)
{
    LockGuard lockGuard(m_lock);
    if (m_errorCount.load() > 5) {
        return NullOpt;
    }
    if (!tryInit(handle)) {
        increaseErrorCount();
        return NullOpt;
    }
    if (!checkModifiedTables(handle, newTables, modifiedTables)) {
        increaseErrorCount();
        return NullOpt;
    }
    if (m_mergingTables.size() == 0) {
        return true;
    }
    if (m_processing) {
        return false;
    }
    OperationQueue::shared().async(handle.getPath(), [](const UnsafeStringView &path) {
        RecyclableDatabase database = Core::shared().getOrCreateDatabase(path);
        if (database != nullptr) {
            database->proccessMerge();
        }
    });
    return false;
}

void MergeFTSIndexLogic::proccessMerge()
{
    StringView table;
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_errorCount > 5) {
            return;
        }
        if (m_mergingTables.size() == 0) {
            return;
        }
        table = *m_mergingTables.begin();
    }
    RecyclableHandle recyclableHandle = m_handleProvider->getMergeIndexHandle();
    if (recyclableHandle == nullptr) {
        return;
    }
    WCTRemedialAssert(!recyclableHandle->isInTransaction(),
                      "Merge Index can't be run in transaction.",
                      return;);

    InnerHandle *handle = recyclableHandle.get();
    handle->markAsCanBeSuspended(true);
    handle->markErrorAsIgnorable(Error::Code::Busy);
    handle->setTableMonitorEnable(false);

    while (!table.empty()) {
        if (!mergeTable(*handle, table)) {
            if (!handle->getError().isIgnorable()) {
                increaseErrorCount();
            }
            break;
        }
        SharedLockGuard lockGuard(m_lock);
        m_mergingTables.erase(table);
        m_mergedTables.emplace(table);
        if (m_mergingTables.size() > 0) {
            table = *m_mergingTables.begin();
        } else {
            break;
        }
    }
    handle->setTableMonitorEnable(true);
}

bool MergeFTSIndexLogic::mergeTable(InnerHandle &handle, const StringView &table)
{
    int preChangeCount;
    Statement mergeSTM
    = StatementInsert()
      .insertIntoTable(table)
      .columns({ Column(table), Column("rank"), Column().rowid() })
      .values({ UnsafeStringView("merge"), 256, WCDB::BindParameter(1) });
    if (!handle.prepare(mergeSTM)) {
        return false;
    }
    void **callbackPointer = new void *[2];
    callbackPointer[0] = (void *) MergeFTSIndexLogic::userMergeCallback;
    callbackPointer[1] = &handle;
    do {
        preChangeCount = handle.getTotalChange();
        handle.bindPointer(callbackPointer, 1, "fts5_user_merge_callback", nullptr);
        if (!handle.step()) {
            handle.finalize();
            delete[] callbackPointer;
            return false;
        } else {
            handle.reset();
        }
        //Use prime numbers to reduce the probability of collision with external logic
        std::this_thread::sleep_for(std::chrono::microseconds(1229));
    } while (handle.getTotalChange() - preChangeCount > 1);
    handle.finalize();
    delete[] callbackPointer;
    return true;
}

void MergeFTSIndexLogic::userMergeCallback(InnerHandle *handle,
                                           int *remainPages,
                                           int totalPagesWriten,
                                           int *lastCheckPages)
{
    if (totalPagesWriten - *lastCheckPages < 16) {
        return;
    }
    *lastCheckPages = totalPagesWriten;
    if (!handle->checkHasBusyRetry()) {
        return;
    }
    *remainPages = totalPagesWriten - 1;
}

bool MergeFTSIndexLogic::checkModifiedTables(InnerHandle &handle,
                                             TableArray newTables,
                                             TableArray modifiedTables)
{
    if (newTables != nullptr && newTables->size() > 0) {
        StatementSelect select
        = StatementSelect()
          .select(Column().rowid())
          .from("sqlite_master")
          .where(Column("name") == BindParameter(1)
                 && Column("sql").like("CREATE VIRTUAL TABLE % USING fts5(%"));

        if (!handle.prepare(select)) {
            return false;
        }

        TableArray fts5Tables(new std::vector<StringView>());

        for (const auto &element : *newTables) {
            if (m_mergedTables.find(element) != m_mergedTables.end()) {
                m_mergedTables.erase(element);
            }
            handle.bindText(element.data(), 1);
            if (!handle.step()) {
                handle.finalize();
                return false;
            }
            if (!handle.done()) {
                m_mergingTables.emplace(element);
                fts5Tables->push_back(element);
            }
            handle.reset();
        }

        handle.finalize();

        for (const auto &element : *fts5Tables) {
            if (!tryConfigUserMerge(handle, element, true)) {
                return false;
            }
        }
    }

    if (modifiedTables != nullptr && modifiedTables->size() > 0) {
        for (const auto &element : *modifiedTables) {
            if (m_mergedTables.find(element) != m_mergedTables.end()) {
                m_mergedTables.erase(element);
                m_mergingTables.emplace(element);
            }
        }
    }
    return true;
}

void MergeFTSIndexLogic::increaseErrorCount()
{
    m_errorCount++;
    if (m_errorCount.load() > 5) {
        Error error(Error::Code::Notice,
                    Error::Level::Notice,
                    "Auto merge fts index is stopped due to too many errors.");
        error.infos.insert_or_assign(ErrorStringKeyPath, m_handleProvider->getPath());
        error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeMergeIndex);
        Notifier::shared().notify(error);
    }
}

#pragma mark - OperationQueue

MergeFTSIndexLogic::OperationQueue &MergeFTSIndexLogic::OperationQueue::shared()
{
    static OperationQueue *g_operationQueue = nullptr;
    if (!g_operationQueue) {
        g_operationQueue = new OperationQueue(AutoMergeFTSIndexQueueName);
    }
    return *g_operationQueue;
}

MergeFTSIndexLogic::OperationQueue::OperationQueue(const UnsafeStringView &name)
: AsyncQueue(name)
{
    run();
}

void MergeFTSIndexLogic::OperationQueue::async(const UnsafeStringView &path,
                                               const OperationCallBack &callback)
{
    m_timedQueue.queue(StringView(path), 0, callback, AsyncMode::ForwardOnly);
}

void MergeFTSIndexLogic::OperationQueue::cancelOperation(const UnsafeStringView &path)
{
    m_timedQueue.remove(StringView(path));
}

void MergeFTSIndexLogic::OperationQueue::main()
{
    m_timedQueue.loop(std::bind(&MergeFTSIndexLogic::OperationQueue::onTimed,
                                this,
                                std::placeholders::_1,
                                std::placeholders::_2));
}

void MergeFTSIndexLogic::OperationQueue::onTimed(const StringView &path,
                                                 const OperationCallBack &callback)
{
    Core::shared().setThreadedErrorIgnorable(true);
    callback(path);
    Core::shared().setThreadedErrorIgnorable(false);
}

} // namespace WCDB
