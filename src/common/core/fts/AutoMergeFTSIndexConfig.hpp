//
// Created by qiuwenchen(qiuwenchen) on 2020/10/22.
//

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

#include "Config.hpp"
#include "Lock.hpp"

namespace WCDB {

class AutoMergeFTSIndexOperator {
public:
    using TableArray = std::shared_ptr<std::vector<StringView>>;

    virtual ~AutoMergeFTSIndexOperator() = 0;

    virtual void
    asyncMergeFTSIndex(const UnsafeStringView& path, TableArray newTables, TableArray modifiedTables)
    = 0;
};

class AutoMergeFTSIndexConfig final : public Config {
public:
    AutoMergeFTSIndexConfig(const std::shared_ptr<AutoMergeFTSIndexOperator>& operator_);
    ~AutoMergeFTSIndexConfig() override;

    bool invoke(InnerHandle* handle) override final;
    bool uninvoke(InnerHandle* handle) override final;

    using TableArray = AutoMergeFTSIndexOperator::TableArray;

protected:
    const StringView m_identifier;

    void onTableModified(const UnsafeStringView& path,
                         const UnsafeStringView& newTable,
                         const UnsafeStringView& modifiedTable);
    bool onCommitted(const UnsafeStringView& path, int pages);

    std::shared_ptr<AutoMergeFTSIndexOperator> m_operator;
    StringViewMap<StringViewSet> m_newTables;
    StringViewMap<StringViewSet> m_modifiedTables;
    mutable SharedLock m_lock;
};

} //namespace WCDB
