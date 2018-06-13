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
    , attachment(this)
    , m_inited(false)
    , m_onInitializing(nullptr)
{
}

HandlePool::~HandlePool()
{
    //wait until all handles back.
    drain(nullptr);
}

void HandlePool::setInitializeNotification(
    const WCDB::HandlePool::InitializeNotificationCallback &onInitializing)
{
    LockGuard lockGuard(m_sharedLock);
    m_onInitializing = onInitializing;
}

bool HandlePool::initialize()
{
    if (!m_inited.load()) {
        WCTInnerAssert(m_aliveHandleCount.load() == 0);
        blockade();
        if (!FileManager::shared()->createDirectoryWithIntermediateDirectories(
                Path::getBaseName(path))) {
            assignWithSharedThreadedError();
            return false;
        }
        if (m_onInitializing && !m_onInitializing(*this)) {
            return false;
        }
        m_inited.store(true);
        unblockade();
    }
    return true;
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
    if (!initialize()) {
        return false;
    }
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
    WCTAssert(isBlockaded(),
              "Unblockade should not be called without blockaded.");
    m_sharedLock.unlock();
}

bool HandlePool::isBlockaded() const
{
    return m_sharedLock.level() == SharedLock::Level::Write;
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
    if (!initialize()) {
        return false;
    }
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
    if (!initialize()) {
        return nullptr;
    }
    SharedLockGuard lockGuard(m_sharedLock);
    std::shared_ptr<ConfiguredHandle> configuredHandle =
        flowOutConfiguredHandle();
    if (!configuredHandle) {
        configuredHandle = generateConfiguredHandle();
    }
    if (configuredHandle) {
        m_sharedLock.lockShared();
        return RecyclableHandle(
            configuredHandle,
            std::bind(&HandlePool::flowBack, this, std::placeholders::_1));
    }
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
    WCTInnerAssert(m_sharedLock.level() > SharedLock::Level::None);
    std::shared_ptr<ConfiguredHandle> configuredHandle = m_handles.popBack();
    if (!configuredHandle) {
        return nullptr;
    }
    if (configuredHandle->getHandle()->getTag() != getTag()) {
        configuredHandle->getHandle()->setTag(getTag());
    }
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
    WCTInnerAssert(m_inited.load());
    WCTInnerAssert(m_sharedLock.level() > SharedLock::Level::None);
    if (m_aliveHandleCount.load() >= HandlePool::maxConcurrency()) {
        Error error;
        error.setCode(Error::Code::Exceed);
        error.message =
            "The concurrency of database exceeds the maximum allowed.";
        error.infos.set("MaxConcurrency", HandlePool::maxConcurrency());
        Notifier::shared()->notify(error);
        setThreadedError(std::move(error));
        return nullptr;
    }

    std::shared_ptr<Handle> handle = generateHandle();
    if (!handle) {
        setThreadedError(Error(Error::Code::NoMemory));
        return nullptr;
    }
    handle->setTag(getTag());
    if (!handle->open()) {
        setThreadedError(handle->getError());
        return nullptr;
    }

    std::shared_ptr<ConfiguredHandle> configuredHandle =
        ConfiguredHandle::configuredHandle(handle);
    if (!configuredHandle) {
        setThreadedError(Error(Error::Code::NoMemory));
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
    WCTInnerAssert(m_sharedLock.level() > SharedLock::Level::None);
    if (!m_handles.pushBack(configuredHandle)) {
        --m_aliveHandleCount;
    }
}

} //namespace WCDB
