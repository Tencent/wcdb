//
//  AutoMigrateConfig.cpp
//  WCDB
//
//  Created by sanhuazhang on 2019/5/25.
//  Copyright © 2019 sanhuazhang. All rights reserved.
//

#include <WCDB/Assertion.hpp>
#include <WCDB/AutoMigrateConfig.hpp>
#include <WCDB/Handle.hpp>

namespace WCDB {

AutoMigrateOperator::~AutoMigrateOperator()
{
}

AutoMigrateConfig::AutoMigrateConfig(const std::shared_ptr<AutoMigrateOperator>& operator_)
: Config(), m_operator(operator_)
{
    WCTInnerAssert(m_operator != nullptr);
}

AutoMigrateConfig::~AutoMigrateConfig()
{
}

bool AutoMigrateConfig::invoke(Handle* handle)
{
    const String& path = handle->getPath();
    if (++getOrCreateRegister(path) == 1) {
        m_operator->asyncMigrate(path);
    }
    return true;
}

bool AutoMigrateConfig::uninvoke(Handle* handle)
{
    const String& path = handle->getPath();
    if (--getOrCreateRegister(path) == 0) {
        m_operator->stopMigrate(path);
    }
    return true;
}

std::atomic<int>& AutoMigrateConfig::getOrCreateRegister(const String& path)
{
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_registers.find(path);
        if (iter != m_registers.end()) {
            WCTInnerAssert(iter->second.load() >= 0);
            return iter->second;
        }
    }
    {
        LockGuard lockGuard(m_lock);
        auto iter = m_registers.find(path);
        if (iter == m_registers.end()) {
            iter = m_registers.emplace(path, 0).first;
        }
        WCTInnerAssert(iter->second.load() >= 0);
        return iter->second;
    }
}

} // namespace WCDB
