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

#ifndef MigrationHandle_hpp
#define MigrationHandle_hpp

#include <WCDB/Abstract.h>

namespace WCDB {

class MigrationInfos;

class MigrationHandle : public Handle {
#pragma mark - Initialize
public:
    MigrationHandle(const std::string &path, MigrationInfos *infos);

protected:
    MigrationInfos *m_infos;

#pragma mark - Override
public:
    bool execute(const Statement &statement) override;
    bool prepare(const Statement &statement) override;
    bool step(bool &done) override;
    void reset() override;
    using Handle::step;
    void finalize() override;
    void bindInteger32(const Integer32 &value, int index) override;
    void bindInteger64(const Integer64 &value, int index) override;
    void bindDouble(const Float &value, int index) override;
    void bindText(const Text &value, int length, int index) override;
    void bindBLOB(const BLOB &value, int index) override;
    void bindNull(int index) override;
};

} //namespace WCDB

#endif /* MigrationHandle_hpp */
