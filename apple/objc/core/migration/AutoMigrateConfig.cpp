//
//  AutoMigrateConfig.cpp
//  WCDB
//
//  Created by sanhuazhang on 2019/5/25.
//  Copyright © 2019 sanhuazhang. All rights reserved.
//

#include <WCDB/AutoMigrateConfig.hpp>
#include <WCDB/Handle.hpp>

namespace WCDB {

AutoMigrateOperator::~AutoMigrateOperator()
{
}

AutoMigrateConfig::AutoMigrateConfig(const std::shared_ptr<AutoMigrateOperator> &operator_)
: Config(), m_operator(operator_)
{
}

AutoMigrateConfig::~AutoMigrateConfig()
{
}

bool AutoMigrateConfig::invoke(Handle *handle)
{
    m_operator->registerAsRequiredMigration(handle->getPath());
    return true;
}

bool AutoMigrateConfig::uninvoke(Handle *handle)
{
    m_operator->registerAsNoMigrationRequired(handle->getPath());
    return true;
}

} // namespace WCDB
