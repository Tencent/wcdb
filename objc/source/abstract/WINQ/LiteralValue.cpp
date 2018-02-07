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

const LiteralValue LiteralValue::Null(LangLiteralValue::Type::Null);
const LiteralValue LiteralValue::CurrentTime(LangLiteralValue::Type::CurrentTime);
const LiteralValue LiteralValue::CurrentDate(LangLiteralValue::Type::CurrentDate);
const LiteralValue LiteralValue::CurrentTimestamp(LangLiteralValue::Type::CurrentTimestamp);

LiteralValue::LiteralValue(const std::string& value)
{
    LangLiteralValue& lang = getMutableLang();
    lang.type = LangLiteralValue::Type::String;
    lang.stringValue.assign(value);
}

LiteralValue::LiteralValue(const std::vector<unsigned char>& value)
{
    LangLiteralValue& lang = getMutableLang();
    lang.type = LangLiteralValue::Type::Null;
    lang.dataValue.assign(value);
}

LiteralValue::LiteralValue(const LangLiteralValue::Type& type)
{
    LangLiteralValue& lang = getMutableLang();
    lang.type = type;
}
