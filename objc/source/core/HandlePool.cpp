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
#include <thread>
#include <unordered_map>

namespace WCDB {

#pragma mark - Initialize
HandlePool::HandlePool(const std::string &thePath, const Configs &configs)
    : path(thePath)
    , m_tag(HandleError::invalidTag)
    , m_configs(configs)
    , m_handles(s_hardwareConcurrency)
    , m_aliveHandleCount(0)
{
}

#pragma mark - Basic
void HandlePool::setTag(const Tag &tag)
{
    WCTAssert(tag != HandleError::invalidTag, "Tag invalid");
    m_tag.store(tag);
}

HandlePool::Tag HandlePool::getTag() const
{
    return m_tag.load();
}

#pragma mark - Error
void HandlePool::setAndReportCoreError(const std::string &message)
{
    CoreError error(getTag(), message);
    setThreadedError(error);
    error.report();
}

void HandlePool::setThreadedError(const HandleError &error) const
{
    setThreadedError(CoreError(error));
}

void HandlePool::setThreadedError(const CoreError &error) const
{
    std::unordered_map<const HandlePool *, CoreError> *errors =
        s_threadedErrors.get();
    auto iter = errors->find(this);
    if (iter != errors->end()) {
        iter->second = error;
    } else {
        errors->insert({this, error});
    }
}

const CoreError &HandlePool::getThreadedError() const
{
    ThreadedErrors *errors = s_threadedErrors.get();
    auto iter = errors->find(this);
    WCTRemedialAssert(
        iter != errors->end(),
        "Error should be obtained if and only if a interface return false.",
        static const CoreError s_coreError;
        return s_coreError;);
    return iter->second;
}

#pragma mark - Config
void HandlePool::setConfig(const std::string &name,
                           const Config::Callback &callback)
{
    m_configs.setConfig(name, callback);
}

void HandlePool::setConfig(const Config &config)
{
    m_configs.setConfig(config);
}

#pragma mark - Handle
const int HandlePool::s_hardwareConcurrency =
    std::thread::hardware_concurrency();
const int HandlePool::s_maxConcurrency =
    std::max<int>(std::thread::hardware_concurrency(), 64);
ThreadLocal<HandlePool::ThreadedErrors> HandlePool::s_threadedErrors;

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
    return Handle::handleWithPath(path, getTag());
}

std::shared_ptr<ConfiguredHandle> HandlePool::flowOutConfiguredHandle()
{
    WCTInnerAssert(m_sharedLock.debug_isSharedLocked() ||
                   m_sharedLock.isLocked());
    std::shared_ptr<ConfiguredHandle> configuredHandle = m_handles.popBack();
    if (!configuredHandle) {
        return nullptr;
    }
    willConfigurateHandle(configuredHandle.get()->getHandle());
    Configs configs = m_configs;
    if (!configuredHandle->configured(configs) &&
        !configuredHandle->configure(configs)) {
        setThreadedError(configuredHandle->getHandle()->getError());
        return nullptr;
    }
    return configuredHandle;
}

std::shared_ptr<ConfiguredHandle> HandlePool::generateConfiguredHandle()
{
    WCTInnerAssert(m_sharedLock.debug_isSharedLocked() ||
                   m_sharedLock.isLocked());
    if (m_aliveHandleCount >= s_maxConcurrency) {
        setAndReportCoreError(
            "The concurrency of database exceeds the maxximum allowed: " +
            std::to_string(s_maxConcurrency));
        return nullptr;
    }
    std::shared_ptr<ConfiguredHandle> configuredHandle =
        ConfiguredHandle::configuredHandle(generateHandle());
    if (!configuredHandle) {
        setAndReportCoreError("Out Of Memory");
        return nullptr;
    }
    Handle *handle = configuredHandle->getHandle();
    Configs configs = m_configs;

    if (m_aliveHandleCount == 0) {
        FileManager::shared()->createDirectoryWithIntermediateDirectories(
            Path::getBaseName(path));
    }
    if (!handle->open()) {
        setThreadedError(handle->getError());
        return nullptr;
    }
    willConfigurateHandle(handle);
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
    WCTInnerAssert(m_sharedLock.debug_isSharedLocked() ||
                   m_sharedLock.isLocked());
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

} //namespace WCDB
