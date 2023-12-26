//
// Created by qiuwenchen on 2023/12/23.
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

#include "HandleOperator.hpp"
#include "MasterItem.hpp"
#include "StatementPragma.hpp"
#include "Vacuum.hpp"
#include <vector>

namespace WCDB {

class VacuumHandleOperator : public HandleOperator, public Repair::VacuumDelegate {
public:
    VacuumHandleOperator(InnerHandle *handle);
    ~VacuumHandleOperator() override;

#pragma mark - Vacuum
public:
    bool executeVacuum() override final;
    const Error &getVacuumError() override final;

private:
    struct TableInfo {
        StringView name;
        StringView sql;
        std::list<StringView> indexSQLs;
    };

    static const char *kOriginSchema;

    bool configDatabase();
    bool initTables();
    bool createTable(const TableInfo &info);
    bool copyWithouRowidTable(const TableInfo &info);
    bool copyNormalTable(const TableInfo &info);

    StringViewMap<TableInfo> m_tables;
    double m_tableWeight;
    std::list<StringView> m_associatedSQLs; // View, Trigger
};

} //namespace WCDB
