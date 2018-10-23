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

BindParameter::BindParameter(int n)
{
    syntax.switcher = SyntaxType::Switch::QuestionSign;
    syntax.n = n;
    syntax.name.clear();
}

BindParameter::BindParameter(const SyntaxString& name)
{
    syntax.switcher = SyntaxType::Switch::QuestionSign;
    syntax.name = name;
}

BindParameters BindParameter::bindParameters(size_t count)
{
    BindParameters result;
    for (size_t i = 1; i <= count; ++i) {
        result.push_back(BindParameter(i));
    }
    return result;
}

BindParameter BindParameter::colon(const SyntaxString& name)
{
    BindParameter bindParameter;
    bindParameter.syntax.switcher = SyntaxType::Switch::ColonSign;
    bindParameter.syntax.name = name;
    return bindParameter;
}

BindParameter BindParameter::dollar(const SyntaxString& name)
{
    BindParameter bindParameter;
    bindParameter.syntax.switcher = SyntaxType::Switch::DollarSign;
    bindParameter.syntax.name = name;
    return bindParameter;
}

} // namespace WCDB
