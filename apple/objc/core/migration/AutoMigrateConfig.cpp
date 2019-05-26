//
//  AutoMigrateConfig.cpp
//  WCDB
//
//  Created by sanhuazhang on 2019/5/25.
//  Copyright © 2019 sanhuazhang. All rights reserved.
//

#include <WCDB/AutoMigrateConfig.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/MigrationQueue.hpp>

namespace WCDB {

AutoMigrateConfig::AutoMigrateConfig(const std::shared_ptr<MigrationQueue> &queue)
: Config(), m_queue(queue)
{
}

AutoMigrateConfig::~AutoMigrateConfig()
{
}

bool AutoMigrateConfig::invoke(Handle *handle)
{
    m_queue->register_(handle->getPath());
    m_queue->put(handle->getPath());
    return true;
}

bool AutoMigrateConfig::uninvoke(Handle *handle)
{
    m_queue->unregister(handle->getPath());
    return true;
}

} // namespace WCDB
