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

BindParameter BindParameter::defaultBindParameter;

BindParameter::BindParameter()
{
    LangBindParameter &lang = getMutableLang();
    lang.type = LangBindParameter::Type::QuestionMark;
}

BindParameter::BindParameter(int n)
{
    LangBindParameter &lang = getMutableLang();
    lang.type = LangBindParameter::Type::QuestionMarkWithNumber;
    lang.n = n;
}

BindParameter::BindParameter(const std::string &name)
{
    LangBindParameter &lang = getMutableLang();
    lang.type = LangBindParameter::Type::Colon;
    lang.name.assign(name);
}
