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

#include <WCDB/FileManager.hpp>
#include <WCDB/HandlePool.hpp>
#include <WCDB/Hash.hpp>
#include <WCDB/Path.hpp>
#include <algorithm>
#include <thread>
#include <unordered_map>

namespace WCDB {

#pragma mark - Initialize
HandlePool::HandlePool(const std::string &thePath, const Configs &configs)
    : path(thePath)
    , tag(hash(thePath))
    , m_configs(configs)
    , m_handles(s_hardwareConcurrency)
    , m_aliveHandleCount(0)
{
}

#pragma mark - Error
void HandlePool::setAndReportCoreError(const std::string &message)
{
    CoreError error;
    error.tag = tag.load();
    error.message = message;
    error.code = (int) Error::Code::Error;
    setThreadedError(error);
    error.report();
}

std::shared_ptr<Error> &HandlePool::getThreadedErrors() const
{
    std::unordered_map<const HandlePool *, std::shared_ptr<Error>> *errors =
        s_threadedErrors.get();
    auto iter = errors->find(this);
    if (iter == errors->end()) {
        iter = errors->insert({this, std::shared_ptr<Error>()}).first;
    }
    return iter->second;
}

void HandlePool::setThreadedError(const HandleError &error) const
{
    getThreadedErrors().reset(new HandleError(error));
}

void HandlePool::setThreadedError(const FileError &error) const
{
    getThreadedErrors().reset(new FileError(error));
}

void HandlePool::setThreadedError(const CoreError &error) const
{
    getThreadedErrors().reset(new CoreError(error));
}

const Error &HandlePool::getThreadedError() const
{
    ThreadedErrors *errors = s_threadedErrors.get();
    auto iter = errors->find(this);
    assert(iter != errors->end());
    return *iter->second.get();
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
    m_rwlock.lockWrite();
}

bool HandlePool::blockadeUntilDone(const BlockadeCallback &onBlockaded)
{
    m_rwlock.lockWrite();
    std::shared_ptr<ConfiguredHandle> configuredHandle =
        flowOutConfiguredHandle();
    if (!configuredHandle) {
        configuredHandle = generateConfiguredHandle();
    }
    bool result = false;
    if (configuredHandle) {
        onBlockaded(configuredHandle->getHandle());
        flowBack(configuredHandle);
        result = true;
    }
    m_rwlock.unlockWrite();
    return result;
}

void HandlePool::unblockade()
{
    m_rwlock.unlockWrite();
}

bool HandlePool::isBlockaded() const
{
    return m_rwlock.isWriting();
}

void HandlePool::drain(const HandlePool::DrainedCallback &onDrained)
{
    m_rwlock.lockWrite();
    ConcurrentList<ConfiguredHandle>::ElementType handle;
    while ((handle = m_handles.popBack())) {
        handle->getHandle()->close();
        --m_aliveHandleCount;
    }
    if (onDrained) {
        onDrained();
    }
    m_rwlock.unlockWrite();
}

void HandlePool::purgeFreeHandles()
{
    m_rwlock.lockRead();
    int size = (int) m_handles.clear();
    m_aliveHandleCount -= size;
    m_rwlock.unlockRead();
}

bool HandlePool::isDrained()
{
    return m_aliveHandleCount == 0;
}

bool HandlePool::canFlowOut()
{
    bool result = false;
    m_rwlock.lockRead();
    {
        std::shared_ptr<ConfiguredHandle> configuredHandle =
            flowOutConfiguredHandle();
        if (!configuredHandle) {
            configuredHandle = generateConfiguredHandle();
        }
        if (configuredHandle) {
            flowBack(configuredHandle);
            result = true;
        }
    }
    return result;
}

RecyclableHandle HandlePool::flowOut()
{
    m_rwlock.lockRead();
    std::shared_ptr<ConfiguredHandle> configuredHandle =
        flowOutConfiguredHandle();
    if (!configuredHandle) {
        configuredHandle = generateConfiguredHandle();
    }
    if (!configuredHandle) {
        m_rwlock.unlockRead();
        return nullptr;
    }
    return RecyclableHandle(configuredHandle, [configuredHandle, this]() {
        flowBack(configuredHandle);
    });
}

void HandlePool::flowBack(
    const std::shared_ptr<ConfiguredHandle> &configuredHandle)
{
    if (configuredHandle) {
        bool inserted = m_handles.pushBack(configuredHandle);
        m_rwlock.unlockRead();
        if (!inserted) {
            --m_aliveHandleCount;
        }
    }
}

std::shared_ptr<Handle> HandlePool::generateHandle()
{
    return Handle::handleWithPath(path, tag.load());
}

std::shared_ptr<ConfiguredHandle> HandlePool::flowOutConfiguredHandle()
{
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
        if (!FileManager::shared()->createDirectoryWithIntermediateDirectories(
                Path::getBaseName(path))) {
            setThreadedError(FileManager::shared()->getError());
            return nullptr;
        }
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
    if (m_aliveHandleCount > s_hardwareConcurrency) {
        CoreError error;
        error.code = (int) Error::Code::Error;
        error.level = Error::Level::Warning;
        error.tag = tag.load();
        error.path = path;
        error.message = "The number of database concurrency exceeds the "
                        "hardware's maximum: " +
                        std::to_string(s_hardwareConcurrency);
        error.report();
    }
    return configuredHandle;
}

void HandlePool::willConfigurateHandle(Handle *handle)
{
    if (handle->getTag() != tag.load()) {
        handle->setTag(tag.load());
    }
}

} //namespace WCDB
