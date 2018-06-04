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

#include <WCDB/Core.h>
#include <WCDB/FileManager.hpp>
#include <WCDB/Path.hpp>
#include <algorithm>
#include <map>
#include <thread>

namespace WCDB {

#pragma mark - Initialize
HandlePool::HandlePool(const std::string &thePath,
                       const std::shared_ptr<const Configs> &configs)
    : path(thePath)
    , m_tag(Handle::invalidTag)
    , m_configs(configs)
    , m_handles(HandlePool::hardwareConcurrency())
    , m_aliveHandleCount(0)
{
}

HandlePool::~HandlePool()
{
    //wait until all handles back.
    blockade();
    unblockade();
}

#pragma mark - Basic
void HandlePool::setTag(const Tag &tag)
{
    WCTAssert(tag != Handle::invalidTag, "Tag invalid");
    m_tag.store(tag);
}

HandlePool::Tag HandlePool::getTag() const
{
    return m_tag.load();
}

#pragma mark - Error
const Error &HandlePool::getError()
{
    return getThreadedError();
}

#pragma mark - Config
void HandlePool::setConfig(const std::shared_ptr<Config> &config)
{
    m_configs = m_configs->configsBySettingConfig(config);
}

void HandlePool::removeConfig(const std::string &name)
{
    m_configs = m_configs->configsByRemovingConfig(name);
}

#pragma mark - Handle
int HandlePool::hardwareConcurrency()
{
    static const int s_hardwareConcurrency =
        std::thread::hardware_concurrency() > 0
            ? std::thread::hardware_concurrency()
            : 8;
    return s_hardwareConcurrency;
}

int HandlePool::maxConcurrency()
{
    static int s_maxConcurrency =
        std::max<int>(HandlePool::hardwareConcurrency(), 64);
    return s_maxConcurrency;
}

void HandlePool::blockade()
{
    m_sharedLock.lock();
}

bool HandlePool::blockadeUntilDone(const BlockadeCallback &onBlockaded)
{
    LockGuard lockGuard(m_sharedLock);
    std::shared_ptr<ConfiguredHandle> configuredHandle =
        flowOutConfiguredHandle();
    if (!configuredHandle) {
        configuredHandle = generateConfiguredHandle();
    }
    bool result = false;
    if (configuredHandle) {
        onBlockaded(configuredHandle->getHandle());
        flowBackConfiguredHandle(configuredHandle);
        result = true;
    }
    return result;
}

void HandlePool::unblockade()
{
    WCTAssert(m_sharedLock.isLocked(),
              "Unblockade should not be called without blockaded.");
    m_sharedLock.unlock();
}

bool HandlePool::isBlockaded() const
{
    return m_sharedLock.isLocked();
}

void HandlePool::drain(const HandlePool::DrainedCallback &onDrained)
{
    LockGuard lockGuard(m_sharedLock);
    ConcurrentList<ConfiguredHandle>::ElementType handle;
    while ((handle = m_handles.popBack())) {
        handle->getHandle()->close();
        --m_aliveHandleCount;
    }
    if (onDrained) {
        onDrained();
    }
}

void HandlePool::purgeFreeHandles()
{
    SharedLockGuard lockGuard(m_sharedLock);
    int size = (int) m_handles.clear();
    m_aliveHandleCount -= size;
}

bool HandlePool::isDrained()
{
    return m_aliveHandleCount == 0;
}

bool HandlePool::canFlowOut()
{
    SharedLockGuard lockGuard(m_sharedLock);
    std::shared_ptr<ConfiguredHandle> configuredHandle =
        flowOutConfiguredHandle();
    if (!configuredHandle) {
        configuredHandle = generateConfiguredHandle();
    }
    if (configuredHandle) {
        flowBackConfiguredHandle(configuredHandle);
        return true;
    }
    return false;
}

RecyclableHandle HandlePool::flowOut()
{
    m_sharedLock.lockShared();
    std::shared_ptr<ConfiguredHandle> configuredHandle =
        flowOutConfiguredHandle();
    if (!configuredHandle) {
        configuredHandle = generateConfiguredHandle();
    }
    if (configuredHandle) {
        return RecyclableHandle(configuredHandle, [configuredHandle, this]() {
            flowBack(configuredHandle);
        });
    }
    m_sharedLock.unlockShared();
    return nullptr;
}

void HandlePool::flowBack(
    const std::shared_ptr<ConfiguredHandle> &configuredHandle)
{
    flowBackConfiguredHandle(configuredHandle);
    m_sharedLock.unlockShared();
}

std::shared_ptr<Handle> HandlePool::generateHandle()
{
    return Handle::handleWithPath(path);
}

std::shared_ptr<ConfiguredHandle> HandlePool::flowOutConfiguredHandle()
{
#ifdef DEBUG
    WCTInnerAssert(m_sharedLock.debug_isSharedLocked() ||
                   m_sharedLock.isLocked());
#endif
    std::shared_ptr<ConfiguredHandle> configuredHandle = m_handles.popBack();
    if (!configuredHandle) {
        return nullptr;
    }
    willConfigurateHandle(configuredHandle.get()->getHandle());
    std::shared_ptr<const Configs> configs = m_configs;
    if (!configuredHandle->configured(configs) &&
        !configuredHandle->configure(configs)) {
        setThreadedError(configuredHandle->getHandle()->getError());
        return nullptr;
    }
    return configuredHandle;
}

std::shared_ptr<ConfiguredHandle> HandlePool::generateConfiguredHandle()
{
#ifdef DEBUG
    WCTInnerAssert(m_sharedLock.debug_isSharedLocked() ||
                   m_sharedLock.isLocked());
#endif
    if (m_aliveHandleCount >= HandlePool::maxConcurrency()) {
        setThreadedError(
            Error(Error::Code::Exceed,
                  "The concurrency of database exceeds the maxximum allowed: " +
                      std::to_string(HandlePool::maxConcurrency())));
        return nullptr;
    }
    std::shared_ptr<ConfiguredHandle> configuredHandle =
        ConfiguredHandle::configuredHandle(generateHandle());
    if (!configuredHandle) {
        setThreadedError(Error(Error::Code::NoMemory));
        return nullptr;
    }
    Handle *handle = configuredHandle->getHandle();

    if (m_aliveHandleCount == 0) {
        FileManager::shared()->createDirectoryWithIntermediateDirectories(
            Path::getBaseName(path));
    }
    willConfigurateHandle(handle);
    if (!handle->open()) {
        setThreadedError(handle->getError());
        return nullptr;
    }
    std::shared_ptr<const Configs> configs = m_configs;
    if (!configuredHandle->configure(configs)) {
        setThreadedError(handle->getError());
        return nullptr;
    }
    ++m_aliveHandleCount;
    return configuredHandle;
}

void HandlePool::flowBackConfiguredHandle(
    const std::shared_ptr<ConfiguredHandle> &configuredHandle)
{
    WCTInnerAssert(configuredHandle != nullptr);
#ifdef DEBUG
    WCTInnerAssert(m_sharedLock.debug_isSharedLocked() ||
                   m_sharedLock.isLocked());
#endif
    if (!m_handles.pushBack(configuredHandle)) {
        --m_aliveHandleCount;
    }
}

bool HandlePool::willConfigurateHandle(Handle *handle)
{
    if (handle->getTag() != getTag()) {
        handle->setTag(getTag());
    }
    return true;
}

#pragma mark - ThreadedErrorProne
void HandlePool::setThreadedError(const Error &error)
{
    Error threadedError = error;
    setThreadedError(std::move(threadedError));
}

void HandlePool::setThreadedError(Error &&error)
{
    if (getTag() != Handle::invalidTag) {
        error.infos.set("Tag", getTag());
    }
    error.infos.set("Path", path);
    Reporter::shared()->report(error);
    ThreadedErrorProne::setThreadedError(std::move(error));
}

ThreadedErrors *HandlePool::getThreadedErrors()
{
    return &m_errors;
}

ThreadedErrors *HandlePoolThreadedErrorProne::getThreadedErrors()
{
    return getHandlePool()->getThreadedErrors();
}

} //namespace WCDB
