//
//  AutoMigrateConfig.hpp
//  WCDB
//
//  Created by sanhuazhang on 2019/5/25.
//  Copyright © 2019 sanhuazhang. All rights reserved.
//

#pragma once

#include <WCDB/Config.hpp>
#include <WCDB/String.hpp>
#include <memory>

namespace WCDB {

class AutoMigrateOperator {
public:
    virtual ~AutoMigrateOperator() = 0;
    virtual void asyncMigrate(const String &path) = 0;
    virtual void stopMigrate(const String &path) = 0;
};

class AutoMigrateConfig final : public Config {
public:
    AutoMigrateConfig(const std::shared_ptr<AutoMigrateOperator> &operator_);
    ~AutoMigrateConfig();

    bool invoke(Handle *handle) override final;
    bool uninvoke(Handle *handle) override final;

protected:
    std::shared_ptr<AutoMigrateOperator> m_operator;
    std::atomic<int> m_invoked;
};

} //namespace WCDB
