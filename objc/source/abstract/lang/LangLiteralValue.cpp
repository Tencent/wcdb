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

#include <WCDB/lang.h>
#include <WCDB/utility.hpp>
#include <sstream>

namespace WCDB {

namespace lang {

LiteralValue::LiteralValue() : type(Type::NotSet), integerValue(0)
{
}

copy_on_write_string LiteralValue::SQL() const
{
    std::string description;
    switch (type) {
        case Type::NumbericInteger:
            description.append(std::to_string(integerValue));
            break;
        case Type::NumbericFloat: {
            std::ostringstream os;
            os << floatValue;
            description.append(os.str());
        } break;
        case Type::BLOB:
            assert(!dataValue.empty());
            description.append(
                LiteralValue::stringByAntiInjecting(dataValue.get()));
            break;
        case Type::String:
            assert(!stringValue.empty());
            description.append(
                LiteralValue::stringByAntiInjecting(stringValue.get()));
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
            assert(false);
            break;
    }
    return description;
}

std::string LiteralValue::stringByAntiInjecting(const std::string &origin)
{
    return "'" + stringByReplacingOccurrencesOfString(origin, "'", "''") + "'";
}

std::string
LiteralValue::stringByAntiInjecting(const std::vector<unsigned char> &origin)
{
    std::string str(origin.begin(), origin.end());
    return "'" + stringByReplacingOccurrencesOfString(str, "'", "''") + "'";
}

} // namespace lang

} // namespace WCDB
