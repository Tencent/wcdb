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

#ifndef _WCDB_MASTER_HPP
#define _WCDB_MASTER_HPP

#include <WCDB/String.hpp>

namespace WCDB {

namespace Repair {

struct Master {
public:
    Master();

    static bool isReservedTableName(const String &tableName);

    String type;
    String name;
    String tableName;
    String sql;
    int rootpage;
};

} //namespace Repair

} //namespace WCDB

#endif /* _WCDB_MASTER_HPP */
