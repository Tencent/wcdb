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

#pragma once

#include <WCDB/CoreConst.h>
#include <WCDB/ErrorProne.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/RecyclableHandle.hpp>
#include <WCDB/ThreadedErrors.hpp>
#include <array>
#include <list>

namespace WCDB {

/*
 * There are two kind of locks in the pool.
 * 1. Memory lock is to protect the memory order of the variable inside the pool.
 * 2. Concurrency lock is to blockade all other operations while closing.
 * Corcurrency is always locked before Memory.
 *
 * When you are writing and reading any variables, you should lock or shared lock memory.
 * When you are operating m_handles, you should lock or shared lock concurrency in addition.
 */

class HandlePool : public ThreadedErrorProne {
#pragma mark - Initialize
public:
    HandlePool() = delete;
    HandlePool(const HandlePool &) = delete;
    HandlePool &operator=(const HandlePool &) = delete;

    HandlePool(const UnsafeStringView &path);
    const StringView path;

    virtual ~HandlePool();

#pragma mark - Concurrency
public:
    void blockade();
    bool isBlockaded() const;
    void unblockade();

    typedef std::function<void(void)> DrainedCallback;
    void drain(const HandlePool::DrainedCallback &onDrained);

protected:
    virtual void didDrain();
    mutable SharedLock m_concurrency;

private:
    bool isNumberOfHandlesAllowed() const;
    void clearAllHandles();

#pragma mark - Handle
public:
    typedef unsigned int Slot;
    RecyclableHandle flowOut(HandleType type);
    void purge();
    size_t numberOfAliveHandles() const;
    bool isAliving() const;

protected:
    virtual std::shared_ptr<Handle> generateSlotedHandle(HandleType type) = 0;
    virtual bool willReuseSlotedHandle(HandleType type, Handle *handle) = 0;

    mutable SharedLock m_memory;

private:
    static unsigned int slotOfHandleType(HandleType type);
    void flowBack(HandleType type, const std::shared_ptr<Handle> &handle);

    std::array<std::set<std::shared_ptr<Handle>>, HandleSlotCount> m_handles;
    std::array<std::list<std::shared_ptr<Handle>>, HandleSlotCount> m_frees;

#pragma mark - Threaded
private:
    struct ReferencedHandle {
        ReferencedHandle();

        std::shared_ptr<Handle> handle;
        int reference;
    };
    typedef struct ReferencedHandle ReferencedHandle;
};

} //namespace WCDB
