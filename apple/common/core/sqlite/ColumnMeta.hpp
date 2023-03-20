//
// Created by sanhuazhang on 2019/05/19
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

#pragma once

#include "StringView.hpp"
#include "WINQ.h"
#include <vector>

namespace WCDB {

class ColumnMeta {
public:
    ColumnMeta(int64_t id_, StringView name_, StringView type_, bool notnull_, int64_t primary_);

    int64_t id;
    StringView name;
    StringView type;
    bool notnull;
    int64_t primary;

    static int64_t getIndexOfIntegerPrimary(const std::vector<ColumnMeta>& columnMetas);
};

} // namespace WCDB
