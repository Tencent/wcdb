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

AutoMigrateConfig::AutoMigrateConfig(const std::shared_ptr<AutoMigrateOperator> &operator_)
: Config(), m_operator(operator_), m_invoked(0)
{
}

AutoMigrateConfig::~AutoMigrateConfig()
{
}

bool AutoMigrateConfig::invoke(Handle *handle)
{
    if (++m_invoked == 1) {
        m_operator->asyncMigrate(handle->getPath());
    }
    return true;
}

bool AutoMigrateConfig::uninvoke(Handle *handle)
{
    if (--m_invoked == 0) {
        m_operator->stopMigrate(handle->getPath());
    }
    WCTInnerAssert(m_invoked.load() >= 0);
    return true;
}

} // namespace WCDB
