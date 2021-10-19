//
// Created by qiuwenchen(陈秋文) on 2020/10/23.
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

#include <WCDB/AsyncQueue.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/RecyclableHandle.hpp>
#include <WCDB/StringView.hpp>
#include <WCDB/TimedQueue.hpp>
#include <array>

namespace WCDB {

class MergeFTSIndexHandleProvider {
public:
    virtual ~MergeFTSIndexHandleProvider() = 0;

protected:
    friend class MergeFTSIndexLogic;
    virtual RecyclableHandle getHandle() = 0;
    virtual const StringView& getPath() const = 0;
};

class MergeFTSIndexLogic {
public:
    MergeFTSIndexLogic() = delete;
    MergeFTSIndexLogic(MergeFTSIndexHandleProvider* provider);
    using TableArray = std::shared_ptr<std::vector<StringView>>;
    std::optional<bool> triggerMerge(TableArray newTables, TableArray modifiedTables);

private:
    bool tryInit(Handle& handle);
    std::optional<bool>
    triggerMerge(Handle& handle, TableArray newTables, TableArray modifiedTables);
    bool tryConfigUserMerge(Handle& handle, const UnsafeStringView& table, bool isNew);
    bool checkModifiedTables(Handle& handle, TableArray newTables, TableArray modifiedTables);
    void proccessMerge();
    bool mergeTable(Handle& handle, const StringView& table);
    void increaseErrorCount();

    static void
    userMergeCallback(Handle* handle, int* remainPages, int totalPagesWriten, int* lastCheckPages);

    MergeFTSIndexHandleProvider* m_handleProvider;

    bool m_hasInit;
    std::atomic<bool> m_processing;
    std::atomic<int> m_errorCount;

    SharedLock m_lock;

    Statement m_getTableStatement;
    std::set<StringView> m_mergingTables;
    std::set<StringView> m_mergedTables;

private:
    class OperationQueue : public AsyncQueue {
    public:
        OperationQueue() = delete;
        OperationQueue(const UnsafeStringView& name);
        static OperationQueue& shared();

        using OperationCallBack = std::function<void(void)>;
        void async(const UnsafeStringView& path, const OperationCallBack& callback);
        void cancelOperation(const UnsafeStringView& path);

    private:
        using AsyncMode = TimedQueue<StringView, OperationCallBack>::Mode;
        void main() override final;
        TimedQueue<StringView, OperationCallBack> m_timedQueue;
        void onTimed(const StringView& path, const OperationCallBack& callback);
    };
};

} // namespace WCDB
