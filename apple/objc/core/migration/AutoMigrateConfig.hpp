//
//  AutoMigrateConfig.hpp
//  WCDB
//
//  Created by sanhuazhang on 2019/5/25.
//  Copyright © 2019 sanhuazhang. All rights reserved.
//

#pragma once

#include <WCDB/Config.hpp>
#include <memory>

namespace WCDB {

class MigrationQueue;

class AutoMigrateConfig final : public Config {
public:
    AutoMigrateConfig(const std::shared_ptr<MigrationQueue> &queue);
    ~AutoMigrateConfig();

    bool invoke(Handle *handle) override final;
    bool uninvoke(Handle *handle) override final;

protected:
    std::shared_ptr<MigrationQueue> m_queue;
};

} //namespace WCDB
