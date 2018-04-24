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

#include <WCDB/WINQ.h>

namespace WCDB {

const StatementBegin &StatementBegin::deferred()
{
    static const StatementBegin s_deferred =
        StatementBegin().begin(Lang::BeginSTMT::Type::Deferred);
    return s_deferred;
}

const StatementBegin &StatementBegin::immediate()
{
    static const StatementBegin s_immediate =
        StatementBegin().begin(Lang::BeginSTMT::Type::Immediate);
    return s_immediate;
}

const StatementBegin &StatementBegin::exclusive()
{
    static const StatementBegin s_exclusive =
        StatementBegin().begin(Lang::BeginSTMT::Type::Exclusive);
    return s_exclusive;
}

StatementBegin &StatementBegin::begin(Lang::BeginSTMT::Type type)
{
    Lang::BeginSTMT &lang = getMutableLang();
    lang.type = type;
    return *this;
}

} // namespace WCDB
