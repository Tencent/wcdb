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

#include <WCDB/Lang.h>
#include <WCDB/String.hpp>

namespace WCDB {

namespace Lang {

LiteralValue::LiteralValue() : type(Type::NotSet)
{
}

CopyOnWriteString LiteralValue::SQL() const
{
    std::string description;
    switch (type) {
        case Type::NumbericInteger:
        case Type::NumbericFloat:
            //TODO use integer to store number
            description.append(stringValue.get());
            break;
        case Type::String:
            description.append("'");
            description.append(
                LiteralValue::stringByAntiInjecting(stringValue).get());
            description.append("'");
            break;
        case Type::BLOB:
            description.append("'");
            description.append(
                LiteralValue::stringByAntiInjecting(dataValue).get());
            description.append("'");
            break;
        case Type::Null:
            description.append("NULL");
            break;
        case Type::CurrentTime:
            description.append("CURRENT_TIME");
            break;
        case Type::CurrentDate:
            description.append("CURRENT_DATE");
            break;
        case Type::CurrentTimestamp:
            description.append("CURRENT_TIMESTAMP");
            break;
        default:
            LangRemedialFatalError();
            break;
    }
    return description;
}

CopyOnWriteString
LiteralValue::stringByAntiInjecting(const CopyOnWriteString &origin)
{
    return String::stringByReplacingOccurrencesOfString(origin, "'", "''");
}

CopyOnWriteString
LiteralValue::stringByAntiInjecting(const CopyOnWriteData &origin)
{
    //TODO use BindParameter to accept data with '\0'
    const std::vector<unsigned char> &data = origin.get();
    CopyOnWriteString cowString;
    cowString.assign(std::string(data.begin(), data.end()));
    return String::stringByReplacingOccurrencesOfString(cowString, "'", "''");
}

} // namespace Lang

} // namespace WCDB
