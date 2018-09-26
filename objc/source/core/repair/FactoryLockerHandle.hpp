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

#ifndef FactoryLockerHandle_hpp
#define FactoryLockerHandle_hpp

#include <WCDB/Abstract.h>
#include <WCDB/Factory.hpp>

namespace WCDB {

class FactoryReadLockerHandle : public Handle, public Repair::ReadLocker {
public:
    void setPath(const std::string &path) override;
    const std::string &getPath() const override;

    bool acquireLock() override;
    bool releaseLock() override;
    const Error &getError() const override;
};

class FactoryWriteLockerHandle : public Handle, public Repair::WriteLocker {
public:
    void setPath(const std::string &path) override;
    const std::string &getPath() const override;

    bool acquireLock() override;
    bool releaseLock() override;
    const Error &getError() const override;
};

} // namespace WCDB

#endif /* FactoryLockerHandle_hpp */
