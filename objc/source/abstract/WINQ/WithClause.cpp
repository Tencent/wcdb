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

#include <WCDB/Assertion.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

WithClause &WithClause::with(const CTETableName &cteTableName, const StatementSelect &selectSTMT)
{
    return with(cteTableName, selectSTMT, false);
}

WithClause &WithClause::withRecursive(const CTETableName &cteTableName,
                                      const StatementSelect &selectSTMT)
{
    return with(cteTableName, selectSTMT, true);
}

WithClause &
WithClause::with(const CTETableName &cteTableName, const StatementSelect &selectSTMT, bool recursive)
{
    Lang::CopyOnWriteLazyLang<Lang::WithClause::Pair> pair;
    {
        Lang::WithClause::Pair &lang = pair.get_or_copy();
        lang.cteTableName.assign(cteTableName.getCOWLang());
        lang.selectSTMT.assign(selectSTMT.getCOWLang());
    }

    Lang::WithClause &lang = getMutableLang();
    lang.pairs.append(pair);
    lang.recursive = recursive;
    return *this;
}

} // namespace WCDB
