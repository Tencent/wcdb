//
// Created by sanhuazhang on 2019/05/02
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

#include "WINQ.h"

namespace WCDB {

JoinConstraint::JoinConstraint() = default;

JoinConstraint::~JoinConstraint() = default;

JoinConstraint& JoinConstraint::on(const Expression& expression)
{
    syntax().expression = expression;
    return *this;
}

JoinConstraint& JoinConstraint::using_()
{
    return *this;
}

JoinConstraint& JoinConstraint::column(const Column& column)
{
    syntax().columns.push_back(column);
    return *this;
}

JoinConstraint& JoinConstraint::columns(const Columns& columns)
{
    syntax().columns = columns;
    return *this;
}

} // namespace WCDB
